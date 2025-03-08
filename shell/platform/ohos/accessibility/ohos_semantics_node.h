/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#ifndef OHOS_SEMANTICS_NODE_H
#define OHOS_SEMANTICS_NODE_H
#include <arkui/native_interface_accessibility.h>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "flutter/lib/ui/semantics/semantics_node.h"

namespace flutter {

typedef flutter::SemanticsFlags FLAGS_;
typedef flutter::SemanticsAction ACTIONS_;

class UIViewerName {
public:
    // these widget names will be shown on the UIViewer
    static constexpr const char* kRootWidgetName = "root";
    static constexpr const char* kOtherWidgetName = "View";
    static constexpr const char* kTextWidgetName = "Text";
    static constexpr const char* kEditTextWidgetName = "TextInput";
    static constexpr const char* kEditMultilineTextWidgetName = "TextArea";
    static constexpr const char* kImageWidgetName = "Image";
    static constexpr const char* kScrollWidgetName = "Scroll";
    static constexpr const char* kButtonWidgetName = "Button";
    static constexpr const char* kLinkWidgetName = "Link";
    static constexpr const char* kSliderWidgetName = "Slider";
    static constexpr const char* kHeaderWidgetName = "Header";
    static constexpr const char* kRadioButtonWidgetName = "Radio";
    static constexpr const char* kCheckBoxWidgetName = "Checkbox";
    static constexpr const char* kSwitchWidgetName = "Toggle";
    static constexpr const char* kSeekbarWidgetName = "SeekBar";
};
struct AbsoluteRect {
    float left;
    float top;
    float right;
    float bottom;
    static constexpr AbsoluteRect MakeEmpty() {
        return AbsoluteRect{0.0, 0.0, 0.0, 0.0};
    }
    static constexpr AbsoluteRect MakeRect(
        float left, float top, float right, float bottom) {
        return AbsoluteRect{left, top, right, bottom};
    }
};
struct SemanticsNodeExtend : flutter::SemanticsNode {

    const int32_t ARKUI_ACCESSIBILITY_ROOT_PARENT_ID = -2100000;

    static constexpr int32_t kFocusableFlags =
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

    static constexpr int32_t kScrollableAction =
        static_cast<int32_t>(ACTIONS_::kScrollLeft) |
        static_cast<int32_t>(ACTIONS_::kScrollRight) |
        static_cast<int32_t>(ACTIONS_::kScrollUp) |
        static_cast<int32_t>(ACTIONS_::kScrollDown);

    bool hasUpdated = false;
    bool hasInit = false;
    bool childrenChanged = false;
    bool scrollChanged = false;
    bool selectChanged = false;
    bool flagChanged = false;
    bool actionChanged = false;
    bool propertyChanged = false;
    bool contentChanged = false;
    bool rectChanged = false;
    bool parentChanged = false;
    bool idChanged = false;
    bool isExist = false;
    
    const char* componentType = UIViewerName::kOtherWidgetName;
    SkM44 globalTransform = SkM44{};
    AbsoluteRect absoluteRect = AbsoluteRect::MakeEmpty();
    int32_t parentId = 0;
    bool hadPreviousConfig = false;
    int32_t previousNodeId = -1;
    int32_t previousFlags = 0;
    int32_t previousActions = 0;
    int32_t previousTextSelectionBase = -1;
    int32_t previousTextSelectionExtent = -1;
    double previousScrollPosition = std::nan("");
    double previousScrollExtentMax = std::nan("");
    double previousScrollExtentMin = std::nan("");
    std::string previousValue;
    std::string previousLabel;

    int32_t scrollEndIndex = 0;
    SemanticsNodeExtend* accessibilityFocusedNode = nullptr;
    SemanticsNodeExtend* parentNode = nullptr;
    SemanticsNodeExtend* previousNode = nullptr;
    SemanticsNodeExtend* nextNode = nullptr;
    std::vector<int64_t> existChildrenInTraversalOrder;
    std::vector<SemanticsNodeExtend*> childrenInTraversalOrderList;
    std::vector<ArkUI_AccessibleAction> operationActions;
    ArkUI_AccessibilityElementInfo* elementInfo = nullptr;

