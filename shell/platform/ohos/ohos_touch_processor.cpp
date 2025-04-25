/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_KHZG file.
 */

#include "flutter/shell/platform/ohos/ohos_touch_processor.h"

#include "flutter/lib/ui/window/pointer_data_packet.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include <arkui/native_type.h>
#include <dlfcn.h>

namespace flutter {

constexpr int MSEC_PER_SECOND = 1000;
constexpr int PER_POINTER_MEMBER = 10;
constexpr int CHANGES_POINTER_MEMBER = 10;
constexpr int TOUCH_EVENT_ADDITIONAL_ATTRIBUTES = 4;
constexpr int PINCH_TOUCH_EVENT_ID_1 = -101;
constexpr int PINCH_TOUCH_EVENT_ID_2 = -102;
constexpr int FLING_TOUCH_EVENT_ID_1 = -103;
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
    OH_NativeXComponent_MouseEventAction mouseAction)
{
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
    OH_NativeXComponent_MouseEventButton mouseButton)
{
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
    int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints + TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
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
    FML_LOG(DEBUG) << "screenX:" << touchPacket->touchEventInput->screenX << " screenY:" <<
      touchPacket->touchEventInput->screenY << " x:" << touchPacket->touchEventInput->x <<
      " y:" << touchPacket->touchEventInput->y;
    for (int i = 0; i < numPoints; i++) {
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].id);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].screenX);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].screenY);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].x);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].y);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].type);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].size);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].force);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].timeStamp);
      package[offset++] = std::to_string(touchPacket->touchEventInput->touchPoints[i].isPressed);
    FML_LOG(DEBUG) << "touches [" << i << "] screenX:" << touchPacket->touchEventInput->touchPoints[i].screenX <<
      " screenY:" << touchPacket->touchEventInput->touchPoints[i].screenY << " x:" <<
      touchPacket->touchEventInput->touchPoints[i].x << " y:" << touchPacket->touchEventInput->touchPoints[i].y;
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
    FML_TRACE_EVENT("flutter", "HandleTouchEvent", "timeStamp", touchEvent->timeStamp);
    const int numTouchPoints = 1;
    std::unique_ptr<flutter::PointerDataPacket> packet = std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
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
    ohos_shell_holder->GetPlatformView()->RunTask(OHOS_THREAD_TYPE::OHOS_THREAD_TYPE_UI, task);

    PlatformViewOnTouchEvent(shell_holderID, toolType, component, touchEvent);
}

OhosTouchProcessor::OhosTouchProcessor()
    : apiVersion_(0),
      loader_(std::make_unique<DynamicLibraryLoader>(UI_INPUT_EVENT_LIB_NAME)),
      dynamicGetDeviceId_(nullptr),
      dynamicGetAxisAction_(nullptr),
      dynamicGetModifierKeyStates_(nullptr) {
  apiVersion_ = DynamicLibraryLoader::GetApiVersion();
  FML_LOG(INFO) << "Current SDK API Version: " << apiVersion_;

  std::vector<SymbolInfo> symbols = {
    { "OH_ArkUI_UIInputEvent_GetDeviceId", reinterpret_cast<void**>(&dynamicGetDeviceId_), 14 },
    { "OH_ArkUI_AxisEvent_GetAxisAction", reinterpret_cast<void**>(&dynamicGetAxisAction_), 15 },
    { "OH_ArkUI_UIInputEvent_GetModifierKeyStates", reinterpret_cast<void**>(&dynamicGetModifierKeyStates_), 17 },
  };

  loader_->LoadSymbols(symbols);
}

OhosTouchProcessor::~OhosTouchProcessor() {
  
}

