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

    // Traverse the updated semantic tree
    for (auto& item : update) {
        auto node = item.second;

        SemanticsNodeExtend* nodeEx = GetOrAddSemanticsNode(item.first);
        nodeEx->UpdatetSemanticsNodeExtend(node);

        if (!nodeEx->IsVisible()) { continue; }

        // update children list
        nodeEx->childrenInTraversalOrderList.clear();
        for (auto nodeId : nodeEx->childrenInTraversalOrder) {
            nodeEx->childrenInTraversalOrderList.emplace_back(GetOrAddSemanticsNode(nodeId));
        }
    }

    std::unordered_set<int32_t> visitedIds;
    SkM44 transform;
    auto* rootNode = GetOrAddSemanticsNode(ROOT_NODE_ID);
    // Recursively update and config the essential properties of the nodes
    if (rootNode) {
        rootNode->UpdateRecursively(visitedIds, transform, false);
        rootNode->SetElementInfoProperties();
    }

    // detele the remain useless nodes from the semantics tree
    for (auto it = flutterSemanticsTree.begin(); 
        it != flutterSemanticsTree.end();) {
        if (visitedIds.find(it->first) == visitedIds.end()) {
            it = flutterSemanticsTree.erase(it);
        } else {
            ++it;
        }
    }

    // request rootNode when routes a new flutter page
    // on touch guide mode (screen reader is on)
    if (isFlutterNavigated_) {
        SendAccessibilityEvent(
            0, ArkUI_AccessibilityEventType::
                ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_REQUEST_ACCESSIBILITY_FOCUS);
        isFlutterNavigated_ = false;
    }

    SendAccessibilityEvent(
        0, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);

    // double-finger scrolling with the focused node following real-time
    if (accessibilityFocusedNode && accessibilityFocusedNode->hasUpdated) {
        SendAccessibilityEvent(
            accessibilityFocusedNode->id,
            ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_FOCUS_NODE_UPDATE);
    }
}

SemanticsNodeExtend* OhosAccessibilityBridge::GetOrAddSemanticsNode(
    int32_t id)
{
    SemanticsNodeExtend* node = flutterSemanticsTree[id];
    if (node == nullptr) {
        node = new SemanticsNodeExtend();
        node->id = id;
        flutterSemanticsTree[id] = node;
    }
    return node;
}

/**
 * send the custom announcement message from dart side
 */
void OhosAccessibilityBridge::Announce(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("Announce -> message: %{public}s", message.get());
}

/**
 * tap the node with spercific id from dart side
 */
void OhosAccessibilityBridge::OnTap(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    SendAccessibilityEvent(static_cast<int64_t>(nodeId),
                           ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED);
    LOGD("OnTap -> nodeId: %{public}d", nodeId);
}

/**
 * longpress the node with spercific id from dart side
 */
void OhosAccessibilityBridge::OnLongPress(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    SendAccessibilityEvent(static_cast<int64_t>(nodeId),
                           ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED);
    LOGD("OnLongPress -> nodeId: %{public}d", nodeId);
}

/**
 * send the custom tooltip message from dart side
 */
void OhosAccessibilityBridge::OnTooltip(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    SendAccessibilityEvent(static_cast<int64_t>(ROOT_NODE_ID),
                           ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_STATE_UPDATE);
    SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("OnTooltip -> message: %{public}s", message.get());
}

/**
 * build ArkUI elementInfo tree by levelOrderTraversal way
 * to support DevEco Testing (UITest, UIViewer and Hypium) 
 */
void OhosAccessibilityBridge::BuildArkuiElementInfoTree(
    SemanticsNodeExtend* flutterNode,
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    flutterNode->SetElementInfoProperties(elementInfoFromList);

    std::queue<SemanticsNodeExtend*> semanticsQue;
    semanticsQue.push(flutterNode);

    while (!semanticsQue.empty()) {
        auto* currNode = semanticsQue.front();
        semanticsQue.pop();

        auto* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
        // set current flutter node properties
        currNode->SetElementInfoProperties(newElementInfo);
        
        for (auto childId: currNode->childrenInTraversalOrder) {
          auto* childNode = GetOrAddSemanticsNode(childId);
          semanticsQue.push(childNode);
        }
    }
}

