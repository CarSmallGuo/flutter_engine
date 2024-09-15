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

#include <string>
#include "flutter/fml/logging.h"
#include "ohos_accessibility_bridge.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "flutter/shell/platform/embedder/embedder.h"

#define SHELL_HOLDER (reinterpret_cast<OHOSShellHolder*>(shell_holder))
namespace flutter {

OhosAccessibilityBridge OhosAccessibilityBridge::bridgeInstance; 

OhosAccessibilityBridge::OhosAccessibilityBridge() {};
OhosAccessibilityBridge::~OhosAccessibilityBridge() {};

OhosAccessibilityBridge* OhosAccessibilityBridge::GetInstance() {
  return &OhosAccessibilityBridge::bridgeInstance;
}


void OhosAccessibilityBridge::announce(std::unique_ptr<char[]>& message) {
  //创建并设置屏幕朗读事件
  ArkUI_AccessibilityEventInfo* announceEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
  int32_t ret1 = OH_ArkUI_AccessibilityEventSetEventType(announceEventInfo, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
  if(ret1 != 0) {
      FML_DLOG(INFO) << "OhosAccessibilityBridge::announce OH_ArkUI_AccessibilityEventSetEventType failed";
      return;
  }
  int32_t ret2 = OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility(announceEventInfo, message.get()); 
  if(ret2 != 0) {
      FML_DLOG(INFO) << "OhosAccessibilityBridge::announce OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility failed";
      return;
  }
  FML_DLOG(INFO) << ("OhosAccessibilityBridge::announce message: ")
                 << (message.get());
  return;
}

/**
 * 根据nodeid获取或创建flutter语义节点
 */
flutter::SemanticsNode OhosAccessibilityBridge::getOrCreateFlutterSemanticsNode(
    int32_t id) {
  flutter::SemanticsNode node;
  if (flutterSemanticsTree_.count(id) > 0) {
    return flutterSemanticsTree_.at(id);
    FML_DLOG(INFO)<<"getOrCreateFlutterSemanticsNode get node.id="<<id;
  } else {
    FML_DLOG(INFO)<<"getOrCreateFlutterSemanticsNode flutterSemanticsTree_ = null"<<id;
    return flutter::SemanticsNode{};
  }
}

/**
 * flutter的语义节点初始化配置给arkui创建的elementInfos
 */
void OhosAccessibilityBridge::FlutterTreeToArkuiTree(ArkUI_AccessibilityElementInfoList* elementInfoList) {
  if(flutterSemanticsTree_.size() == 0) {
    FML_DLOG(ERROR)<<"OhosAccessibilityBridge::FlutterTreeToArkuiTree flutterSemanticsTree_.size() = 0";
    return;
  }
  //将flutter语义节点树传递给arkui的无障碍elementinfo
  for(const auto& item: flutterSemanticsTree_) {
    flutter::SemanticsNode flutterNode = item.second;

    //创建elementinfo，系统自动加入到elementinfolist
    ArkUI_AccessibilityElementInfo* elementInfo =  OH_ArkUI_AddAndGetAccessibilityElementInfo(elementInfoList);
    if (elementInfo == nullptr) {
      FML_DLOG(INFO) << "OhosAccessibilityBridge::FlutterTreeToArkuiTree "
                      "elementInfo is null";
      return;
    }
    //设置elementinfo的屏幕坐标范围
    int32_t left = static_cast<int32_t>(flutterNode.rect.fLeft);
    int32_t top  = static_cast<int32_t>(flutterNode.rect.fTop);
    int32_t right = static_cast<int32_t>(flutterNode.rect.fRight);
    int32_t bottom  = static_cast<int32_t>(flutterNode.rect.fBottom);
    ArkUI_AccessibleRect rect = {left, top, right, bottom}; 
    OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfo, &rect);

    //设置elementinfo的action类型
    int32_t actionTypeNum = 3;
    ArkUI_AccessibleAction actions[actionTypeNum];
    actions[0].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
    actions[0].description = "获取焦点动作事件";
    actions[1].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
    actions[1].description = "清除焦点动作事件";
    actions[2].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
    actions[2].description = "点击动作事件";
    OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfo, actionTypeNum, actions);

    //设置elementid
    OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfo, flutterNode.id);

    //设置父节点id
    int32_t parentId = GetParentId(flutterNode.id);
    if(flutterNode.id == 0) {
      OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfo, -2100000);
      FML_DLOG(INFO) << "FlutterTreeToArkuiTree parent.id= "<<parentId;
      
    } else {
      OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfo, parentId);
      FML_DLOG(INFO) << "FlutterTreeToArkuiTree parent.id= "<<parentId;
    }

    //设置孩子节点
    int32_t childCount = static_cast<int32_t>(flutterNode.childrenInTraversalOrder.size());
    int64_t childNodeIds[childCount];
    for(int32_t i=0; i<childCount; i++) {
      childNodeIds[i] = static_cast<int64_t>(flutterNode.childrenInTraversalOrder[i]);
      FML_DLOG(INFO) << "FlutterTreeToArkuiTree flutterNode.id= "<<flutterNode.id<<" childCount= "<<childCount<<" childNodeId="<<childNodeIds[i];
    }
    OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(elementInfo, childCount, childNodeIds);

    // 配置常用属性，force to true for debugging
    OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfo, true);
    OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfo, true);
    OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfo, true);
    OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfo, true);
    OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfo, true);

    //设置组件类型
    std::string componentTypeName = GetNodeComponentType(flutterNode);
    if(componentTypeName == "button") {
      OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfo, componentTypeName.c_str());
      // OH_ArkUI_AccessibilityElementInfoSetFocused(elementInfo, true);
    } else {
      OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfo, componentTypeName.c_str());
    }
    std::string contents = componentTypeName + "_content";
    OH_ArkUI_AccessibilityElementInfoSetContents(elementInfo, contents.c_str());

    //设置无障碍相关属性
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfo, flutterNode.label.c_str());
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(elementInfo, "yes");
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfo, false);
  }
  FML_DLOG(INFO)<<"FlutterTreeToArkuiTree is end";
}

/**
 * 获取当前elementid的父节点id
 */