    SemanticsNodeExtend();
    ~SemanticsNodeExtend();

    bool HasPrevAction(SemanticsAction action) const {
        return (previousActions & this->actions) != 0;
    }
    bool HasPrevFlag(SemanticsFlags flag) const {
        return (previousFlags & this->flags) != 0;
    }

    void UpdateRecursively(std::unordered_set<int32_t>& visitorId,
                           SkM44& fatherTransform, bool needUpdate);
    void UpdatetSemanticsNodeExtend(flutter::SemanticsNode& node);

    void SetAbsoluteRect(float left, float top, float right, float bottom) {
        absoluteRect.left = left;
        absoluteRect.top = top;
        absoluteRect.right = right;
        absoluteRect.bottom = bottom;
    }

    void UpdateComponetType();
    void ObtainElementInfoOperationActions();

    void SetElementInfoProperties(ArkUI_AccessibilityElementInfo* elementInfo);
    void SetElementInfoProperties();
    void FillElementInfoWithId(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithProperty(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithContent(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithChildren(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithScroll(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithRect(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithSelect(ArkUI_AccessibilityElementInfo* elementInfo);
    void FillElementInfoWithParent(ArkUI_AccessibilityElementInfo* elementInfo);

    bool IsTextField() { return HasFlag(FLAGS_::kIsTextField); }
    bool IsEditable() { return IsTextField() && !HasFlag(FLAGS_::kIsReadOnly); }
    bool IsSlider() { return HasFlag(FLAGS_::kIsSlider); }
    bool IsVisible() { return !HasFlag(FLAGS_::kIsHidden); }
    bool IsCheckable() {
        return HasFlag(FLAGS_::kHasCheckedState) ||
               HasFlag(FLAGS_::kHasToggledState);
    }
    bool IsChecked() {
        return HasFlag(FLAGS_::kIsChecked) || HasFlag(FLAGS_::kIsToggled);
    }
    bool IsSelected() { return HasFlag(FLAGS_::kIsSelected); }
    bool IsPassword() {
        return HasFlag(FLAGS_::kIsTextField) && HasFlag(FLAGS_::kIsObscured);
    }
    bool IsEnabled() {
        return !HasFlag(FLAGS_::kHasEnabledState) || HasFlag(FLAGS_::kIsEnabled);
    }
    bool IsClickable() { return HasAction(ACTIONS_::kTap); }
    bool IsHasLongPress() { return HasAction(ACTIONS_::kLongPress); }
    bool HasScrolled() {
        return !std::isnan(scrollPosition) &&
               !std::isnan(previousScrollPosition) &&
               previousScrollPosition != scrollPosition;
    }
    bool HasChangedLabel() {
        if (label.empty() && previousLabel.empty()) {
            return false;
        }
        return label.empty() || previousLabel.empty() || label != previousLabel;
    }
    bool IsFocusable() {
        if (HasFlag(FLAGS_::kScopesRoute)) {
        return false;
        }
        if (HasFlag(FLAGS_::kIsFocusable)) {
        return true;
        }
        if (IsPlatformViewNode()) {
        return true;
        }
        if ((flags & kFocusableFlags) != 0) {
        return true;
        }
        if ((actions & ~kScrollableAction) != 0) {
        return true;
        }
        return !label.empty() || !value.empty() || !hint.empty();
    }
    bool IsFocused() { return HasFlag(FLAGS_::kIsFocused); }
    bool IsScrollable() {
        return HasAction(ACTIONS_::kScrollLeft) || 
               HasAction(ACTIONS_::kScrollRight) ||
               HasAction(ACTIONS_::kScrollUp) || 
               HasAction(ACTIONS_::kScrollDown);
    }
    bool IsShowOnScreen() {
        return HasAction(ACTIONS_::kShowOnScreen);
    }
};

}
#endif