/**
 * Called to obtain element information based on a specified node.
 * (OH ArkUI accessibility callback function, but ohos only supports
 *  ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT, and
 *  ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN
 *  search modes now)
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
    
    auto* flutterNode = GetOrAddSemanticsNode(static_cast<int32_t>(elementId > 0 ? elementId : 0));
    CHECK_NULL_PTR_WITH_RET(flutterNode, GetOrAddSemanticsNode);

    auto* elementInfoFromList = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
    CHECK_NULL_PTR_WITH_RET(elementInfoFromList, OH_ArkUI_AddAndGetAccessibilityElementInfo);

    switch(mode) {
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT:
            /** Search for current nodes. (mode = 0) */
            flutterNode->SetElementInfoProperties(elementInfoFromList);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS:
            /** Search for parent nodes. (mode = 1) */
            flutterNode->SetElementInfoProperties(elementInfoFromList);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS:
            /** Search for sibling nodes. (mode = 2) */
            flutterNode->SetElementInfoProperties(elementInfoFromList);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CHILDREN:
            /** Search for child nodes at the next level. (mode = 4) */
            flutterNode->SetElementInfoProperties(elementInfoFromList);
            break;
        case ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN:
            /** Search for all child nodes. (mode = 8) */
            BuildArkuiElementInfoTree(flutterNode, elementInfoFromList, elementList);
            break;
        default:
            flutterNode->SetElementInfoProperties(elementInfoFromList);
    }
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

/**
 * Called to perform accessibility actions based on the specified node.
 * (OH ArkUI accessibility callback function in ohos accessibility-framework thread)
 */
int32_t OhosAccessibilityBridge::ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments,
    int32_t requestId)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction input-params-> elementId="
                   << elementId << " action=" << action;
    CHECK_NULL_PTR(actionArguments, ExecuteAccessibilityAction);

    auto* flutterNode = GetOrAddSemanticsNode(static_cast<int32_t>(elementId));
    CHECK_NULL_PTR_WITH_RET(flutterNode, GetOrAddSemanticsNode);

    // Display obscured flutter nodes on the screen when scrolling
    // ArkUI accessibility service does not support this feature
    PerformShowOnScreenAction(flutterNode);

    switch (action) {
        /** Response to a click. 16 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
            PerformClickAction(action, flutterNode);
            break;
        /** Response to a long click. 32 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
            PerformLongClickAction(action, flutterNode);
            break;
        /** Accessibility focus acquisition. 64 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
            PerformGainFocusnAction(action, flutterNode);
            break;
        /** Accessibility focus clearance. 128 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
            PerformClearFocusAction(action, flutterNode);
            break;
        /** Forward scroll action. 256 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
            PerformScrollUpAction(action, flutterNode);
            break;
        /** Backward scroll action. 512 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
            PerformScrollDownAction(action, flutterNode);
            break;
        /** Copy, Paste, Cut action for text content. 1024, 2048, 4096*/
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE:
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
            PerformClipboardAction(action, flutterNode);
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
            PerformInvalidAction(action, flutterNode);
            break;
        default:
            /** custom semantics action */
            PerformCustomAction(flutterNode, action, actionArguments);
    }
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
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
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    /** Click event, sent after the UI component responds. 1 */
    auto clickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED;
    SendAccessibilityEvent(flutterNode->id, clickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: click(" << action
                   << ")" << " event: click(" << clickEventType << ")";
    auto flutterTapAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(flutterNode->id, flutterTapAction, {});
}

void OhosAccessibilityBridge::PerformLongClickAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    /** Long click event, sent after the UI component responds. 2 */
    auto longClickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED;
    SendAccessibilityEvent(flutterNode->id, longClickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: longclick("
                   << action << ")" << " event: longclick("
                   << longClickEventType << ")";
    auto flutterLongPressAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(flutterNode->id, flutterLongPressAction, {});
}

void OhosAccessibilityBridge::PerformGainFocusnAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    // update the accessibility focused node
    accessibilityFocusedNode = flutterNode;
    // Accessibility focus event, sent after the UI component responds. 32768
    auto focusEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUSED;
    SendAccessibilityEvent(flutterNode->id, focusEventType);

    if (accessibilityFocusedNode->hasUpdated) {
        SendAccessibilityEvent(
            accessibilityFocusedNode->id, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_FOCUS_NODE_UPDATE);
    }
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: focus(" << action
                   << ")" << " event: focus(" << focusEventType << ")";
}

void OhosAccessibilityBridge::PerformClearFocusAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    if (accessibilityFocusedNode &&
        (flutterNode->id == accessibilityFocusedNode->id || flutterNode->id == 0)) {
        accessibilityFocusedNode = nullptr;
        /** Accessibility focus cleared event, sent after the UI component responds. 65536 */
        auto clearFocusEventType = ArkUI_AccessibilityEventType::
            ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED;
        SendAccessibilityEvent(flutterNode->id, clearFocusEventType);
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: clearfocus("
                       << action << ")" << " event: clearfocus("
                       << clearFocusEventType << ")";
    }
}

void OhosAccessibilityBridge::PerformScrollUpAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    // flutter scroll forward with different situations
    if (flutterNode->HasAction(ACTIONS_::kScrollUp)) {
        auto flutterScrollUpAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(flutterNode->id, flutterScrollUpAction, {});
    } else if (flutterNode->HasAction(ACTIONS_::kScrollLeft)) {
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kScrollLeft, {});
    } else if (flutterNode->HasAction(ACTIONS_::kIncrease)) {
        flutterNode->value = flutterNode->increasedValue;
        flutterNode->valueAttributes = flutterNode->increasedValueAttributes;
        SendAccessibilityEvent(
            flutterNode->id, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kIncrease, {});
    }
}

