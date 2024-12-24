// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VSYNC_WAITER_OHOS_H
#define VSYNC_WAITER_OHOS_H
#include <memory>

#include <native_vsync/native_vsync.h>
#include "flutter/fml/macros.h"
#include "flutter/shell/common/vsync_waiter.h"

namespace flutter {

class VsyncWaiterOHOS final : public VsyncWaiter {
 public:
  explicit VsyncWaiterOHOS(const flutter::TaskRunners& task_runners);
  static void OnUpdateRefreshRate(long long refresh_rate);

  ~VsyncWaiterOHOS() override;

 private:
  thread_local static bool firstCall;
  // |VsyncWaiter|
  void AwaitVSync() override;

  static void OnVsyncFromOHOS(long long timestamp, void* data);
  static void ConsumePendingCallback(std::weak_ptr<VsyncWaiter>* weak_this,
                                     fml::TimePoint frame_start_time,
                                     fml::TimePoint frame_target_time);

  OH_NativeVSync* vsyncHandle;
  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiterOHOS);
};
}  // namespace flutter
#endif
