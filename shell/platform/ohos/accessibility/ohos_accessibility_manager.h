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
#ifndef OHOS_ACCESSIBILITY_MANAGER_H
#define OHOS_ACCESSIBILITY_MANAGER_H

// #include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"
// #include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
// #include "types.h"
// #include "ohos_logging.h"
// #include "flutter/fml/logging.h"
// #include <functional>

// #include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {
/**
 * 无障碍辅助管理类
 */
class OhosAccessibilityManager {
 public:
  OhosAccessibilityManager();
  ~OhosAccessibilityManager();

  void onAccessibilityStateChanged(bool isOhosAccessibilityEnabled);
  bool getOhosAccessibilityEnabled();
  void setOhosAccessibilityEnabled(bool isEnabled);

 private:
  bool isOhosAccessibilityEnabled_ = false;
};

}  // namespace flutter

#endif