/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_semantics_node.h"
#include "ohos_accessibility_bridge.h"

namespace flutter {

void SemanticsNodeExtend::UpdatetSemanticsNodeExtend(flutter::SemanticsNode& node) {
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
            componentType == OHWidgetName::kOtherWidgetName) {
            componentType = OHWidgetName::kTextWidgetName;
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
    bool checkResult = globalRect.setBoundsCheck(points, 4);
    if (!checkResult) {
        FML_DLOG(WARNING) << "RelativeRectToScreenRect -> Transformed points can't make a rect ";
    }
    globalRect.setBounds(points, 4);

    SetAbsoluteRect(globalRect.left(),  globalRect.top(),
                    globalRect.right(), globalRect.bottom());
    rectChanged = true;
  }

  int previousNodeId = -1;
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
    // update children elementInfo
    childNode->previousNodeId = previousNodeId;
    previousNodeId = childNode->id;
    exist_children.emplace_back(childNode->id);
    childNode->UpdateRecursively(visitorId, globalTransform, needUpdate);
    childNode->SetElementInfoProperties();
  }

  if (exist_children != existChildrenInTraversalOrder) {
    existChildrenInTraversalOrder = std::move(exist_children);
    childrenChanged = true;
  }
}

void SemanticsNodeExtend::SetElementInfoProperties(
    ArkUI_AccessibilityElementInfo* elementInfo) {
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
      // scroll event need sent, so we don't make it false.
      // scrollChanged = false;
    }
    if (!hasInit || rectChanged) {
      FillElementInfoWithRect(elementInfo);
      rectChanged = false;
      hasUpdated = true;
    }
    if (!hasInit || selectChanged) {
      FillElementInfoWithSelect(elementInfo);
      hasUpdated = true;
      // select event need sent, so we don't make it false.
      // selectChanged = false;
    }
    hasInit = true;
}

void SemanticsNodeExtend::FillElementInfoWithId(
    ArkUI_AccessibilityElementInfo* elementInfo) {
  OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfo, id);
  OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfo, false);
}

void SemanticsNodeExtend::FillElementInfoWithProperty(
    ArkUI_AccessibilityElementInfo* elementInfo) {
  OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfo, operationActions.size(),operationActions.data());
  OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfo, componentType);
  OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfo, IsEnabled());
  OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfo, IsFocusable());
  OH_ArkUI_AccessibilityElementInfoSetFocused(elementInfo, IsFocused());
  OH_ArkUI_AccessibilityElementInfoSetSelected(elementInfo, IsSelected());
  OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfo, IsVisible());
  OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfo, IsClickable());
  OH_ArkUI_AccessibilityElementInfoSetLongClickable(elementInfo, IsHasLongPress());
  OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfo, IsCheckable());
  OH_ArkUI_AccessibilityElementInfoSetChecked(elementInfo, IsChecked());
  OH_ArkUI_AccessibilityElementInfoSetScrollable(elementInfo, IsScrollable());
  OH_ArkUI_AccessibilityElementInfoSetEditable(elementInfo, IsEditable());
  OH_ArkUI_AccessibilityElementInfoSetIsPassword(elementInfo, IsPassword());
}

void SemanticsNodeExtend::FillElementInfoWithContent(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
    std::string text = label + value;
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfo, text.c_str());
    OH_ArkUI_AccessibilityElementInfoSetContents(elementInfo, text.c_str());
    OH_ArkUI_AccessibilityElementInfoSetHintText(elementInfo, hint.c_str());
}

void SemanticsNodeExtend::FillElementInfoWithChildren(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
  // childrenInTraversalOrderList may less then childrenInTraversalOrder
  OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(
      elementInfo, existChildrenInTraversalOrder.size(),
      existChildrenInTraversalOrder.data());
}

void SemanticsNodeExtend::FillElementInfoWithParent(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
  if (id == 0) {
    OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfo, ARKUI_ACCESSIBILITY_ROOT_PARENT_ID);
  } else {
    OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfo, parentId);
  }
}

void SemanticsNodeExtend::FillElementInfoWithScroll(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
  if (scrollChildren > 0) {
    OH_ArkUI_AccessibilityElementInfoSetItemCount(elementInfo, scrollChildren);
    OH_ArkUI_AccessibilityElementInfoSetStartItemIndex(elementInfo, scrollIndex);
    // auto* currentFocusedNode = 
    //     OhosAccessibilityBridge::GetInstance()->accessibilityFocusedNode;
    // if (currentFocusedNode) {
    //     OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex(elementInfo, currentFocusedNode->id);
    // }
    OH_ArkUI_AccessibilityElementInfoSetEndItemIndex(elementInfo, scrollIndex + visibleChildrenNum - 1);
    // OH_ArkUI_AccessibilityElementInfoSetAccessibilityOffset(elementInfo, scrollPosition);
  }
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
  OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfo, &rect);
}

void SemanticsNodeExtend::FillElementInfoWithSelect(
    ArkUI_AccessibilityElementInfo* elementInfo)
{
  if (textSelectionBase != -1 && textSelectionExtent != -1) {
    OH_ArkUI_AccessibilityElementInfoSetSelectedTextStart(elementInfo,
                                                          textSelectionBase);
    OH_ArkUI_AccessibilityElementInfoSetSelectedTextEnd(elementInfo,
                                                        textSelectionExtent);
  }
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
      componentType = OHWidgetName::kRootWidgetName;
    } else if (HasFlag(FLAGS_::kIsButton)) {
      componentType = OHWidgetName::kButtonWidgetName;
    } else if (HasFlag(FLAGS_::kIsTextField)) {
      componentType = OHWidgetName::kEditTextWidgetName;
    } else if (HasFlag(FLAGS_::kIsMultiline)) {
      componentType = OHWidgetName::kEditMultilineTextWidgetName;
    } else if (HasFlag(FLAGS_::kIsLink)) {
      componentType = OHWidgetName::kLinkWidgetName;
    } else if (HasFlag(FLAGS_::kIsSlider) || HasAction(ACTIONS_::kIncrease) ||
               HasAction(ACTIONS_::kDecrease)) {
      componentType = OHWidgetName::kSliderWidgetName;
    } else if (HasFlag(FLAGS_::kIsHeader)) {
      componentType = OHWidgetName::kHeaderWidgetName;
    } else if (HasFlag(FLAGS_::kIsImage)) {
      componentType = OHWidgetName::kImageWidgetName;
    } else if (HasFlag(FLAGS_::kHasCheckedState)) {
      if (HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup)) {
        componentType = OHWidgetName::kRadioButtonWidgetName;
      } else {
        componentType = OHWidgetName::kCheckBoxWidgetName;
      }
    } else if (HasFlag(FLAGS_::kHasToggledState)) {
      componentType = OHWidgetName::kSwitchWidgetName;
    } else if (HasAction(ACTIONS_::kIncrease) || HasAction(ACTIONS_::kDecrease)) {
      componentType = OHWidgetName::kSeekbarWidgetName;
    } else if (HasFlag(FLAGS_::kHasImplicitScrolling)) {
      componentType = OHWidgetName::kScrollWidgetName;
    } else if ((!label.empty() || !tooltip.empty() || !hint.empty())) {
      componentType = OHWidgetName::kTextWidgetName;
    } else {
      componentType = OHWidgetName::kOtherWidgetName;
    }
}

}