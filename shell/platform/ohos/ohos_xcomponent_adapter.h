/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
 * All rights reserved. Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE_KHZG file.
 */

#ifndef OHOS_XCOMPONENT_ADAPTER_H
#define OHOS_XCOMPONENT_ADAPTER_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_interface_accessibility.h>
#include <deviceinfo.h>
#include <map>
#include <mutex>
#include <string>
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/ohos_touch_processor.h"
#include "flutter/shell/platform/ohos/utils/ohos_utils.h"
#include "napi/native_api.h"
#include "napi_common.h"
#include "ohos_shell_holder.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_ddl.h"
#include "flutter/shell/platform/ohos/utils/arkui_accessibility_constant.h"
namespace flutter {

class XComponentBase {
 private:
  void BindXComponentCallback();
  void BindAccessibilityProviderCallback();

 public:
  XComponentBase(std::string id);
  ~XComponentBase();

  void AttachFlutterEngine(std::string shellholderId);
  void DetachFlutterEngine();
  void SetNativeXComponent(OH_NativeXComponent* nativeXComponent);

  // Callback, called by ACE XComponent
  void OnSurfaceCreated(OH_NativeXComponent* component, void* window);
  void OnSurfaceChanged(OH_NativeXComponent* component, void* window);
  void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window);
  void OnDispatchTouchEvent(OH_NativeXComponent* component, void* window);
  void OnDispatchMouseEvent(OH_NativeXComponent* component, void* window);
  void OnDispatchMouseWheelEvent(mouseWheelEvent event);
  void OnDispatchMouseLeaveEvent(OH_NativeXComponent* component);

  // Accessibility callback
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
  int32_t ClearFocusedFocusAccessibilityNode(int64_t elementId);
  int32_t GetAccessibilityNodeCursorPosition(int64_t elementId,
                                             int32_t requestId,
                                             int32_t* index);
  ArkUI_AccessibilityProvider* GetArkUIAccessibilityServiceProvider(
      OH_NativeXComponent* nativeXComponent);

  OH_NativeXComponent_TouchEvent touchEvent_;
  OH_NativeXComponent_Callback callback_;
  OH_NativeXComponent_MouseEvent_Callback mouseCallback_;
  ArkUI_AccessibilityProviderCallbacks accessibilityProviderCallback_;

  std::string id_;
  std::string shellholderId_;
  bool isEngineAttached_;
  bool isWindowAttached_;
  OH_NativeXComponent* nativeXComponent_;
  void* window_;
  uint64_t width_;
  uint64_t height_;
  OhosTouchProcessor ohosTouchProcessor_;
  ArkUI_AccessibilityProvider* provider_;
  OHOSShellHolder* shellholder_ptr_ = nullptr;
};

class XComponentAdapter {
 public:
  XComponentAdapter(/* args */);
  ~XComponentAdapter();
  static XComponentAdapter* GetInstance();
  bool Export(napi_env env, napi_value exports);
  void SetNativeXComponent(std::string& id,
                           OH_NativeXComponent* nativeXComponent);
  void AttachFlutterEngine(std::string& id, std::string& shellholderId);
  void DetachFlutterEngine(std::string& id);
  void OnMouseWheel(std::string& id, mouseWheelEvent event);

  XComponentBase* GetCurrentXcomponent();
  void SetCurrentXcomponentId(std::string id);

 public:
  std::map<std::string, XComponentBase*> xcomponetMap_;
  std::string current_xcomponent_id_ = "";
  std::mutex xcomponentMap_mutex_;

 private:
  static XComponentAdapter mXComponentAdapter;
};

}  // namespace flutter

#endif