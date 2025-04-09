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

#include "flutter/shell/platform/ohos/ohos_touch_processor.h"
#include "flutter/fml/trace_event.h"
#include "flutter/lib/ui/window/pointer_data_packet.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include <arkui/ui_input_event.h>
#include <error_code.h>

namespace flutter {

constexpr int MSEC_PER_SECOND = 1000;
constexpr int PER_POINTER_MEMBER = 10;
constexpr int CHANGES_POINTER_MEMBER = 10;
constexpr int TOUCH_EVENT_ADDITIONAL_ATTRIBUTES = 4;
constexpr double ZOOM_IN = 10.0 / 8.0;
constexpr double ZOOM_OUT = 1.0 / ZOOM_IN;

PointerData::Change OhosTouchProcessor::getPointerChangeForAction(
    int maskedAction) {
  switch (maskedAction) {
    case OH_NATIVEXCOMPONENT_DOWN:
      return PointerData::Change::kDown;
    case OH_NATIVEXCOMPONENT_UP:
      return PointerData::Change::kUp;
    case OH_NATIVEXCOMPONENT_CANCEL:
      return PointerData::Change::kCancel;
    case OH_NATIVEXCOMPONENT_MOVE:
      return PointerData::Change::kMove;
  }
  return PointerData::Change::kCancel;
}

PointerData::Change OhosTouchProcessor::getPointerChangeForMouseAction(
    OH_NativeXComponent_MouseEventAction mouseAction) {
  switch (mouseAction) {
    case OH_NATIVEXCOMPONENT_MOUSE_PRESS:
      return PointerData::Change::kDown;
    case OH_NATIVEXCOMPONENT_MOUSE_RELEASE:
      return PointerData::Change::kUp;
    case OH_NATIVEXCOMPONENT_MOUSE_MOVE:
      return PointerData::Change::kMove;
    default:
      return PointerData::Change::kCancel;
  }
}

PointerButtonMouse OhosTouchProcessor::getPointerButtonFromMouse(
    OH_NativeXComponent_MouseEventButton mouseButton) {
  switch (mouseButton) {
    case OH_NATIVEXCOMPONENT_LEFT_BUTTON:
      return kPointerButtonMousePrimary;
    case OH_NATIVEXCOMPONENT_RIGHT_BUTTON:
      return kPointerButtonMouseSecondary;
    case OH_NATIVEXCOMPONENT_MIDDLE_BUTTON:
      return kPointerButtonMouseMiddle;
    case OH_NATIVEXCOMPONENT_BACK_BUTTON:
      return kPointerButtonMouseBack;
    case OH_NATIVEXCOMPONENT_FORWARD_BUTTON:
      return kPointerButtonMouseForward;
    default:
      return kPointerButtonMousePrimary;
  }
}

PointerData::DeviceKind OhosTouchProcessor::getPointerDeviceTypeForToolType(
    int toolType) {
  switch (toolType) {
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_FINGER:
      return PointerData::DeviceKind::kTouch;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_PEN:
      return PointerData::DeviceKind::kStylus;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_RUBBER:
      return PointerData::DeviceKind::kInvertedStylus;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_BRUSH:
      return PointerData::DeviceKind::kStylus;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_PENCIL:
      return PointerData::DeviceKind::kStylus;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_AIRBRUSH:
      return PointerData::DeviceKind::kStylus;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_MOUSE:
      return PointerData::DeviceKind::kMouse;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_LENS:
      return PointerData::DeviceKind::kTouch;
    case OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN:
      return PointerData::DeviceKind::kTouch;
  }
  return PointerData::DeviceKind::kTouch;
}

std::shared_ptr<std::string[]> OhosTouchProcessor::packagePacketData(
    std::unique_ptr<OhosTouchProcessor::TouchPacket> touchPacket) {
  if (touchPacket == nullptr) {
    return nullptr;
  }
  int numPoints = touchPacket->touchEventInput->numPoints;
  int offset = 0;
  int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints +
             TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
  std::shared_ptr<std::string[]> package(new std::string[size]);

  package[offset++] = std::to_string(touchPacket->touchEventInput->numPoints);

  package[offset++] = std::to_string(touchPacket->touchEventInput->id);
  package[offset++] = std::to_string(touchPacket->touchEventInput->screenX);
  package[offset++] = std::to_string(touchPacket->touchEventInput->screenY);
  package[offset++] = std::to_string(touchPacket->touchEventInput->x);
  package[offset++] = std::to_string(touchPacket->touchEventInput->y);
  package[offset++] = std::to_string(touchPacket->touchEventInput->type);
  package[offset++] = std::to_string(touchPacket->touchEventInput->size);
  package[offset++] = std::to_string(touchPacket->touchEventInput->force);
  package[offset++] = std::to_string(touchPacket->touchEventInput->deviceId);
  package[offset++] = std::to_string(touchPacket->touchEventInput->timeStamp);
  for (int i = 0; i < numPoints; i++) {
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].id);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].screenX);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].screenY);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].x);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].y);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].type);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].size);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].force);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].timeStamp);
    package[offset++] =
        std::to_string(touchPacket->touchEventInput->touchPoints[i].isPressed);
  }
  package[offset++] = std::to_string(touchPacket->toolTypeInput);
  package[offset++] = std::to_string(touchPacket->tiltX);
  package[offset++] = std::to_string(touchPacket->tiltY);
  return package;
}

