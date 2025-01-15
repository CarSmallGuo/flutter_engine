/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
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