// 处理轴事件：触控板捏合缩放和滚动抛滑手势，鼠标：滚轮滑动和Ctrl+滚轮缩放
void OhosTouchProcessor::HandleAxisEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }

  if (apiVersion_ < 15) { // API15 前轴事件接口不完善，会走 XComponentBase::OnDispatchMouseWheelEvent 处理滚动
    return;
  }

  // 获取工具类型和功能键情况判断事件
  uint64_t keys = 0;
  int32_t toolType = OH_ArkUI_UIInputEvent_GetToolType(event);
  int32_t errorCode = dynamicGetModifierKeyStates_ != nullptr ? dynamicGetModifierKeyStates_(event, &keys) : 0;
  if (toolType == UI_INPUT_EVENT_TOOL_TYPE_MOUSE &&
      errorCode != ARKUI_ERROR_CODE_PARAM_INVALID &&
      keys & ARKUI_MODIFIER_KEY_CTRL) {
    // Ctrl+鼠标滚轮
    HandleScaleEvent(shell_holderID, component, event);
  } else {
    // 判断手势类型并调用相应的处理函数
    float pinchScale = OH_ArkUI_AxisEvent_GetPinchAxisScaleValue(event);
    const float pinchThreshold = 0.0001;
    if (fabs(pinchScale - 0.0) > pinchThreshold) {
      // 捏合缩放手势 
      HandlePinchEvent(shell_holderID, component, event);
    } else {
      // 滚动抛滑手势
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
  
  // 获取事件时间和设备ID
  int64_t eventTime = OH_ArkUI_UIInputEvent_GetEventTime(event);
  int32_t deviceId = dynamicGetDeviceId_ != nullptr ? dynamicGetDeviceId_(event) : 0;

  // 获取手势中心位置（XComponent组件坐标）
  float centerX = OH_ArkUI_PointerEvent_GetX(event);
  float centerY = OH_ArkUI_PointerEvent_GetY(event);

  // 获取滑动初始位置（应用窗口坐标）
  float windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
  float windowY = OH_ArkUI_PointerEvent_GetWindowY(event);

  // 获取捏合缩放比例
  float pinchScale = OH_ArkUI_AxisEvent_GetPinchAxisScaleValue(event);
  
  // 模拟两个手指从中心向外扩张，计算偏移量
  const double baseSeparation = 0.01; // 基准分离距离（单位：点）
  double separation = baseSeparation * (pinchScale - 1.0);
  double offsetX = baseSeparation + separation;
  double offsetY = baseSeparation + separation;
  
  // 获取触摸状态类型
  OH_NativeXComponent_TouchEventType type;
  int32_t axisAction = dynamicGetAxisAction_ != nullptr ? dynamicGetAxisAction_(event) : UI_TOUCH_EVENT_ACTION_CANCEL;
  switch (axisAction) {
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
      FML_LOG(ERROR) << "HandlePinchEvent: AxisAction is not defined";
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
  }

  // 创建两个模拟触摸事件
  auto touchEvent1 = std::make_unique<OH_NativeXComponent_TouchEvent>();
  if (touchEvent1 == nullptr) {
    FML_LOG(ERROR) << "HandlePinchEvent: Failed to allocate touchEvent1";
    return;
  }
  auto touchEvent2 = std::make_unique<OH_NativeXComponent_TouchEvent>();
  if (touchEvent2 == nullptr) {
    FML_LOG(ERROR) << "HandlePinchEvent: Failed to allocate touchEvent2";
    return;
  }
  
  // 设置第一个触摸事件：坐标向左上偏移
  touchEvent1->screenX = windowX - offsetX;
  touchEvent1->screenY = windowY - offsetY;
  touchEvent1->x = centerX - offsetX;
  touchEvent1->y = centerY - offsetY;
  touchEvent1->id = PINCH_TOUCH_EVENT_ID_1;
  touchEvent1->size = 0;
  touchEvent1->type = type;
  touchEvent1->force = 1.0;
  touchEvent1->deviceId = deviceId;
  touchEvent1->numPoints = 1;
  touchEvent1->timeStamp = eventTime;

  // 设置第二个触摸事件：坐标向右下偏移
  touchEvent2->screenX = windowX + offsetX;
  touchEvent2->screenY = windowY + offsetY;
  touchEvent2->x = centerX + offsetX;
  touchEvent2->y = centerY + offsetY;
  touchEvent2->id = PINCH_TOUCH_EVENT_ID_2;
  touchEvent2->size = 0;
  touchEvent2->type = type;
  touchEvent2->force = 1.0;
  touchEvent2->deviceId = deviceId;
  touchEvent2->numPoints = 1;
  touchEvent2->timeStamp = eventTime;
  
  // 上报两个触摸事件
  HandleTouchEvent(shell_holderID, component, touchEvent1.get());
  HandleTouchEvent(shell_holderID, component, touchEvent2.get());
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
  
  // 获取事件时间和设备ID
  int64_t eventTime = OH_ArkUI_UIInputEvent_GetEventTime(event);
  int32_t deviceId = dynamicGetDeviceId_ != nullptr ? dynamicGetDeviceId_(event) : 0;

  // 获取滑动初始位置（XComponent组件坐标）
  float startX = OH_ArkUI_PointerEvent_GetX(event);
  float startY = OH_ArkUI_PointerEvent_GetY(event);

  // 获取滑动初始位置 (应用窗口坐标)
  float windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
  float windowY = OH_ArkUI_PointerEvent_GetWindowY(event);

  // 获取当前 delta 值
  float deltaX = 0 - OH_ArkUI_AxisEvent_GetHorizontalAxisValue(event);
  float deltaY = 0 - OH_ArkUI_AxisEvent_GetVerticalAxisValue(event);

  // 获取触摸状态类型并处理累计值
  OH_NativeXComponent_TouchEventType type;
  int32_t axisAction = dynamicGetAxisAction_ != nullptr ? dynamicGetAxisAction_(event) : UI_TOUCH_EVENT_ACTION_CANCEL;
  switch (axisAction) {
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
    case UI_TOUCH_EVENT_ACTION_DOWN:
      type = OH_NATIVEXCOMPONENT_DOWN;
      // 重置累计值
      accumulatedDeltaX_ = 0.0;
      accumulatedDeltaY_ = 0.0;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      type = OH_NATIVEXCOMPONENT_MOVE;
      // 更新累计值
      accumulatedDeltaX_ += deltaX;
      accumulatedDeltaY_ += deltaY;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      type = OH_NATIVEXCOMPONENT_UP;
      break;
    default:
      FML_LOG(ERROR) << "HandleFlingEvent: AxisAction is not defined";
      type = OH_NATIVEXCOMPONENT_CANCEL;
      break;
  }

  // 创建模拟触摸事件
  auto touchEvent = std::make_unique<OH_NativeXComponent_TouchEvent>();
  if (touchEvent == nullptr) {
    FML_LOG(ERROR) << "HandleFlingEvent: Failed to allocate touchEvent";
    return;
  }
 
  // 设置触摸事件：根据累计偏移设置坐标
  touchEvent->screenX = windowX + accumulatedDeltaX_;
  touchEvent->screenY = windowY + accumulatedDeltaY_;
  touchEvent->x = startX + accumulatedDeltaX_;
  touchEvent->y = startY + accumulatedDeltaY_;
  touchEvent->id = FLING_TOUCH_EVENT_ID_1;
  touchEvent->size = 0;
  touchEvent->type = type;
  touchEvent->force = 1.0;
  touchEvent->deviceId = deviceId;
  touchEvent->numPoints = 1;
  touchEvent->timeStamp = eventTime;

  // 上报触摸事件
  HandleTouchEvent(shell_holderID, component, touchEvent.get());
  return;
}

// 处理Ctrl+鼠标滚轮缩放
void OhosTouchProcessor::HandleScaleEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    ArkUI_UIInputEvent* event) {
  if (event == nullptr) {
    return;
  }
  
  const int numTouchPoints = 1;
  std::unique_ptr<flutter::PointerDataPacket> packet =
      std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
  PointerData pointerData;
  pointerData.Clear();

  // 获取 PointerData 状态类型并处理累计值
  int32_t axisAction = dynamicGetAxisAction_ != nullptr ? dynamicGetAxisAction_(event) : UI_TOUCH_EVENT_ACTION_CANCEL;
  switch (axisAction) {
    case UI_TOUCH_EVENT_ACTION_CANCEL:
      pointerData.change = PointerData::Change::kCancel;
      break;
    case UI_TOUCH_EVENT_ACTION_DOWN:
      pointerData.change = PointerData::Change::kPanZoomStart;
      // 重置累计值
      accumulatedScale_ = 1.0;
      break;
    case UI_TOUCH_EVENT_ACTION_MOVE:
      pointerData.change = PointerData::Change::kPanZoomUpdate;
      // 更新累计值
      accumulatedScale_ *= OH_ArkUI_AxisEvent_GetVerticalAxisValue(event) < 0 ? ZOOM_IN : ZOOM_OUT;
      break;
    case UI_TOUCH_EVENT_ACTION_UP:
      pointerData.change = PointerData::Change::kPanZoomEnd;
      break;
    default:
      FML_LOG(ERROR) << "HandleScaleEvent: AxisAction is not defined";
      pointerData.change = PointerData::Change::kCancel;
      break;
  }

  pointerData.scale = accumulatedScale_;
  pointerData.physical_x = OH_ArkUI_PointerEvent_GetWindowX(event);
  pointerData.physical_y = OH_ArkUI_PointerEvent_GetWindowY(event);
  pointerData.time_stamp = OH_ArkUI_UIInputEvent_GetEventTime(event);
  pointerData.embedder_id = dynamicGetDeviceId_ != nullptr ? dynamicGetDeviceId_(event) : 0;

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
    OH_NativeXComponent_TouchEvent* touchEvent)
{
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

    std::shared_ptr<std::string[]> touchPacketString = packagePacketData(std::move(touchPacket));
    int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints + TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
    auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shellHolderID);
    ohos_shell_holder->GetPlatformView()->OnTouchEvent(touchPacketString, size);
}

