/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include "ohos_accessibility_bridge.h"
#include <limits>
#include <cstring>
#include <unordered_set>
#include "flutter/fml/logging.h"
#include "flutter/shell/platform/ohos/ohos_logging.h"
#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/platform/embedder/embedder.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include "flutter/shell/platform/ohos/ohos_xcomponent_adapter.h"

namespace flutter {

std::unique_ptr<OhosAccessibilityBridge> OhosAccessibilityBridge::bridgeInstance_ = nullptr;
/**
 * singleton instance of OhosAccessibilityBridge with thread-safe mode
 */
OhosAccessibilityBridge* OhosAccessibilityBridge::GetInstance() 
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        bridgeInstance_.reset(new OhosAccessibilityBridge());
    });
    return bridgeInstance_.get();
}

OhosAccessibilityBridge::OhosAccessibilityBridge()
: native_shell_holder_id_(0),
  isFlutterNavigated_(false),
  isTouchGuideOn_(false),
  isAccessibilityEnabled_(false)
{
    this->DynamicLoadAccessibilityLibrary();
}

/**
 * listening to the state change of ohos accessibility
 */
void OhosAccessibilityBridge::OnOhosAccessibilityStateChange(
    bool ohosAccessibilityEnabled, int64_t shellholderId)
{
    native_shell_holder_id_ = shellholderId;
    nativeAccessibilityChannel_ = std::make_shared<NativeAccessibilityChannel>();
    accessibilityFeatures_ = std::make_shared<OhosAccessibilityFeatures>();

    if (ohosAccessibilityEnabled) {
        isAccessibilityEnabled_ = ohosAccessibilityEnabled;
        nativeAccessibilityChannel_->OnOhosAccessibilityEnabled(native_shell_holder_id_);
    } else {
        accessibilityFeatures_->SetAccessibleNavigation(false, native_shell_holder_id_);
        nativeAccessibilityChannel_->OnOhosAccessibilityDisabled(native_shell_holder_id_);
    }
}

/**
 * update flutter semantics tree from the platform view
 */
void OhosAccessibilityBridge::UpdateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions)
{
    FML_DLOG(INFO) << "OhosAccessibilityBridge::UpdateSemantics()";
    std::vector<SemanticsNodeExtend> updatedFlutterNodes;

    // request rootNode when routes a new flutter page
    // on touch guide mode (screen reader is on)
    if (isFlutterNavigated_) {
        RequestFocusWhenPageUpdate(0);
        isFlutterNavigated_ = false;
    }

    // Traverse the updated semantic tree
    for (const auto& item : update) {
        const auto& node = item.second;

        auto nodeEx = UpdatetSemanticsNodeExtend(std::move(node));

        g_flutterSemanticsTree[nodeEx.id] = nodeEx;

        if (!IsNodeVisible(nodeEx)) { continue; }

        // add the nodes which have changed
        if (nodeEx.hadPreviousConfig) {
            updatedFlutterNodes.emplace_back(std::move(nodeEx));
            FML_DLOG(INFO) << "updatedFlutterNodes -> node.id=" << nodeEx.id;
        }
    }

    std::unordered_set<int32_t> visitedIds;
    // Iteratively update the essential properties of the nodes
    UpdateIteratively(visitedIds);

    FML_DLOG(INFO) << "before -> g_flutterSemanticsTree size: " << g_flutterSemanticsTree.size();
    // detele the remain useless nodes from the semantics tree
    for (auto it = g_flutterSemanticsTree.begin(); 
        it != g_flutterSemanticsTree.end();) {
        if (visitedIds.find(it->first) == visitedIds.end()) {
            FML_DLOG(INFO) << "g_flutterSemanticsTree delete node.id: " << it->first;
            it = g_flutterSemanticsTree.erase(it);
        } else {
            ++it;
        }
    }
    FML_DLOG(INFO) << "after -> g_flutterSemanticsTree size: " << g_flutterSemanticsTree.size();

    // if the screen reader starts (touch guide state is true),
    // sending the a11y event and draw the green rect 
    Flutter_SendAccessibilityAsyncEvent(
        0, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);
    
    // Traverse the updated nodes
    for (auto& nodeEx: updatedFlutterNodes) {

        if (HasScrolled(nodeEx)) { DoScroll(std::move(nodeEx)); }

        if (nodeEx.HasFlag(FLAGS_::kIsLiveRegion) && HasChangedLabel(nodeEx)) {
            FML_DLOG(INFO) << "liveRegion -> page content update, nodeEx.id=" << nodeEx.id;
            Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeEx.id),
                                                ArkUI_AccessibilityEventType::
                                                    ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);
        }
    }
}

void OhosAccessibilityBridge::DoScroll(SemanticsNodeExtend nodeEx)
{
    FML_DLOG(INFO) << "DoScroll -> node.id=" << nodeEx.id;
    auto* elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();

    FlutterSetElementInfoProperties(elementInfo, static_cast<int64_t>(nodeEx.id));
    FlutterScrollExecution(nodeEx, elementInfo); 

    // when screen reader is on and the touch guide is active,
    // double finger scroll with focused rect following in time
    if (isTouchGuideOn_) {
        Flutter_SendAccessibilityAsyncEvent(
            static_cast<int64_t>(accessibilityFocusedNode.id),
            ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_FOCUS_NODE_UPDATE);
    }

    OH_ArkUI_DestoryAccessibilityElementInfo(elementInfo);
    elementInfo = nullptr;
}

