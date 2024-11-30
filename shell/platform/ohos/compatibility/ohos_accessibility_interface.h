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
 
/**
 * @addtogroup ArkUI_Accessibility
 * @{
 *
 * @brief Describes the native capabilities supported by ArkUI Accessibility, such as querying accessibility nodes and
 * reporting accessibility events.
 *
 * @since 13
 */

/**
 * @file native_interface_accessibility.h
 *
 * @brief Declares the APIs used to access the native Accessibility.
 *
 * @library libace_ndk.z.so
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @kit ArkUI
 * @since 13
 */
#ifndef OHOS_ACCESSIBILITY_INTERFACE_H
#define OHOS_ACCESSIBILITY_INTERFACE_H

#include <arkui/native_interface_accessibility.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtains the pointer to the <b> ArkUI_AccessibilityProvider</b>
 * instance of this <b>OH_NativeXComponent</b> instance.
 *
 * @param component Indicates the pointer to the <b>OH_NativeXComponent</b> instance.
 * @param handle Indicates the pointer to the <b>ArkUI_AccessibilityProvider</b> instance.
 * @return Returns {@link OH_NATIVEXCOMPONENT_RESULT_SUCCESS} if the operation is successful.
 *         Returns {@link OH_NATIVEXCOMPONENT_RESULT_BAD_PARAMETER} if a parameter error occurs.
 * @since 13
 */
int32_t OH_NativeXComponent_GetNativeAccessibilityProvider(
    OH_NativeXComponent* component, ArkUI_AccessibilityProvider** handle);

/**
 * @brief Registers a callback for this <b>ArkUI_AccessibilityProvider</b> instance.
 *
 * @param provider Indicates the pointer to the <b>ArkUI_AccessibilityProvider</b> instance.
 * @param callbacks Indicates the pointer to the <b>GetAccessibilityNodeCursorPosition</b> callback.
 * @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
 *         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
 * @since 13
 */
int32_t OH_ArkUI_AccessibilityProviderRegisterCallback(
    ArkUI_AccessibilityProvider* provider, ArkUI_AccessibilityProviderCallbacks* callbacks);

/**
 * @brief Sends accessibility event information.
 *
 * @param provider Indicates the pointer to the <b>ArkUI_AccessibilityProvider</b> instance.
 * @param eventInfo Indicates the pointer to the accessibility event information.
 * @param callback Indicates the pointer to the callback that is called after the event is sent.
 * @since 13
 */
void OH_ArkUI_SendAccessibilityAsyncEvent(
    ArkUI_AccessibilityProvider* provider, ArkUI_AccessibilityEventInfo* eventInfo,
    void (*callback)(int32_t errorCode));

/**
 * @brief Adds and obtains the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
 *
 * @param list Indicates the pointer to an <b>ArkUI_AccessibilityElementInfoList</b> object.
 * @return Returns the pointer to the <b>ArkUI_AccessibilityElementInfo</b> object.
 * @since 13
 */
ArkUI_AccessibilityElementInfo* OH_ArkUI_AddAndGetAccessibilityElementInfo(
    ArkUI_AccessibilityElementInfoList* list);

