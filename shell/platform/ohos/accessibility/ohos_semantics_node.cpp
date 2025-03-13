/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_semantics_node.h"
#include "ohos_accessibility_bridge.h"

namespace flutter {

SemanticsNodeExtend::SemanticsNodeExtend()
{
    // each flutter node is mapped with an elementInfo
    this->elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();
}
SemanticsNodeExtend::~SemanticsNodeExtend()
{
    OH_ArkUI_DestoryAccessibilityElementInfo(elementInfo);
}

void SemanticsNodeExtend::UpdatetSemanticsNodeExtend(flutter::SemanticsNode& node)
{
    isExist = true;
    hadPreviousConfig = true;

    if (id != node.id) {
      id = node.id;
      idChanged = true;
    }

    // tooltip may use for componentType
    previousLabel = label;
    if (value != node.value || label != node.label || hint != node.hint ||
        tooltip != node.tooltip) {
        value = std::move(node.value);
        label = std::move(node.label);
        hint = std::move(node.hint);
        tooltip = std::move(node.tooltip);
        if ((!label.empty() || !tooltip.empty() || !hint.empty()) &&
            componentType == UIViewerName::kOtherWidgetName) {
            componentType = UIViewerName::kTextWidgetName;
        }
        contentChanged = true;
    }
  
    previousFlags = flags;
    if (flags != node.flags) {
        flags = node.flags;
        flagChanged = true;
    }
  
    // IsFocusable need check flag and content
    // We will add focus action in ActionsUpdate using IsFocusable.
    previousActions = actions;
    if (actions != node.actions) {
        actions = node.actions;
        actionChanged = true;
    }
  
    if (textSelectionBase != node.textSelectionBase ||
        textSelectionExtent != node.textSelectionExtent) {
        textSelectionBase = node.textSelectionBase;
        textSelectionExtent = node.textSelectionExtent;
        selectChanged = true;
    }
  
    previousScrollPosition = scrollPosition;
    if (scrollIndex != node.scrollIndex ||
        scrollChildren != node.scrollChildren) {
        scrollPosition = node.scrollPosition;
        scrollExtentMax = node.scrollExtentMax;
        scrollExtentMin = node.scrollExtentMin;
        scrollIndex = node.scrollIndex;
        scrollChildren = node.scrollChildren;
        scrollChanged = true;
        // we need visible children num to update info.
    }
  
    // childrenChanged is check in UpdateSelfRecursively
    // we need know which node is not exist
    childrenInTraversalOrder = std::move(node.childrenInTraversalOrder);
  
    rectChanged = ((rect != node.rect) || (transform != node.transform));
    if (rectChanged) {
        rect = node.rect;
        transform = node.transform;
    }
  
    if (actionChanged || flagChanged || contentChanged || id == 0) {
        UpdateComponetType();
        ObtainElementInfoOperationActions();
        propertyChanged = true;
    }
  
    maxValueLength = node.maxValueLength;
    currentValueLength = node.currentValueLength;
    platformViewId = node.platformViewId;
    elevation = node.elevation;
    thickness = node.thickness;
    valueAttributes = std::move(node.valueAttributes);
    labelAttributes = std::move(node.labelAttributes);
    hintAttributes = std::move(node.hintAttributes);
    increasedValue = std::move(node.increasedValue);
    increasedValueAttributes = std::move(node.increasedValueAttributes);
    decreasedValue = std::move(node.decreasedValue);
    decreasedValueAttributes = std::move(node.decreasedValueAttributes);
    textDirection = node.textDirection;
    childrenInHitTestOrder = std::move(node.childrenInHitTestOrder);
    customAccessibilityActions = std::move(node.customAccessibilityActions);
}

void SemanticsNodeExtend::UpdateRecursively(
    std::unordered_set<int32_t>& visitorId,
    SkM44& fatherTransform,
    bool needUpdate)
{
    visitorId.insert(this->id);
    if (rectChanged) { needUpdate = true; }
    if (needUpdate) {
        auto [left, top, right, bottom] = rect;
        globalTransform = SkM44(fatherTransform, transform);

        int pointsNum = 4;
        SkPoint points[4] = {
            SkPoint::Make(left, top),     // top-left point
            SkPoint::Make(right, top),    // top-right point
            SkPoint::Make(right, bottom), // bottom-right point
            SkPoint::Make(left, bottom)   // bottom-left point
        };

        for (auto& point : points) {
            SkV4 vector = globalTransform.map(point.x(), point.y(), 0, 1);
            point = SkPoint::Make(vector.x / vector.w, vector.y / vector.w);
        }

        SkRect globalRect;
        bool checkResult = globalRect.setBoundsCheck(points, pointsNum);
        if (!checkResult) {
            FML_DLOG(WARNING) << "RelativeRectToScreenRect -> Transformed points can't make a rect ";
        }
        globalRect.setBounds(points, pointsNum);

        SetAbsoluteRect(globalRect.left(),  globalRect.top(),
                        globalRect.right(), globalRect.bottom());
        rectChanged = true;
    }

    SemanticsNodeExtend* prevNode = nullptr;
    int32_t visibleChildrenNum = 0;
    std::vector<int64_t> exist_children;
    for (auto& childNode : childrenInTraversalOrderList) {
        if (!childNode->isExist) {
            // some child is not updated by UpdateSemantics.
            continue;
        }
        if (childNode->IsVisible()) {
            ++visibleChildrenNum;
        }
        // update parent elementInfo
        if (!childNode->parentNode || childNode->parentNode->id != id) {
            childNode->parentNode = this;
            childNode->parentId = this->id;
            childNode->parentChanged = true;
        }
        if (prevNode) {
            prevNode->nextNode = childNode;
        }
        childNode->previousNode = prevNode;
        childNode->nextNode = nullptr;
        prevNode = childNode;
      
        // update children elementInfo
        exist_children.emplace_back(childNode->id);
        childNode->UpdateRecursively(visitorId, globalTransform, needUpdate);
        childNode->SetElementInfoProperties();
    }
    // update scroll end index
    scrollEndIndex = scrollIndex + visibleChildrenNum - 1;

    if (exist_children != existChildrenInTraversalOrder) {
        existChildrenInTraversalOrder = std::move(exist_children);
        childrenChanged = true;
    }
}

void SemanticsNodeExtend::SetElementInfoProperties(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    if (!elementInfo) { return; }
    FillElementInfoWithId(elementInfo);
    FillElementInfoWithProperty(elementInfo);
    FillElementInfoWithContent(elementInfo);
    FillElementInfoWithChildren(elementInfo);
    FillElementInfoWithParent(elementInfo);
    FillElementInfoWithScroll(elementInfo);
    FillElementInfoWithRect(elementInfo);
    FillElementInfoWithSelect(elementInfo);
}

void SemanticsNodeExtend::SetElementInfoProperties() {
    if (!hasInit || idChanged) {
      FillElementInfoWithId(elementInfo);
      idChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || propertyChanged) {
      FillElementInfoWithProperty(elementInfo);
      propertyChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || contentChanged) {
      FillElementInfoWithContent(elementInfo);
      contentChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || childrenChanged) {
      FillElementInfoWithChildren(elementInfo);
      childrenChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || parentChanged) {
      FillElementInfoWithParent(elementInfo);
      parentChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || scrollChanged) {
      FillElementInfoWithScroll(elementInfo);
      hasUpdated = true;
    }
    if (!hasInit || rectChanged) {
      FillElementInfoWithRect(elementInfo);
      rectChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || selectChanged) {
      FillElementInfoWithSelect(elementInfo);
      hasUpdated = true;
    }
    hasInit = true;
}

void SemanticsNodeExtend::FillElementInfoWithId(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfo, id)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfo, false)
    );
}