void OhosTouchProcessor::HandleTouchEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    OH_NativeXComponent_TouchEvent* touchEvent) {
  if (touchEvent == nullptr) {
    return;
  }
  FML_TRACE_EVENT("flutter", "HandleTouchEvent", "timeStamp",
                  touchEvent->timeStamp);
  const int numTouchPoints = 1;
  std::unique_ptr<flutter::PointerDataPacket> packet =
      std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
  PointerData pointerData;
  pointerData.Clear();
  pointerData.embedder_id = touchEvent->id;
  pointerData.time_stamp = touchEvent->timeStamp / MSEC_PER_SECOND;
  pointerData.change = getPointerChangeForAction(touchEvent->type);
  pointerData.physical_y = touchEvent->y;
  pointerData.physical_x = touchEvent->x;
  // Delta will be generated in pointer_data_packet_converter.cc.
  pointerData.physical_delta_x = 0.0;
  pointerData.physical_delta_y = 0.0;
  pointerData.device = touchEvent->id;
  // Pointer identifier will be generated in pointer_data_packet_converter.cc.
  pointerData.pointer_identifier = 0;
  // XComponent not support Scroll
  pointerData.signal_kind = PointerData::SignalKind::kNone;
  pointerData.scroll_delta_x = 0.0;
  pointerData.scroll_delta_y = 0.0;
  pointerData.pressure = touchEvent->force;
  pointerData.pressure_max = 1.0;
  pointerData.pressure_min = 0.0;
  OH_NativeXComponent_TouchPointToolType toolType;
  OH_NativeXComponent_GetTouchPointToolType(component, 0, &toolType);
  pointerData.kind = getPointerDeviceTypeForToolType(toolType);
  // 适配PC兼容模式，kind的值可能不是kTouch，需要确保 pointerData.buttons 被赋值
  if (pointerData.change == PointerData::Change::kDown ||
      pointerData.change == PointerData::Change::kMove) {
    pointerData.buttons = kPointerButtonTouchContact;
  }
  pointerData.pan_x = 0.0;
  pointerData.pan_y = 0.0;
  // Delta will be generated in pointer_data_packet_converter.cc.
  pointerData.pan_delta_x = 0.0;
  pointerData.pan_delta_y = 0.0;
  // The contact area between the fingerpad and the screen
  pointerData.size = touchEvent->size;
  pointerData.scale = 1.0;
  pointerData.rotation = 0.0;
  packet->SetPointerData(0, pointerData);
  auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shell_holderID);
  ohos_shell_holder->GetPlatformView()->DispatchPointerDataPacket(
      std::move(packet));

  // For DFX
  fml::closure task = [timeStampDFX = touchEvent->timeStamp](void) {
    FML_TRACE_EVENT("flutter", "HandleTouchEventUI", "timeStamp", timeStampDFX);
  };
  ohos_shell_holder->GetPlatformView()->RunTask(OhosThreadType::kUI, task);
  PlatformViewOnTouchEvent(shell_holderID, toolType, component, touchEvent);
}