int32_t OhosAccessibilityBridge::GetParentId(int64_t elementId) {
  int32_t childElementId = static_cast<int32_t>(elementId);
  if(!parentChildIdVec.size()) {
    FML_DLOG(INFO) << "OhosAccessibilityBridge::GetParentId parentChildIdMap.size()=0";
    return -2100000;
  }
  for(const auto& item: parentChildIdVec) {
    if(item.second == childElementId) {
      return item.first;
    }
  }
  return -2100000;
}

/**
 * 设置并获取xcomponet上渲染的组件的画框的绝对坐标rect
 */
void OhosAccessibilityBridge::SetAbsoluteScreenRect(int32_t flutterNodeId, float left, float top, float right, float bottom) {
   if(screenRectMap_.find(flutterNodeId) == screenRectMap_.end()) {
      screenRectMap_.insert({flutterNodeId, std::make_pair(std::make_pair(left, top), std::make_pair(right, bottom))});
      FML_DLOG(INFO) << "SetAbsoluteScreenRect -> insert { "<<flutterNodeId<<", <"<<left<<", "<<top<<", "<<right<<", "<<bottom<<"> } is succeed";
   } else {
      FML_DLOG(ERROR) << "SetAbsoluteScreenRect -> flutterNodeId="<<flutterNodeId<<" already exists !";
   }
}

std::pair<std::pair<float, float>, std::pair<float, float>> OhosAccessibilityBridge::GetAbsoluteScreenRect(int32_t flutterNodeId) {
  if(!screenRectMap_.empty() && screenRectMap_.count(flutterNodeId) > 0) {
    return screenRectMap_.at(flutterNodeId);
  } else {
    FML_DLOG(ERROR) << "GetAbsoluteScreenRect -> flutterNodeId="<<flutterNodeId<<" is not found !";
    return {};
  }
}

/**
 * flutter语义节点的子节点相对父节点坐标的矩阵变换，并返回转换后绝对屏幕坐标的rect
 */
void OhosAccessibilityBridge::ConvertChildRelativeRectToSceenRect(flutter::SemanticsNode currNode) {
  //获取当前flutter节点的相对rect
  auto currLeft = static_cast<float>(currNode.rect.fLeft);
  auto currTop = static_cast<float>(currNode.rect.fTop);
  auto currRight = static_cast<float>(currNode.rect.fRight);
  auto currBottom = static_cast<float>(currNode.rect.fBottom);

  //获取当前flutter节点的缩放、平移、透视等矩阵坐标转换
  SkMatrix transform = currNode.transform.asM33();
  auto _kMScaleX = transform.get(SkMatrix::kMScaleX);
  auto _kMTransX = transform.get(SkMatrix::kMTransX);
  auto _kMScaleY = transform.get(SkMatrix::kMScaleY);
  auto _kMTransY = transform.get(SkMatrix::kMTransY);
  /** 以下矩阵坐标变换参数（如：旋转/错切、透视）场景目前暂不考虑 */
  // auto _kMSkewX  = transform.get(SkMatrix::kMSkewX);
  // auto _kMSkewY  = transform.get(SkMatrix::kMSkewY);
  // auto _kMPersp0 = transform.get(SkMatrix::kMPersp0);
  // auto _kMPersp1 = transform.get(SkMatrix::kMPersp1);
  // auto _kMPersp2 = transform.get(SkMatrix::kMPersp2);

  //获取当前flutter节点的父节点的相对rect
  int32_t parentId = GetParentId(currNode.id);
  auto parentNode = getOrCreateFlutterSemanticsNode(parentId); 
  // auto parentLeft = parentNode.rect.fLeft;
  // auto parentTop  = parentNode.rect.fTop;
  auto parentRight = parentNode.rect.fRight;
  auto parentBottom  = parentNode.rect.fBottom;

  //获取当前flutter节点的父节点的绝对坐标
  auto _rectPairs = GetAbsoluteScreenRect(parentNode.id);
  auto realParentLeft = _rectPairs.first.first;
  auto realParentTop  = _rectPairs.first.second;
  auto realParentRight = _rectPairs.second.first;
  auto realParentBottom  = _rectPairs.second.second;

  //真实缩放系数
  float realScaleFactor = realParentRight / parentRight * 1.0;
  // float xOffset = realParentLeft;
  // float yOffset = realParentTop;
  float newLeft, newTop, newRight, newBottom;

  if(_kMScaleX > 1 && _kMScaleY > 1 ) {
    //子节点相对父节点进行变化（常用的缩放、 平移）
    newLeft   = currLeft + _kMTransX * _kMScaleX;
    newTop    = currTop  + _kMTransY * _kMScaleY;
    newRight  = currRight  * _kMScaleX;
    newBottom = currBottom * _kMScaleY;
    //更新当前flutter节点currNode的相对坐标 -> 屏幕绝对坐标
    SetAbsoluteScreenRect(currNode.id, newLeft, newTop, newRight, newBottom);
    FML_DLOG(INFO) << "ConvertChildRelativeRectToSceenRect -> { nodeId: "<<currNode.id<<", ("<<newLeft<<", "<<newTop<<", "<<newRight<<", "<<newBottom<<")}";

  } else {
    //若当前节点的相对坐标与父亲节点的相对坐标值相同，则直接继承坐标值
    if(currRight == parentRight && currBottom == parentBottom) {
      newLeft   = realParentLeft;
      newTop    = realParentTop;
      newRight  = realParentRight;
      newBottom = realParentBottom;
    } else {
      // 子节点的屏幕绝对坐标转换，包括offset偏移值计算、缩放系数变换
      newLeft   = (currLeft + _kMTransX) * realScaleFactor;
      newTop    = (currTop  + _kMTransY) * realScaleFactor;
      newRight  = (currLeft + _kMTransX + currRight) * realScaleFactor;
      newBottom = (currTop + _kMTransY + currBottom) * realScaleFactor;
    }
    SetAbsoluteScreenRect(currNode.id, newLeft, newTop, newRight, newBottom);
    FML_DLOG(INFO) << "ConvertChildRelativeRectToSceenRect -> { nodeId: "<<currNode.id<<", ("<<newLeft<<", "<<newTop<<", "<<newRight<<", "<<newBottom<<")}";
  }
}

