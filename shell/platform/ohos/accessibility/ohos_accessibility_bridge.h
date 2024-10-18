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
#include <arkui/native_interface_accessibility.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "flutter/fml/mapping.h"
#include "flutter/lib/ui/semantics/custom_accessibility_action.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_manager.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {

typedef flutter::SemanticsFlags FLAGS_;
typedef flutter::SemanticsAction ACTIONS_;
typedef flutter::SemanticsNode FLUTTER_NODE_;

/**
 * flutter和ohos的无障碍服务桥接
 */
struct AbsoluteRect {
  float left;
  float top;
  float right;
  float bottom;

  static constexpr AbsoluteRect MakeEmpty() {
    return AbsoluteRect{0.0, 0.0, 0.0, 0.0};
  }
};

struct SemanticsNodeExtent : flutter::SemanticsNode {
  int32_t parentId = -1;
  AbsoluteRect abRect = AbsoluteRect::MakeEmpty();

  int32_t previousFlags;
  int32_t previousActions;
  int32_t previousTextSelectionBase;
  int32_t previousTextSelectionExtent;
  float previousScrollPosition;
  float previousScrollExtentMax;
  float previousScrollExtentMin;
  std::string previousValue;
  std::string previousLabel;
};

class OhosAccessibilityBridge {
 public:
  OhosAccessibilityBridge();
  ~OhosAccessibilityBridge();

  static OhosAccessibilityBridge* GetInstance();

  bool isOhosAccessibilityEnabled_;
  bool IS_FLUTTER_NAVIGATE = false;
  int64_t nativeShellHolder_;
  ArkUI_AccessibilityProvider* provider_;

  void updateSemantics(flutter::SemanticsNodeUpdates update,
                       flutter::CustomAccessibilityActionUpdates actions);

  void DispatchSemanticsAction(int32_t id,
                               flutter::SemanticsAction action,
                               fml::MallocMapping args);

  void announce(std::unique_ptr<char[]>& message);

  // obtain the flutter semnatics node
  flutter::SemanticsNode getOrCreateFlutterSemanticsNode(int32_t id);

  int32_t FindAccessibilityNodeInfosById(
      int64_t elementId,
      ArkUI_AccessibilitySearchMode mode,
      int32_t requestId,
      ArkUI_AccessibilityElementInfoList* elementList);
  int32_t FindAccessibilityNodeInfosByText(
      int64_t elementId,
      const char* text,
      int32_t requestId,
      ArkUI_AccessibilityElementInfoList* elementList);
  int32_t FindFocusedAccessibilityNode(
      int64_t elementId,
      ArkUI_AccessibilityFocusType focusType,
      int32_t requestId,
      ArkUI_AccessibilityElementInfo* elementinfo);
  int32_t FindNextFocusAccessibilityNode(
      int64_t elementId,
      ArkUI_AccessibilityFocusMoveDirection direction,
      int32_t requestId,
      ArkUI_AccessibilityElementInfo* elementList);
  int32_t ExecuteAccessibilityAction(
      int64_t elementId,
      ArkUI_Accessibility_ActionType action,
      ArkUI_AccessibilityActionArguments* actionArguments,
      int32_t requestId);
  int32_t ClearFocusedFocusAccessibilityNode();
  int32_t GetAccessibilityNodeCursorPosition(int64_t elementId,
                                             int32_t requestId,
                                             int32_t* index);

  void Flutter_SendAccessibilityAsyncEvent(
      int64_t elementId,
      ArkUI_AccessibilityEventType eventType);
  void FlutterNodeToElementInfoById(
      ArkUI_AccessibilityElementInfo* elementInfoFromList,
      int64_t elementId);
  int32_t GetParentId(int64_t elementId);

  void ConvertChildRelativeRectToScreenRect(flutter::SemanticsNode node);
  std::pair<std::pair<float, float>, std::pair<float, float>>
  GetAbsoluteScreenRect(int32_t flutterNodeId);
  void SetAbsoluteScreenRect(int32_t flutterNodeId,
                             float left,
                             float top,
                             float right,
                             float bottom);

  SemanticsNodeExtent SetAndGetSemanticsNodeExtent(flutter::SemanticsNode node);

  void FlutterScrollExecution(
      flutter::SemanticsNode node,
      ArkUI_AccessibilityElementInfo* elementInfoFromList);

  void ClearFlutterSemanticsCaches();

 private:
  static OhosAccessibilityBridge bridgeInstance;
  static const int32_t ROOT_NODE_ID = 0;
  constexpr static const double SCROLL_EXTENT_FOR_INFINITY = 100000.0;
  constexpr static const double SCROLL_POSITION_CAP_FOR_INFINITY = 70000.0;

  flutter::SemanticsNode inputFocusedNode;
  flutter::SemanticsNode lastInputFocusedNode;
  flutter::SemanticsNode accessibilityFocusedNode;

  std::shared_ptr<OhosAccessibilityManager> ax_manager_;
  std::vector<std::pair<int32_t, int32_t>> parentChildIdVec;
  std::unordered_map<int32_t, flutter::SemanticsNode> flutterSemanticsTree_;
  std::unordered_map<
      int32_t,
      std::pair<std::pair<float, float>, std::pair<float, float>>>
      screenRectMap_;
  std::unordered_map<int32_t, flutter::CustomAccessibilityAction> actions_mp_;
  std::vector<int32_t> flutterNavigationVec_;