// 处理轴事件（触控板）：捏合缩放和滚动抛滑手势
void OhosTouchProcessor::HandleAxisEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }

  // 获取工具类型和功能键情况判断事件
  uint64_t keys = 0;
  int32_t toolType = OH_ArkUI_UIInputEvent_GetToolType(event);
  int32_t keyStates = OH_ArkUI_UIInputEvent_GetModifierKeyStates(event, &keys);
  
  if (toolType == UI_INPUT_EVENT_TOOL_TYPE_MOUSE &&
      keyStates != ERROR_CODE_PARAM_INVAILD &&
      keyStates & ARKUI_MODIFIER_KEY_CTRL) {
    // Ctrl + 鼠标滚轮
    HandleScaleEvent(shell_holderID, component, event);
  }
  else {
    // 判断手势类型并调用相应的处理函数
    float pinchScale = OH_ArkUI_AxisEvent_GetPinchAxisScaleValue(event);
    const float pinchThreshold = 0.0001;
    if (fabs(pinchScale - 0.0) > pinchThreshold) {
      // 捏合手势 
      HandlePinchEvent(shell_holderID, component, event);
    } else {
      // 滚动/抛滑手势
      HandleFlingEvent(shell_holderID, component, event);
    }
  }
  return;
}

// 处理捏合缩放手势（Pinch）：模拟双指触摸
void OhosTouchProcessor::HandlePinchEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }
  
  // 获取事件时间
  int64_t eventTime = OH_ArkUI_UIInputEvent_GetEventTime(event);

  // 获取手势中心位置（XComponent组件坐标）
  float centerX = OH_ArkUI_PointerEvent_GetX(event);
  float centerY = OH_ArkUI_PointerEvent_GetY(event);

  // 获取滑动初始位置（应用窗口坐标）
  float windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
  float windowY = OH_ArkUI_PointerEvent_GetWindowY(event);

  // 获取捏合缩放比例
  float pinchScale = OH_ArkUI_AxisEvent_GetPinchAxisScaleValue(event);
  
  // 模拟两个手指从中心向外扩张，计算偏移量
  const double baseSeparation = 0.01; // 基准分离距离（单位：点），可根据需要调整
  double separation = baseSeparation * (pinchScale - 1.0);
  double offsetX = baseSeparation + separation;
  double offsetY = baseSeparation + separation;
  
  // 获取触摸状态类型
  OH_NativeXComponent_TouchEventType type;
  switch (OH_ArkUI_AxisEvent_GetAxisAction(event)) {
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
    case UI_TOUCH_EVENT_ACTION_DOWN:
      type = OH_NATIVEXCOMPONENT_DOWN;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      type = OH_NATIVEXCOMPONENT_MOVE;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      type = OH_NATIVEXCOMPONENT_UP;
      break;
    default:
      LOGE("HandlePinchEvent: AxisAction is not defined");
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
  }

  // 生成两个模拟触摸事件
  OH_NativeXComponent_TouchEvent* touchEvent1 = new OH_NativeXComponent_TouchEvent(*reinterpret_cast<OH_NativeXComponent_TouchEvent*>(event));
  OH_NativeXComponent_TouchEvent* touchEvent2 = new OH_NativeXComponent_TouchEvent(*reinterpret_cast<OH_NativeXComponent_TouchEvent*>(event));
  
  // 设置第一个触摸事件：坐标向左上偏移
  touchEvent1->screenX = windowX - offsetX;
  touchEvent1->screenY = windowY - offsetY;
  touchEvent1->x = centerX - offsetX;
  touchEvent1->y = centerY - offsetY;
  touchEvent1->id = 0;
  touchEvent1->size = 0;
  touchEvent1->type = type;
  touchEvent1->force = 1.0;
  touchEvent1->timeStamp = eventTime;

  // 设置第二个触摸事件：坐标向右下偏移
  touchEvent2->screenX = windowX + offsetX;
  touchEvent2->screenY = windowY + offsetY;
  touchEvent2->x = centerX + offsetX;
  touchEvent2->y = centerY + offsetY;
  touchEvent2->id = 1;
  touchEvent2->size = 0;
  touchEvent2->type = type;
  touchEvent2->force = 1.0;
  touchEvent2->timeStamp = eventTime;
  
  // 调用函数 HandleTouchEvent 分别上报两个触摸事件
  HandleTouchEvent(shell_holderID, component, touchEvent1);
  HandleTouchEvent(shell_holderID, component, touchEvent2);
  
  // 释放模拟的触摸事件内存
  delete touchEvent1;
  delete touchEvent2;
  return;
}

