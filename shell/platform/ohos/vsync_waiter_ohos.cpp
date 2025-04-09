/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "flutter/shell/platform/ohos/vsync_waiter_ohos.h"
#include "napi_common.h"
#include "ohos_logging.h"
#include <dlfcn.h>
#include <qos/qos.h>

namespace flutter {

static std::atomic_uint g_refresh_rate_ = 60;
static constexpr int32_t SUPPORT_API_VERSION = 14;

const char* flutterSyncName = "flutter_connect";
const char* NATIVE_DVSYNC_SO = "libnative_vsync.so";

thread_local bool VsyncWaiterOHOS::firstCall = true;

VsyncWaiterOHOS::VsyncWaiterOHOS(const flutter::TaskRunners& task_runners)
    : VsyncWaiter(task_runners) {
  vsyncHandle =
      OH_NativeVSync_Create("flutterSyncName", strlen(flutterSyncName));
}

VsyncWaiterOHOS::~VsyncWaiterOHOS() {
  OH_NativeVSync_Destroy(vsyncHandle);
  vsyncHandle = nullptr;
  nativeDvsyncFunc_ = nullptr;
  if (handle_) {
    dlclose(handle_);
    handle_ = nullptr;
  }
}

void VsyncWaiterOHOS::AwaitVSync() {
  TRACE_EVENT0("flutter", "VsyncWaiterOHOS::AwaitVSync");
  if (vsyncHandle == nullptr) {
    LOGE("AwaitVSync vsyncHandle is nullptr");
    return;
  }
  auto* weak_this = new std::weak_ptr<VsyncWaiter>(shared_from_this());
  OH_NativeVSync* handle = vsyncHandle;
  if (inScrollingStatus == false) {
    if (dvsyncEnabled == true) {
      dvsyncEnabled = false;
      SetDvsyncSwitch(false);
    }
  }

  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetUITaskRunner(), [weak_this, handle]() {
        int32_t ret = 0;
        if (0 != (ret = OH_NativeVSync_RequestFrame(handle, &OnVsyncFromOHOS,
                                                    weak_this))) {
          FML_DLOG(ERROR) << "AwaitVSync...failed:" << ret;
        }
      });
}

void VsyncWaiterOHOS::OnVsyncFromOHOS(long long timestamp, void* data) {
  if (data == nullptr) {
    FML_LOG(ERROR) << "VsyncWaiterOHOS::OnVsyncFromOHOS, data is nullptr.";
    return;
  }
  if (VsyncWaiterOHOS::firstCall) {
    int ret = OH_QoS_SetThreadQoS(QoS_Level::QOS_USER_INTERACTIVE);
    FML_DLOG(INFO) << "qos set VsyncWaiterOHOS result:" << ret << ",tid:" << gettid();
    VsyncWaiterOHOS::firstCall = false;
  }
  int64_t frame_nanos = static_cast<int64_t>(timestamp);
  auto frame_time = fml::TimePoint::FromEpochDelta(
      fml::TimeDelta::FromNanoseconds(frame_nanos));
  auto target_time = frame_time + fml::TimeDelta::FromNanoseconds(
                                      1000000000.0 / g_refresh_rate_);
  auto* weak_this = reinterpret_cast<std::weak_ptr<VsyncWaiter>*>(data);
  ConsumePendingCallback(weak_this, frame_time, target_time);
}

void VsyncWaiterOHOS::ConsumePendingCallback(
    std::weak_ptr<VsyncWaiter>* weak_this,
    fml::TimePoint frame_start_time,
    fml::TimePoint frame_target_time) {
  std::shared_ptr<VsyncWaiter> shared_this = weak_this->lock();
  delete weak_this;

  if (shared_this) {
    shared_this->FireCallback(frame_start_time, frame_target_time);
  }
}

void VsyncWaiterOHOS::OnUpdateRefreshRate(long long refresh_rate) {
  FML_DCHECK(refresh_rate > 0);
  g_refresh_rate_ = static_cast<int>(refresh_rate);
}

int VsyncWaiterOHOS::GetRefreshRate(void)
{
    return g_refresh_rate_;
}

void VsyncWaiterOHOS::DisableDVsync() {
  inScrollingStatus.store(false);
  if (dvsyncEnabled.load()) {
    SetDvsyncSwitch(false);
    dvsyncEnabled.store(false);
  }
}

void VsyncWaiterOHOS::EnableDVsync() {
  inScrollingStatus.store(true);
  if (!dvsyncEnabled.load()) {
    SetDvsyncSwitch(true);
    dvsyncEnabled.store(true);
  }
}

void VsyncWaiterOHOS::DisableDVsyncWithoutFling() {
  if (dvsyncEnabled == true) {
    dvsyncEnabled = false;
    TRACE_EVENT0("DisableDVsyncWithoutFling", "");
    SetDvsyncSwitch(false);
  }
}

void VsyncWaiterOHOS::EnableDVsyncWithoutFling() {
  if (dvsyncEnabled == false && inScrollingStatus == true) {
    dvsyncEnabled = true;
    TRACE_EVENT0("EnableDVsyncWithoutFling", "");
    SetDvsyncSwitch(true);
  }
}


void VsyncWaiterOHOS::SetDvsyncSwitch(bool enableDvsync) {
  if (apiVersion_ == 0) {
    apiVersion_ = OH_GetSdkApiVersion();
  }
  if (apiVersion_ < SUPPORT_API_VERSION) {
    LOGI("current api version not support native dvsync!");
    return;
  }
  if (!handle_) {
    handle_ = dlopen(NATIVE_DVSYNC_SO, RTLD_NOW);
  }
  if (!handle_) {
    LOGE("SetDvsyncSwitch load %{public}s failed!", NATIVE_DVSYNC_SO);
    return;
  }

  if (!nativeDvsyncFunc_) {
    nativeDvsyncFunc_ = reinterpret_cast<NativeDvsyncFunc>(dlsym(handle_, "OH_NativeVSync_DVSyncSwitch"));
  }
  if (!nativeDvsyncFunc_) {
    LOGE("SetDvsyncSwitch load OH_NativeVSync_DVSyncSwitch failed!");
    dlclose(handle_);
    handle_ = nullptr;
    return;
  }
  nativeDvsyncFunc_(vsyncHandle, enableDvsync);
}

}  // namespace flutter