void SemanticsNodeExtend::FillElementInfoWithProperty(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfo, componentType)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfo, IsEnabled())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfo, IsFocusable())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetFocused(elementInfo, IsFocused())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetSelected(elementInfo, IsSelected())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfo, IsVisible())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfo, IsClickable())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetLongClickable(elementInfo, IsHasLongPress())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfo, IsCheckable())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetChecked(elementInfo, IsChecked())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetScrollable(elementInfo, IsScrollable())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetEditable(elementInfo, IsEditable())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetIsPassword(elementInfo, IsPassword())
    );
    if (operationActions.empty()) { return; }
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetOperationActions(
            elementInfo, operationActions.size(), operationActions.data())
    );
}

void SemanticsNodeExtend::FillElementInfoWithContent(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    std::string text = label + value;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfo, text.c_str())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetContents(elementInfo, text.c_str())
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetHintText(elementInfo, hint.c_str())
    );
}

void SemanticsNodeExtend::FillElementInfoWithChildren(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    // childrenInTraversalOrderList may less then childrenInTraversalOrder
    if (existChildrenInTraversalOrder.empty()) { return; }
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(
            elementInfo, existChildrenInTraversalOrder.size(),
            existChildrenInTraversalOrder.data())
    );
}

void SemanticsNodeExtend::FillElementInfoWithParent(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetParentId(
            elementInfo, id != 0 ? parentId : ARKUI_ACCESSIBILITY_ROOT_PARENT_ID)
    );
}

