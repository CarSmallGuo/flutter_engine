// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

 private:
  std::shared_ptr<std::string[]> packagePacketData(std::unique_ptr<OhosTouchProcessor::TouchPacket> touchPacket);

 public:
  OH_NativeXComponent_TouchPointToolType touchType_;

 private:
};
}  // namespace flutter
#endif  // XComponent_OhosTouchProcessor_H