void OhosAccessibilityBridge::PerformScrollDownAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    // flutter scroll down with different situations
    if (flutterNode->HasAction(ACTIONS_::kScrollDown)) {
        auto flutterScrollDownAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(flutterNode->id, flutterScrollDownAction, {});
    } else if (flutterNode->HasAction(ACTIONS_::kScrollRight)) {
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kScrollRight, {});
    } else if (flutterNode->HasAction(ACTIONS_::kDecrease)) {
        flutterNode->value = flutterNode->decreasedValue;
        flutterNode->valueAttributes = flutterNode->decreasedValueAttributes;
        SendAccessibilityEvent(
            flutterNode->id, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kDecrease, {});
    }
}

void OhosAccessibilityBridge::PerformClipboardAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: copy(" << action << ")";
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kCopy, {});
    } else if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: paste(" << action << ")";
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kPaste, {});
    } else {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: cut(" << action << ")";
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kCut, {});
    }
}

void OhosAccessibilityBridge::PerformInvalidAction(
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtend* flutterNode)
{
    /** Invalid event. 0 */
    ArkUI_AccessibilityEventType invalidEventType =
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_INVALID;
    SendAccessibilityEvent(flutterNode->id, invalidEventType);
    FML_DLOG(ERROR) << "ExecuteAccessibilityAction -> action: invalid("
                    << action << ")" << " event: innvalid("
                    << invalidEventType << ")";
}

void OhosAccessibilityBridge::PerformSetText(
    SemanticsNodeExtend* flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    char* newText;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(actionArguments, ARKUI_ACTION_ARG_SET_TEXT, &newText));
    CHECK_NULL_PTR(newText, PerformSetText);

    auto flutterSetTextAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(flutterNode->id,
                            flutterSetTextAction,
                            fml::MallocMapping::Copy(newText, strlen(newText)));
    flutterNode->value = newText;
    flutterNode->valueAttributes = {};
    LOGI("ExecuteAccessibilityAction -> action: set text(%{public}d), newText=%{public}s", action, newText);
}