// 处理滚动抛滑手势（Fling）：模拟单指触摸
void OhosTouchProcessor::HandleFlingEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }
  
  // 获取事件时间
  int64_t eventTime = OH_ArkUI_UIInputEvent_GetEventTime(event);

  // 获取滑动初始位置（XComponent组件坐标）
  float startX = OH_ArkUI_PointerEvent_GetX(event);
  float startY = OH_ArkUI_PointerEvent_GetY(event);

  // 获取滑动初始位置 (应用窗口坐标)
  float windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
  float windowY = OH_ArkUI_PointerEvent_GetWindowY(event);

  // 获取当前 delta 值
  float deltaX = 0 - OH_ArkUI_AxisEvent_GetHorizontalAxisValue(event);
  float deltaY = 0 - OH_ArkUI_AxisEvent_GetVerticalAxisValue(event);

  // 使用静态变量累计 delta
  static float accumulatedDeltaX = 0.0;
  static float accumulatedDeltaY = 0.0;

  // 获取触摸状态类型
  OH_NativeXComponent_TouchEventType type;
  switch (OH_ArkUI_AxisEvent_GetAxisAction(event)) {
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
    case UI_TOUCH_EVENT_ACTION_DOWN:
      type = OH_NATIVEXCOMPONENT_DOWN;
      accumulatedDeltaX = 0.0;
      accumulatedDeltaY = 0.0;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      type = OH_NATIVEXCOMPONENT_MOVE;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      type = OH_NATIVEXCOMPONENT_UP;
      break;
    default:
      LOGE("HandleFlingEvent: AxisAction is not defined");
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
  }
  accumulatedDeltaX += deltaX;
  accumulatedDeltaY += deltaY;

  // 生成模拟触摸事件
  OH_NativeXComponent_TouchEvent* touchEvent = new OH_NativeXComponent_TouchEvent(*reinterpret_cast<OH_NativeXComponent_TouchEvent*>(event));
 
  // 设置触摸事件：坐标偏移
  touchEvent->screenX = windowX + accumulatedDeltaX;
  touchEvent->screenY = windowY + accumulatedDeltaY;
  touchEvent->x = startX + accumulatedDeltaX;
  touchEvent->y = startY + accumulatedDeltaY;
  touchEvent->id = 0;
  touchEvent->size = 0;
  touchEvent->type = type;
  touchEvent->force = 1.0;
  touchEvent->timeStamp = eventTime;

  // 调用函数 HandleTouchEvent 上报触摸事件
  HandleTouchEvent(shell_holderID, component, touchEvent);
  
  // 释放模拟的触摸事件内存
  delete touchEvent;
  return;
}

// 处理 Ctrl + 鼠标滚轮 缩放
void OhosTouchProcessor::HandleScaleEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }
  
  // 获取事件时间
  int64_t eventTime = OH_ArkUI_UIInputEvent_GetEventTime(event);

  const int numTouchPoints = 1;
  std::unique_ptr<flutter::PointerDataPacket> packet =
      std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
  PointerData pointerData;
  pointerData.Clear();
  
  pointerData.time_stamp = eventTime;
  pointerData.physical_x = OH_ArkUI_PointerEvent_GetWindowX(event);
  pointerData.physical_y = OH_ArkUI_PointerEvent_GetWindowY(event);

  switch (OH_ArkUI_AxisEvent_GetAxisAction(event)) {
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
    case UI_TOUCH_EVENT_ACTION_DOWN:
      pointerData.change = PointerData::Change::kPanZoomStart;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      pointerData.change = PointerData::Change::kPanZoomUpdate;
      pointerData.scale = OH_ArkUI_AxisEvent_GetVerticalAxisValue(event) < 0 ? ZOOM_IN : ZOOM_OUT;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      pointerData.change = PointerData::Change::kPanZoomEnd;
      break;
    default:
      LOGE("HandleScaleEvent: AxisAction is not defined");
      pointerData.change = PointerData::Change::kCancel;
      break;
  }

  packet->SetPointerData(0, pointerData);
  auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shell_holderID);
  ohos_shell_holder->GetPlatformView()->DispatchPointerDataPacket(
      std::move(packet));
  return;
}

void OhosTouchProcessor::PlatformViewOnTouchEvent(
    int64_t shellHolderID,
    OH_NativeXComponent_TouchPointToolType toolType,
    OH_NativeXComponent* component,
    OH_NativeXComponent_TouchEvent* touchEvent) {
  int numPoints = touchEvent->numPoints;
  float tiltX = 0.0;
  float tiltY = 0.0;
  OH_NativeXComponent_GetTouchPointTiltX(component, 0, &tiltX);
  OH_NativeXComponent_GetTouchPointTiltY(component, 0, &tiltY);
  std::unique_ptr<OhosTouchProcessor::TouchPacket> touchPacket =
      std::make_unique<OhosTouchProcessor::TouchPacket>();
  touchPacket->touchEventInput = touchEvent;
  touchPacket->toolTypeInput = toolType;
  touchPacket->tiltX = tiltX;
  touchPacket->tiltX = tiltY;

  std::shared_ptr<std::string[]> touchPacketString =
      packagePacketData(std::move(touchPacket));
  int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints +
             TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
  auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shellHolderID);
  ohos_shell_holder->GetPlatformView()->OnTouchEvent(touchPacketString, size);
}

