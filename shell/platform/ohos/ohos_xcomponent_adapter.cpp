/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_KHZG file.
 */

#include "ohos_xcomponent_adapter.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "types.h"
#include "ohos_logging.h"
#include <functional>
#include "flutter/fml/logging.h"

namespace flutter {

bool g_isMouseLeftActive = false;
double g_scrollDistance = 0.0;
double g_resizeRate = 0.8;

XComponentAdapter XComponentAdapter::mXComponentAdapter;

XComponentAdapter::XComponentAdapter(/* args */) {}

XComponentAdapter::~XComponentAdapter() {}

XComponentAdapter* XComponentAdapter::GetInstance() {
  return &XComponentAdapter::mXComponentAdapter;
}

bool XComponentAdapter::Export(napi_env env, napi_value exports) {
  napi_status status;
  napi_value exportInstance = nullptr;
  OH_NativeXComponent* nativeXComponent = nullptr;
  int32_t ret;
  char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
  uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;

  status = napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ,
                                   &exportInstance);
  LOGD("napi_get_named_property,status = %{public}d", status);
  if (status != napi_ok) {
    return false;
  }

  status = napi_unwrap(env, exportInstance,
                       reinterpret_cast<void**>(&nativeXComponent));
  LOGD("napi_unwrap,status = %{public}d", status);
  if (status != napi_ok) {
    return false;
  }

  ret = OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize);
  LOGD("NativeXComponent id:%{public}s", idStr);
  if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    return false;
  }
  std::string id(idStr);
  auto context = XComponentAdapter::GetInstance();
  if (context) {
    context->SetNativeXComponent(id, nativeXComponent);
    return true;
  }

  return false;
}

void XComponentAdapter::SetNativeXComponent(
    std::string& id,
    OH_NativeXComponent* nativeXComponent) {
  auto iter = xcomponetMap_.find(id);
  if (iter == xcomponetMap_.end()) {
    XComponentBase* xcomponet = new XComponentBase(id);
    xcomponetMap_[id] = xcomponet;
  }

  iter = xcomponetMap_.find(id);
  if (iter != xcomponetMap_.end()) {
    iter->second->SetNativeXComponent(nativeXComponent);
  }
}

void XComponentAdapter::AttachFlutterEngine(std::string& id,
                                            std::string& shellholderId) {
  auto iter = xcomponetMap_.find(id);
  if (iter == xcomponetMap_.end()) {
    XComponentBase* xcomponet = new XComponentBase(id);
    xcomponetMap_[id] = xcomponet;
  }

  auto findIter = xcomponetMap_.find(id);
  if (findIter != xcomponetMap_.end()) {
    findIter->second->AttachFlutterEngine(shellholderId);
  }
}

void XComponentAdapter::DetachFlutterEngine(std::string& id) {
  auto iter = xcomponetMap_.find(id);
  if (iter != xcomponetMap_.end()) {
    iter->second->DetachFlutterEngine();
  }
}

void XComponentAdapter::OnMouseWheel(std::string& id, mouseWheelEvent event)
{
    auto iter = xcomponetMap_.find(id);
    if (iter != xcomponetMap_.end()) {
        iter->second->OnDispatchMouseWheelEvent(event);
    }
}

#include <native_window/external_window.h>

static int32_t SetNativeWindowOpt(OHNativeWindow* nativeWindow,
                                  int32_t width,
                                  int height) {
  // Set the read and write scenarios of the native window buffer.
  uint64_t usage = 0;
  int ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, GET_USAGE, &usage);
  usage |= BUFFER_USAGE_MEM_DMA | (BUFFER_USAGE_HW_COMPOSER);
  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, SET_USAGE, usage);
  if (ret) {
    LOGE(
        "Set NativeWindow Usage Failed :window:%{public}p ,w:%{public}d x "
        "%{public}d:%{public}d",
        nativeWindow, width, height, ret);
  }
  // Set the width and height of the native window buffer.
  int code = SET_BUFFER_GEOMETRY;
  ret =
      OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, width, height);
  if (ret) {
    LOGE(
        "Set NativeWindow GEOMETRY  Failed :window:%{public}p ,w:%{public}d x "
        "%{public}d:%{public}d",
        nativeWindow, width, height, ret);
  }
  // Set the step of the native window buffer.
  code = SET_STRIDE;
  int32_t stride = 0x8;
  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, stride);
  if (ret) {
    LOGE(
        "Set NativeWindow stride   Failed :window:%{public}p ,w:%{public}d x "
        "%{public}d:%{public}d",
        nativeWindow, width, height, ret);
  }
  // Set the format of the native window buffer.
  code = SET_FORMAT;
  int32_t format = PIXEL_FMT_RGBA_8888;

  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, format);
  if (ret) {
    LOGE(
        "Set NativeWindow PIXEL_FMT_RGBA_8888   Failed :window:%{public}p "
        ",w:%{public}d x %{public}d:%{public}d",
        nativeWindow, width, height, ret);
  }

  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, SET_TIMEOUT, 0);
  if (ret) {
    LOGE(
        "Set NativeWindow SET_TIMEOUT   Failed :window:%{public}p "
        ",w:%{public}d x %{public}d:%{public}d",
        nativeWindow, width, height, ret);
  }
  return ret;
}

