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
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <arkui/native_interface_accessibility.h>

#include "flutter/fml/log_level.h"
#include "flutter/lib/ui/semantics/custom_accessibility_action.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_manager.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {

typedef flutter::SemanticsFlags FLAGS_;
typedef flutter::SemanticsAction ACTIONS_;
typedef flutter::SemanticsNode SEMANTICS_NODE_;

/**
 * flutter和ohos的无障碍服务桥接
 */
class OhosAccessibilityBridge {
 public:
  OhosAccessibilityBridge();
  ~OhosAccessibilityBridge();

  static OhosAccessibilityBridge* GetInstance();

  bool isOhosAccessibilityEnabled_;

  void DispatchSemanticsAction(int32_t id, flutter::SemanticsAction action);

  void announce(std::unique_ptr<char[]>& message);

  void updateSemantics(flutter::SemanticsNodeUpdates update,
                       flutter::CustomAccessibilityActionUpdates actions);

  // obtain the flutter semnatics node
  flutter::SemanticsNode getOrCreateFlutterSemanticsNode(int32_t id);

  int32_t FindAccessibilityNodeInfosById(int64_t elementId, ArkUI_AccessibilitySearchMode mode, int32_t requestId, ArkUI_AccessibilityElementInfoList* elementList);
  int32_t FindAccessibilityNodeInfosByText(int64_t elementId, const char* text, int32_t requestId, ArkUI_AccessibilityElementInfoList* elementList);
  int32_t FindFocusedAccessibilityNode(int64_t elementId, ArkUI_AccessibilityFocusType focusType, int32_t requestId, ArkUI_AccessibilityElementInfo* elementinfo);
  int32_t FindNextFocusAccessibilityNode(int64_t elementId, ArkUI_AccessibilityFocusMoveDirection direction, int32_t requestId, ArkUI_AccessibilityElementInfo* elementList);
  int32_t ExecuteAccessibilityAction(int64_t elementId, ArkUI_Accessibility_ActionType action, ArkUI_AccessibilityActionArguments *actionArguments, int32_t requestId);
  int32_t ClearFocusedFocusAccessibilityNode();
  int32_t GetAccessibilityNodeCursorPosition(int64_t elementId, int32_t requestId, int32_t* index);
  ArkUI_AccessibilityProvider* provider_;

  void Flutter_SendAccessibilityAsyncEvent(int64_t elementId, ArkUI_AccessibilityEventType eventType);
  void Flutter_InitSpercificElementInfoById(ArkUI_AccessibilityElementInfo* elementInfoFromList, int64_t elementId);
  int32_t GetParentId(int64_t elementId);

 private:
  static OhosAccessibilityBridge bridgeInstance; 

  std::shared_ptr<OhosAccessibilityManager> ax_manager_;
  std::unordered_map<int32_t, int32_t> parentChildIdMap;
  std::unordered_map<int32_t, flutter::SemanticsNode> flutterSemanticsTree_;
  std::unordered_map<int32_t, flutter::CustomAccessibilityAction> actions_mp_;
  std::vector<int32_t> flutterNavigationStack;

  static const int32_t ROOT_NODE_ID = 0;

  void FlutterTreeToArkuiTree(ArkUI_AccessibilityElementInfoList* elementInfoList);

  flutter::SemanticsNode getFlutterRootSemanticsNode();
  bool IsNodeFocusable(const flutter::SemanticsNode& node);
  std::string GetNodeComponentType(const flutter::SemanticsNode& node);


  void onWindowNameChange(flutter::SemanticsNode route);
  void removeSemanticsNode(flutter::SemanticsNode nodeToBeRemoved);
   
  void printTest(flutter::SemanticsNode node);
  void printTestActions(flutter::CustomAccessibilityAction customAccessibilityAction);
};

}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
