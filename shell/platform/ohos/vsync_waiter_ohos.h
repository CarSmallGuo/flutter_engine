/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VSYNC_WAITER_OHOS_H
#define VSYNC_WAITER_OHOS_H
#include <atomic>
#include <memory>

#include <deviceinfo.h>
#include <native_vsync/native_vsync.h>
#include "flutter/fml/macros.h"
#include "flutter/shell/common/vsync_waiter.h"

namespace flutter {
using NativeDvsyncFunc = int (*)(OH_NativeVSync* nativeVSync, bool enable);

class VsyncWaiterOHOS final : public VsyncWaiter {
 public:
  explicit VsyncWaiterOHOS(const flutter::TaskRunners& task_runners);
  static void OnUpdateRefreshRate(long long refresh_rate);

  ~VsyncWaiterOHOS() override;

  void DisableDVsync() override;
  void EnableDVsync() override;

 private:
  std::atomic<bool> dvsyncEnabled{false};
  thread_local static bool firstCall;
  // |VsyncWaiter|
  void AwaitVSync() override;

  static void OnVsyncFromOHOS(long long timestamp, void* data);
  static void ConsumePendingCallback(std::weak_ptr<VsyncWaiter>* weak_this,
                                     fml::TimePoint frame_start_time,
                                     fml::TimePoint frame_target_time);
  void SetDvsyncSwitch(bool enableDvsync);

  OH_NativeVSync* vsyncHandle;
  NativeDvsyncFunc nativeDvsyncFunc_ = nullptr;
  void *handle_ = nullptr;
  int32_t apiVersion_ = 0;
  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiterOHOS);
};
}  // namespace flutter
#endif