void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
  for(auto it: XComponentAdapter::GetInstance()->xcomponetMap_)
  {
    if(it.second->nativeXComponent_ == component) {
      LOGD("OnSurfaceCreatedCB is called");
      it.second->OnSurfaceCreated(component, window);
    }
  }
}

void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
  for(auto it: XComponentAdapter::GetInstance()->xcomponetMap_)
  {
    if(it.second->nativeXComponent_ == component) {
      it.second->OnSurfaceChanged(component, window);
    }
  }
}

void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
  std::lock_guard<std::mutex> lock(XComponentAdapter::GetInstance()->mutex_);
  for(auto it = XComponentAdapter::GetInstance()->xcomponetMap_.begin(); 
    it != XComponentAdapter::GetInstance()->xcomponetMap_.end();)
  {
    if(it->second->nativeXComponent_ == component) {
      it->second->OnSurfaceDestroyed(component, window);
      delete it->second;
      // 将当前要销毁的xcomponent对应的无障碍provider指针置nullptr
      it->second->accessibilityProvider_ = nullptr;
      // delete the semantics tree of the destroyed xcomponent
      // OhosAccessibilityBridge::GetInstance()->g_flutterSemanticsTreeXComponents.erase(it->first);
      it = XComponentAdapter::GetInstance()->xcomponetMap_.erase(it);
    } else {
      ++it;
    }
  }

}
void DispatchTouchEventCB(OH_NativeXComponent* component, void* window) {
  for(auto it: XComponentAdapter::GetInstance()->xcomponetMap_)
  {
    if(it.second->nativeXComponent_ == component) {
      it.second->OnDispatchTouchEvent(component, window);
    }
  }
}

void DispatchMouseEventCB(OH_NativeXComponent* component, void* window)
{
    for (auto it: XComponentAdapter::GetInstance()->xcomponetMap_) {
        if (it.second->nativeXComponent_ == component) {
            it.second->OnDispatchMouseEvent(component, window);
        }
    }
}

void DispatchHoverEventCB(OH_NativeXComponent* component, bool isHover)
{
    LOGD("XComponentManger::DispatchHoverEventCB");
}

void XComponentBase::BindXComponentCallback() {
  callback_.OnSurfaceCreated = OnSurfaceCreatedCB;
  callback_.OnSurfaceChanged = OnSurfaceChangedCB;
  callback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
  callback_.DispatchTouchEvent = DispatchTouchEventCB;
  mouseCallback_.DispatchMouseEvent = DispatchMouseEventCB;
  mouseCallback_.DispatchHoverEvent = DispatchHoverEventCB;
}


/** Called when need to get element infos based on a specified node. */
int32_t FindAccessibilityNodeInfosById(
    int64_t elementId,
    ArkUI_AccessibilitySearchMode mode,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList)
{
  OhosAccessibilityBridge::GetInstance()->FindAccessibilityNodeInfosById(elementId, mode, requestId, elementList);
  FML_DLOG(INFO) << "accessibilityProviderCallback_.FindAccessibilityNodeInfosById";
  return 0;
}

