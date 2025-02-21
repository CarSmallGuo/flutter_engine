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
struct SemanticsNodeExtent : flutter::SemanticsNode {
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
    bool operator==(const SemanticsNodeExtent& other) const {
        return id == other.id;
    }
    struct Hash {
        std::size_t operator()(const SemanticsNodeExtent& obj) const {
            return std::hash<int>()(obj.id);
        }
    };
};

/**
 * 桥接flutter无障碍功能和ohos无障碍系统服务
 */
class OhosAccessibilityBridge {
public:
    static OhosAccessibilityBridge* GetInstance();
    OhosAccessibilityBridge(const OhosAccessibilityBridge&) = delete;
    OhosAccessibilityBridge& operator=(const OhosAccessibilityBridge&) = delete;

    std::string xcomponentId_;
    int64_t native_shell_holder_id_;
    bool isFlutterNavigated_;
    bool isTouchGuideOn_;

    std::unordered_map<int32_t, SemanticsNodeExtent> g_flutterSemanticsTree;
    std::unordered_map<std::string, std::unordered_map<int32_t, SemanticsNodeExtent>> g_flutterSemanticsTreeXComponents;

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

    SemanticsNodeExtent GetFlutterSemanticsNode(int32_t id);
    int32_t GetParentId(int64_t elementId);

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

    void RelativeRectToScreenRect(SemanticsNodeExtent& node);
    AbsoluteRect GetAbsoluteScreenRect(const SemanticsNodeExtent& flutterNode);
    void SetAbsoluteScreenRect(SemanticsNodeExtent& flutterNode,
                               float left,
                               float top,
                               float right,
                               float bottom);

    SemanticsNodeExtent UpdatetSemanticsNodeExtent(flutter::SemanticsNode node);

    void FlutterScrollExecution(
        SemanticsNodeExtent node,
        ArkUI_AccessibilityElementInfo* elementInfoFromList);
    
    void ClearFlutterSemanticsCaches();

private:
    OhosAccessibilityBridge();
    bool isAccessibilityEnabled_;
    static std::unique_ptr<OhosAccessibilityBridge> bridgeInstance_;
    std::shared_ptr<NativeAccessibilityChannel> nativeAccessibilityChannel_;
    std::shared_ptr<OhosAccessibilityFeatures> accessibilityFeatures_;

    SemanticsNodeExtent inputFocusedNode;
    SemanticsNodeExtent lastInputFocusedNode;
    SemanticsNodeExtent accessibilityFocusedNode;

    static const int32_t OHOS_API_VERSION; 
    static const int32_t ARKUI_ACCESSIBILITY_ROOT_PARENT_ID = -2100000;
    static const int32_t RET_ERROR_STATE_CODE = -1;
    static const int32_t ROOT_NODE_ID = 0;
    constexpr static const double SCROLL_EXTENT_FOR_INFINITY = 100000.0;
    constexpr static const double SCROLL_POSITION_CAP_FOR_INFINITY = 70000.0;
    
