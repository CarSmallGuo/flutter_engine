/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#ifndef OHOS_ACCESSIBILITY_BRIDGE_H
#define OHOS_ACCESSIBILITY_BRIDGE_H
#include <cstdint>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <memory>
#include <mutex>
#include <arkui/native_interface_accessibility.h>
#include "flutter/fml/mapping.h"
#include "native_accessibility_channel.h"
#include "ohos_accessibility_features.h"
#include "ohos_accessibility_ddl.h"
#include "flutter/shell/platform/ohos/utils/ohos_utils.h"
#include "flutter/shell/platform/ohos/utils/arkui_accessibility_constant.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "ohos_semantics_node.h"

namespace flutter {

/**
 * Bridge Flutter accessibility and OHOS accessibility services
 */
class OhosAccessibilityBridge {
public:
    static OhosAccessibilityBridge* GetInstance();
    OhosAccessibilityBridge(const OhosAccessibilityBridge&) = delete;
    OhosAccessibilityBridge& operator=(const OhosAccessibilityBridge&) = delete;

    int64_t native_shell_holder_id_;
    bool isFlutterNavigated_;
    
    SemanticsNodeExtend* accessibilityFocusedNode = nullptr;
    std::unordered_map<int32_t, SemanticsNodeExtend*> flutterSemanticsTree;

    void OnOhosAccessibilityStateChange(bool ohosAccessibilityEnabled, int64_t shellholderId);

    void UpdateSemantics(flutter::SemanticsNodeUpdates update,
                         flutter::CustomAccessibilityActionUpdates actions);

    void DispatchSemanticsAction(int32_t id,
                                 flutter::SemanticsAction action,
                                 fml::MallocMapping args);

    void Announce(std::unique_ptr<char[]>& message);
    void OnTap(int32_t nodeId);
    void OnLongPress(int32_t nodeId);
    void OnTooltip(std::unique_ptr<char[]>& message);

    SemanticsNodeExtend* GetOrAddSemanticsNode(int32_t id);

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

    void SendAccessibilityEvent(
        int64_t elementId,
        ArkUI_AccessibilityEventType eventType);
    void SendAccessibilityAnnounceEvent(
        std::unique_ptr<char[]>& message,
        ArkUI_AccessibilityEventType eventType);

    void ClearFlutterSemanticsCaches();

private:
    bool isAccessibilityEnabled_;
    static std::unique_ptr<OhosAccessibilityBridge> bridgeInstance_;
    std::shared_ptr<NativeAccessibilityChannel> nativeAccessibilityChannel_;
    std::shared_ptr<OhosAccessibilityFeatures> accessibilityFeatures_;

    static const int32_t ROOT_NODE_ID = 0;
    const char* ARKUI_ACTION_ARG_SET_TEXT = "setText";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_START = "selectTextBegin";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_END = "selectTextEnd";
    
    // function pointers from libflutter_accessibility.so
    GetElemFunc OH_ArkUI_AddAndGetAccessibilityElementInfo;
    CreateElemInfoFunc OH_ArkUI_CreateAccessibilityElementInfo;
    DestroyElemFunc OH_ArkUI_DestoryAccessibilityElementInfo;
    CreateEventInfoFunc OH_ArkUI_CreateAccessibilityEventInfo;
    DestroyEventFunc OH_ArkUI_DestoryAccessibilityEventInfo;
    SendAsyncEventFunc OH_ArkUI_SendAccessibilityAsyncEvent;
    GetFindActionArgs OH_ArkUI_FindAccessibilityActionArgumentByKey;
    SetElemChildFunc OH_ArkUI_AccessibilityElementInfoSetChildNodeIds;
    SetElemOperActionsFunc OH_ArkUI_AccessibilityElementInfoSetOperationActions;
    SetElemSreenRectFunc OH_ArkUI_AccessibilityElementInfoSetScreenRect;
    SetEventElemFunc OH_ArkUI_AccessibilityEventSetElementInfo;
    SetEventFunc OH_ArkUI_AccessibilityEventSetEventType;
    SetReqFocusFunc OH_ArkUI_AccessibilityEventSetRequestFocusId;
    SetEventStringFunc OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetElementId;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetParentId;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetStartItemIndex;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetEndItemIndex;
    SetElemIntFunc OH_ArkUI_AccessibilityElementInfoSetItemCount;
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetComponentType;
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetContents;
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetHintText;
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityText;
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetCheckable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetChecked;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetFocusable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetFocused;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetVisible;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetSelected;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetClickable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetLongClickable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetEnabled;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetIsPassword;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetScrollable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetEditable;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup;

    OhosAccessibilityBridge();

    void BuildArkuiElementInfoTree(
        SemanticsNodeExtend* flutterNode,
        ArkUI_AccessibilityElementInfo* elementInfoFromList,
        ArkUI_AccessibilityElementInfoList* elementList);

    void DynamicLoadAccessibilityLibrary();
    void DynamicLoadSetElemIntFunc();
    void DynamicLoadSetElemStringFunc();
    void DynamicLoadSetElemBoolFunc();

    flutter::SemanticsAction ArkuiActionsToFlutterActions(
        ArkUI_Accessibility_ActionType arkui_action);
        
    void PerformClickAction(ArkUI_Accessibility_ActionType action,
                            SemanticsNodeExtend* flutterNode);
    void PerformLongClickAction(ArkUI_Accessibility_ActionType action,
                                SemanticsNodeExtend* flutterNode);
    void PerformGainFocusnAction(ArkUI_Accessibility_ActionType action,
                                 SemanticsNodeExtend* flutterNode);
    void PerformClearFocusAction(ArkUI_Accessibility_ActionType action,
                                 SemanticsNodeExtend* flutterNode);
    void PerformScrollUpAction(ArkUI_Accessibility_ActionType action,
                               SemanticsNodeExtend* flutterNode);
    void PerformScrollDownAction(ArkUI_Accessibility_ActionType action,
                                 SemanticsNodeExtend* flutterNode);
    void PerformClipboardAction(ArkUI_Accessibility_ActionType action,
                                SemanticsNodeExtend* flutterNode);
    void PerformInvalidAction(ArkUI_Accessibility_ActionType action,
                              SemanticsNodeExtend* flutterNode);
    void PerformSetText(SemanticsNodeExtend* flutterNode,
                        ArkUI_Accessibility_ActionType action,
                        ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSelectText(SemanticsNodeExtend* flutterNode,
                           ArkUI_Accessibility_ActionType action,
                           ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSetCursorPosition(SemanticsNodeExtend* flutterNode,
                                  ArkUI_Accessibility_ActionType action,
                                  ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformCustomAction(SemanticsNodeExtend* flutterNode,
                             ArkUI_Accessibility_ActionType action,
                             ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformShowOnScreenAction(SemanticsNodeExtend* flutterNode);
};

}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