/**
* @brief Sets the element ID for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param elementId Indicates the element ID.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetElementId(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t elementId);

/**
* @brief Sets the parent ID for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param parentId Indicates the parent ID.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetParentId(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t parentId);

/**
* @brief Sets the component type for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param componentType Indicates the component type.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetComponentType(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* componentType);

/**
* @brief Sets the component content for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param contents Indicates the component content.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetContents(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* contents);

/**
* @brief Sets the hint text for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param hintText Indicates the hint text.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetHintText(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* hintText);

/**
* @brief Sets the accessibility text for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param accessibilityText Indicates the accessibility text.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* accessibilityText);

/**
* @brief Sets the accessibility description for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param accessibilityDescription Indicates the accessibility description.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityDescription(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* accessibilityDescription);

/**
* @brief Set the number of child nodes and child node IDs for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param childCount Indicates the number of child nodes.
* @param childNodeIds Indicates an array of child node IDs.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t childCount, int64_t* childNodeIds);

/**
* @brief Sets the operation actions for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param operationCount Indicates the operation count.
* @param operationActions Indicates the operation actions.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetOperationActions(ArkUI_AccessibilityElementInfo* elementInfo,
    int32_t operationCount, ArkUI_AccessibleAction* operationActions);

/**
* @brief Sets the screen area for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param screenRect Indicates the screen area.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetScreenRect(
    ArkUI_AccessibilityElementInfo* elementInfo, ArkUI_AccessibleRect* screenRect);

/**
* @brief Sets whether the element is checkable for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param checkable Indicates whether the element is checkable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetCheckable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool checkable);

/**
* @brief Sets whether the element is checked for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param checked Indicates whether the element is checked.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetChecked(
    ArkUI_AccessibilityElementInfo* elementInfo, bool checked);

/**
* @brief Sets whether the element is focusable for an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param focusable Indicates whether the element is focusable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetFocusable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool focusable);

/**
* @brief Sets whether the element is focused for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param isFocused Indicates whether the element is focused.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetFocused(
    ArkUI_AccessibilityElementInfo* elementInfo, bool isFocused);

/**
* @brief Sets whether the element is visible for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param isVisible Indicates whether the element is visible.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetVisible(
    ArkUI_AccessibilityElementInfo* elementInfo, bool isVisible);

/**
* @brief Sets the accessibility focus state for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param accessibilityFocused Indicates whether the element has accessibility focus.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused(
    ArkUI_AccessibilityElementInfo* elementInfo, bool accessibilityFocused);

/**
* @brief Sets whether the element is selected for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param selected Indicates whether the element is selected.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetSelected(
    ArkUI_AccessibilityElementInfo* elementInfo, bool selected);

/**
* @brief Sets whether the element is clickable for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param clickable Indicates whether the element is clickable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetClickable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool clickable);

/**
* @brief Sets whether the element is long clickable for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param longClickable Indicates whether the element is long clickable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetLongClickable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool longClickable);

/**
* @brief Sets whether the element is enabled for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param isEnabled Indicates whether the element is enabled.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetEnabled(
    ArkUI_AccessibilityElementInfo* elementInfo, bool isEnabled);

/**
* @brief Sets whether the element is a password for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param isPassword Indicates whether the element is a password.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetIsPassword(
    ArkUI_AccessibilityElementInfo* elementInfo, bool isPassword);

/**
* @brief Sets whether the element is scrollable for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param scrollable Indicates whether the element is scrollable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetScrollable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool scrollable);

/**
* @brief Sets whether the element is editable for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param editable Indicates whether the element is editable.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetEditable(
    ArkUI_AccessibilityElementInfo* elementInfo, bool editable);

/**
* @brief Sets whether the element is a hint for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param isHint Indicates whether the element is a hint.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetIsHint(
    ArkUI_AccessibilityElementInfo* elementInfo, bool isHint);

/**
* @brief Sets the range information for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param rangeInfo Indicates the range information.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetRangeInfo(
    ArkUI_AccessibilityElementInfo* elementInfo, ArkUI_AccessibleRangeInfo* rangeInfo);

/**
* @brief Sets the grid information for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param gridInfo Indicates the grid information.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetGridInfo(
    ArkUI_AccessibilityElementInfo* elementInfo, ArkUI_AccessibleGridInfo* gridInfo);

/**
* @brief Sets the grid item for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param gridItem Indicates the grid item.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetGridItemInfo(
    ArkUI_AccessibilityElementInfo* elementInfo, ArkUI_AccessibleGridItemInfo* gridItem);

/**
* @brief Sets the starting index of the selected text for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param selectedTextStart Indicates the starting index of the selected text
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetSelectedTextStart(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t selectedTextStart);

/**
* @brief Sets the end index of the selected text for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param selectedTextEnd Indicates the end index of the selected text
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetSelectedTextEnd(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t selectedTextEnd);

/**
* @brief Sets the index of the currently selected item for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param currentItemIndex Indicates the index of the currently selected item.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t currentItemIndex);

/**
* @brief Sets the index of the first item for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param startItemIndex Indicates the index of the first item.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetStartItemIndex(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t startItemIndex);

/**
* @brief Sets the index of the last item for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param endItemIndex Indicates the index of the last item.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetEndItemIndex(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t endItemIndex);

/**
* @brief Sets the number of items for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param itemCount Indicates the number of items.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetItemCount(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t itemCount);

/**
* @brief Sets the offset for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param offset Indicates the scroll pixel offset relative to the top of the element.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityOffset(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t offset);

/**
* @brief Sets the accessibility group for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param accessibilityGroup Indicates the accessibility group.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(
    ArkUI_AccessibilityElementInfo* elementInfo, bool accessibilityGroup);

/**
* @brief Sets the accessibility level for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param accessibilityLevel Indicates the accessibility level.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* accessibilityLevel);

/**
* @brief Sets the z-index for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param zIndex Indicates the z-index value.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetZIndex(
    ArkUI_AccessibilityElementInfo* elementInfo, int32_t zIndex);

/**
* @brief Sets the opacity for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param opacity Indicates the opacity.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetAccessibilityOpacity(
    ArkUI_AccessibilityElementInfo* elementInfo, float opacity);

/**
* @brief Sets the background color for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param backgroundColor Indicates the background color.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetBackgroundColor(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* backgroundColor);

/**
* @brief Sets the background image for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param backgroundImage Indicates the backgroundImage.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetBackgroundImage(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* backgroundImage);

/**
* @brief Sets the blur effect for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param blur Indicates the blur effect.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetBlur(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* blur);

/**
* @brief Sets the hit test behavior for an <b>ArkUI_AccessibilityElementInfo</b> object.
*
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @param hitTestBehavior Indicates the hit test behavior.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityElementInfoSetHitTestBehavior(
    ArkUI_AccessibilityElementInfo* elementInfo, const char* hitTestBehavior);

/**
 * @brief Creates an <b>ArkUI_AccessibilityElementInfo</b> object.
 *
 * @return Returns the <b>ArkUI_AccessibilityElementInfo</b> object, or NULL if it fails to create.
 *         The possible reason for failure is that the memory error occurred during object creation.
 * @since 13
 * @version 1.0
 */