void OhosAccessibilityBridge::PerformSelectText(
    SemanticsNodeExtend* flutterNode,
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
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_START, flutterNode->textSelectionBase});
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_END, flutterNode->textSelectionExtent});
    }
    // serialize map<string, int32_t> to byte vector
    std::vector<uint8_t> encodedData = OHOSUtils::SerializeStringIntMap(selectionMap);
    DispatchSemanticsAction(flutterNode->id,
                            flutterSelectTextAction,
                            fml::MallocMapping::Copy(encodedData.data(), encodedData.size() * sizeof(uint8_t)));
}

void OhosAccessibilityBridge::PerformSetCursorPosition(
    SemanticsNodeExtend* flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: set cursor position (" << action << ")";
    return;
}

void OhosAccessibilityBridge::PerformCustomAction(
    SemanticsNodeExtend* flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: custom action (" << action << ")";
    DispatchSemanticsAction(flutterNode->id, ACTIONS_::kCustomAction, {});
    return;
}

void OhosAccessibilityBridge::PerformShowOnScreenAction(
    SemanticsNodeExtend* flutterNode)
{
    if (!flutterNode->IsShowOnScreen()) {
        DispatchSemanticsAction(flutterNode->id, ACTIONS_::kShowOnScreen, {});
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

void OhosAccessibilityBridge::SendAccessibilityAnnounceEvent(
    std::unique_ptr<char[]>& message,
    ArkUI_AccessibilityEventType eventType)
{
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider();
    CHECK_NULL_PTR_RET_VOID(provider_, SendAccessibilityAnnounceEvent);

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

void OhosAccessibilityBridge::SendAccessibilityEvent(
    int64_t elementId,
    ArkUI_AccessibilityEventType eventType)
{
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider();
    CHECK_NULL_PTR_RET_VOID(provider_, SendAccessibilityEvent);

    auto* eventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
    CHECK_NULL_PTR(eventInfo, SendAccessibilityEvent);


    auto* flutterNode = GetOrAddSemanticsNode((int32_t)elementId);
    if (flutterNode) {
        ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetElementInfo(eventInfo, flutterNode->elementInfo));
        flutterNode->hasUpdated = false;
    }
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetEventType(eventInfo, eventType));

    auto callback = [](int32_t errorCode) {
        FML_DLOG(INFO)
            << "SendAccessibilityEvent callback-> errorCode ="
            << errorCode;
    };

    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, eventInfo, callback);

    OH_ArkUI_DestoryAccessibilityEventInfo(eventInfo);
    eventInfo = nullptr;
    return;
}

/**
 * when the system accessibility service is shut down,
 * clear all the flutter semantics-relevant caches like maps, vectors
 */
void OhosAccessibilityBridge::ClearFlutterSemanticsCaches()
{
    flutterSemanticsTree.clear();
    SendAccessibilityEvent(
        accessibilityFocusedNode->id,
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusedNode = nullptr;
}

/**
 * Dynamically load the ArkUI accessiblity C-API interface to 
 * be compatible with API-12+ versions
 */
void OhosAccessibilityBridge::DynamicLoadAccessibilityLibrary()
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
    OH_ArkUI_AccessibilityElementInfoSetScreenRect = 
        OhosAccessibilityDDL::DLLoadSetElemSreenRectFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCREEN_RECT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScreenRect);
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

void OhosAccessibilityBridge::DynamicLoadSetElemIntFunc()
{
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
    OH_ArkUI_AccessibilityElementInfoSetElementId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_NODE_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetElementId);
    OH_ArkUI_AccessibilityElementInfoSetParentId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_PARENT_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetParentId);
}

void OhosAccessibilityBridge::DynamicLoadSetElemStringFunc()
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

void OhosAccessibilityBridge::DynamicLoadSetElemBoolFunc()
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
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_GROUP);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup);
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