/**
 * 在flutter引擎里实现对特定id的elementinfo的节点属性配置
 */
void OhosAccessibilityBridge::FlutterNodeToElementInfoById(ArkUI_AccessibilityElementInfo* elementInfoFromList, int64_t elementId) {
  if(elementInfoFromList == nullptr) {
    FML_DLOG(INFO) << "OhosAccessibilityBridge::FlutterNodeToElementInfoById "
                      "elementInfoFromList is null";
    return;
  }
  FML_DLOG(INFO) << "FlutterNodeToElementInfoById elementInfoFromList= "<<elementInfoFromList;

  /** NOTE: when elementId == 0 || elementId == -1  */
  if(elementId == 0 || elementId == -1) {
    //获取flutter的root节点
    flutter::SemanticsNode flutterNode = getOrCreateFlutterSemanticsNode(static_cast<int32_t>(0)); 

    //设置elementinfo的屏幕坐标范围
    int32_t left = static_cast<int32_t>(flutterNode.rect.fLeft);
    int32_t top  = static_cast<int32_t>(flutterNode.rect.fTop);
    int32_t right = static_cast<int32_t>(flutterNode.rect.fRight);
    int32_t bottom  = static_cast<int32_t>(flutterNode.rect.fBottom);
    ArkUI_AccessibleRect rect = {left, top, right, bottom}; //debug 
    OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfoFromList, &rect);

    //设置elementinfo的action类型
    int32_t actionTypeNum = 2;
    ArkUI_AccessibleAction actions[actionTypeNum];
    actions[0].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
    actions[0].description = "获取焦点动作事件";
    actions[1].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
    actions[1].description = "清空焦点动作事件";
    OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, actionTypeNum, actions);

    //根据flutternode信息配置对应的elementinfo
    OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfoFromList, 0); 
    // NOTE: 系统强制设置root的父节点id = -2100000
    OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfoFromList, -2100000); 
    //设置无障碍播报文本
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfoFromList, flutterNode.label.empty()? "root" : flutterNode.label.c_str()); // debug
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(elementInfoFromList, "yes");
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfoFromList, false);

    //配置child节点信息
    int32_t childCount = static_cast<int32_t>(flutterNode.childrenInTraversalOrder.size());
    int64_t childNodeIds[childCount];
    for(int32_t i=0; i<childCount; i++) {
      childNodeIds[i] = static_cast<int64_t>(flutterNode.childrenInTraversalOrder[i]);
      FML_DLOG(INFO) << "FlutterNodeToElementInfoById -> elementid=0 childCount="<<childCount<<" childNodeIds="<<childNodeIds[i];
    }
    OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(elementInfoFromList, childCount, childNodeIds);

    // 配置常用属性
    OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfoFromList, true);
    OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfoFromList, true);
    OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfoFromList, true);
    OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfoFromList, true);
    OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfoFromList, true);

    OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfoFromList, "root"); // debug
    OH_ArkUI_AccessibilityElementInfoSetContents(elementInfoFromList, "root_content");

    return;
  }

  /** ----------------------------------------------- NOTE: when elementId >= 1 -------------------------------------------- */
  //根据elementid获取对应的flutter节点
  FML_DLOG(INFO) << "FlutterNodeToElementInfoById elementId = "<<elementId;
  flutter::SemanticsNode flutterNode = getOrCreateFlutterSemanticsNode(static_cast<int32_t>(elementId));

  // NOTE: 语义组件的父子节点相对坐标位置的矩阵变换，转化为绝对屏幕坐标
  ConvertChildRelativeRectToSceenRect(flutterNode);
  auto rectPairs = GetAbsoluteScreenRect(flutterNode.id);
  //设置elementinfo的屏幕坐标范围
  int32_t left = rectPairs.first.first;
  int32_t top  = rectPairs.first.second;
  int32_t right = rectPairs.second.first;
  int32_t bottom  = rectPairs.second.second;
  ArkUI_AccessibleRect rect = {left, top, right, bottom}; 
  OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfoFromList, &rect);
  FML_DLOG(INFO) << "FlutterNodeToElementInfoById -> node.id= "<<flutterNode.id <<" SceenRect = ("<<left<<", "<<top<<", "<<right<<", "<<bottom<<")";

  //设置elementinfo的action类型
  int32_t actionTypeNum = 5;
  ArkUI_AccessibleAction actions[actionTypeNum];
  actions[0].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
  actions[0].description = "获取焦点动作事件";
  actions[1].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
  actions[1].description = "清除焦点动作事件";
  actions[2].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
  actions[2].description = "点击动作事件";
  actions[3].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD;
  actions[3].description = "向上滑动动作事件";
  actions[4].actionType = ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD;
  actions[4].description = "向下滑动动作事件";
  OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, actionTypeNum, actions);
  

  //根据flutternode信息配置对应的elementinfo
  OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfoFromList, flutterNode.id);

  //设置父节点id
  int32_t parentId = GetParentId(elementId);
  if(parentId < 0) {
    FML_DLOG(ERROR) << "FlutterNodeToElementInfoById GetParentId is null, assigned to "<<parentId;
    OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfoFromList, parentId); 
  } else {
    OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfoFromList, parentId);
    FML_DLOG(INFO) << "FlutterNodeToElementInfoById GetParentId = "<<parentId;
  }

  //设置无障碍播报文本
  std::string text = flutterNode.label.empty()? "当前节点id为" + std::to_string(flutterNode.id) : flutterNode.label;
  OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfoFromList, text.c_str());
  FML_DLOG(INFO) << "FlutterNodeToElementInfoById SetAccessibilityText = "<<text;

  //配置child节点信息
  int32_t childCount = static_cast<int32_t>(flutterNode.childrenInTraversalOrder.size());
  int64_t childNodeIds[childCount];
  for(int32_t i=0; i<childCount; i++) {
    childNodeIds[i] = static_cast<int64_t>(flutterNode.childrenInTraversalOrder[i]);
    FML_DLOG(INFO) << "FlutterNodeToElementInfoById -> elementid="<<elementId<<" childCount="<<childCount<<" childNodeIds="<<childNodeIds[i];
  }
  OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(elementInfoFromList, childCount, childNodeIds);

  // 配置常用属性
  OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfoFromList, true);
  OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfoFromList, true);
  OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfoFromList, true);
  // if(elementId == 4) {
  //     OH_ArkUI_AccessibilityElementInfoSetFocused(elementInfoFromList, true); 
  // } 
  OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfoFromList, true);
  // OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused(elementInfoFromList, true);
  OH_ArkUI_AccessibilityElementInfoSetSelected(elementInfoFromList, true);
  OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfoFromList, true);

  //FIXME: 元素所属的组件类型（如：root， button，text等）
  std::string componentTypeName = GetNodeComponentType(flutterNode);
  if(elementId == 0) {
      OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfoFromList, "root");
  } else {
      OH_ArkUI_AccessibilityElementInfoSetComponentType(elementInfoFromList, componentTypeName.c_str());
  }
  FML_DLOG(INFO) << "FlutterNodeToElementInfoById SetComponentType: "<<componentTypeName;

  if(!flutterNode.label.empty()) {
    //无障碍重要性，用于控制某个组件是否可被无障碍辅助服务所识别。支持的值为:
    // “auto”：根据组件不同会转换为“yes”或者“no”。
    // “yes”：当前组件可被无障碍辅助服务所识别。
    // “no”：当前组件不可被无障碍辅助服务所识别。
    // “no-hide-descendants”：当前组件及其所有子组件不可被无障碍辅助服务所识别。
    // 默认值：“auto”
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(elementInfoFromList, "yes");
    //无障碍组，设置为true时表示该组件及其所有子组件为一整个可以选中的组件，无障碍服务将不再关注其子组件内容。默认值：false
    OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfoFromList, false);
  }
  FML_DLOG(INFO) << "OhosAccessibilityBridge::FlutterNodeToElementInfoById is end";
}