void OhosAccessibilityBridge::FlutterScrollExecution(
    const SemanticsNodeExtend& node,
    ArkUI_AccessibilityElementInfo* elementInfoFromList)
{
    if (node.scrollChildren > 0) {
        int32_t itemCount = node.scrollChildren;
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetItemCount(elementInfoFromList, itemCount)
        );
        int32_t startItemIndex = node.scrollIndex;
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetStartItemIndex(elementInfoFromList, startItemIndex)
        );
        // set current focused node index
        int32_t currItemIndex = accessibilityFocusedNode.id;
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex(elementInfoFromList, currItemIndex)
        );
        // count the current scroll node's visible children
        int visibleChildren = 0;
        // handle hidden children at the beginning and end of the list.
        for (const auto& childId : node.childrenInHitTestOrder) {
            auto childNode = GetFlutterSemanticsNode(childId);
            if (IsNodeVisible(childNode)) {
                visibleChildren += 1;
            }
        }
        int32_t endItemIndex = node.scrollIndex + visibleChildren - 1;
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEndItemIndex(elementInfoFromList, endItemIndex)
        );
    }
}

/**
 * when flutter page route to new page, 
 * the screen reader requests the root node
 */
void OhosAccessibilityBridge::RequestFocusWhenPageUpdate(int32_t requestFocusId)
{
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider();
    CHECK_NULL_PTR_RET_VOID(provider_, RequestFocusWhenPageUpdate);
    
    auto* reqFocusEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
    auto* elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();

    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetEventType(
            reqFocusEventInfo,
            ArkUI_AccessibilityEventType::
                ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_REQUEST_ACCESSIBILITY_FOCUS)
    );
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetRequestFocusId(reqFocusEventInfo, requestFocusId));
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetElementInfo(reqFocusEventInfo, elementInfo));

    auto callback = [](int32_t errorCode) {
        FML_DLOG(WARNING) << "PageStateUpdate callback-> errorCode =" << errorCode;
    };

    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, reqFocusEventInfo, callback);

    OH_ArkUI_DestoryAccessibilityEventInfo(reqFocusEventInfo);
    reqFocusEventInfo = nullptr;
    OH_ArkUI_DestoryAccessibilityElementInfo(elementInfo);
    elementInfo = nullptr;
}

/**
 * send the custom announcement message from dart side
 */
void OhosAccessibilityBridge::Announce(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("Announce -> message: %{public}s", message.get());
}

/**
 * tap the node with spercific id from dart side
 */
void OhosAccessibilityBridge::OnTap(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeId),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED);
    LOGD("OnTap -> nodeId: %{public}d", nodeId);
}

/**
 * longpress the node with spercific id from dart side
 */
void OhosAccessibilityBridge::OnLongPress(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeId),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED);
    LOGD("OnLongPress -> nodeId: %{public}d", nodeId);
}

/**
 * send the custom tooltip message from dart side
 */
void OhosAccessibilityBridge::OnTooltip(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(ROOT_NODE_ID),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_STATE_UPDATE);
    Flutter_SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("OnTooltip -> message: %{public}s", message.get());
}

SemanticsNodeExtend OhosAccessibilityBridge::GetFlutterSemanticsNode(
    int32_t id)
{
    auto it = g_flutterSemanticsTree.find(id);
    if (it !=  g_flutterSemanticsTree.end()) {
        return it->second;
    }
    return {};
}

int32_t OhosAccessibilityBridge::GetParentId(const SemanticsNodeExtend& node)
{
    return node.id != 0 ? node.parentId : ARKUI_ACCESSIBILITY_ROOT_PARENT_ID;
}

void OhosAccessibilityBridge::SetAbsoluteScreenRect(SemanticsNodeExtend& flutterNode,
                                                    float left,
                                                    float top,
                                                    float right,
                                                    float bottom)
{
    flutterNode.absoluteRect = {left, top, right, bottom};
    FML_DLOG(INFO) << "SetAbsoluteScreenRect -> id=" << flutterNode.id
                   << ", {" << left << ", " << top << ", " << right << ", "<< bottom << "> }";
}

const AbsoluteRect& OhosAccessibilityBridge::GetAbsoluteScreenRect(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.absoluteRect;
}

void OhosAccessibilityBridge::UpdateIteratively(
    std::unordered_set<int32_t>& visitedIds)
{
  std::queue<SemanticsNodeExtend> semanticsQue;

  auto rootNode = GetFlutterSemanticsNode(ROOT_NODE_ID);
  rootNode.globalTransform = rootNode.transform;
  rootNode.parentId = ARKUI_ACCESSIBILITY_ROOT_PARENT_ID;
  g_flutterSemanticsTree[rootNode.id] = rootNode;
  semanticsQue.push(rootNode);
  visitedIds.insert(rootNode.id);

  while (!semanticsQue.empty()) {
      auto currNode = semanticsQue.front();
      semanticsQue.pop();

      for (const auto& childId: currNode.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childId);
        childNode.parentId = currNode.id;
        childNode.globalTransform = currNode.globalTransform * childNode.transform;
        g_flutterSemanticsTree[childId] = childNode;
        semanticsQue.push(childNode);
        visitedIds.insert(childNode.id);
      }
  }
  
}

SkPoint OhosAccessibilityBridge::ApplyTransform(
    SkPoint& point, const SkM44& transform) {
  SkV4 vector = transform.map(point.x(), point.y(), 0, 1);
  return SkPoint::Make(vector.x / vector.w, vector.y / vector.w);
}

/**
 * convert local(relative) rect to global(absolut) rect
 */
void OhosAccessibilityBridge::RelativeRectToScreenRect(SemanticsNodeExtend& node)
{
    auto [left, top, right, bottom] = node.rect;
    SkM44 globalTransform = node.globalTransform;

    SkPoint points[4] = {
        SkPoint::Make(left, top),     // top-left point
        SkPoint::Make(right, top),    // top-right point
        SkPoint::Make(right, bottom), // bottom-right point
        SkPoint::Make(left, bottom)   // bottom-left point
    };

    for (auto& point : points) {
        point = ApplyTransform(point, globalTransform);
    }

    SkRect globalRect;
    bool checkResult = globalRect.setBoundsCheck(points, 4);
    if (!checkResult) {
        FML_DLOG(WARNING) << "RelativeRectToScreenRect -> Transformed points can't make a rect ";
    }
    globalRect.setBounds(points, 4);

    SetAbsoluteScreenRect(node, globalRect.left(),  globalRect.top(),
                                globalRect.right(), globalRect.bottom());
}

