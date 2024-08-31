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
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_manager.h"
#include "flutter/fml/logging.h"

namespace flutter {
OhosAccessibilityManager::OhosAccessibilityManager() {};
OhosAccessibilityManager::~OhosAccessibilityManager() {};

void OhosAccessibilityManager::onAccessibilityStateChanged(
    bool isOhosAccessibilityEnabled) {};

void OhosAccessibilityManager::setOhosAccessibilityEnabled(bool isEnabled) {
  FML_DLOG(INFO) << "OhosAccessibilityManager::setOhosAccessibilityEnabled = "
                 << isEnabled;
  this->isOhosAccessibilityEnabled_ = isEnabled;
}

bool OhosAccessibilityManager::getOhosAccessibilityEnabled() {
  return this->isOhosAccessibilityEnabled_;
}

}  // namespace flutter
