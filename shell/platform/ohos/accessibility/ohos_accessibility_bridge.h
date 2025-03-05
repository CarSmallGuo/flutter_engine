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
#include "flutter/lib/ui/semantics/custom_accessibility_action.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
#include "native_accessibility_channel.h"
#include "ohos_accessibility_features.h"
#include "ohos_accessibility_ddl.h"
#include "flutter/shell/platform/ohos/utils/ohos_utils.h"
#include "flutter/shell/platform/ohos/utils/arkui_accessibility_constant.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkPoint.h"

namespace flutter {
typedef flutter::SemanticsFlags FLAGS_;
typedef flutter::SemanticsAction ACTIONS_;

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
    SkM44 globalTransform = SkM44{};
    AbsoluteRect absoluteRect = AbsoluteRect::MakeEmpty();
    int32_t parentId = -1;
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
    bool HasPrevAction(SemanticsAction action) const {
        return (previousActions & this->actions) != 0;
    }
    bool HasPrevFlag(SemanticsFlags flag) const {
        return (previousFlags & this->flags) != 0;
    }
};

/**
 * 桥接flutter无障碍功能和ohos无障碍系统服务
 */
class OhosAccessibilityBridge {
public:
    static OhosAccessibilityBridge* GetInstance();
    OhosAccessibilityBridge(const OhosAccessibilityBridge&) = delete;
    OhosAccessibilityBridge& operator=(const OhosAccessibilityBridge&) = delete;

    int64_t native_shell_holder_id_;
    bool isFlutterNavigated_;
    bool isTouchGuideOn_;

    std::unordered_map<int32_t, SemanticsNodeExtend> g_flutterSemanticsTree;

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

    SemanticsNodeExtend GetFlutterSemanticsNode(int32_t id);
    int32_t GetParentId(const SemanticsNodeExtend& node);

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
    void Flutter_SendAccessibilityAnnounceEvent(
        std::unique_ptr<char[]>& message,
        ArkUI_AccessibilityEventType eventType);

    void RelativeRectToScreenRect(SemanticsNodeExtend& node);
    const AbsoluteRect& GetAbsoluteScreenRect(const SemanticsNodeExtend& flutterNode);
    void SetAbsoluteScreenRect(SemanticsNodeExtend& flutterNode,
                               float left,
                               float top,
                               float right,
                               float bottom);

    SemanticsNodeExtend UpdatetSemanticsNodeExtend(flutter::SemanticsNode node);

    void FlutterScrollExecution(
        const SemanticsNodeExtend& node,
        ArkUI_AccessibilityElementInfo* elementInfoFromList);
    
    void ClearFlutterSemanticsCaches();

private:
    OhosAccessibilityBridge();
    bool isAccessibilityEnabled_;
    static std::unique_ptr<OhosAccessibilityBridge> bridgeInstance_;
    std::shared_ptr<NativeAccessibilityChannel> nativeAccessibilityChannel_;
    std::shared_ptr<OhosAccessibilityFeatures> accessibilityFeatures_;

    SemanticsNodeExtend inputFocusedNode;
    SemanticsNodeExtend lastInputFocusedNode;
    SemanticsNodeExtend accessibilityFocusedNode;
    
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
    SetElemStringFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityDescription;
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
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetIsHint;
    SetElemBoolFunc OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup;

    static const int32_t ARKUI_ACCESSIBILITY_ROOT_PARENT_ID = -2100000;
    static const int32_t RET_ERROR_STATE_CODE = -1;
    static const int32_t ROOT_NODE_ID = 0;
    
    const char* ARKUI_ACTION_ARG_SET_TEXT = "setText";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_START = "selectTextBegin";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_END = "selectTextEnd";
    