/**
 * set arkui elementinfo properties
 */
void OhosAccessibilityBridge::FlutterSetElementInfoOperationActions(
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    const SemanticsNodeExtend& node)
{
    int32_t actionTypeNum = 30; // declare an unreachable array length
    ArkUI_AccessibleAction actions[actionTypeNum];
    size_t idx = 0; // real length of array
    if (node.HasAction(ACTIONS_::kTap)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
        actions[idx++].description = "click action";
    }
    if (node.HasAction(ACTIONS_::kLongPress)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK;
        actions[idx++].description = "longClick action";
    }
    if (node.HasAction(ACTIONS_::kSetText)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT;
        actions[idx++].description = "setText action";
    }
    if (node.HasAction(ACTIONS_::kSetSelection)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT;
        actions[idx++].description = "setSelection action";
    }
    if (node.HasAction(ACTIONS_::kCopy)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY;
        actions[idx++].description = "copy action";
    }
    if (node.HasAction(ACTIONS_::kCut)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT;
        actions[idx++].description = "cut action";
    }
    if (node.HasAction(ACTIONS_::kPaste)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE;
        actions[idx++].description = "paste action";
    }
    if (node.HasAction(ACTIONS_::kScrollLeft) ||
        node.HasAction(ACTIONS_::kScrollUp) ||
        node.HasAction(ACTIONS_::kIncrease)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD;
        actions[idx++].description = "scrollForward action";
    } 
    if (node.HasAction(ACTIONS_::kScrollRight) ||
        node.HasAction(ACTIONS_::kScrollDown) ||
        node.HasAction(ACTIONS_::kDecrease)) {
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD;
        actions[idx++].description = "scrollBackward action";
    }
    actions[idx].actionType = ArkUI_Accessibility_ActionType::
    ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
    actions[idx++].description = "focus action";
    actions[idx].actionType = ArkUI_Accessibility_ActionType::
        ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
    actions[idx++].description = "clearFocus action";
    
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, idx, actions)
    );
}

/**
 * convert flutter node properties to arkui node properties
 */
