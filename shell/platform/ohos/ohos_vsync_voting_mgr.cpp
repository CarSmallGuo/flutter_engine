#include "ohos_vsync_voting_mgr.h"

#include <dlfcn.h>
#include <algorithm>

#include "flutter/fml/logging.h"
#include "flutter/fml/trace_event.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {

thread_local int64_t touchTimestamp = 0;

static std::shared_ptr<OhosVsyncVotingMgr> instance = nullptr;
static std::once_flag instanceFlag;

static constexpr int32_t PHYSICAL_PIXEL_DENSITY = 160;
// 1 inch equals 25.4 millimeters
static constexpr double INCH_2_MILL = 25.4;

static constexpr int32_t FPS_120 = 120;
static constexpr int32_t FPS_90 = 90;
static constexpr int32_t FPS_72 = 72;
static constexpr int32_t FPS_60 = 60;
static constexpr int32_t FPS_30 = 30;

constexpr char LIB_NATIVE_VSYNC_NAME[] = "libnative_vsync.so";
constexpr char FUN_SET_FREAM_RATE_NAME[] =
    "OH_NativeVSync_SetExpectedFrameRateRange";

constexpr char FRAMES_CFG_JSON[] = "framesconfig.json";
constexpr char SWITCH_KEY[] = "SWITCH";
constexpr char TRANSLATE_KEY[] = "TRANSLATE";
constexpr char SCALE_KEY[] = "SCALE";
constexpr char ROTATION_KEY[] = "ROTATION";

static constexpr fml::TimeDelta TOUCH_3_SEC = fml::TimeDelta::FromSeconds(3);

std::shared_ptr<OhosVsyncVotingMgr> OhosVsyncVotingMgr::GetInstance(void) {
  std::call_once(instanceFlag, [&] {
    instance = std::shared_ptr<OhosVsyncVotingMgr>(new OhosVsyncVotingMgr);
  });

  return instance;
}

OhosVsyncVotingMgr::OhosVsyncVotingMgr() : libHandle_(nullptr) {
  libHandle_ = dlopen(LIB_NATIVE_VSYNC_NAME, RTLD_LAZY | RTLD_LOCAL);
  if (libHandle_ == nullptr) {
    FML_LOG(ERROR) << "Failed to dlopen libnative_vsync.so";
    return;
  }

  setExpectedFrameRateRangeFunc_ =
      reinterpret_cast<SetExpectedFrameRateRangeFunc_>(
          dlsym(libHandle_, FUN_SET_FREAM_RATE_NAME));
  if (setExpectedFrameRateRangeFunc_ == nullptr) {
    dlclose(libHandle_);
    libHandle_ = nullptr;
    FML_LOG(ERROR)
        << "Failed to dlsym OH_NativeVSync_SetExpectedFrameRateRange";
  }
}

OhosVsyncVotingMgr::~OhosVsyncVotingMgr() {
  if (libHandle_ != nullptr) {
    dlclose(libHandle_);
  }
}

void OhosVsyncVotingMgr::VoteAnimationValue(AnimationType ANType,
                                            double devicePixelRatio,
                                            double velocity) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  double velocityTmp = abs(velocity);
  if (devicePixelRatio != 0.0) {
    // V(millimeter) = V(pixel) * 25.4 / (devicePixelRatio * 160)
    velocityTmp = velocityTmp / (devicePixelRatio * PHYSICAL_PIXEL_DENSITY);
    velocityTmp = velocityTmp * INCH_2_MILL;
  }

  switch (ANType) {
    case AnimationType::AN_TYPE_TRANSLATE:
      VoteANTranslate(velocityTmp);
      break;
    case AnimationType::AN_TYPE_SCALE:
      VoteANScale(velocityTmp);
      break;
    case AnimationType::AN_TYPE_ROTATION:
      VoteANRotation(velocityTmp);
      break;
    default:
      break;
  }
  return;
}

void OhosVsyncVotingMgr::VoteTouchValue(VVMTouchType type, int64_t timestamp) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  if (type == VVMTouchType::TOUCH_TYPE_UP) {
    touchVoting_ = FPS_120;
    touchTimestamp = timestamp;
    isTouchDown_ = false;
    VotingBySelf();
  } else if (type == VVMTouchType::TOUCH_TYPE_UP_3_SEC_AFTER) {
    if (timestamp - touchTimestamp >= 3000) {
      touchVoting_ = FPS_60;
      VotingBySelf();
      touchVoting_ = 0;
      VotingBySelf();
    }
  } else if (type == VVMTouchType::TOUCH_TYPE_DOWN) {
    touchVoting_ = FPS_120;
    isTouchDown_ = true;
    VotingBySelf();
  }
  return;
}