/** Called when need to get element infos based on a specified node and text content. */
int32_t FindAccessibilityNodeInfosByText(
    int64_t elementId,
    const char* text,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList)
{
  OhosAccessibilityBridge::GetInstance()->FindAccessibilityNodeInfosByText(elementId, text, requestId, elementList);
  LOGD("accessibilityProviderCallback_.FindAccessibilityNodeInfosByText");
  return 0;
}

/** Called when need to get the focused element info based on a specified node. */
int32_t FindFocusedAccessibilityNode(
    int64_t elementId, 
    ArkUI_AccessibilityFocusType focusType,
    int32_t requestId, 
    ArkUI_AccessibilityElementInfo* elementinfo)
{
  OhosAccessibilityBridge::GetInstance()->FindFocusedAccessibilityNode(elementId, focusType, requestId, elementinfo);
  LOGD("accessibilityProviderCallback_.FindFocusedAccessibilityNode");
  return 0;
}

/** Query the node that can be focused based on the reference node. Query the next node that can be focused based on the mode and direction. */
int32_t FindNextFocusAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo *elementList)
{
  OhosAccessibilityBridge::GetInstance()->FindNextFocusAccessibilityNode(elementId, direction, requestId, elementList);
  LOGD("accessibilityProviderCallback_.FindNextFocusAccessibilityNode");
  return 0;
}

/** Performing the Action operation on a specified node. */
int32_t ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments,
    int32_t requestId)
{
  OhosAccessibilityBridge::GetInstance()->ExecuteAccessibilityAction(elementId, action, actionArguments, requestId);
  LOGD("accessibilityProviderCallback_.ExecuteAccessibilityAction");
  return 0;
}

/** Clears the focus status of the currently focused node */
int32_t ClearFocusedFocusAccessibilityNode()
{
  LOGD("accessibilityProviderCallback_.ClearFocusedFocusAccessibilityNode");
  return 0;
}

/** Queries the current cursor position of a specified node. */
int32_t GetAccessibilityNodeCursorPosition(
    int64_t elementId,
    int32_t requestId,
    int32_t* index)
{
  OhosAccessibilityBridge::GetInstance()->GetAccessibilityNodeCursorPosition(elementId, requestId, index);
  LOGD("accessibilityProviderCallback_.GetAccessibilityNodeCursorPosition");
  return 0;
}

void XComponentBase::BindAccessibilityProviderCallback() {
  accessibilityProviderCallback_.findAccessibilityNodeInfosById = FindAccessibilityNodeInfosById;
  accessibilityProviderCallback_.findAccessibilityNodeInfosByText = FindAccessibilityNodeInfosByText;
  accessibilityProviderCallback_.findFocusedAccessibilityNode = FindFocusedAccessibilityNode;
  accessibilityProviderCallback_.findNextFocusAccessibilityNode = FindNextFocusAccessibilityNode;
  accessibilityProviderCallback_.executeAccessibilityAction = ExecuteAccessibilityAction;
  accessibilityProviderCallback_.clearFocusedFocusAccessibilityNode = ClearFocusedFocusAccessibilityNode;
  accessibilityProviderCallback_.getAccessibilityNodeCursorPosition = GetAccessibilityNodeCursorPosition;
}

XComponentBase::XComponentBase(std::string id){
  id_ = id;
  isEngineAttached_ = false;
}

XComponentBase::~XComponentBase() {}

void XComponentBase::AttachFlutterEngine(std::string shellholderId) {
  LOGD(
      "XComponentManger::AttachFlutterEngine xcomponentId:%{public}s, "
      "shellholderId:%{public}s",
      id_.c_str(), shellholderId.c_str());
  shellholderId_ = shellholderId;
  isEngineAttached_ = true;
  if (window_ != nullptr) {
    PlatformViewOHOSNapi::SurfaceCreated(std::stoll(shellholderId_), window_);
  } else {
    LOGE("OnSurfaceCreated XComponentBase is not attached");
  }
}

void XComponentBase::DetachFlutterEngine() {
  LOGD(
      "XComponentManger::DetachFlutterEngine xcomponentId:%{public}s, "
      "shellholderId:%{public}s",
      id_.c_str(), shellholderId_.c_str());
  if (window_ != nullptr) {
    PlatformViewOHOSNapi::SurfaceDestroyed(std::stoll(shellholderId_));
  } else {
    LOGE("DetachFlutterEngine XComponentBase is not attached");
  }
  shellholderId_ = "";
  isEngineAttached_ = false;
}