void OhosTouchProcessor::HandleMouseEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    OH_NativeXComponent_MouseEvent mouseEvent,
    double offsetY,
    bool isLeave)
{
    const int numTouchPoints = 1;
    std::unique_ptr<flutter::PointerDataPacket> packet = std::make_unique<flutter::PointerDataPacket>(numTouchPoints);
    PointerData pointerData;
    pointerData.Clear();
    pointerData.embedder_id = mouseEvent.button;
    pointerData.time_stamp = mouseEvent.timestamp / MSEC_PER_SECOND;
    pointerData.change = getPointerChangeForMouseAction(mouseEvent.action);
    // If this is a leave event, dispath a point event that leaves the area.
    pointerData.physical_y = isLeave ? -1 :mouseEvent.y;
    pointerData.physical_x = isLeave ? -1 : mouseEvent.x;
    // Delta will be generated in pointer_data_packet_converter.cc.
    pointerData.physical_delta_x = 0.0;
    pointerData.physical_delta_y = 0.0;
    pointerData.device = mouseEvent.button;
    // Pointer identifier will be generated in pointer_data_packet_converter.cc.
    pointerData.pointer_identifier = 0;
    // XComponent not support Scroll
    // now it's support
    pointerData.signal_kind = offsetY != 0 ? PointerData::SignalKind::kScroll : PointerData::SignalKind::kNone;
    pointerData.scroll_delta_x = 0.0;
    pointerData.scroll_delta_y = offsetY;
    pointerData.pressure = 0.0;
    pointerData.pressure_max = 1.0;
    pointerData.pressure_min = 0.0;
    pointerData.kind = PointerData::DeviceKind::kMouse; // kMouse支持鼠标框选文字
    pointerData.buttons = getPointerButtonFromMouse(mouseEvent.button);
    // hover support
    if (mouseEvent.button == OH_NATIVEXCOMPONENT_NONE_BUTTON && pointerData.change == PointerData::Change::kMove) {
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
    ohos_shell_holder->GetPlatformView()->DispatchPointerDataPacket(std::move(packet));
    return;
}

void OhosTouchProcessor::HandleVirtualTouchEvent(
    int64_t shell_holderID,
    OH_NativeXComponent* component,
    OH_NativeXComponent_TouchEvent* touchEvent)
{
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
    
    std::shared_ptr<std::string[]> touchPacketString = packagePacketData(std::move(touchPacket));
    int size = CHANGES_POINTER_MEMBER + PER_POINTER_MEMBER * numPoints + TOUCH_EVENT_ADDITIONAL_ATTRIBUTES;
    ohos_shell_holder->GetPlatformView()->OnTouchEvent(touchPacketString, size);
    return;
}
}  // namespace flutter