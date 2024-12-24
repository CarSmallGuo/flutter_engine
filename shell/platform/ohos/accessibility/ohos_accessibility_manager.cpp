// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ohos_accessibility_manager.h"

namespace flutter {

OhosAccessibilityManager::OhosAccessibilityManager() {}

OhosAccessibilityManager::~OhosAccessibilityManager() {}

/**
 * 监听ohos平台是否开启无障碍屏幕朗读功能
 */
void OhosAccessibilityManager::OnAccessibilityStateChanged(
    bool ohosAccessibilityEnabled) {}

void OhosAccessibilityManager::SetOhosAccessibilityEnabled(bool isEnabled) 
{
  this->isOhosAccessibilityEnabled_ = isEnabled;
}

bool OhosAccessibilityManager::GetOhosAccessibilityEnabled() 
{
  return this->isOhosAccessibilityEnabled_;
}

}  // namespace flutter