/**
 * Called to obtain element information based on a specified node.
 * NOTE: 该arkui接口需要在系统无障碍服务开启时，才能触发调用
 * TODO: 当前实现版本为实现简易功能，正在完善实现实际使用场景逻辑并逐步优化；另外：ohos侧当前只发送elementinfo=-1、0两种情况，故目前只能获取root节点
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosById(
    int64_t elementId,
    ArkUI_AccessibilitySearchMode mode,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList) {
  FML_DLOG(INFO) << "#### FindAccessibilityNodeInfosById input-params ####: elementId = "<<elementId<<" mode="<<mode<<" requestId="<<requestId<<" elementList= "<<elementList;

  if(flutterSemanticsTree_.size() == 0) {
    FML_DLOG(INFO) << "FindAccessibilityNodeInfosById flutterSemanticsTree_ is null";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
  }
  if (elementList == nullptr) {
    FML_DLOG(INFO) << "FindAccessibilityNodeInfosById elementList is null";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
  }

  //从elementinfolist中获取elementinfo
  ArkUI_AccessibilityElementInfo* elementInfoFromList = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
  if (elementInfoFromList == nullptr) {
    FML_DLOG(INFO) << "FindAccessibilityNodeInfosById elementInfoFromList is null";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
  }

  //将flutter语义节点树 -> arkui语义节点树
  // FlutterTreeToArkuiTree(elementList);  

  if(mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT) {
      /** Search for current nodes. (mode = 0) */
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
      int64_t elementInfoCount = static_cast<int64_t>(flutterSemanticsTree_.size());
      for(int64_t i=1; i<elementInfoCount; i++) {
        ArkUI_AccessibilityElementInfo* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
        FlutterNodeToElementInfoById(newElementInfo, i); 
      }

  } else if(mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS) {
      /** Search for parent nodes. (mode = 1) */
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
      // int64_t elementInfoCount = static_cast<int64_t>(flutterSemanticsTree_.size());
      // for(int64_t i=1; i<elementInfoCount; i++) {
      //   ArkUI_AccessibilityElementInfo* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
      //   FlutterNodeToElementInfoById(newElementInfo, i); 
      // }

  } else if(mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS) {
      /** Search for sibling nodes. (mode = 2) */
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
      // int64_t elementInfoCount = static_cast<int64_t>(flutterSemanticsTree_.size());
      // for(int64_t i=1; i<elementInfoCount; i++) {
      //   ArkUI_AccessibilityElementInfo* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
      //   FlutterNodeToElementInfoById(newElementInfo, i); 
      // }

  } else if(mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CHILDREN) {
      /** Search for child nodes at the next level. (mode = 4) */
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
      // int64_t elementInfoCount = static_cast<int64_t>(flutterSemanticsTree_.size());
      // for(int64_t i=1; i<elementInfoCount; i++) {
      //   ArkUI_AccessibilityElementInfo* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
      //   FlutterNodeToElementInfoById(newElementInfo, i); 
      // }

  } else if(mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN) {
      /** Search for all child nodes. (mode = 8) */
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
      int64_t elementInfoCount = static_cast<int64_t>(flutterSemanticsTree_.size());
      for(int64_t i=1; i<elementInfoCount; i++) {
        ArkUI_AccessibilityElementInfo* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
        FlutterNodeToElementInfoById(newElementInfo, i); 
      }

  } else {
      FlutterNodeToElementInfoById(elementInfoFromList, elementId); 
  }

  FML_DLOG(INFO) << "--- FindAccessibilityNodeInfosById is end ---";
  return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

/** Called to obtain element information based on a specified node and text
 * content. */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosByText(
    int64_t elementId,
    const char* text,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList) {
  //TODO
  FML_DLOG(INFO)<<"=== FindAccessibilityNodeInfosByText is end";
  return 0;
}
int32_t OhosAccessibilityBridge::FindFocusedAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusType focusType,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementinfo) {
  //TODO
  FML_DLOG(INFO)<<"=== FindFocusedAccessibilityNode is end";

  return 0;
}
int32_t OhosAccessibilityBridge::FindNextFocusAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementList) {
  //TODO
  FML_DLOG(INFO)<<"=== FindNextFocusAccessibilityNode is end";
  return 0;
}