    const char* ARKUI_ACTION_ARG_SET_TEXT = "setText";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_START = "selectTextBegin";
    const char* ARKUI_ACTION_ARG_SELECT_TEXT_END = "selectTextEnd";

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
        const SemanticsNodeExtent& node);
    void BuildArkUISemanticsTree(
        int64_t elementId,
        ArkUI_AccessibilityElementInfo* elementInfoFromList,
        ArkUI_AccessibilityElementInfoList* elementList);

    std::vector<int64_t> GetLevelOrderTraversalTree(int32_t rootId);
    SemanticsNodeExtent GetFlutterRootSemanticsNode();
    std::string GetNodeComponentType(const SemanticsNodeExtent& node);
    flutter::SemanticsAction ArkuiActionsToFlutterActions(
        ArkUI_Accessibility_ActionType arkui_action);

    void ComputeGlobalTransformAndParentId();
    void ConvertRectToGlobal(SemanticsNodeExtent& node);
    SkPoint ApplyTransform(SkPoint& point, const SkM44& transform);
    
    bool HasScrolled(const SemanticsNodeExtent& flutterNode);
    bool HasChangedLabel(const SemanticsNodeExtent& flutterNode);

    bool IsNodeFocusable(const SemanticsNodeExtent& flutterNode);
    bool IsNodeFocused(const SemanticsNodeExtent& flutterNode);
    bool IsNodeCheckable(const SemanticsNodeExtent& flutterNode);
    bool IsNodeChecked(const SemanticsNodeExtent& flutterNode);
    bool IsNodeSelected(const SemanticsNodeExtent& flutterNode);
    bool IsNodeClickable(const SemanticsNodeExtent& flutterNode);
    bool IsNodeScrollable(const SemanticsNodeExtent& flutterNode);
    bool IsNodePassword(const SemanticsNodeExtent& flutterNode);
    bool IsNodeVisible(const SemanticsNodeExtent& flutterNode);
    bool IsNodeEnabled(const SemanticsNodeExtent& flutterNode);
    bool IsNodeHasLongPress(const SemanticsNodeExtent& flutterNode);
    bool IsNodeShowOnScreen(const SemanticsNodeExtent& flutterNode);
    bool IsTextField(const SemanticsNodeExtent& flutterNode);
    bool IsSlider(const SemanticsNodeExtent& flutterNode);
    bool IsScrollableWidget(const SemanticsNodeExtent& flutterNode);

    void PerformClickAction(int64_t elementId,
                            ArkUI_Accessibility_ActionType action,
                            const SemanticsNodeExtent& flutterNode);
    void PerformLongClickAction(int64_t elementId,
                                ArkUI_Accessibility_ActionType action,
                                const SemanticsNodeExtent& flutterNode);
    void PerformGainFocusnAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 const SemanticsNodeExtent& flutterNode);
    void PerformClearFocusAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 const SemanticsNodeExtent& flutterNode);
    void PerformScrollUpAction(int64_t elementId,
                               ArkUI_Accessibility_ActionType action,
                               SemanticsNodeExtent& flutterNode);
    void PerformScrollDownAction(int64_t elementId,
                                 ArkUI_Accessibility_ActionType action,
                                 SemanticsNodeExtent& flutterNode);
    void PerformClipboardAction(int64_t elementId,
                                const ArkUI_Accessibility_ActionType& action);
    void PerformInvalidAction(int64_t elementId,
                              ArkUI_Accessibility_ActionType action,
                              const SemanticsNodeExtent& flutterNode);
    void PerformSetText(SemanticsNodeExtent& flutterNode,
                        ArkUI_Accessibility_ActionType action,
                        ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSelectText(const SemanticsNodeExtent& flutterNode,
                           ArkUI_Accessibility_ActionType action,
                           ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformSetCursorPosition(SemanticsNodeExtent flutterNode,
                                  ArkUI_Accessibility_ActionType action,
                                  ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformCustomAction(const SemanticsNodeExtent& flutterNode,
                             ArkUI_Accessibility_ActionType action,
                             ArkUI_AccessibilityActionArguments* actionArguments);
    void PerformShowOnScreenAction(const SemanticsNodeExtent& flutterNode);

    void GetSemanticsNodeDebugInfo(const SemanticsNodeExtent& node);
    void GetSemanticsFlagsDebugInfo(const SemanticsNodeExtent& node);
    void GetCustomActionDebugInfo(
        const flutter::CustomAccessibilityAction& customAccessibilityAction);

    void RequestFocusWhenPageUpdate(int32_t requestFocusId);
    void GetSemanticsDebugInfo();
    void AccessibiltiyChangesWithXComponentId();
};

}  // namespace flutter
#endif  // OHOS_ACCESSIBILITY_BRIDGE_H