    const char* ROOT_WIDGET_NAME = "root";
    const std::string OTHER_WIDGET_NAME = "View";
    const std::string TEXT_WIDGET_NAME = "Text";
    const std::string EDIT_TEXT_WIDGET_NAME = "TextInput";
    const std::string EDIT_MULTILINE_TEXT_WIDGET_NAME = "TextArea";
    const std::string IMAGE_WIDGET_NAME = "Image";
    const std::string SCROLL_WIDGET_NAME = "Scroll";
    const std::string BUTTON_WIDGET_NAME = "Button";
    const std::string LINK_WIDGET_NAME = "Link";
    const std::string SLIDER_WIDGET_NAME = "Slider";
    const std::string HEADER_WIDGET_NAME = "Header";
    const std::string RADIO_BUTTON_WIDGET_NAME = "Radio";
    const std::string CHECK_BOX_WIDGET_NAME = "Checkbox";
    const std::string SWITCH_WIDGET_NAME = "Toggle";
    const std::string SEEKBAR_WIDGET_NAME = "SeekBar";

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
        const SemanticsNodeExtend& node);
    void BuildArkUISemanticsTree(
        int64_t elementId,
        ArkUI_AccessibilityElementInfo* elementInfoFromList,
        ArkUI_AccessibilityElementInfoList* elementList);

    std::string GetNodeComponentType(const SemanticsNodeExtend& node);
    flutter::SemanticsAction ArkuiActionsToFlutterActions(
        ArkUI_Accessibility_ActionType arkui_action);

    void UpdateIteratively(std::unordered_set<int32_t>& visitedIds);
    void ConvertRectToGlobal(SemanticsNodeExtend& node);
    SkPoint ApplyTransform(SkPoint& point, const SkM44& transform);
    
    bool HasScrolled(const SemanticsNodeExtend& flutterNode);
    void DoScroll(SemanticsNodeExtend nodeEx);
    bool HasChangedLabel(const SemanticsNodeExtend& flutterNode);

    void RequestFocusWhenPageUpdate(int32_t requestFocusId);
    void DynamicLoadAccessibilityLibrary();

    bool IsNodeFocusable(const SemanticsNodeExtend& flutterNode);
    bool IsNodeFocused(const SemanticsNodeExtend& flutterNode);
    bool IsNodeCheckable(const SemanticsNodeExtend& flutterNode);
    bool IsNodeChecked(const SemanticsNodeExtend& flutterNode);
    bool IsNodeSelected(const SemanticsNodeExtend& flutterNode);
    bool IsNodeClickable(const SemanticsNodeExtend& flutterNode);
    bool IsNodeScrollable(const SemanticsNodeExtend& flutterNode);
    bool IsNodePassword(const SemanticsNodeExtend& flutterNode);
    bool IsNodeVisible(const SemanticsNodeExtend& flutterNode);
    bool IsNodeEnabled(const SemanticsNodeExtend& flutterNode);
    bool IsNodeHasLongPress(const SemanticsNodeExtend& flutterNode);
    bool IsNodeShowOnScreen(const SemanticsNodeExtend& flutterNode);
    bool IsTextField(const SemanticsNodeExtend& flutterNode);
    bool IsSlider(const SemanticsNodeExtend& flutterNode);
    bool IsScrollableWidget(const SemanticsNodeExtend& flutterNode);

    void PerformClickAction(int64_t elementId,
                            ArkUI_Accessibility_ActionType action,
                            const SemanticsNodeExtend& flutterNode);
    void PerformLongClickAction(int64_t elementId,
                                ArkUI_Accessibility_ActionType action,
                                const SemanticsNodeExtend& flutterNode);
    void PerformGainFocusnAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 const SemanticsNodeExtend& flutterNode);
    void PerformClearFocusAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 const SemanticsNodeExtend& flutterNode);
    void PerformScrollUpAction(int64_t elementId,
                               ArkUI_Accessibility_ActionType action,
                               SemanticsNodeExtend& flutterNode);
    void PerformScrollDownAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 SemanticsNodeExtend& flutterNode);
    void PerformClipboardAction(int64_t elementId,
                                const ArkUI_Accessibility_ActionType& action);
    void PerformInvalidAction(int64_t elementId,
                              ArkUI_Accessibility_ActionType action,
                              const SemanticsNodeExtend& flutterNode);
    void PerformSetText(SemanticsNodeExtend& flutterNode,
                        ArkUI_Accessibility_ActionType action,
                        ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSelectText(const SemanticsNodeExtend& flutterNode,
                           ArkUI_Accessibility_ActionType action,
                           ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSetCursorPosition(SemanticsNodeExtend flutterNode,
                                  ArkUI_Accessibility_ActionType action,
                                  ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformCustomAction(const SemanticsNodeExtend& flutterNode,
                             ArkUI_Accessibility_ActionType action,
                             ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformShowOnScreenAction(const SemanticsNodeExtend& flutterNode);
};

}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