void OhosAccessibilityBridge::FlutterSetElementInfoProperties(
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    int64_t elementId)
{
    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId > 0 ? elementId : 0));

    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfoFromList, flutterNode.id)
    );

    ArkUI_AccessibleRect rect;
    if (elementId < 1) { // root node
        int32_t left = flutterNode.rect.fLeft;
        int32_t top = flutterNode.rect.fTop;
        int32_t right = flutterNode.rect.fRight;
        int32_t bottom = flutterNode.rect.fBottom;
        SetAbsoluteScreenRect(flutterNode, left, top, right, bottom);
        rect = {static_cast<int32_t>(left), static_cast<int32_t>(top),
                static_cast<int32_t>(right), static_cast<int32_t>(bottom)};
    } else { // other nodes
        RelativeRectToScreenRect(flutterNode);
        auto [left, top, right, bottom] = GetAbsoluteScreenRect(flutterNode);
        rect = {static_cast<int32_t>(left), static_cast<int32_t>(top),
                static_cast<int32_t>(right), static_cast<int32_t>(bottom)};
    }
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfoFromList, &rect)
    );
    
    FlutterSetElementInfoOperationActions(elementInfoFromList, flutterNode);
    
    int32_t parentId = GetParentId(flutterNode);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfoFromList, parentId)
    );
    FML_DLOG(INFO) << "SetElementInfo GetParentId = " << parentId;
    
    std::string text = flutterNode.label + flutterNode.value;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfoFromList, text.c_str())
    );
    FML_DLOG(INFO) << "SetElementInfo SetAccessibilityText = " << text;

    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetContents(elementInfoFromList, text.c_str())
    );

    std::string hint = flutterNode.hint;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetHintText(elementInfoFromList, hint.c_str())
    );

    // set current node's children ids
    int32_t childCount = flutterNode.childrenInTraversalOrder.size();
    if (childCount > 0) {
        auto childrenIdsVec = flutterNode.childrenInTraversalOrder;
        int64_t childNodeIds[childCount];
        for (int32_t i = 0; i < childCount; i++) {
            childNodeIds[i] = static_cast<int64_t>(childrenIdsVec[i]);
            FML_DLOG(INFO) << "SetElementInfo -> elementid=" << elementId
                           << " childCount=" << childCount
                           << " childNodeIds=" << childNodeIds[i];
        }
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(elementInfoFromList, childCount, childNodeIds)
        );
    }
    if (IsNodeEnabled(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetEnabled -> true";
    }
    if (IsNodeClickable(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetClickable -> true";
    }
    if (IsNodeFocusable(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetFocusable -> true";
    }
    if (IsNodeFocused(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused -> true";
    }
    if (IsNodePassword(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetIsPassword(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetIsPassword -> true";
    }
    if (IsNodeCheckable(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetCheckable -> true";
    }
    if (IsNodeChecked(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetChecked(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetChecked -> true";
    }
    if (IsNodeVisible(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetVisible -> true";
    }
    if (IsNodeSelected(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetSelected(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetSelected -> true";
    }
    if (IsNodeScrollable(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetScrollable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetScrollable -> true";
    }
    if (IsTextField(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEditable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetEditable -> true";
    }
    if (IsNodeHasLongPress(flutterNode)) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetLongClickable(elementInfoFromList, true)
        );
        FML_DLOG(INFO)
            << "flutterNode.id=" << flutterNode.id
            << " OH_ArkUI_AccessibilityElementInfoSetLongClickable -> true";
    }

    std::string componentTypeName = GetNodeComponentType(flutterNode);
    FML_DLOG(INFO) << "SetElementInfo componentTypeName = "
                    << componentTypeName;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetComponentType(
            elementInfoFromList, elementId < 1 ? ROOT_WIDGET_NAME : componentTypeName.c_str())
    );
    FML_DLOG(INFO) << "SetElementInfo SetComponentType: "
                   << componentTypeName;

    /**
     * 无障碍重要性，用于控制某个组件是否可被无障碍辅助服务所识别。支持的值为（默认值：“auto”）：
     * “auto”：根据组件不同会转换为“yes”或者“no”
     * “yes”：当前组件可被无障碍辅助服务所识别
     * “no”：当前组件不可被无障碍辅助服务所识别
     * “no-hide-descendants”：当前组件及其所有子组件不可被无障碍辅助服务所识别
     */
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(
            elementInfoFromList, componentTypeName != OTHER_WIDGET_NAME ? "yes" : "no"); 
    );
    // 无障碍组，设置为true时表示该组件及其所有子组件为一整个可以选中的组件，无障碍服务将不再关注其子组件内容。默认值：false
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfoFromList, false);
    );
}

/**
 * build ArkUI elementInfo tree by levelOrderTraversal way
 * to support DevEco Testing (UITest, UIViewer and Hypium) 
 */
void OhosAccessibilityBridge::BuildArkUISemanticsTree(
    int64_t elementId,
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    FlutterSetElementInfoProperties(elementInfoFromList, elementId);
    std::queue<SemanticsNodeExtend> semanticsQue;

    auto root = GetFlutterSemanticsNode(static_cast<int32_t>(elementId));
    semanticsQue.push(root);

    while (!semanticsQue.empty()) {
        auto currNode = semanticsQue.front();
        semanticsQue.pop();

        auto* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
        // set current flutter node properties
        FlutterSetElementInfoProperties(newElementInfo, static_cast<int64_t>(currNode.id));
        
        for (const auto& childId: currNode.childrenInTraversalOrder) {
          auto childNode = GetFlutterSemanticsNode(childId);
          semanticsQue.push(childNode);
        }
    }
}

/**
 * Called to obtain element information based on a specified node.
 * (OH ArkUI accessibility callback function)
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosById(
    int64_t elementId,
    ArkUI_AccessibilitySearchMode mode,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    FML_DLOG(INFO)
        << "#### FindAccessibilityNodeInfosById input-params ####: elementId = "
        << elementId << " mode=" << mode;
    CHECK_NULL_PTR_WITH_RET(elementList, FindAccessibilityNodeInfosById);

    if (g_flutterSemanticsTree.empty()) {
        FML_DLOG(ERROR)
            << "FindAccessibilityNodeInfosById g_flutterSemanticsTree is null";
        return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
    }
    
    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId));

    auto* elementInfoFromList = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
    CHECK_NULL_PTR_WITH_RET(elementInfoFromList, OH_ArkUI_AddAndGetAccessibilityElementInfo);

    switch(mode) {
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT:
            /** Search for current nodes. (mode = 0) */
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS:
            /** Search for parent nodes. (mode = 1) */
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS:
            /** Search for sibling nodes. (mode = 2) */
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CHILDREN:
            /** Search for child nodes at the next level. (mode = 4) */
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN:
            /** Search for all child nodes. (mode = 8) */
            BuildArkUISemanticsTree(elementId, elementInfoFromList, elementList);
            break;
        default:
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
    }
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

/**
 * Called to perform accessibility actions based on the specified node.
 * (OH ArkUI accessibility callback function)
 */
int32_t OhosAccessibilityBridge::ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments,
    int32_t requestId)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction input-params-> elementId="
                   << elementId << " action=" << action;
    CHECK_NULL_PTR_WITH_RET(actionArguments, ExecuteAccessibilityAction);

    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId));

    // Display obscured flutter nodes on the screen when scrolling
    // ArkUI accessibility service does not support this feature
    PerformShowOnScreenAction(flutterNode);

    switch (action) {
        /** Response to a click. 16 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
            PerformClickAction(elementId, action, flutterNode);
            break;
        /** Response to a long click. 32 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
            PerformLongClickAction(elementId, action, flutterNode);
            break;
        /** Accessibility focus acquisition. 64 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
            PerformGainFocusnAction(elementId, action, flutterNode);
            break;
        /** Accessibility focus clearance. 128 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
            PerformClearFocusAction(elementId, action, flutterNode);
            break;
        /** Forward scroll action. 256 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
            PerformScrollUpAction(elementId, action, flutterNode);
            break;
        /** Backward scroll action. 512 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
            PerformScrollDownAction(elementId, action, flutterNode);
            break;
        /** Copy, Paste, Cut action for text content. 1024, 2048, 4096*/
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE:
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
            PerformClipboardAction(elementId, action);
            break;
        /** Text selection action, requiring the setting of <b>selectTextBegin</b>,
         * <b>TextEnd</b>, and <b>TextInForward</b> parameters to select a text
         * segment in the text box. 8192 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT:
            PerformSelectText(flutterNode, action, actionArguments);
            break;
        /** Text content setting action. 16384 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT:
            PerformSetText(flutterNode, action, actionArguments);
            break;
        /** Cursor position setting action. 1048576 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_CURSOR_POSITION:
            PerformSetCursorPosition(flutterNode, action, actionArguments);
            break;
        /** Invalid action. 0 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_INVALID:
            PerformInvalidAction(elementId, action, flutterNode);
            break;
        default:
            /** custom semantics action */
            PerformCustomAction(flutterNode, action, actionArguments);
    }
    FML_DLOG(INFO) << "--- ExecuteAccessibilityAction is end ---";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

SemanticsNodeExtend OhosAccessibilityBridge::UpdatetSemanticsNodeExtend(
    flutter::SemanticsNode node)
{
    SemanticsNodeExtend nodeEx = SemanticsNodeExtend();

    if (!g_flutterSemanticsTree.empty()) {
        auto prevNode = GetFlutterSemanticsNode(node.id);
        nodeEx.hadPreviousConfig = true;
        nodeEx.parentId = prevNode.parentId;
        nodeEx.previousFlags = prevNode.flags;
        nodeEx.previousActions = prevNode.actions;
        nodeEx.previousTextSelectionBase = prevNode.textSelectionBase;
        nodeEx.previousTextSelectionExtent = prevNode.textSelectionExtent;
        nodeEx.previousScrollPosition = prevNode.scrollPosition;
        nodeEx.previousScrollExtentMax = prevNode.scrollExtentMax;
        nodeEx.previousScrollExtentMin = prevNode.scrollExtentMin;
        nodeEx.previousValue = std::move(prevNode.value);
        nodeEx.previousLabel = std::move(prevNode.label);
    }
    nodeEx.id = node.id;
    nodeEx.flags = node.flags;
    nodeEx.actions = node.actions;
    nodeEx.maxValueLength = node.maxValueLength;
    nodeEx.currentValueLength = node.currentValueLength;
    nodeEx.textSelectionBase = node.textSelectionBase;
    nodeEx.textSelectionExtent = node.textSelectionExtent;
    nodeEx.platformViewId = node.platformViewId;
    nodeEx.scrollChildren = node.scrollChildren;
    nodeEx.scrollIndex = node.scrollIndex;
    nodeEx.scrollPosition = node.scrollPosition;
    nodeEx.scrollExtentMax = node.scrollExtentMax;
    nodeEx.scrollExtentMin = node.scrollExtentMin;
    nodeEx.elevation = node.elevation;
    nodeEx.thickness = node.thickness;
    nodeEx.label = std::move(node.label);
    nodeEx.labelAttributes = std::move(node.labelAttributes);
    nodeEx.hint = std::move(node.hint);
    nodeEx.hintAttributes = std::move(node.hintAttributes);
    nodeEx.value = std::move(node.value);
    nodeEx.valueAttributes = std::move(node.valueAttributes);
    nodeEx.increasedValue = std::move(node.increasedValue);
    nodeEx.increasedValueAttributes = std::move(node.increasedValueAttributes);
    nodeEx.decreasedValue = std::move(node.decreasedValue);
    nodeEx.decreasedValueAttributes = std::move(node.decreasedValueAttributes);
    nodeEx.tooltip = std::move(node.tooltip);
    nodeEx.textDirection = node.textDirection;
    nodeEx.rect = std::move(node.rect);
    nodeEx.transform = std::move(node.transform);
    nodeEx.childrenInTraversalOrder = std::move(node.childrenInTraversalOrder);
    nodeEx.childrenInHitTestOrder = std::move(node.childrenInHitTestOrder);
    nodeEx.customAccessibilityActions = std::move(node.customAccessibilityActions);
    return nodeEx;
}

void OhosAccessibilityBridge::DispatchSemanticsAction(
    int32_t id,
    flutter::SemanticsAction action,
    fml::MallocMapping args)
{
    nativeAccessibilityChannel_->DispatchSemanticsAction(native_shell_holder_id_,
                                                         id,
                                                         action,
                                                         std::move(args));
}

void OhosAccessibilityBridge::PerformClickAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    const SemanticsNodeExtend& flutterNode)
{
    /** Click event, sent after the UI component responds. 1 */
    auto clickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED;
    Flutter_SendAccessibilityAsyncEvent(elementId, clickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: click(" << action
                   << ")" << " event: click(" << clickEventType << ")";
    auto flutterTapAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterTapAction, {});
}

void OhosAccessibilityBridge::PerformLongClickAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    const SemanticsNodeExtend& flutterNode)
{
    /** Long click event, sent after the UI component responds. 2 */
    auto longClickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED;
    Flutter_SendAccessibilityAsyncEvent(elementId, longClickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: longclick("
                   << action << ")" << " event: longclick("
                   << longClickEventType << ")";
    auto flutterLongPressAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterLongPressAction, {});
}

void OhosAccessibilityBridge::PerformGainFocusnAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    const SemanticsNodeExtend& flutterNode)
{
    // update the accessibility focused node
    accessibilityFocusedNode = flutterNode;

    auto flutterGainFocusAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId),
                            flutterGainFocusAction, {});
    // Accessibility focus event, sent after the UI component responds. 32768
    auto focusEventType = ArkUI_AccessibilityEventType::
        ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUSED;
    Flutter_SendAccessibilityAsyncEvent(elementId, focusEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: focus(" << action
                   << ")" << " event: focus(" << focusEventType << ")";

    if (flutterNode.HasAction(ACTIONS_::kIncrease) ||
        flutterNode.HasAction(ACTIONS_::kDecrease)) {
        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::
                ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
    }
}

void OhosAccessibilityBridge::PerformClearFocusAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    const SemanticsNodeExtend& flutterNode)
{
    auto flutterLoseFocusAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterLoseFocusAction, {});
    /** Accessibility focus cleared event, sent after the UI component responds. 65536 */
    auto clearFocusEventType = ArkUI_AccessibilityEventType::
        ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED;
    Flutter_SendAccessibilityAsyncEvent(elementId, clearFocusEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: clearfocus("
                   << action << ")" << " event: clearfocus("
                   << clearFocusEventType << ")";
}

void OhosAccessibilityBridge::PerformScrollUpAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend& flutterNode)
{
    // flutter scroll forward with different situations
    if (flutterNode.HasAction(ACTIONS_::kScrollUp)) {
        auto flutterScrollUpAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterScrollUpAction, {});
    } else if (flutterNode.HasAction(ACTIONS_::kScrollLeft)) {
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kScrollLeft, {});
    } else if (flutterNode.HasAction(ACTIONS_::kIncrease)) {
        flutterNode.value = flutterNode.increasedValue;
        flutterNode.valueAttributes = flutterNode.increasedValueAttributes;
        
        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kIncrease, {});
    }
}