  const std::unordered_map<std::string, ArkUI_Accessibility_ActionType>
      ArkUI_ACTION_TYPE_MAP_ = {
          {"invalid", ArkUI_Accessibility_ActionType::
                          ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_INVALID},
          {"click", ArkUI_Accessibility_ActionType::
                        ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK},
          {"long press", ArkUI_Accessibility_ActionType::
                             ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK},
          {"focus acquisition",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS},
          {"focus clearance",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS},
          {"forward scroll",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD},
          {"backward scroll",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD},
          {"copy text", ArkUI_Accessibility_ActionType::
                            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY},
          {"paste text", ArkUI_Accessibility_ActionType::
                             ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE},
          {"cut text", ArkUI_Accessibility_ActionType::
                           ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT},
          {"text selection",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT},
          {"set text", ArkUI_Accessibility_ActionType::
                           ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT},
          {"text cursor position setting",
           ArkUI_Accessibility_ActionType::
               ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_CURSOR_POSITION},
  };

  static const int32_t FOCUSABLE_FLAGS =
      static_cast<int32_t>(FLAGS_::kHasCheckedState) |
      static_cast<int32_t>(FLAGS_::kIsChecked) |
      static_cast<int32_t>(FLAGS_::kIsSelected) |
      static_cast<int32_t>(FLAGS_::kIsTextField) |
      static_cast<int32_t>(FLAGS_::kIsFocused) |
      static_cast<int32_t>(FLAGS_::kHasEnabledState) |
      static_cast<int32_t>(FLAGS_::kIsEnabled) |
      static_cast<int32_t>(FLAGS_::kIsInMutuallyExclusiveGroup) |
      static_cast<int32_t>(FLAGS_::kHasToggledState) |
      static_cast<int32_t>(FLAGS_::kIsToggled) |
      static_cast<int32_t>(FLAGS_::kHasToggledState) |
      static_cast<int32_t>(FLAGS_::kIsFocusable) |
      static_cast<int32_t>(FLAGS_::kIsSlider);

  static const int32_t SCROLLABLE_ACTIONS =
      static_cast<int32_t>(ACTIONS_::kScrollUp) |
      static_cast<int32_t>(ACTIONS_::kScrollDown) |
      static_cast<int32_t>(ACTIONS_::kScrollLeft) |
      static_cast<int32_t>(ACTIONS_::kScrollRight);

  void FlutterSetElementInfoProperties(
      ArkUI_AccessibilityElementInfo* elementInfoFromList,
      int64_t elementId);
  void FlutterSetElementInfoOperationActions(
      ArkUI_AccessibilityElementInfo* elementInfoFromList,
      std::string widget_type);
  void FlutterTreeToArkuiTree(
      ArkUI_AccessibilityElementInfoList* elementInfoList);

  flutter::SemanticsNode getFlutterRootSemanticsNode();
  std::string GetNodeComponentType(const flutter::SemanticsNode& node);
  flutter::SemanticsAction ArkuiActionsToFlutterActions(
      ArkUI_Accessibility_ActionType arkui_action);

  bool HasScrolled(const flutter::SemanticsNode& flutterNode);

  bool IsNodeFocusable(const flutter::SemanticsNode& flutterNode);
  bool IsNodeCheckable(flutter::SemanticsNode flutterNode);
  bool IsNodeChecked(flutter::SemanticsNode flutterNode);
  bool IsNodeSelected(flutter::SemanticsNode flutterNode);
  bool IsNodeClickable(flutter::SemanticsNode flutterNode);
  bool IsNodeScrollable(flutter::SemanticsNode flutterNode);
  bool IsNodePassword(flutter::SemanticsNode flutterNode);
  bool IsNodeVisible(flutter::SemanticsNode flutterNode);
  bool IsNodeEnabled(flutter::SemanticsNode flutterNode);
  bool IsNodeHasLongPress(flutter::SemanticsNode flutterNode);

  bool IsTextField(flutter::SemanticsNode flutterNode);
  bool IsSlider(flutter::SemanticsNode flutterNode);
  bool IsScrollableWidget(flutter::SemanticsNode flutterNode);
  void PerformSetText(flutter::SemanticsNode flutterNode,
                      ArkUI_Accessibility_ActionType action,
                      ArkUI_AccessibilityActionArguments* actionArguments);
  void PerformSelectText(flutter::SemanticsNode flutterNode,
                         ArkUI_Accessibility_ActionType action,
                         ArkUI_AccessibilityActionArguments* actionArguments);

  void AddRouteNodes(std::vector<flutter::SemanticsNode> edges,
                     flutter::SemanticsNode node);
  std::string GetRouteName(flutter::SemanticsNode node);
  void onWindowNameChange(flutter::SemanticsNode route);
  void removeSemanticsNode(flutter::SemanticsNode nodeToBeRemoved);

  void GetSemanticsNodeDebugInfo(flutter::SemanticsNode node);
  void GetSemanticsFlagsDebugInfo(flutter::SemanticsNode node);
  void GetCustomActionDebugInfo(
      flutter::CustomAccessibilityAction customAccessibilityAction);

  void PageStateUpdate(int64_t elementId);
  void RequestFocusWhenPageUpdate();

  bool Contains(const std::string& source, const std::string& target);
};

}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
