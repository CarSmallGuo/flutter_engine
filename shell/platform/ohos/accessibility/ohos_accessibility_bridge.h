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

  int32_t ExecuteAccessibilityAction(int64_t elementId, int64_t action, int64_t  actionArguments, int32_t requestId);

  

 private:
  std::shared_ptr<OhosAccessibilityManager> ax_manager_;
  std::unordered_map<int32_t, flutter::SemanticsNode> flutterSemanticsTree_;
  std::unordered_map<int32_t, flutter::CustomAccessibilityAction> actions_mp_;
  static const int32_t ROOT_NODE_ID = 0;
  int32_t previousRouteId = ROOT_NODE_ID;

  // A Java/Android cached representation of the Flutter app's navigation stack.
  // The Flutter navigation stack is tracked so that accessibility announcements
  // can be made during Flutter's navigation changes.
  std::vector<int32_t> flutterNavigationStack;

  flutter::SemanticsNode getRootSemanticsNode();


  // native os interfaces

  /**
   * Informs the TalkBack user about window name changes.
   * it creates a {@link AccessibilityEvent#TYPE_WINDOW_STATE_CHANGED} and sends
   * the event to Android's accessibility system. In both cases, TalkBack
   * announces the label of the route and re-addjusts the accessibility focus.
   *
   * <p>The given {@code route} should be a {@link SemanticsNode} that
   * represents a navigation route in the Flutter app.
   */
  void onWindowNameChange(flutter::SemanticsNode route);
  /**
   * Hook called just before a {@link SemanticsNode} is removed from the Android cache of Flutter's
   * semantics tree.
   */
  void removeSemanticsNode(flutter::SemanticsNode nodeToBeRemoved);
   
  void printTest(flutter::SemanticsNode node);
  void printTestActions(flutter::CustomAccessibilityAction customAccessibilityAction);
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