void OhosAccessibilityBridge::PerformScrollDownAction(
    int64_t elementId, 
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend& flutterNode)
{
    // flutter scroll down with different situations
    if (flutterNode.HasAction(ACTIONS_::kScrollDown)) {
        auto flutterScrollDownAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterScrollDownAction, {});
    } else if (flutterNode.HasAction(ACTIONS_::kScrollRight)) {
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kScrollRight, {});
    } else if (flutterNode.HasAction(ACTIONS_::kDecrease)) {
        flutterNode.value = flutterNode.decreasedValue;
        flutterNode.valueAttributes = flutterNode.decreasedValueAttributes;

        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kDecrease, {});
    }
}

void OhosAccessibilityBridge::PerformClipboardAction(
    int64_t elementId,
    const ArkUI_Accessibility_ActionType& action)
{
    if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: copy(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kCopy, {});
    } else if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: paste(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kPaste, {});
    } else {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: cut(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kCut, {});
    }
}

void OhosAccessibilityBridge::PerformInvalidAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    const SemanticsNodeExtend& flutterNode)
{
    /** Invalid event. 0 */
    ArkUI_AccessibilityEventType invalidEventType =
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_INVALID;
    Flutter_SendAccessibilityAsyncEvent(elementId, invalidEventType);
    FML_DLOG(ERROR) << "ExecuteAccessibilityAction -> action: invalid("
                    << action << ")" << " event: innvalid("
                    << invalidEventType << ")";
}