void OhosVsyncVotingMgr::VoteWebValue(void) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  touchVoting_ = 0;
  videoVoting_ = 0;
  animationVoting_ = 0;
  return;
}

void OhosVsyncVotingMgr::VoteVideoValue(int second, int frameCount) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  if (second <= 0 || frameCount <= 0) {
    return;
  }

  int frameRate = frameCount / second;
  if (frameRate <= FPS_30) {
    videoVoting_ = FPS_30;
  } else {
    videoVoting_ = FPS_60;
  }
  return;
}

inline void OhosVsyncVotingMgr::VoteANTranslate(double velocity) {
  if (lastVelocity_ < 1) {
    firstVoteFrame_ = true;
  }

  int expectedRateTmp = 0;
  size_t framesSetSize = framesSet.size();
  for (std::vector<std::map<string, int>>::iterator it = framesSet.begin();
       it != framesSet.end(); it++) {
    if (velocity > static_cast<double>((*it)["min"])) {
      expectedRateTmp = (*it)["preferred_fps"];
      break;
    }
  }

  lastVelocity_ = velocity;
  if (!firstVoteFrame_ &&
      (expectedRateTmp == PlatformViewOHOSNapi::display_refresh_rate)) {
    return;
  }

  firstVoteFrame_ = false;
  animationVoting_ = expectedRateTmp;
}

inline void OhosVsyncVotingMgr::VoteANScale(double velocity) {
  // TODO
  return;
}

inline void OhosVsyncVotingMgr::VoteANRotation(double velocity) {
  // TODO
  return;
}

void OhosVsyncVotingMgr::AttachNativeVsync(string handleName,
                                           OH_NativeVSync* handle) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (handle == nullptr) {
    FML_LOG(ERROR) << "handle is null";
    return;
  }

  nativeVsyncMap_.insert(std::pair(handleName, handle));
  return;
}

void OhosVsyncVotingMgr::DettachNativeVsync(string handleName) {
  nativeVsyncMap_.erase(handleName);
  return;
}