ArkUI_AccessibilityProvider* XComponentAdapter::GetAccessibilityProvider(const std::string& xcompId)
{
    auto it = xcomponetMap_.find(xcompId);
    if (it != xcomponetMap_.end()) {
        return it->second->accessibilityProvider_;
    } else {
        return nullptr;
    }
}

void XComponentBase::RegisterArkUIAccessibilityService(OH_NativeXComponent* nativeXComponent)
{
    BindAccessibilityProviderCallback();

    auto OH_NativeXComponent_GetNativeAccessibilityProvider =
        OhosAccessibilityDDL::DLLoadGetNativeA11yProvider(ArkUIAccessibilityConstant::OH_GET_A11Y_PROVIDER);
    CHECK_DLL_NULL_PTR(OH_NativeXComponent_GetNativeAccessibilityProvider);

    ArkUI_AccessibilityProvider* a11yProvider = nullptr;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_NativeXComponent_GetNativeAccessibilityProvider(nativeXComponent, &a11yProvider)
    );

    auto OH_ArkUI_AccessibilityProviderRegisterCallback =
        OhosAccessibilityDDL::DLLoadRegisterFunc(ArkUIAccessibilityConstant::ARKUI_REGISTER_CALLBACK);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityProviderRegisterCallback);
    CHECK_NULL_PTR(a11yProvider, RegisterArkUIAccessibilityService);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityProviderRegisterCallback(a11yProvider, &accessibilityProviderCallback_)
    );

    // std::lock_guard<std::mutex> lock(XComponentAdapter::GetInstance()->mutex_);
    auto* base = XComponentAdapter::GetInstance()->xcomponetMap_[id_];
    base->accessibilityProvider_ = a11yProvider;
    base->nativeXComponent_ = nativeXComponent;

    FML_DLOG(INFO) << "RegisterArkUIAccessibilityService is finished";
}

void XComponentBase::SetNativeXComponent(OH_NativeXComponent* nativeXComponent){
  nativeXComponent_ = nativeXComponent;
  if (nativeXComponent_ != nullptr) {
    BindXComponentCallback();
    OH_NativeXComponent_RegisterCallback(nativeXComponent_, &callback_);
    OH_NativeXComponent_RegisterMouseEventCallback(nativeXComponent_, &mouseCallback_);
    // register the OH_ArkUI accessibility callbacks
    if (OH_GetSdkApiVersion() < 13) { return; }
    RegisterArkUIAccessibilityService(nativeXComponent_);
  }
}

void XComponentBase::OnSurfaceCreated(OH_NativeXComponent* component,
                                      void* window) {
  LOGD(
      "XComponentManger::OnSurfaceCreated window = %{public}p component = "
      "%{public}p",
      window, component);
      window_ = window;
  int32_t ret = OH_NativeXComponent_GetXComponentSize(component, window,
                                                      &width_, &height_);
  if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGD("XComponent Current width:%{public}d,height:%{public}d",
         static_cast<int>(width_), static_cast<int>(height_));
  } else {
    LOGE("GetXComponentSize result:%{public}d", ret);
  }

  LOGD("OnSurfaceCreated,window.size:%{public}d,%{public}d", (int)width_,
       (int)height_);
  ret = SetNativeWindowOpt((OHNativeWindow*)window, width_, height_);
  if (ret) {
    LOGE("SetNativeWindowOpt failed:%{public}d", ret);
  }
  if (isEngineAttached_) {
    ret = OH_NativeWindow_NativeObjectReference(window);
    if (ret) {
      LOGE("NativeObjectReference failed:%{public}d", ret);
    }
    PlatformViewOHOSNapi::SurfaceCreated(std::stoll(shellholderId_), window);
  } else {
    LOGE("OnSurfaceCreated XComponentBase is not attached");
  }
}

void XComponentBase::OnSurfaceChanged(OH_NativeXComponent* component, void* window)
{
  LOGD("XComponentManger::OnSurfaceChanged ");
  int32_t ret = OH_NativeXComponent_GetXComponentSize(component, window,
                                                      &width_, &height_);
  if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGD("XComponentManger::OnSurfaceChanged Current width:%{public}d,height:%{public}d",
         static_cast<int>(width_), static_cast<int>(height_));
  }
  if (isEngineAttached_) {
    PlatformViewOHOSNapi::SurfaceChanged(std::stoll(shellholderId_), width_,
                                         height_);
  } else {
    LOGE("XComponentManger::OnSurfaceChanged XComponentBase is not attached");
  }
}