void OhosTouchProcessor::HandleMouseEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    OH_NativeXComponent_MouseEvent mouseEvent,
    double offsetY,
    bool isLeave) {
  const int numTouchPoints = 1;
  std::unique_ptr<flutter::PointerDataPacket> packet =
      std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
  PointerData pointerData;
  pointerData.Clear();
  pointerData.embedder_id = mouseEvent.button;
  pointerData.time_stamp = mouseEvent.timestamp / MSEC_PER_SECOND;
  pointerData.change = getPointerChangeForMouseAction(mouseEvent.action);
  // If this is a leave event, dispath a point event that leaves the area.
  pointerData.physical_y = isLeave ? -1 : mouseEvent.y;
  pointerData.physical_x = isLeave ? -1 : mouseEvent.x;
  // Delta will be generated in pointer_data_packet_converter.cc.
  pointerData.physical_delta_x = 0.0;
  pointerData.physical_delta_y = 0.0;
  pointerData.device = mouseEvent.button;
  // Pointer identifier will be generated in pointer_data_packet_converter.cc.
  pointerData.pointer_identifier = 0;
  // XComponent not support Scroll
  // now it's support
  pointerData.signal_kind = offsetY != 0 ? PointerData::SignalKind::kScroll
                                         : PointerData::SignalKind::kNone;
  pointerData.scroll_delta_x = 0.0;
  pointerData.scroll_delta_y = offsetY;
  pointerData.pressure = 0.0;
  pointerData.pressure_max = 1.0;
  pointerData.pressure_min = 0.0;
  pointerData.kind = PointerData::DeviceKind::kMouse; // kMouse支持鼠标框选文字
  pointerData.buttons = getPointerButtonFromMouse(mouseEvent.button);
  // hover support
  if (mouseEvent.button == OH_NATIVEXCOMPONENT_NONE_BUTTON &&
      pointerData.change == PointerData::Change::kMove) {
    pointerData.change = PointerData::Change::kHover;
    pointerData.kind = PointerData::DeviceKind::kMouse;
    pointerData.buttons = 0;
  }
  pointerData.pan_x = 0.0;
  pointerData.pan_y = 0.0;
  // Delta will be generated in pointer_data_packet_converter.cc.
  pointerData.pan_delta_x = 0.0;
  pointerData.pan_delta_y = 0.0;
  // The contact area between the fingerpad and the screen
  pointerData.size = 0.0;
  pointerData.scale = 1.0;
  pointerData.rotation = 0.0;
  packet->SetPointerData(0, pointerData);
  auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shell_holderID);
  ohos_shell_holder->GetPlatformView()->DispatchPointerDataPacket(
      std::move(packet));
  return;
}

void OhosTouchProcessor::HandleVirtualTouchEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    OH_NativeXComponent_TouchEvent* touchEvent) {
  int numPoints = touchEvent->numPoints;
  float tiltX = 0.0;
  float tiltY = 0.0;
  auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shell_holderID);
  OH_NativeXComponent_TouchPointToolType toolType;
  OH_NativeXComponent_GetTouchPointToolType(component, 0, &toolType);
  OH_NativeXComponent_GetTouchPointTiltX(component, 0, &tiltX);
  OH_NativeXComponent_GetTouchPointTiltY(component, 0, &tiltY);
  std::unique_ptr<OhosTouchProcessor::TouchPacket> touchPacket =
      std::make_unique<OhosTouchProcessor::TouchPacket>();
  touchPacket->touchEventInput = touchEvent;
  touchPacket->toolTypeInput = toolType;
  touchPacket->tiltX = tiltX;
  touchPacket->tiltX = tiltY;

  std::shared_ptr<std::string[]> touchPacketString =
      packagePacketData(std::move(touchPacket));
  int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints +
             TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
  ohos_shell_holder->GetPlatformView()->OnTouchEvent(touchPacketString, size);
  return;
}
}  // namespace flutter