/** 将arkui的action类型转化为flutter的action类型 */
int32_t ActionTypeConversion(ArkUI_Accessibility_ActionType arkui_action) {
  //部分arkui操作和flutter操作的映射，其余action暂时无法适配, 这里先return 0
  switch(arkui_action) {
    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
      return static_cast<int32_t>(ACTIONS_::kTap);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
      return static_cast<int32_t>(ACTIONS_::kLongPress);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
      return static_cast<int32_t>(ACTIONS_::kScrollUp);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
      return static_cast<int32_t>(ACTIONS_::kScrollDown);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
      return static_cast<int32_t>(ACTIONS_::kCopy);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
      return static_cast<int32_t>(ACTIONS_::kCut);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
      return static_cast<int32_t>(ACTIONS_::kDidGainAccessibilityFocus);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
      return static_cast<int32_t>(ACTIONS_::kDidLoseAccessibilityFocus);

    //Text selection action, requiring the setting of <b>selectTextBegin</b>, <b>TextEnd</b>, and <b>TextInForward</b>
    //parameters to select a text segment in the text box. */
    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT:
      return static_cast<int32_t>(ACTIONS_::kSetSelection);

    case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT:
      return static_cast<int32_t>(ACTIONS_::kSetText);

    default: 
      return static_cast<int32_t>(ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_INVALID);
  }
}

void DispatchSemanticsAction(int32_t id, flutter::SemanticsAction action ) {
    FML_DLOG(INFO)<<"DispatchSemanticsAction is called";
    // SHELL_HOLDER->GetPlatformView()->DispatchSemanticsAction(id, action, {});
}

/**
 * TODO: 执行语义动作解析，当FindAccessibilityNodeInfosById找到相应的elementinfo时才会触发该回调函数
 */
