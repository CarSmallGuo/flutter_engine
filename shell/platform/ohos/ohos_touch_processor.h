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

#ifndef OHOS_TOUCH_PROCESSOR_H
#define OHOS_TOUCH_PROCESSOR_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <vector>
#include <string>
#include "flutter/lib/ui/window/pointer_data.h"
#include "napi_common.h"

namespace flutter {

class OhosTouchProcessor {
public:
    typedef struct {
      OH_NativeXComponent_TouchEvent* touchEventInput;
      OH_NativeXComponent_TouchPointToolType toolTypeInput;
      float tiltX;
      float tiltY;
    } TouchPacket;

public:
    void HandleTouchEvent(int64_t shell_holderID,
                            OH_NativeXComponent* component,
                            OH_NativeXComponent_TouchEvent* touchEvent);
    void HandleMouseEvent(int64_t shell_holderID,
                            OH_NativeXComponent* component,
                            OH_NativeXComponent_MouseEvent mouseEvent,
                            double offsetY);
    void HandleVirtualTouchEvent(int64_t shell_holderID,
                                OH_NativeXComponent* component,
                                OH_NativeXComponent_TouchEvent* touchEvent);
    flutter::PointerData::Change getPointerChangeForAction(int maskedAction);
    flutter::PointerData::DeviceKind getPointerDeviceTypeForToolType(
        int toolType);
    flutter::PointerData::Change getPointerChangeForMouseAction(
        OH_NativeXComponent_MouseEventAction mouseAction);
    PointerButtonMouse getPointerButtonFromMouse(
        OH_NativeXComponent_MouseEventButton mouseButton);

public:
    OH_NativeXComponent_TouchPointToolType touchType_;

private:
    std::shared_ptr<std::string[]> packagePacketData(std::unique_ptr<OhosTouchProcessor::TouchPacket> touchPacket);

    void PlatformViewOnTouchEvent(int64_t shellHolderID,
                                OH_NativeXComponent_TouchPointToolType toolType,
                                OH_NativeXComponent* component,
                                OH_NativeXComponent_TouchEvent* touchEvent);
};
}  // namespace flutter
#endif  // XComponent_OhosTouchProcessor_H