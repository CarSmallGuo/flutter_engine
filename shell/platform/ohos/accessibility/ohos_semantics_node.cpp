/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_semantics_node.h"
#include "flutter/shell/platform/ohos/utils/ohos_utils.h"

namespace flutter {

SemanticsNodeExtend::SemanticsNodeExtend()
{   
    // load the needed accessibiltiy functon pointers
    this->DynamicLoadAccessibilityLibrary();
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
    int visible_num = 0;
    focusableInSubtree = IsFocusable();
    int last_visible_index = 0;
    int child_index = 0;
  
    std::vector<int64_t> exist_children;
    for (auto& childNode : childrenInTraversalOrderList) {
        if (!childNode->isExist) {
            // some child is not updated by UpdateSemantics.
            continue;
        }
        if (childNode->IsVisible()) {
            ++visible_num;
            last_visible_index = child_index;
        }
        if (childNode->isAccessibilityFocued) {
            scrollCurrentIndex = child_index;
        }
        child_index++;
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

        focusableInSubtree = focusableInSubtree || childNode->focusableInSubtree;
    }

    if (exist_children != existChildrenInTraversalOrder) {
        existChildrenInTraversalOrder = std::move(exist_children);
        childrenChanged = true;
    }

    // Update scroll info: it need the visible child node num.
    if (scrollChildren != 0 &&
        (visible_num != scrollEndIndex - scrollIndex + 1 || scrollChanged)) {
        scrollVisibleNum = visible_num;
        scrollVisibleEndIndex = last_visible_index;

        scrollEndIndex = scrollIndex + visible_num - 1;
        scrollChanged = true;
        if (scrollIndex + visible_num > scrollChildren) {
        FML_DLOG(WARNING)
            << "UpdateSelfRecurs+-ively -> Scroll index is out of bounds "
            << scrollIndex << " visibleNum" << visible_num << " children "
            << scrollChildren;
        }
        if (!childrenInHitTestOrder.size()) {
        FML_DLOG(WARNING) << "UpdateSelfRecursively -> Had scrollChildren but no "
                            "childrenInHitTestOrder";
        }
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
    /* Make sure the focusable node can be recognized */
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(
            elementInfo, IsFocusable() ? "yes" : "auto")
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

/**
 * Dynamically load the ArkUI accessiblity C-API interface to 
 * be compatible with API-12+ versions
 */
void SemanticsNodeExtend::DynamicLoadAccessibilityLibrary()
{
    DynamicLoadSetElemIntFunc();
    DynamicLoadSetElemStringFunc();
    DynamicLoadSetElemBoolFunc();
    OH_ArkUI_CreateAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityElementInfo);
    OH_ArkUI_DestoryAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadDestroyElemFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityElementInfo);
    OH_ArkUI_AccessibilityElementInfoSetOperationActions = 
        OhosAccessibilityDDL::DLLoadSetElemOperActionsFunc(ArkUIAccessibilityConstant::ARKUI_SET_ACTIONS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetOperationActions);
    OH_ArkUI_AccessibilityElementInfoSetScreenRect = 
        OhosAccessibilityDDL::DLLoadSetElemSreenRectFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCREEN_RECT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScreenRect);
    FML_DLOG(INFO) << "DynamicLoadAccessibilityLibrary is finished";
}

void SemanticsNodeExtend::DynamicLoadSetElemIntFunc()
{
    OH_ArkUI_AccessibilityElementInfoSetItemCount =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_ITEM_COUNT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetItemCount);
    OH_ArkUI_AccessibilityElementInfoSetStartItemIndex =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_START_ITEM_IDX);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetStartItemIndex);
    OH_ArkUI_AccessibilityElementInfoSetEndItemIndex =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_END_ITEM_IDX);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEndItemIndex);
    OH_ArkUI_AccessibilityElementInfoSetElementId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_NODE_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetElementId);
    OH_ArkUI_AccessibilityElementInfoSetParentId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_PARENT_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetParentId);
}

void SemanticsNodeExtend::DynamicLoadSetElemStringFunc()
{
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityText =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_TEXT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityText);
    OH_ArkUI_AccessibilityElementInfoSetContents =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_CONTENTS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetContents);
    OH_ArkUI_AccessibilityElementInfoSetHintText =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_HINT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetHintText);
    OH_ArkUI_AccessibilityElementInfoSetChildNodeIds =
        OhosAccessibilityDDL::DLLoadSetElemChildFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHILD_IDS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetChildNodeIds); 
    OH_ArkUI_AccessibilityElementInfoSetComponentType =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_COMPONENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetComponentType);
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_LEVEL);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel);
}

void SemanticsNodeExtend::DynamicLoadSetElemBoolFunc()
{
    OH_ArkUI_AccessibilityElementInfoSetEnabled =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_ENABLED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEnabled);
    OH_ArkUI_AccessibilityElementInfoSetClickable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CLICKABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetClickable);
    OH_ArkUI_AccessibilityElementInfoSetFocusable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_FOCUSABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetFocusable);
    OH_ArkUI_AccessibilityElementInfoSetFocused =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_FOCUSED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetFocused);
    OH_ArkUI_AccessibilityElementInfoSetIsPassword = 
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_IS_PASSWORD);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetIsPassword);
    OH_ArkUI_AccessibilityElementInfoSetCheckable = 
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHECKABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetCheckable);
    OH_ArkUI_AccessibilityElementInfoSetChecked = 
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHECKED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetChecked);
    OH_ArkUI_AccessibilityElementInfoSetVisible =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_VISIBLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetVisible);
    OH_ArkUI_AccessibilityElementInfoSetSelected =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_SELECTED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetSelected);
    OH_ArkUI_AccessibilityElementInfoSetScrollable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCROLLABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScrollable);
    OH_ArkUI_AccessibilityElementInfoSetEditable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_EDITABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEditable);
    OH_ArkUI_AccessibilityElementInfoSetLongClickable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_LONG_PRESS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetLongClickable);
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_GROUP);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup);
}

}