int32_t OhosAccessibilityBridge::ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments *actionArguments,
    int32_t requestId) {

  FML_DLOG(INFO)<<"ExecuteAccessibilityAction input-params-> elementId="<<elementId<<" action="<<action<<" requestId="<<requestId<<" *actionArguments="<<actionArguments;

  if(actionArguments == nullptr) {
    FML_DLOG(ERROR)<<"OhosAccessibilityBridge::ExecuteAccessibilityAction actionArguments = null";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
  }

  // std::string key = "";
  // char* value;
  // int32_t ret = OH_ArkUI_FindAccessibilityActionArgumentByKey(actionArguments, key.c_str(), &value);
  // if(value == nullptr) {
  //   FML_DLOG(ERROR)<<"ExecuteAccessibilityAction OH_ArkUI_FindAccessibilityActionArgumentByKey value = null";
  //   return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
  // } 
  // if(ret == ArkUI_AcessbilityErrorCode::ARKUI_ACCESSIBILITY_NATIVE_RESULT_BAD_PARAMETER) {
  //   FML_DLOG(ERROR)<<"ExecuteAccessibilityAction OH_ArkUI_FindAccessibilityActionArgumentByKey BAD_PARAMETER";
  // } else if(ret == ArkUI_AcessbilityErrorCode::ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED) {
  //   FML_DLOG(ERROR)<<"ExecuteAccessibilityAction OH_ArkUI_FindAccessibilityActionArgumentByKey RESULT_FAILED";
  // } else {
  //   FML_DLOG(INFO)<<"ExecuteAccessibilityAction OH_ArkUI_FindAccessibilityActionArgumentByKey RESULT_SUCCESSFUL";
  // }

  //根据当前elementid和无障碍动作类型，发送无障碍事件
  ArkUI_AccessibilityEventType eventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUSED;
  Flutter_SendAccessibilityAsyncEvent(elementId, eventType); 

  FML_DLOG(INFO)<<"OhosAccessibilityBridge::ExecuteAccessibilityAction is end";
  return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

int32_t OhosAccessibilityBridge::ClearFocusedFocusAccessibilityNode() {
  //todo ...
  FML_DLOG(INFO)<<"=== ClearFocusedFocusAccessibilityNode is end";
  return 0;
}
int32_t OhosAccessibilityBridge::GetAccessibilityNodeCursorPosition(int64_t elementId,
                                           int32_t requestId,
                                           int32_t* index) {
  //todo ...
  FML_DLOG(INFO)<<"=== GetAccessibilityNodeCursorPosition is end";
  return 0;
}

/**
 * 自定义无障碍异步事件发送
 */
void OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent(int64_t elementId, ArkUI_AccessibilityEventType eventType) {
  //1.创建eventInfo对象
  ArkUI_AccessibilityEventInfo *eventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
  if(eventInfo == nullptr) {
    FML_DLOG(ERROR)<<"Flutter_SendAccessibilityAsyncEvent OH_ArkUI_CreateAccessibilityEventInfo eventInfo = null";
    return;
  }


  //2.创建的elementinfo并根据对应id的flutternode进行属性初始化
  ArkUI_AccessibilityElementInfo* _elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();
  FlutterNodeToElementInfoById(_elementInfo, elementId);
  OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused(_elementInfo, true); //debug

  //3.设置发送事件，如配置获焦事件
  OH_ArkUI_AccessibilityEventSetEventType(eventInfo, eventType);

  //4.将eventinfo事件和当前elementinfo进行绑定
  OH_ArkUI_AccessibilityEventSetElementInfo(eventInfo, _elementInfo);

  //5.调用接口发送到ohos侧
  auto callback = [](int32_t errorCode) {
    FML_DLOG(INFO)<<"Flutter_SendAccessibilityAsyncEvent callback-> errorCode ="<<errorCode;
  };

  //6.发送event到OH侧
  if(provider_ == nullptr) {
    FML_DLOG(ERROR)<<"Flutter_SendAccessibilityAsyncEvent AccessibilityProvider = nullptr";
    return;
  }
  OH_ArkUI_SendAccessibilityAsyncEvent(provider_, eventInfo, callback);

  //7.销毁新创建的elementinfo
  // OH_ArkUI_DestoryAccessibilityElementInfo(_elementInfo);

  FML_DLOG(INFO)<<"OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent is end";
  return;
}

/**
 * 判断当前语义节点是否获焦
 */
bool OhosAccessibilityBridge::IsNodeFocusable(
    const flutter::SemanticsNode& node) {

  if (node.HasFlag(FLAGS_::kScopesRoute)) {
    return false;
  }
  if (node.HasFlag(FLAGS_::kIsFocusable)) {
    return true;
  }
  // Always consider platform views focusable.
  if (node.IsPlatformViewNode()) {
    return true;
  }
  // Always consider actionable nodes focusable.
  if (node.actions != 0) {
    return true;
  }
  // Consider text nodes focusable.
  return !node.label.empty() || !node.value.empty() || !node.hint.empty();
}

/**
 * 从dart侧传递到c++侧的flutter语义树节点更新过程
 */
void OhosAccessibilityBridge::updateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions) {
  FML_DLOG(INFO)<< ("OhosAccessibilityBridge::updateSemantics is called");

  //每次flutter语义节点更新，需要清空语义树容器节点信息
  flutterSemanticsTree_.clear();
  parentChildIdVec.clear();

  std::set<SEMANTICS_NODE_> visitedObjs;
  // SEMANTICS_NODE_ rootObj = getRootSemanticsNode();
  std::vector<SEMANTICS_NODE_> newRoutes;

  // 遍历更新的actions，并将所有的actions的id添加进actionMap
  for (const auto& item : actions) {
    const flutter::CustomAccessibilityAction action = item.second;
    printTestActions(action);
    actions_mp_[action.id] = action;
  }

  // Dispatch a TYPE_WINDOW_STATE_CHANGED event if the most recent route id
  // changed from the
  // previously cached route id.
  // Finds the last route that is not in the previous routes.
  // SEMANTICS_NODE_ lastAdded;
  // lastAdded.id = -2;
  // for (const auto& _node : newRoutes) {
  //   if (!flutterNavigationStack[_node.id]) {
  //     lastAdded = _node;
  //   }
  // }
  // // If all the routes are in the previous route, get the last route.
  // if (lastAdded.id == -2 && newRoutes.size() > 0) {
  //   lastAdded = newRoutes[newRoutes.size() - 1];
  // }

  // There are two cases if lastAdded != nil
  // 1. lastAdded is not in previous routes. In this case,
  //    lastAdded.id != previousRouteId
  // 2. All new routes are in previous routes and
  //    lastAdded = newRoutes.last.
  // In the first case, we need to announce new route. In the second case,
  // we need to announce if one list is shorter than the other.
  // if (lastAdded.id != -2 && (lastAdded.id != previousRouteId ||
  //                         newRoutes.size() !=
  // flutterNavigationStack.size())) {
  //   previousRouteId = lastAdded.id;
  //   onWindowNameChange(lastAdded);  // todo
  // }
  // flutterNavigationStack.clear();
  // for (const auto& _node : newRoutes) {
  //   flutterNavigationStack.emplace_back(_node.id);
  // }

  // for (const auto& item : flutterSemanticsTree_) {
  //   SEMANTICS_NODE_ obj = item.second;
  //   if (visitedObjs.find(obj) == visitedObjs.end()) {
  //     removeSemanticsNode(obj);
  //     flutterSemanticsTree_.erase(item.first);
  //   }
  // }

  for (auto& item : update) {
    // 获取当前更新的节点node
    const flutter::SemanticsNode& node = item.second;
    printTest(node);  // print node struct for debugging

    //构建flutter无障碍语义节点树
    flutterSemanticsTree_.insert({node.id, node});

    if (node.HasFlag(FLAGS_::kIsHidden)) {  // 判断当前更新节点是隐藏的
      continue;
    }
    if (IsNodeFocusable(node)) {  // 判断当前更新节点是否获焦
      FML_DLOG(INFO) <<"UpdateSemantics -> flutterNode is focusable, node.id="<<node.id;
      //将获焦语义节点加入到flutterSemanticsTree
      // flutterSemanticsTree_.insert({node.id, node});
      // flutterSemanticsTreeVec.push_back(node);
    }

    // todo：根据nodeId获取当前os对应的真实节点
    //  currentNode =
    //  ArkUI_AccessibilityElementInfo::createAccessibilityElementInfo(node.id);

    // todo：获取当前节点的全部子节点数量，并构建当前节点的全部更新子节点
    int32_t childNodeCount = node.childrenInTraversalOrder.size();
    for (int32_t i = 0; i < childNodeCount; i++) {
      parentChildIdVec.emplace_back(std::make_pair(node.id, node.childrenInTraversalOrder[i]));
      FML_DLOG(INFO) <<"UpdateSemantics parentChildIdMap -> ("<<node.id<<", "<<node.childrenInTraversalOrder[i]<<")";

      // todo：通过遍历当前节点的子节点，并对所有子节点进行逐一构建os对应的elementinfo
      // AccessibilityElementInfo* child = createAccessibilityElementInfo(node.childrenInTraversalOrder[i]);
      // todo：将所有的新child节点加入到os对应的elementInfoList中
    }
    // TODO: 将更新后的全部子节点赋值给当前真实节点
    // currentNode = newChildren

    // todo: 是否触发滑动操作
    bool didScroll = true;
    if (didScroll) {
      // 1. 声明并创建accessibilityEvent类型，比如滑动事件
      // 2.
      // 获取semanticsNode里scrollPosition、scrollExtensionMax、scrollExtensionMin字段
      // 3. 发送事件中包含上述scroll位置变动信息(如下所示)
      // int32_t scrollChildren = 0;
      // int32_t scrollIndex = 0;
      // double scrollPosition = std::nan("");
      // double scrollExtentMax = std::nan("");
      // double scrollExtentMin = std::nan("");
      int32_t scrollChildren = 0;
      if (scrollChildren > 0) {
        // todo 发送事件，包含scrollChildren数量、scrollIndex
        // int visibleChildren = 0;
        // // handle hidden children at the beginning and end of the list.
        // for (flutter::SemanticsNode child : node.childrenInHitTestOrder) {
        //   if (!child.hasFlag(Flag.IS_HIDDEN)) {
        //     visibleChildren += 1;
        //   }
        // }
      }
      // sendAccessibilityEvent(event)
    }
    // todo: 判断是否触发liveRegion活动区，是否活跃
    if (node.HasFlag(FLAGS_::kIsLiveRegion)) {
      // sendWindowContentChangeEvent(object.id);
    }

    // todo：当前焦点语义节点
    bool isHadFlag = false;  // 这里判断previousFlag和当前flag是否相同
    std::shared_ptr<flutter::SemanticsNode> accessibilityFocusedSemanticsNode;
    if (accessibilityFocusedSemanticsNode != nullptr &&
        accessibilityFocusedSemanticsNode->id == node.id && !isHadFlag &&
        node.HasFlag(FLAGS_::kIsSelected)) {
      // todo：创建并发送事件
      // AccessibilityEvent event = obtainAccessibilityEvent(
      //     node.id, AccessibilityEvent.TYPE_VIEW_SELECTED);
      // event.getText().add(object.label);
      // sendAccessibilityEvent(event);
    }

    // todo: 若该对象是输入焦点节点，且发生更新变化，则发送给os有关它的信息
    std::shared_ptr<flutter::SemanticsNode>
        inputFocusedSemanticsNode;  // 当前输入焦点节点
    std::shared_ptr<flutter::SemanticsNode>
        lastInputFocusedSemanticsNode;  // 上一个输入焦点节点
    if (inputFocusedSemanticsNode != nullptr &&
        inputFocusedSemanticsNode->id == node.id &&
        (lastInputFocusedSemanticsNode == nullptr ||
         lastInputFocusedSemanticsNode->id != inputFocusedSemanticsNode->id)) {
      // 上次输入焦点节点 -> 当前输入焦点节点
      lastInputFocusedSemanticsNode = inputFocusedSemanticsNode;
      // 发送相应的输入焦点改变事件
      // sendAccessibilityEvent(obtainAccessibilityEvent(object.id,
      // AccessibilityEvent.TYPE_VIEW_FOCUSED));
    } else if (inputFocusedSemanticsNode == nullptr) {
      // There's no TYPE_VIEW_CLEAR_FOCUSED event, so if the current input focus
      // becomes null, then we just set the last one to null too, so that it
      // sends the event again when something regains focus.
      lastInputFocusedSemanticsNode = nullptr;
    }

    if (inputFocusedSemanticsNode != nullptr &&
        inputFocusedSemanticsNode->id == node.id && isHadFlag &&
        node.HasFlag(FLAGS_::kIsTextField)
        // If we have a TextField that has InputFocus, we should avoid
        // announcing it if something else we track has a11y focus. This needs
        // to still work when, e.g., IME has a11y focus or the "PASTE" popup is
        // used though. See more discussion at
        // https://github.com/flutter/flutter/issues/23180
        && (accessibilityFocusedSemanticsNode == nullptr ||
            (accessibilityFocusedSemanticsNode->id ==
             inputFocusedSemanticsNode->id))) {
      // 这里写输入框更新文字内容，将老旧的文本替换为新输入文字，并发送textchange事件
      // AccessibilityEvent event = createTextChangedEvent(object.id, oldValue,
      // newValue); sendAccessibilityEvent(event);

      // todo：若当前textselection部分和之前的textselection部分不同，则触发
      int32_t previousTextSelectionBase = 0;
      int32_t previousTextSelectionExtent = 1;
      if (previousTextSelectionBase != node.textSelectionBase ||
          previousTextSelectionExtent != node.textSelectionExtent) {
        // 创建并发送textselection改变事件
        //  AccessibilityEvent selectionEvent = obtainAccessibilityEvent(
        //      object.id, AccessibilityEvent.TYPE_VIEW_TEXT_SELECTION_CHANGED);
        //  selectionEvent.getText().add(newValue);
        //  selectionEvent.setFromIndex(object.textSelectionBase);
        //  selectionEvent.setToIndex(object.textSelectionExtent);
        //  selectionEvent.setItemCount(newValue.length());
        //  sendAccessibilityEvent(selectionEvent);
      }
    }
  }

  for(const auto& item: flutterSemanticsTree_) {
    FML_DLOG(INFO)<<"flutterSemanticsTree_ -> {"<<item.first<<", "<<item.second.id<<"}";
  }
  for(const auto& item: parentChildIdVec) {
    FML_DLOG(INFO)<<"parentChildIdVec -> ("<<item.first<<", "<<item.second<<")";
  }
}