void SemanticsNodeExtend::FillElementInfoWithScroll(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    if (scrollChildren <= 0) { return; }
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetItemCount(elementInfo, scrollChildren)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetStartItemIndex(elementInfo, scrollIndex)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetEndItemIndex(elementInfo, scrollEndIndex)
    );
}

void SemanticsNodeExtend::FillElementInfoWithRect(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    ArkUI_AccessibleRect rect = {
        static_cast<int32_t>(absoluteRect.left),
        static_cast<int32_t>(absoluteRect.top),
        static_cast<int32_t>(absoluteRect.right),
        static_cast<int32_t>(absoluteRect.bottom),
    };
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfo, &rect)
    );
}

void SemanticsNodeExtend::FillElementInfoWithSelect(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    if (textSelectionBase == -1 || textSelectionExtent == -1) { return; }
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetSelectedTextStart(elementInfo, textSelectionBase)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetSelectedTextEnd(elementInfo, textSelectionExtent)
    );
}

void SemanticsNodeExtend::ObtainElementInfoOperationActions()
{
    operationActions.clear();
    if (HasAction(ACTIONS_::kTap)) {
        operationActions.push_back(
            {ArkUI_Accessibility_ActionType::
                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK,
            "click action"});
    }
    if (HasAction(ACTIONS_::kLongPress)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK,
                            "longPress action"});
    }
    if (HasAction(ACTIONS_::kScrollUp) || 
        HasAction(ACTIONS_::kScrollLeft) ||
        HasAction(ACTIONS_::kIncrease)) {
        operationActions.push_back(
            {ArkUI_Accessibility_ActionType::
                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD,
            "scrollForward action"});
    }
    if (HasAction(ACTIONS_::kScrollDown) ||
        HasAction(ACTIONS_::kScrollRight) ||
        HasAction(ACTIONS_::kDecrease)) {
        operationActions.push_back(
            {ArkUI_Accessibility_ActionType::
                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD,
            "scrollBackward action"});
    }
    if (HasAction(ACTIONS_::kSetText)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                 ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT,
                             "setText action"});
    }
    if (HasAction(ACTIONS_::kSetSelection)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT,
                            "setSelection action"});
    }
    if (HasAction(ACTIONS_::kCopy)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY,
                            "copy action"});
    }
    if (HasAction(ACTIONS_::kCut)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT,
                            "cut action"});
    }
    if (HasAction(ACTIONS_::kPaste)) {
        operationActions.push_back({ArkUI_Accessibility_ActionType::
                                ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE,
                            "paste action"});
    }
    operationActions.push_back({
        ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS,
        "gainFocus action"});
    operationActions.push_back({
        ArkUI_Accessibility_ActionType::
        ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS,
        "clearFoucs action"});
}

void SemanticsNodeExtend::UpdateComponetType() {
    if (id == 0) {
        componentType = UIViewerName::kRootWidgetName;
    } else if (HasFlag(FLAGS_::kIsButton)) {
        componentType = UIViewerName::kButtonWidgetName;
    } else if (HasFlag(FLAGS_::kIsTextField)) {
        componentType = UIViewerName::kEditTextWidgetName;
    } else if (HasFlag(FLAGS_::kIsMultiline)) {
        componentType = UIViewerName::kEditMultilineTextWidgetName;
    } else if (HasFlag(FLAGS_::kIsLink)) {
        componentType = UIViewerName::kLinkWidgetName;
    } else if (HasFlag(FLAGS_::kIsSlider) || HasAction(ACTIONS_::kIncrease) ||
               HasAction(ACTIONS_::kDecrease)) {
        componentType = UIViewerName::kSliderWidgetName;
    } else if (HasFlag(FLAGS_::kIsHeader)) {
        componentType = UIViewerName::kHeaderWidgetName;
    } else if (HasFlag(FLAGS_::kIsImage)) {
        componentType = UIViewerName::kImageWidgetName;
    } else if (HasFlag(FLAGS_::kHasCheckedState)) {
      if (HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup)) {
          componentType = UIViewerName::kRadioButtonWidgetName;
      } else {
          componentType = UIViewerName::kCheckBoxWidgetName;
      }
    } else if (HasFlag(FLAGS_::kHasToggledState)) {
        componentType = UIViewerName::kSwitchWidgetName;
    } else if (HasAction(ACTIONS_::kIncrease) || HasAction(ACTIONS_::kDecrease)) {
        componentType = UIViewerName::kSeekbarWidgetName;
    } else if (HasFlag(FLAGS_::kHasImplicitScrolling)) {
        componentType = UIViewerName::kScrollWidgetName;
    } else if ((!label.empty() || !tooltip.empty() || !hint.empty())) {
        componentType = UIViewerName::kTextWidgetName;
    } else {
        componentType = UIViewerName::kOtherWidgetName;
    }
}

}