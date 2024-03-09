/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
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

#ifndef PLATFORM_OHOS_OHOS_XCOMPONENT_ADAPTER_H
#define PLATFORM_OHOS_OHOS_XCOMPONENT_ADAPTER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <map>
#include <string>

#include "flutter/shell/platform/ohos/ohos_touch_processor.h"
#include "napi/native_api.h"
#include "napi_common.h"

namespace flutter {

class XComponentBase {
 public:
    explicit XComponentBase(const std::string& id);
    ~XComponentBase();

    void AttachFlutterEngine(const std::string& shell_holder_id);
    void DetachFlutterEngine();
    void SetNativeXComponent(OH_NativeXComponent* native_xcomponent);

    // Callback, called by ACE XComponent
    void OnSurfaceCreated(OH_NativeXComponent* component, void* window);
    void OnSurfaceChanged(OH_NativeXComponent* component, void* window);
    void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window);
    void OnDispatchTouchEvent(OH_NativeXComponent* component, void* window);

    OH_NativeXComponent* native_xcomponent() const { return native_xcomponent_; }

 private:
    void BindXComponentCallback();

    OH_NativeXComponent_TouchEvent touch_event_;
    OH_NativeXComponent_Callback callback_;
    std::string id_;
    std::string shell_holder_id_;
    bool isEngineAttached_;
    OH_NativeXComponent* native_xcomponent_;
    void* window_;
    uint64_t width_;
    uint64_t height_;
    OhosTouchProcessor ohos_touch_processor_;
};

using XComponentBaseMap = std::map<std::string, XComponentBase*>;
class XComponentAdapter {
 public:
    static XComponentAdapter* GetInstance();

    // Forbid copy and assign
    XComponentAdapter(const XComponentAdapter&) = delete;
    XComponentAdapter& operator=(const XComponentAdapter&) = delete;

    bool Export(napi_env env, napi_value exports);
    void SetNativeXComponent(const std::string& id,
                             OH_NativeXComponent* native_xcomponent);
    void AttachFlutterEngine(const std::string& id,
                             const std::string& shell_holder_id);
    void DetachFlutterEngine(const std::string& id);

    XComponentBaseMap& xcomponent_map() { return xcomponent_map_; }

 private:
    XComponentAdapter();
    ~XComponentAdapter();

    XComponentBaseMap xcomponent_map_;
};

}  // namespace flutter

#endif  // PLATFORM_OHOS_OHOS_XCOMPONENT_ADAPTER_H