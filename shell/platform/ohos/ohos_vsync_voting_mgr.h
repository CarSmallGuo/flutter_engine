#ifndef FLUTTER_SHELL_PLATFORM_OHOS_OHOS_VSYNC_VOTING_MGR_H_
#define FLUTTER_SHELL_PLATFORM_OHOS_OHOS_VSYNC_VOTING_MGR_H_

#include <cstdint>
#include <memory>
#include <map>
#include <json/json.h>

#include <native_vsync/native_vsync.h>
#include "flutter/fml/time/time_point.h"

#include "flutter/shell/platform/ohos/ohos_asset_provider.h"

namespace flutter {
using namespace std;
using SetExpectedFrameRateRangeFunc_ = int(*)(OH_NativeVSync* nativeVsync,
                                              OH_NativeVSync_ExpectedRateRange* range);

enum class VsyncVotingPriority {
  VOTING_LEVEL_HIGH = 0,
  VOTING_LEVEL_ANIMATION,
  VOTING_LEVEL_TOUCH,
  VOTING_LEVEL_WEB,
  VOTING_LEVEL_VIDEO,
  VOTING_LEVEL_LOW,
};

enum class VsyncVotingType {
  VOTING_TYPE_ANIMATION = 0,
  VOTING_TYPE_TOUCH,
  VOTING_TYPE_WEDB,
  VOTING_TYPE_VIDEO,
  VOTING_LEVEL_INVALID,
};

enum class AnimationType {
  AN_TYPE_TRANSLATE = 0,
  AN_TYPE_SCALE,
  AN_TYPE_ROTATION,
};

enum class VVMTouchType {
  TOUCH_TYPE_DOWN,
  TOUCH_TYPE_UP,
  TOUCH_TYPE_UP_3_SEC_AFTER,
};

class OhosVsyncVotingMgr {
public:
  OhosVsyncVotingMgr();
  ~OhosVsyncVotingMgr();

  static shared_ptr<OhosVsyncVotingMgr> GetInstance(void);

  void VoteAnimationValue(AnimationType ANType, double devicePixelRatio, double velocity);

  void VoteTouchValue(VVMTouchType type, int64_t timestamp);

  void VoteWebValue(void);

  void VoteVideoValue(int second, int frameCount);

  void AttachNativeVsync(string handleName, OH_NativeVSync* handle);

  void DettachNativeVsync(string handleName);

  void VotingByNativeVsync(OH_NativeVSync* handle);

  void ParseFramesCfg(void);

  void SetAssetProvider(std::unique_ptr<OHOSAssetProvider> hap_asset_provider);

private:
  inline void VoteANTranslate(double velocity);

  inline void VoteANScale(double velocity);

  inline void VoteANRotation(double velocity);

  void VotingBySelf();

  inline void ParseTranslate(const Json::Value& arr);

private:
  int animationVoting_ = 0;

  int touchVoting_ = 0;

  int videoVoting_ = 0;

  int resultFrameRate_ = 0;

  int localFrameRate_ = 0;

  uint32_t switchStatus_ = 0;

  double lastVelocity_ = 0.0;

  bool firstVoteFrame_ = true;

  bool isTouchDown_ = false;

  map<string, OH_NativeVSync*> nativeVsyncMap_;

  unique_ptr<OHOSAssetProvider> asset_provider_;

  vector<map<string, int> > framesSet;

  void* libHandle_;

  SetExpectedFrameRateRangeFunc_ setExpectedFrameRateRangeFunc_ = nullptr;
}; // class OhosVsyncVotingMgr

} // namespace flutter
#endif // FLUTTER_SHELL_PLATFORM_OHOS_OHOS_VSYNC_VOTING_MGR_H_
