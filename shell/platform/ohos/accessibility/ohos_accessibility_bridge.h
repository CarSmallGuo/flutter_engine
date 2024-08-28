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

#ifndef OHOS_ACCESSIBILITY_BRIDGE_H
#define OHOS_ACCESSIBILITY_BRIDGE_H

#include "flutter/fml/log_level.h"
// #include "flutter/fml/macros.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace flutter {

/**
 * flutter和ohos的无障碍服务桥接
 */
class OhosAccessibilityBridge  {
 public:
  OhosAccessibilityBridge();
  ~OhosAccessibilityBridge();

  void updateSemantics(std::vector<uint8_t> buffer,
                       std::vector<std::string> strings,
                       std::vector<std::vector<uint8_t>> string_attribute_args);

  void updateCustomAccessibilityActions(std::vector<uint8_t> buffer,
                                        std::vector<std::string> strings);

 private:
  std::unordered_map<int32_t, flutter::SemanticsNode> flutterSemanticsTree;
};
}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