/** 获取当前flutter节点的组件类型 */
std::string OhosAccessibilityBridge::GetNodeComponentType(
    const flutter::SemanticsNode& node) {

  if((!node.label.empty() || !node.tooltip.empty() || !node.hint.empty()) && node.id != 0) {
    return "text";
  }

  if (node.HasFlag(FLAGS_::kIsButton)) {
    return "button";
  }

  if (node.HasFlag(FLAGS_::kIsTextField)) {
    return "TextField";
  }

  if (node.HasFlag(FLAGS_::kIsLink)) {
    return "Link";
  }

  if (node.HasFlag(FLAGS_::kIsSlider) || node.HasAction(ACTIONS_::kIncrease) ||
      node.HasAction(ACTIONS_::kDecrease)) {
    return "Slider";
  }

  if (node.HasFlag(FLAGS_::kIsHeader)) {
    return "Header";
  }
  if (node.HasFlag(FLAGS_::kIsImage)) {
    return "Image";
  }

  if (node.HasFlag(FLAGS_::kHasCheckedState)) {
    if (node.HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup)) {
      return "RadioButton";
    } else {
      return "CheckBox";
    }
  }

  if (node.HasFlag(FLAGS_::kHasToggledState)) {
    return "ToggleSwitch";
  }

  return "Widget" + std::to_string(node.id);
}

// 获取根节点
flutter::SemanticsNode OhosAccessibilityBridge::getFlutterRootSemanticsNode() {
  if(!flutterSemanticsTree_.size()) {
     FML_DLOG(ERROR)<<"getFlutterRootSemanticsNode -> flutterSemanticsTree_.size()=0";
    return flutter::SemanticsNode{};
  }
  if(flutterSemanticsTree_.find(0) == flutterSemanticsTree_.end()) {
     FML_DLOG(ERROR)<<"getFlutterRootSemanticsNode -> flutterSemanticsTree_ has no keys = 0";
      return flutter::SemanticsNode{};
  }
  return flutterSemanticsTree_.at(0);
}

void OhosAccessibilityBridge::onWindowNameChange(flutter::SemanticsNode route) {
    //todo ...
}

