/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OHOS_XCOMPONENT_ADAPTER_H
#define OHOS_XCOMPONENT_ADAPTER_H
#include <deviceinfo.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_interface_accessibility.h>
#include <string>
#include <map>
#include <mutex>
#include "flutter/shell/platform/ohos/ohos_touch_processor.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "napi/native_api.h"
#include "napi_common.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"
#include "flutter/shell/platform/ohos/utils/ddl_utils.h"
namespace flutter {

class XComponentBase
{
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

  void RegisterArkUIAccessibilityService(
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
  ArkUI_AccessibilityProvider* accessibilityProvider_;
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

  ArkUI_AccessibilityProvider* GetAccessibilityProvider(const std::string& xcompId);

 public:
  std::map<std::string, XComponentBase*> xcomponetMap_;
  std::string currentXComponentId_;
  std::mutex mutex_;

 private:
  static XComponentAdapter mXComponentAdapter;
};

}  // namespace flutter

#endif