// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef OHOS_ACCESSIBILITY_MANAGER_H
#define OHOS_ACCESSIBILITY_MANAGER_H
#include <memory>

namespace flutter {
/**
 * 无障碍辅助管理类
 */
class OhosAccessibilityManager {
 public:
  OhosAccessibilityManager();
  ~OhosAccessibilityManager();

  void OnAccessibilityStateChanged(bool ohosAccessibilityEnabled);

  bool GetOhosAccessibilityEnabled();

  void SetOhosAccessibilityEnabled(bool isEnabled);

 private:
   bool isOhosAccessibilityEnabled_;
};

}  // namespace flutter

#endif