void OhosAccessibilityBridge::removeSemanticsNode(
    flutter::SemanticsNode nodeToBeRemoved) {
  //todo ...
  if(!flutterSemanticsTree_.size()) {
     FML_DLOG(ERROR)<<"OhosAccessibilityBridge::removeSemanticsNode -> flutterSemanticsTree_.szie()=0";
    return;
  }
  if (flutterSemanticsTree_.find(nodeToBeRemoved.id) ==
      flutterSemanticsTree_.end()) {
    FML_DLOG(INFO) << "Attempted to remove a node that is not in the tree.";
  }
  // if (flutterSemanticsTree_.at(nodeToBeRemoved.id) != nodeToBeRemoved) {
  //   FML_DLOG(ERROR) << "Flutter semantics tree failed to get expected node "
  //                      "when searching by id.";
  // }
  // nodeToBeRemoved.parent = nullptr;
  if (nodeToBeRemoved.platformViewId != -1) {
  }
}

void OhosAccessibilityBridge::printTest(flutter::SemanticsNode node) {
  FML_DLOG(INFO) << "==================SemanticsNode=====================";
  SkMatrix _transform = node.transform.asM33();
  FML_DLOG(INFO) << "node.id=" << node.id;
  FML_DLOG(INFO) << "node.flags=" << node.flags;
  FML_DLOG(INFO) << "node.actions=" << node.actions;
  FML_DLOG(INFO) << "node.rect= {" << node.rect.fLeft<<", "<<node.rect.fTop<<", "<<node.rect.fRight<<", "<<node.rect.fBottom<<"}";
  FML_DLOG(INFO) << "node.transform -> kMScaleX=" << _transform.get(SkMatrix::kMScaleX);
  FML_DLOG(INFO) << "node.transform -> kMSkewX=" << _transform.get(SkMatrix::kMSkewX);
  FML_DLOG(INFO) << "node.transform -> kMTransX=" << _transform.get(SkMatrix::kMTransX);
  FML_DLOG(INFO) << "node.transform -> kMSkewY=" << _transform.get(SkMatrix::kMSkewY);
  FML_DLOG(INFO) << "node.transform -> kMScaleY=" << _transform.get(SkMatrix::kMScaleY);
  FML_DLOG(INFO) << "node.transform -> kMTransY=" << _transform.get(SkMatrix::kMTransY);
  FML_DLOG(INFO) << "node.transform -> kMPersp0=" << _transform.get(SkMatrix::kMPersp0);
  FML_DLOG(INFO) << "node.transform -> kMPersp1=" << _transform.get(SkMatrix::kMPersp1);
  FML_DLOG(INFO) << "node.transform -> kMPersp2=" << _transform.get(SkMatrix::kMPersp2);
  FML_DLOG(INFO) << "node.maxValueLength=" << node.maxValueLength;
  FML_DLOG(INFO) << "node.currentValueLength=" << node.currentValueLength;
  FML_DLOG(INFO) << "node.textSelectionBase=" << node.textSelectionBase;
  FML_DLOG(INFO) << "node.textSelectionExtent=" << node.textSelectionExtent;
  FML_DLOG(INFO) << "node.textSelectionBase=" << node.textSelectionBase;
  FML_DLOG(INFO) << "node.platformViewId=" << node.platformViewId;
  FML_DLOG(INFO) << "node.scrollChildren=" << node.scrollChildren;
  FML_DLOG(INFO) << "node.scrollIndex=" << node.scrollIndex;
  FML_DLOG(INFO) << "node.scrollPosition=" << node.scrollPosition;
  FML_DLOG(INFO) << "node.scrollIndex=" << node.scrollIndex;
  FML_DLOG(INFO) << "node.scrollPosition=" << node.scrollPosition;
  FML_DLOG(INFO) << "node.scrollExtentMax=" << node.scrollExtentMax;
  FML_DLOG(INFO) << "node.scrollExtentMin=" << node.scrollExtentMin;
  FML_DLOG(INFO) << "node.elevation=" << node.elevation;
  FML_DLOG(INFO) << "node.thickness=" << node.thickness;
  FML_DLOG(INFO) << "node.label=" << node.label;
  FML_DLOG(INFO) << "node.tooltip=" << node.tooltip;
  FML_DLOG(INFO) << "node.hint=" << node.hint;
  FML_DLOG(INFO) << "node.textDirection=" << node.textDirection;
  FML_DLOG(INFO) << "node.childrenInTraversalOrder.size()="
                 << node.childrenInTraversalOrder.size();
  for (uint32_t i = 0; i < node.childrenInTraversalOrder.size(); i++) {
    FML_DLOG(INFO) << "node.childrenInTraversalOrder[" << i
                   << "]=" << node.childrenInTraversalOrder[i];
  }

  FML_DLOG(INFO) << "node.childrenInHitTestOrder.size()="
                 << node.childrenInHitTestOrder.size();
  for (uint32_t i = 0; i < node.childrenInHitTestOrder.size(); i++) {
    FML_DLOG(INFO) << "node.childrenInHitTestOrder[" << i
                   << "]=" << node.childrenInHitTestOrder[i];
  }

  FML_DLOG(INFO) << "node.customAccessibilityActions.size()="
                 << node.customAccessibilityActions.size();
  for (uint32_t i = 0; i < node.customAccessibilityActions.size(); i++) {
    FML_DLOG(INFO) << "node.customAccessibilityActions[" << i
                   << "]=" << node.customAccessibilityActions[i];
  }
  FML_DLOG(INFO) << "=================SemanticsNode======================";
}

void OhosAccessibilityBridge::printTestActions(
    flutter::CustomAccessibilityAction customAccessibilityAction) {
  FML_DLOG(INFO) << "----------------SemanticsAction-------------------------";
  FML_DLOG(INFO) << "customAccessibilityAction.id="
                 << customAccessibilityAction.id;
  FML_DLOG(INFO) << "customAccessibilityAction.overrideId="
                 << customAccessibilityAction.overrideId;
  FML_DLOG(INFO) << "customAccessibilityAction.label="
                 << customAccessibilityAction.label;
  FML_DLOG(INFO) << "customAccessibilityAction.hint="
                 << customAccessibilityAction.hint;
  FML_DLOG(INFO) << "----------------SemanticsAction--------------------";
}

}  // namespace flutter