void XComponentBase::OnSurfaceDestroyed(OH_NativeXComponent* component,
                                        void* window) {
  window_ = nullptr;
  LOGD("XComponentManger::OnSurfaceDestroyed");
  if (isEngineAttached_) {
    PlatformViewOHOSNapi::SurfaceDestroyed(std::stoll(shellholderId_));
    int32_t ret = OH_NativeWindow_NativeObjectUnreference(window);
    if (ret) {
      LOGE("NativeObjectUnreference failed:%{public}d", ret);
    }
  } else {
    LOGE("XComponentManger::OnSurfaceDestroyed XComponentBase is not attached");
  }
}

void XComponentBase::OnDispatchTouchEvent(OH_NativeXComponent* component,
                                          void* window) {
  int32_t ret =
      OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent_);
  if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    if (isEngineAttached_) {
      // if this touchEvent triggered by mouse, return
      OH_NativeXComponent_EventSourceType sourceType;
      int32_t ret2 = OH_NativeXComponent_GetTouchEventSourceType(component, touchEvent_.id, &sourceType);
      if (ret2 == OH_NATIVEXCOMPONENT_RESULT_SUCCESS &&
          sourceType == OH_NATIVEXCOMPONENT_SOURCE_TYPE_MOUSE) {
          ohosTouchProcessor_.HandleVirtualTouchEvent(std::stoll(shellholderId_), component, &touchEvent_);
          return;
      }
      ohosTouchProcessor_.HandleTouchEvent(std::stoll(shellholderId_),
                                           component, &touchEvent_);
    } else {
      LOGE(
          "XComponentManger::DispatchTouchEvent XComponentBase is not "
          "attached");
    }
  }
}

void XComponentBase::OnDispatchMouseEvent(OH_NativeXComponent* component, void* window)
{
    OH_NativeXComponent_MouseEvent mouseEvent;
    int32_t ret = OH_NativeXComponent_GetMouseEvent(component, window, &mouseEvent);
    if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS && isEngineAttached_) {
        if (mouseEvent.button == OH_NATIVEXCOMPONENT_LEFT_BUTTON) {
            if (mouseEvent.action == OH_NATIVEXCOMPONENT_MOUSE_PRESS) {
                g_isMouseLeftActive = true;
            } else if (mouseEvent.action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
                g_isMouseLeftActive = false;
            }
        }
        ohosTouchProcessor_.HandleMouseEvent(std::stoll(shellholderId_), component, mouseEvent, 0.0);
        return;
    }
    LOGE("XComponentManger::DispatchMouseEvent XComponentBase is not attached");
}

void XComponentBase::OnDispatchMouseWheelEvent(mouseWheelEvent event)
{
    std::string shell_holder_str = std::to_string(event.shellHolder);
    if (shell_holder_str != shellholderId_) {
        return;
    }
    if (isEngineAttached_) {
        if (g_isMouseLeftActive) {
            return;
        }
        if (event.eventType == "actionUpdate") {
            OH_NativeXComponent_MouseEvent mouseEvent;
            // 调整鼠标滚轮滚动时，列表滑动的方向。和Windows保持一致。
            double scrollY = g_scrollDistance - event.offsetY;
            g_scrollDistance = event.offsetY;
            // fix resize ratio
            mouseEvent.x = event.globalX / g_resizeRate;
            mouseEvent.y = event.globalY / g_resizeRate;
            scrollY = scrollY / g_resizeRate;
            mouseEvent.button = OH_NATIVEXCOMPONENT_NONE_BUTTON;
            mouseEvent.action = OH_NATIVEXCOMPONENT_MOUSE_NONE;
            mouseEvent.timestamp = event.timestamp;
            ohosTouchProcessor_.HandleMouseEvent(std::stoll(shellholderId_), nullptr, mouseEvent, scrollY);
        } else {
            g_scrollDistance = 0.0;
        }
    } else {
        LOGE("XComponentManger::DispatchMouseWheelEvent XComponentBase is not attached");
    }
}
}  // namespace flutter