void OhosAccessibilityBridge::PerformSetText(
    SemanticsNodeExtend& flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    char* newText;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(actionArguments, ARKUI_ACTION_ARG_SET_TEXT, &newText));
    CHECK_NULL_PTR(newText, PerformSetText);

    auto flutterSetTextAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(flutterNode.id,
                            flutterSetTextAction,
                            fml::MallocMapping::Copy(newText, strlen(newText)));
    flutterNode.value = newText;
    flutterNode.valueAttributes = {};
    LOGI("ExecuteAccessibilityAction -> action: set text(%{public}d), newText=%{public}s", action, newText);
}

void OhosAccessibilityBridge::PerformSelectText(
    const SemanticsNodeExtend& flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: select text(" << action << ")";

    auto flutterSelectTextAction = ArkuiActionsToFlutterActions(action);

    char* textSelectBase;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(
            actionArguments, ARKUI_ACTION_ARG_SELECT_TEXT_START, &textSelectBase)
    );
    CHECK_NULL_PTR(textSelectBase, PerformSelectText);

    char* textSelectExtent;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(
            actionArguments, ARKUI_ACTION_ARG_SELECT_TEXT_END, &textSelectExtent)
    );
    CHECK_NULL_PTR(textSelectExtent, PerformSelectText);

    std::map<std::string, int32_t> selectionMap;
    bool hasSelected = actionArguments != nullptr &&
                       textSelectBase != nullptr &&
                       textSelectExtent != nullptr;
    if (hasSelected) {
        int32_t base;
        int32_t extent;
        OHOSUtils::CharArrayToInt32(textSelectBase, base);
        OHOSUtils::CharArrayToInt32(textSelectBase, extent);
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_START, base});
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_END, extent});
    } else {
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_START, flutterNode.textSelectionBase});
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_END, flutterNode.textSelectionExtent});
    }
    // serialize map<string, int32_t> to byte vector
    std::vector<uint8_t> encodedData = OHOSUtils::SerializeStringIntMap(selectionMap);
    DispatchSemanticsAction(flutterNode.id,
                            flutterSelectTextAction,
                            fml::MallocMapping::Copy(encodedData.data(), encodedData.size() * sizeof(uint8_t)));
}

void OhosAccessibilityBridge::PerformSetCursorPosition(
    SemanticsNodeExtend flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: set cursor position (" << action << ")";
    return;
}

void OhosAccessibilityBridge::PerformCustomAction(
    const SemanticsNodeExtend& flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: custom action (" << action << ")";
    DispatchSemanticsAction(flutterNode.id, ACTIONS_::kCustomAction, {});
    return;
}

void OhosAccessibilityBridge::PerformShowOnScreenAction(
    const SemanticsNodeExtend& flutterNode)
{
    if (!IsNodeShowOnScreen(flutterNode)) {
        DispatchSemanticsAction(flutterNode.id, ACTIONS_::kShowOnScreen, {});
    }
}

flutter::SemanticsAction OhosAccessibilityBridge::ArkuiActionsToFlutterActions(
    ArkUI_Accessibility_ActionType arkui_action)
{
    switch (arkui_action) {
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
            return ACTIONS_::kTap;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
            return ACTIONS_::kLongPress;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
            return ACTIONS_::kScrollUp;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
            return ACTIONS_::kScrollDown;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
            return ACTIONS_::kCopy;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
            return ACTIONS_::kCut;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
            return ACTIONS_::kDidGainAccessibilityFocus;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
            return ACTIONS_::kDidLoseAccessibilityFocus;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT:
            return ACTIONS_::kSetSelection;
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT:
            return ACTIONS_::kSetText;
        default:
            // might not match to the valid action in arkui
            return ACTIONS_::kCustomAction;
    }
}

void OhosAccessibilityBridge::Flutter_SendAccessibilityAnnounceEvent(
    std::unique_ptr<char[]>& message,
    ArkUI_AccessibilityEventType eventType)
{
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider();
    CHECK_NULL_PTR_RET_VOID(provider_, Flutter_SendAccessibilityAnnounceEvent);

    auto* announceEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetEventType(
            announceEventInfo,
            ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY));

    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility(
            announceEventInfo, message.get()));
    FML_DLOG(INFO) << ("announce -> message: ") << (message.get());

    auto callback = [](int32_t errorCode) {
        FML_DLOG(WARNING) << "announce callback-> errorCode =" << errorCode;
    };

    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, announceEventInfo, callback);

    OH_ArkUI_DestoryAccessibilityEventInfo(announceEventInfo);
    announceEventInfo = nullptr;
}

void OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent(
    int64_t elementId,
    ArkUI_AccessibilityEventType eventType)
{
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider();
    CHECK_NULL_PTR_RET_VOID(provider_, Flutter_SendAccessibilityAsyncEvent);

    auto* eventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
    CHECK_NULL_PTR(eventInfo, Flutter_SendAccessibilityAsyncEvent);

    ArkUI_AccessibilityElementInfo* elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();
    FlutterSetElementInfoProperties(elementInfo, elementId);

    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetElementInfo(eventInfo, elementInfo));
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetEventType(eventInfo, eventType));

    auto callback = [](int32_t errorCode) {
        FML_DLOG(INFO)
            << "Flutter_SendAccessibilityAsyncEvent callback-> errorCode ="
            << errorCode;
    };

    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, eventInfo, callback);

    OH_ArkUI_DestoryAccessibilityElementInfo(elementInfo);
    elementInfo = nullptr;
    OH_ArkUI_DestoryAccessibilityEventInfo(eventInfo);
    eventInfo = nullptr;
    
    FML_DLOG(INFO) << "OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent is end";
    return;
}

/**
 * map flutter node (widget) to arkui elementinfo (component)
 */
std::string OhosAccessibilityBridge::GetNodeComponentType(
    const SemanticsNodeExtend& node)
{
    if (node.HasFlag(FLAGS_::kIsButton)) {
        return BUTTON_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsTextField)) {
        return EDIT_TEXT_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsMultiline)) {
        return EDIT_MULTILINE_TEXT_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsLink)) {
        return LINK_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsSlider) || node.HasAction(ACTIONS_::kIncrease) ||
        node.HasAction(ACTIONS_::kDecrease)) {
        return SLIDER_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsHeader)) {
        return HEADER_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsImage)) {
        return IMAGE_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kHasCheckedState)) {
        if (node.HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup)) {
            return RADIO_BUTTON_WIDGET_NAME;
        } else {
            return CHECK_BOX_WIDGET_NAME;
        }
    }
    if (node.HasFlag(FLAGS_::kHasToggledState)) {
        return SWITCH_WIDGET_NAME;
    }
    if (node.HasAction(ACTIONS_::kIncrease) || 
        node.HasAction(ACTIONS_::kDecrease)) {
        return SEEKBAR_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kHasImplicitScrolling)) {
        if (node.HasAction(ACTIONS_::kScrollLeft) ||
            node.HasAction(ACTIONS_::kScrollRight)) {
            return SCROLL_WIDGET_NAME;
        } else {
            return SCROLL_WIDGET_NAME;
        }
    }
    if ((!node.label.empty() || !node.tooltip.empty() || !node.hint.empty())) {
        return TEXT_WIDGET_NAME;
    }
    return OTHER_WIDGET_NAME;
}

bool OhosAccessibilityBridge::IsTextField(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsTextField);
}

bool OhosAccessibilityBridge::IsSlider(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsSlider);
}

bool OhosAccessibilityBridge::IsNodeClickable(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kTap);
}

bool OhosAccessibilityBridge::IsNodeVisible(
    const SemanticsNodeExtend& flutterNode)
{
    return !flutterNode.HasFlag(FLAGS_::kIsHidden);
}

bool OhosAccessibilityBridge::IsNodeCheckable(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kHasCheckedState) ||
           flutterNode.HasFlag(FLAGS_::kHasToggledState);
}

bool OhosAccessibilityBridge::IsNodeChecked(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsChecked) ||
           flutterNode.HasFlag(FLAGS_::kIsToggled);
}

bool OhosAccessibilityBridge::IsNodeSelected(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsSelected);
}

bool OhosAccessibilityBridge::IsNodePassword(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsTextField) &&
           flutterNode.HasFlag(FLAGS_::kIsObscured);
}

bool OhosAccessibilityBridge::IsNodeHasLongPress(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kLongPress);
}

bool OhosAccessibilityBridge::IsNodeEnabled(
    const SemanticsNodeExtend& flutterNode)
{
    return !flutterNode.HasFlag(FLAGS_::kHasEnabledState) ||
           flutterNode.HasFlag(FLAGS_::kIsEnabled);
}

bool OhosAccessibilityBridge::IsNodeShowOnScreen(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kShowOnScreen);
}

bool OhosAccessibilityBridge::HasScrolled(
    const SemanticsNodeExtend& flutterNode)
{
    return !std::isnan(flutterNode.scrollPosition) &&
           !std::isnan(flutterNode.previousScrollPosition) &&
           flutterNode.previousScrollPosition != flutterNode.scrollPosition;
}

bool OhosAccessibilityBridge::HasChangedLabel(const SemanticsNodeExtend& flutterNode)
{
    if (flutterNode.label.empty() && flutterNode.previousLabel.empty()) {
        return false;
    }
    return flutterNode.label.empty() ||
           flutterNode.previousLabel.empty() ||
           flutterNode.label != flutterNode.previousLabel;
}

bool OhosAccessibilityBridge::IsNodeFocusable(
    const SemanticsNodeExtend& node)
{
    if (node.HasFlag(FLAGS_::kScopesRoute)) {
        return false;
    }
    if (node.HasFlag(FLAGS_::kIsFocusable)) {
        return true;
    }
    // Always consider platform views focusable.
    if (node.IsPlatformViewNode()) {
        return true;
    }
    if ((node.flags & FOCUSABLE_FLAGS) != 0) {
        return true;
    }
    if ((node.actions & ~FOCUSABLE_FLAGS) != 0) {
        return true;
    }
    // Consider text nodes focusable.
    return !node.label.empty() || !node.value.empty() || !node.hint.empty();
}

bool OhosAccessibilityBridge::IsNodeFocused(const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsFocused);
}

bool OhosAccessibilityBridge::IsNodeScrollable(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kScrollLeft) ||
           flutterNode.HasAction(ACTIONS_::kScrollRight) ||
           flutterNode.HasAction(ACTIONS_::kScrollUp) ||
           flutterNode.HasAction(ACTIONS_::kScrollDown);
}

