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

#include "flutter/fml/log_level.h"
// #include "flutter/fml/macros.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
// #include "flutter/shell/platform/common/accessibility_bridge.h"
#include "flutter/lib/ui/semantics/custom_accessibility_action.h"
// #include "flutter/shell/platform/ohos/accessibility/native_interface_accessibility.h"

namespace flutter {

typedef flutter::SemanticsFlags FLAGS_;
typedef flutter::SemanticsAction ACTIONS_;

/**
 * flutter和ohos的无障碍服务桥接
 */
class OhosAccessibilityBridge {
 private:
  OhosAccessibilityBridge();

 public:
   ~OhosAccessibilityBridge();
  static OhosAccessibilityBridge& GetInstance();

  void announce(std::unique_ptr<char[]>& message);

  void updateSemantics(flutter::SemanticsNodeUpdates update,
                       flutter::CustomAccessibilityActionUpdates actions);
  // obtain the flutter semnatics node
  flutter::SemanticsNode getOrCreateSemanticsNode(int32_t id);

  void performAction(int32_t virtualViewId, int32_t inputAction);

 private:
  std::unordered_map<int32_t, flutter::SemanticsNode> flutterSemanticsTree_;
  std::unordered_map<int32_t, flutter::CustomAccessibilityAction> actions_mp_;

  flutter::SemanticsNode semanticsNode_;
  // flutter::SemanticsFlags flags_;

  flutter::SemanticsNode getRootSemanticsNode();
  int32_t convertToInt32(flutter::SemanticsAction inputAction);

  // native os interfaces
  int32_t FindFocusedAccessibilityNode(int64_t elementId,
                                       int32_t focusType,
                                       int32_t requestId,
                                       int32_t elementinfo);
  int32_t FindNextFocusAccessibilityNode(int64_t elementId,
                                         int32_t direction,
                                         int32_t requestId,
                                         int32_t elementList);
                                      
};

class ArkUI_AccessibilityElementInfo {
 public:
  ArkUI_AccessibilityElementInfo() = default;
  ~ArkUI_AccessibilityElementInfo() = default;
  void createAccessibilityElementInfo(int vid);
};
/**
struct SemanticsNode {
  SemanticsNode();

  SemanticsNode(const SemanticsNode& other);

  ~SemanticsNode();

  bool HasAction(SemanticsAction action) const;
  bool HasFlag(SemanticsFlags flag) const;

  // Whether this node is for embedded platform views.
  bool IsPlatformViewNode() const;

  int32_t id = 0;
  int32_t flags = 0;
  int32_t actions = 0;
  int32_t maxValueLength = -1;
  int32_t currentValueLength = -1;
  int32_t textSelectionBase = -1;
  int32_t textSelectionExtent = -1;
  int32_t platformViewId = -1;
  int32_t scrollChildren = 0;
  int32_t scrollIndex = 0;
  double scrollPosition = std::nan("");
  double scrollExtentMax = std::nan("");
  double scrollExtentMin = std::nan("");
  double elevation = 0.0;
  double thickness = 0.0;
  std::string label;
  StringAttributes labelAttributes;
  std::string hint;
  StringAttributes hintAttributes;
  std::string value;
  StringAttributes valueAttributes;
  std::string increasedValue;
  StringAttributes increasedValueAttributes;
  std::string decreasedValue;
  StringAttributes decreasedValueAttributes;
  std::string tooltip;
  int32_t textDirection = 0;  // 0=unknown, 1=rtl, 2=ltr

  SkRect rect = SkRect::MakeEmpty();  // Local space, relative to parent.
  SkM44 transform = SkM44{};          // Identity
  std::vector<int32_t> childrenInTraversalOrder;
  std::vector<int32_t> childrenInHitTestOrder;
  std::vector<int32_t> customAccessibilityActions;
};
*/

/**
 enum class SemanticsFlags : int32_t {
  kHasCheckedState = 1 << 0,
  kIsChecked = 1 << 1,
  kIsSelected = 1 << 2,
  kIsButton = 1 << 3,
  kIsTextField = 1 << 4,
  kIsFocused = 1 << 5,
  kHasEnabledState = 1 << 6,
  kIsEnabled = 1 << 7,
  kIsInMutuallyExclusiveGroup = 1 << 8,
  kIsHeader = 1 << 9,
  kIsObscured = 1 << 10,
  kScopesRoute = 1 << 11,
  kNamesRoute = 1 << 12,
  kIsHidden = 1 << 13,
  kIsImage = 1 << 14,
  kIsLiveRegion = 1 << 15,
  kHasToggledState = 1 << 16,
  kIsToggled = 1 << 17,
  kHasImplicitScrolling = 1 << 18,
  kIsMultiline = 1 << 19,
  kIsReadOnly = 1 << 20,
  kIsFocusable = 1 << 21,
  kIsLink = 1 << 22,
  kIsSlider = 1 << 23,
  kIsKeyboardKey = 1 << 24,
  kIsCheckStateMixed = 1 << 25,
};
 */

/**
 enum class SemanticsAction : int32_t {
  kTap = 1 << 0,
  kLongPress = 1 << 1,
  kScrollLeft = 1 << 2,
  kScrollRight = 1 << 3,
  kScrollUp = 1 << 4,
  kScrollDown = 1 << 5,
  kIncrease = 1 << 6,
  kDecrease = 1 << 7,
  kShowOnScreen = 1 << 8,
  kMoveCursorForwardByCharacter = 1 << 9,
  kMoveCursorBackwardByCharacter = 1 << 10,
  kSetSelection = 1 << 11,
  kCopy = 1 << 12,
  kCut = 1 << 13,
  kPaste = 1 << 14,
  kDidGainAccessibilityFocus = 1 << 15,
  kDidLoseAccessibilityFocus = 1 << 16,
  kCustomAction = 1 << 17,
  kDismiss = 1 << 18,
  kMoveCursorForwardByWord = 1 << 19,
  kMoveCursorBackwardByWord = 1 << 20,
  kSetText = 1 << 21,
};
 */
}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
