/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef ARKUI_ACCESSIBILITY_CONSTANT_H
#define ARKUI_ACCESSIBILITY_CONSTANT_H
namespace flutter {

/**
 * 封装ArkUI无障碍C-API的符号表中接口名称常量，
 * 方便动态加载libflutter_accessiblity.so中的无障碍接口
 */
class ArkUIAccessibilityConstant {
public:
    static constexpr char OH_GET_A11Y_PROVIDER[] = "OH_NativeXComponent_GetNativeAccessibilityProvider";
    static constexpr char ARKUI_REGISTER_CALLBACK[] = "OH_ArkUI_AccessibilityProviderRegisterCallback";
    static constexpr char ARKUI_SEND_A11Y_EVENT[] = "OH_ArkUI_SendAccessibilityAsyncEvent";
    static constexpr char ARKUI_GET_A11Y_NODE[] = "OH_ArkUI_AddAndGetAccessibilityElementInfo";

    static constexpr char ARKUI_CREATE_NODE[] = "OH_ArkUI_CreateAccessibilityElementInfo";
    static constexpr char ARKUI_DESTORY_NODE[] = "OH_ArkUI_DestoryAccessibilityElementInfo";
    static constexpr char ARKUI_CREATE_EVENT[] = "OH_ArkUI_CreateAccessibilityEventInfo";
    static constexpr char ARKUI_DESTORY_EVENT[] = "OH_ArkUI_DestoryAccessibilityEventInfo";

    static constexpr char ARKUI_SET_NODE_ID[] = "OH_ArkUI_AccessibilityElementInfoSetElementId";
    static constexpr char ARKUI_SET_PARENT_ID[] = "OH_ArkUI_AccessibilityElementInfoSetParentId";
    static constexpr char ARKUI_SET_COMPONENT_TYPE[] = "OH_ArkUI_AccessibilityElementInfoSetComponentType";
    static constexpr char ARKUI_SET_CONTENTS[] = "OH_ArkUI_AccessibilityElementInfoSetContents";
    static constexpr char ARKUI_SET_HINT[] = "OH_ArkUI_AccessibilityElementInfoSetHintText";
    static constexpr char ARKUI_SET_A11Y_TEXT[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityText";
    static constexpr char ARKUI_SET_A11Y_DESC[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityDescription";
    static constexpr char ARKUI_SET_CHILD_IDS[] = "OH_ArkUI_AccessibilityElementInfoSetChildNodeIds";
    static constexpr char ARKUI_SET_ACTIONS[] = "OH_ArkUI_AccessibilityElementInfoSetOperationActions";
    static constexpr char ARKUI_SET_SCREEN_RECT[] = "OH_ArkUI_AccessibilityElementInfoSetScreenRect";
    static constexpr char ARKUI_SET_CHECKABLE[] = "OH_ArkUI_AccessibilityElementInfoSetCheckable";
    static constexpr char ARKUI_SET_CHECKED[] = "OH_ArkUI_AccessibilityElementInfoSetChecked";
    static constexpr char ARKUI_SET_FOCUSABLE[] = "OH_ArkUI_AccessibilityElementInfoSetFocusable";
    static constexpr char ARKUI_SET_FOCUSED[] = "OH_ArkUI_AccessibilityElementInfoSetFocused";
    static constexpr char ARKUI_SET_VISIBLE[] = "OH_ArkUI_AccessibilityElementInfoSetVisible";
    static constexpr char ARKUI_SET_A11Y_FOCUSED[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused";
    static constexpr char ARKUI_SET_SELECTED[] = "OH_ArkUI_AccessibilityElementInfoSetSelected";
    static constexpr char ARKUI_SET_CLICKABLE[] = "OH_ArkUI_AccessibilityElementInfoSetClickable";
    static constexpr char ARKUI_SET_LONG_PRESS[] = "OH_ArkUI_AccessibilityElementInfoSetLongClickable";
    static constexpr char ARKUI_SET_ENABLED[] = "OH_ArkUI_AccessibilityElementInfoSetEnabled";
    static constexpr char ARKUI_SET_IS_PASSWORD[] = "OH_ArkUI_AccessibilityElementInfoSetIsPassword";
    static constexpr char ARKUI_SET_SCROLLABLE[] = "OH_ArkUI_AccessibilityElementInfoSetScrollable";
    static constexpr char ARKUI_SET_EDITABLE[] = "OH_ArkUI_AccessibilityElementInfoSetEditable";
    static constexpr char ARKUI_SET_IS_HINT[] = "OH_ArkUI_AccessibilityElementInfoSetIsHint";
    static constexpr char ARKUI_SET_A11Y_GROUP[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup";
    static constexpr char ARKUI_SET_A11Y_LEVEL[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel";

    static constexpr char ARKUI_SET_CURR_ITEM_IDX[] = "OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex";
    static constexpr char ARKUI_SET_START_ITEM_IDX[] = "OH_ArkUI_AccessibilityElementInfoSetStartItemIndex";
    static constexpr char ARKUI_SET_END_ITEM_IDX[] = "OH_ArkUI_AccessibilityElementInfoSetEndItemIndex";
    static constexpr char ARKUI_SET_ITEM_COUNT[] = "OH_ArkUI_AccessibilityElementInfoSetItemCount";

    static constexpr char ARKUI_SET_EVENT_TYPE[] = "OH_ArkUI_AccessibilityEventSetEventType";
    static constexpr char ARKUI_SET_ANNOUNCED_TEXT[] = "OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility";
    static constexpr char ARKUI_SET_REQ_FOCUSED_ID[] = "OH_ArkUI_AccessibilityEventSetRequestFocusId";
    static constexpr char ARKUI_EVENT_SET_NODE[] = "OH_ArkUI_AccessibilityEventSetElementInfo";

    static constexpr char ARKUI_SET_SELECTED_START[] = "OH_ArkUI_AccessibilityElementInfoSetSelectedTextStart";
    static constexpr char ARKUI_SET_SELECTED_END[] = "OH_ArkUI_AccessibilityElementInfoSetSelectedTextEnd";
    static constexpr char ARKUI_FIND_ACTION_ARG_BY_KEY[] = "OH_ArkUI_FindAccessibilityActionArgumentByKey";

    static constexpr char ARKUI_SET_RANGE_INFO[] = "OH_ArkUI_AccessibilityElementInfoSetRangeInfo";
    static constexpr char ARKUI_SET_GRID_INFO[] = "OH_ArkUI_AccessibilityElementInfoSetGridInfo";
    static constexpr char ARKUI_SET_A11Y_OFFSET[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityOffset";
    static constexpr char ARKUI_SET_Z_INDEX[] = "OH_ArkUI_AccessibilityElementInfoSetZIndex";
    static constexpr char ARKUI_SET_A11Y_OPACITY[] = "OH_ArkUI_AccessibilityElementInfoSetAccessibilityOpacity";
    static constexpr char ARKUI_SET_BACKGROUND_COLOR[] = "OH_ArkUI_AccessibilityElementInfoSetBackgroundColor";
    static constexpr char ARKUI_SET_BRACKGROUND_IMG[] = "OH_ArkUI_AccessibilityElementInfoSetBackgroundImage";
    static constexpr char ARKUI_SET_BLUR[] = "OH_ArkUI_AccessibilityElementInfoSetBlur";
    static constexpr char ARKUI_SET_HIT_TEST_BEHAVIOR[] = "OH_ArkUI_AccessibilityElementInfoSetHitTestBehavior";
};

}
#endif