ArkUI_AccessibilityElementInfo* OH_ArkUI_CreateAccessibilityElementInfo(void);

/**
 * @brief Destroys an <b>ArkUI_AccessibilityElementInfo</b> object.
 *
 * @param elementInfo Indicates the pointer to the <b>ArkUI_AccessibilityElementInfo</b> object to destroy.
 * @since 13
 * @version 1.0
 */
void OH_ArkUI_DestoryAccessibilityElementInfo(ArkUI_AccessibilityElementInfo* elementInfo);

/**
 * @brief Creates an <b>ArkUI_AccessibilityEventInfo</b> object.
 *
 * @return Returns the <b>ArkUI_AccessibilityEventInfo</b> object, or NULL if it fails to create.
 *         The possible reason for failure is that the memory error occurred during object creation.
 * @since 13
 */
ArkUI_AccessibilityEventInfo* OH_ArkUI_CreateAccessibilityEventInfo(void);

/**
 * @brief Destroys an <b>ArkUI_AccessibilityEventInfo</b> object.
 *
 * @param eventInfo Indicates the pointer to the <b>ArkUI_AccessibilityEventInfo</b> object to destroy.
 * @since 13
 */
void OH_ArkUI_DestoryAccessibilityEventInfo(ArkUI_AccessibilityEventInfo* eventInfo);

/**
* @brief Sets the event type for an <b>ArkUI_AccessibilityEventInfo</b> object.
*
* @param eventInfo Indicates the pointer to an <b>ArkUI_AccessibilityEventInfo</b> object.
* @param eventType Indicates the event type.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityEventSetEventType(
    ArkUI_AccessibilityEventInfo* eventInfo,  ArkUI_AccessibilityEventType eventType);

/**
* @brief Sets the text announced for accessibility for an <b>ArkUI_AccessibilityEventInfo</b> object.
*
* @param eventInfo Indicates the pointer to an <b>ArkUI_AccessibilityEventInfo</b> object.
* @param textAnnouncedForAccessibility Indicates the text announced for accessibility.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility(
    ArkUI_AccessibilityEventInfo* eventInfo,  const char* textAnnouncedForAccessibility);

/**
* @brief Sets the request focus ID for an <b>ArkUI_AccessibilityEventInfo</b> object.
*
* @param eventInfo Indicates the pointer to an <b>ArkUI_AccessibilityEventInfo</b> object.
* @param requestFocusId Indicates the request focus ID.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityEventSetRequestFocusId(
    ArkUI_AccessibilityEventInfo* eventInfo,  int32_t requestFocusId);

/**
* @brief Sets the element information for an <b>ArkUI_AccessibilityEventInfo</b> object.
*
* @param eventInfo Indicates the pointer to an <b>ArkUI_AccessibilityEventInfo</b> object.
* @param elementInfo Indicates the pointer to an <b>ArkUI_AccessibilityElementInfo</b> object.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_AccessibilityEventSetElementInfo(
    ArkUI_AccessibilityEventInfo* eventInfo,  ArkUI_AccessibilityElementInfo* elementInfo);

/**
* @brief Obtains the value of a key from an <b>ArkUI_AccessibilityActionArguments</b> object.
*
* @param arguments Indicates the pointer to an <b>ArkUI_AccessibilityActionArguments</b> object.
* @param key Indicates the key.
* @param value Indicates the value.
* @return Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL} if the operation is successful.
*         Returns {@link ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER} if a parameter is incorrect.
* @since 13
*/
int32_t OH_ArkUI_FindAccessibilityActionArgumentByKey(
    ArkUI_AccessibilityActionArguments* arguments, const char* key, char** value);
#ifdef __cplusplus
};
#endif
#endif // _NATIVE_INTERFACE_ACCESSIBILITY_H
/** @} */