void OhosVsyncVotingMgr::VotingByNativeVsync(OH_NativeVSync* handle) {
  if (libHandle_ == nullptr || setExpectedFrameRateRangeFunc_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  if (handle == nullptr) {
    return;
  }

  if (animationVoting_ != 0 && !isTouchDown_) {
    resultFrameRate_ = animationVoting_;
  } else if (touchVoting_ != 0) {
    resultFrameRate_ = touchVoting_;
  } else if (videoVoting_ != 0) {
    resultFrameRate_ = videoVoting_;
  } else {
    resultFrameRate_ = 0;
  }

  if (resultFrameRate_ == 0 && resultFrameRate_ == localFrameRate_) {
    return;
  }

  if (resultFrameRate_ == PlatformViewOHOSNapi::display_refresh_rate &&
      resultFrameRate_ == localFrameRate_) {
    return;
  }

  localFrameRate_ = resultFrameRate_;

  int min = 0;
  int max = 0;
  if (resultFrameRate_ != 0) {
    min = FPS_30;
    max = FPS_120;
  }

  std::ostringstream oss;
  oss << "{" << min << "," << max << "," << resultFrameRate_ << "}";
  std::string rangeStr = oss.str();
  FML_LOG(INFO) << "SetExpectedFrameRateRange : " << rangeStr.c_str();
  TRACE_EVENT1("flutter", "SetExpectedFrameRateRange", "range",
               rangeStr.c_str());

  OH_NativeVSync_ExpectedRateRange range = {min, max, resultFrameRate_};

  int ret = setExpectedFrameRateRangeFunc_(handle, &range);
  if (ret != 0) {
    FML_LOG(ERROR) << "SetExpectedFrameRateRange failed, ret = " << ret;
  }

  return;
}

void OhosVsyncVotingMgr::VotingBySelf() {
  if (libHandle_ == nullptr) {
    return;
  }

  if (switchStatus_ != 1) {
    return;
  }

  if (nativeVsyncMap_.size() == 0) {
    return;
  }

  if (animationVoting_ != 0 && !isTouchDown_) {
    resultFrameRate_ = animationVoting_;
  } else if (touchVoting_ != 0) {
    resultFrameRate_ = touchVoting_;
  } else if (videoVoting_ != 0) {
    resultFrameRate_ = videoVoting_;
  } else {
    resultFrameRate_ = 0;
  }

  if (resultFrameRate_ == 0 && resultFrameRate_ == localFrameRate_) {
    return;
  }

  if (resultFrameRate_ == PlatformViewOHOSNapi::display_refresh_rate &&
      resultFrameRate_ == localFrameRate_) {
    return;
  }

  localFrameRate_ = resultFrameRate_;

  int min = 0;
  int max = 0;
  if (resultFrameRate_ != 0) {
    min = FPS_30;
    max = FPS_120;
  }

  std::ostringstream oss;
  oss << "{" << min << "," << max << "," << resultFrameRate_ << "}";
  std::string rangeStr = oss.str();
  FML_LOG(INFO) << "BySelf SetExpectedFrameRateRange : " << rangeStr.c_str();
  TRACE_EVENT1("flutter", "BySelf SetExpectedFrameRateRange", "range",
               rangeStr.c_str());

  OH_NativeVSync_ExpectedRateRange range = {min, max, resultFrameRate_};

  int ret = 0;
  for (auto it = nativeVsyncMap_.begin(); it != nativeVsyncMap_.end(); it++) {
    OH_NativeVSync* handleTmp = it->second;
    ret = setExpectedFrameRateRangeFunc_(handleTmp, &range);
    if (ret != 0) {
      FML_LOG(ERROR) << "BySelf SetExpectedFrameRateRange failed, ret = "
                     << ret;
    }
  }
  return;
}

inline void OhosVsyncVotingMgr::ParseTranslate(const Json::Value& arr) {
  if (arr.empty()) {
    FML_LOG(ERROR) << "The array is empty";
  }

  const char* tags[] = {"serial_number", "min", "max", "preferred_fps"};
  size_t num = sizeof(tags) / sizeof(char*);

  int valueTmp = 0;
  int number = 1;
  size_t size = arr.size();
  for (unsigned int i = 0; i < size; i++) {
    std::map<string, int> mapTmp;
    if (arr[i].isObject()) {
      for (unsigned int j = 0; j < num; j++) {
        const char* key = tags[j];
        if (!arr[i].isMember(key)) {
          FML_LOG(ERROR) << "config tag missed, key = " << key;
          return;
        } else if (!arr[i][key].isInt()) {
          FML_LOG(ERROR) << "config value invalid, key = " << key;
          return;
        } else {
          valueTmp = arr[i][key].asInt();
          if ((j == 0) && (valueTmp != number)) {
            FML_LOG(ERROR) << "config value serial_number is wrong";
          }
          mapTmp.insert(std::pair<string, int>(string(key), valueTmp));
        }
      }
    }

    framesSet.push_back(mapTmp);
    number++;
  }
}

void OhosVsyncVotingMgr::ParseFramesCfg(void) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (asset_provider_ == nullptr) {
    FML_LOG(ERROR) << "asset_provider_ is null";
    return;
  }

  std::unique_ptr<fml::Mapping> framesCfgMapping =
      asset_provider_->GetAsMapping(std::string(FRAMES_CFG_JSON));
  if (framesCfgMapping == nullptr) {
    FML_LOG(ERROR) << "Failed to GetAsMapping";
    return;
  }

  const char* data =
      reinterpret_cast<const char*>(framesCfgMapping->GetMapping());
  if (data == nullptr) {
    FML_LOG(ERROR) << "Failed to GetBuffer";
    return;
  }

  int size = static_cast<int>(framesCfgMapping->GetSize());

  Json::Value root;
  Json::CharReaderBuilder charReaderBuilder;
  std::string errs;
  std::unique_ptr<Json::CharReader> jsonReader(
      charReaderBuilder.newCharReader());
  bool isJson = jsonReader->parse(data, data + size, &root, &errs);
  if (!isJson || !errs.empty()) {
    FML_LOG(ERROR) << "Failed to parse frameconfig.json, err = " << errs;
    return;
  }

  if (root.isMember(SWITCH_KEY)) {
    if (root[SWITCH_KEY].isNumeric()) {
      switchStatus_ = root[SWITCH_KEY].asUInt();
    } else {
      FML_LOG(ERROR) << "Failed to parse key of SWITCH";
      return;
    }
  }

  if (root.isMember(TRANSLATE_KEY)) {
    if (root[TRANSLATE_KEY].isArray()) {
      ParseTranslate(root[TRANSLATE_KEY]);
    } else {
      FML_LOG(ERROR) << "Failed to parse key of SWITCH";
      return;
    }
  }

  return;
}

void OhosVsyncVotingMgr::SetAssetProvider(
    std::unique_ptr<OHOSAssetProvider> hap_asset_provider) {
  if (libHandle_ == nullptr) {
    return;
  }

  if (hap_asset_provider == nullptr) {
    FML_LOG(ERROR) << "hap_asset_provider is null";
    return;
  }

  asset_provider_ = std::move(hap_asset_provider);
  return;
}

}  // namespace flutter