bool OhosAccessibilityBridge::IsScrollableWidget(
    const SemanticsNodeExtend& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kHasImplicitScrolling);
}

/**
 * when the system accessibility service is shut down,
 * clear all the flutter semantics-relevant caches like maps, vectors
 */
void OhosAccessibilityBridge::ClearFlutterSemanticsCaches()
{
    g_flutterSemanticsTree.clear();
    Flutter_SendAccessibilityAsyncEvent(
        accessibilityFocusedNode.id,
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusedNode = {};
}

/**
 * Dynamically load the ArkUI accessiblity C-API interface to 
 * be compatible with API-12 and above versions
 */
void OhosAccessibilityBridge::DynamicLoadAccessibilityLibrary()
{
    OH_ArkUI_CreateAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityElementInfo);
    OH_ArkUI_DestoryAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadDestroyElemFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityElementInfo);
    OH_ArkUI_AccessibilityElementInfoSetItemCount =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_ITEM_COUNT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetItemCount);
    OH_ArkUI_AccessibilityElementInfoSetStartItemIndex =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_START_ITEM_IDX);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetStartItemIndex);
    OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_CURR_ITEM_IDX);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex);
    OH_ArkUI_AccessibilityElementInfoSetEndItemIndex =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_END_ITEM_IDX);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEndItemIndex);
    OH_ArkUI_CreateAccessibilityEventInfo = 
        OhosAccessibilityDDL::DLLoadCreateEventInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityEventInfo);
    OH_ArkUI_AccessibilityEventSetEventType =
        OhosAccessibilityDDL::DLLoadSetEventFunc(ArkUIAccessibilityConstant::ARKUI_SET_EVENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetEventType);
    OH_ArkUI_AccessibilityEventSetRequestFocusId =
        OhosAccessibilityDDL::DLLoadSetReqFocusFunc(ArkUIAccessibilityConstant::ARKUI_SET_REQ_FOCUSED_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetRequestFocusId);
    OH_ArkUI_AccessibilityEventSetElementInfo =
        OhosAccessibilityDDL::DLLoadSetEventElemFunc(ArkUIAccessibilityConstant::ARKUI_EVENT_SET_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetElementInfo);
    OH_ArkUI_SendAccessibilityAsyncEvent =
        OhosAccessibilityDDL::DLLoadSendAsyncEventFunc(ArkUIAccessibilityConstant::ARKUI_SEND_A11Y_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_SendAccessibilityAsyncEvent);
    OH_ArkUI_DestoryAccessibilityEventInfo =
        OhosAccessibilityDDL::DLLoadDestroyEventFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityEventInfo);
    OH_ArkUI_AccessibilityElementInfoSetOperationActions = 
        OhosAccessibilityDDL::DLLoadSetElemOperActionsFunc(ArkUIAccessibilityConstant::ARKUI_SET_ACTIONS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetOperationActions);
    OH_ArkUI_AccessibilityElementInfoSetElementId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_NODE_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetElementId);
    OH_ArkUI_AccessibilityElementInfoSetScreenRect = 
        OhosAccessibilityDDL::DLLoadSetElemSreenRectFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCREEN_RECT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScreenRect);
    OH_ArkUI_AccessibilityElementInfoSetParentId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_PARENT_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetParentId);
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
    OH_ArkUI_AccessibilityElementInfoSetEnabled =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_ENABLED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEnabled);
    OH_ArkUI_AccessibilityElementInfoSetClickable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CLICKABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetClickable);
    OH_ArkUI_AccessibilityElementInfoSetFocusable =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_FOCUSABLE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetFocusable);
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_FOCUSED);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused);
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
    OH_ArkUI_AccessibilityElementInfoSetComponentType =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_COMPONENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetComponentType);
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_LEVEL);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel);
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_GROUP);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup);
    OH_ArkUI_AddAndGetAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadGetElemFunc(ArkUIAccessibilityConstant::ARKUI_GET_A11Y_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AddAndGetAccessibilityElementInfo);
    OH_ArkUI_FindAccessibilityActionArgumentByKey =
        OhosAccessibilityDDL::DLLoadGetFindActionArgs(ArkUIAccessibilityConstant::ARKUI_FIND_ACTION_ARG_BY_KEY);
    CHECK_DLL_NULL_PTR(OH_ArkUI_FindAccessibilityActionArgumentByKey);
    OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility =
        OhosAccessibilityDDL::DLLoadSetEventStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_ANNOUNCED_TEXT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility);
    FML_DLOG(INFO) << "DynamicLoadAccessibilityLibrary is finished";
}

/**
 * FindAccessibilityNodeInfosByText, FindFocusedAccessibilityNode,
 * FindNextFocusAccessibilityNode, ClearFocusedFocusAccessibilityNode,
 * GetAccessibilityNodeCursorPosition
 * (OH ArkUI accessibility callback functions, and not supported yet)
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosByText(
    int64_t elementId, const char* text,
    int32_t requestId, ArkUI_AccessibilityElementInfoList* elementList) { return 0; }
int32_t OhosAccessibilityBridge::FindFocusedAccessibilityNode(
    int64_t elementId, ArkUI_AccessibilityFocusType focusType,
    int32_t requestId, ArkUI_AccessibilityElementInfo* elementinfo) { return 0; }
int32_t OhosAccessibilityBridge::FindNextFocusAccessibilityNode(
    int64_t elementId, ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t requestId, ArkUI_AccessibilityElementInfo* elementList) { return 0; }
int32_t OhosAccessibilityBridge::ClearFocusedFocusAccessibilityNode() { return 0; }
int32_t OhosAccessibilityBridge::GetAccessibilityNodeCursorPosition(
    int64_t elementId, int32_t requestId, int32_t* index) { return 0; }

}  // namespace flutter
