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

// #include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"
#include "flutter/fml/logging.h"
#include "ohos_accessibility_bridge.h"

namespace flutter {

OhosAccessibilityBridge::OhosAccessibilityBridge() {
  
  // 判断是否开启无障碍服务
  if (isOhosAccessibilityEnabled_) {
    FML_DLOG(INFO) << "OhosAccessibilityBridge::OhosAccessibilityBridge "
                      "isOhosAccessibilityEnabled = true";
  }
}
OhosAccessibilityBridge::~OhosAccessibilityBridge() {};

OhosAccessibilityBridge& OhosAccessibilityBridge::GetInstance() {
  static OhosAccessibilityBridge ohosAccessibilityBridge;
  return ohosAccessibilityBridge;
}

void OhosAccessibilityBridge::announce(std::unique_ptr<char[]>& message) {
  FML_DLOG(INFO) << ("Native C++ OhosAccessibilityBridge::announce message: ")
                 << (message.get());
  //创建并设置屏幕朗读事件
  ArkUI_AccessibilityEventInfo* announceEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
  int32_t ret1 = OH_ArkUI_SetAccessibilityEventEventType(announceEventInfo, ArkUI_AccessibilityEventType::ARKUI_NATIVE_ACCESSIBILITY_TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY);
  if(ret1 != 0) {
      FML_DLOG(INFO) << "OhosAccessibilityBridge::announce OH_ArkUI_AccessibilityEventSetEventType failed";
      return;
  }
  int32_t ret2 = OH_ArkUI_SetAccessibilityEventTextAnnouncedForAccessibility(announceEventInfo, message.get()); 
  if(ret2 != 0) {
      FML_DLOG(INFO) << "OhosAccessibilityBridge::announce OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility failed";
      return;
  }

  return;
}

flutter::SemanticsNode OhosAccessibilityBridge::getOrCreateSemanticsNode(
    int32_t id) {
  flutter::SemanticsNode node;
  if (flutterSemanticsTree_.find(id) != flutterSemanticsTree_.end()) {
    node = flutterSemanticsTree_.at(id);
  } else {
    flutterSemanticsTree_[id] = node;
  }
  return node;
}
/**
 * flutter初始化配置给arkui创建的elementInfo
 */
void OhosAccessibilityBridge::flutterInitElementInfo(ArkUI_AccessibilityElementInfo* elementInfo) {
  if(elementInfo == nullptr) {
    FML_DLOG(ERROR)<<"OhosAccessibilityBridge::flutterInitElementInfo -> elementInfo = nullptr";
  }
  if(!flutterSemanticsTree_.size()) {
    FML_DLOG(ERROR)<<"OhosAccessibilityBridge::flutterInitElementInfo flutterSemanticsTree_.size() = 0";
    return;
  }
  //将flutter语义节点树传递给arkui的无障碍elementinfo
  for(const auto& item: flutterSemanticsTree_) {
    SEMANTICS_NODE_ flutterNode = item.second;
    OH_ArkUI_SetAccessibilityElementInfoElementId(elementInfo, flutterNode.id);
    OH_ArkUI_SetAccessibilityElementInfoAccessibilityText(elementInfo, flutterNode.label.c_str());

    int32_t childCount = static_cast<int32_t>(flutterNode.childrenInTraversalOrder.size());
    int64_t childNodeIds[childCount];
    for(int32_t i=0; i<childCount; i++) {
      childNodeIds[i] = static_cast<int64_t>(flutterNode.childrenInTraversalOrder[i]);
    }
    OH_ArkUI_SetAccessibilityElementInfoChildNodeIds(elementInfo, childCount, childNodeIds);

    // force to true for debugging
    OH_ArkUI_SetAccessibilityElementInfoFocusable(elementInfo, true);
    OH_ArkUI_SetAccessibilityElementInfoFocused(elementInfo, true); 
    OH_ArkUI_SetAccessibilityElementInfoVisible(elementInfo, true);
    OH_ArkUI_SetAccessibilityElementInfoAccessibilityFocused(elementInfo, true);
    OH_ArkUI_SetAccessibilityElementInfoSelected(elementInfo, true);
    OH_ArkUI_SetAccessibilityElementInfoEnabled(elementInfo, true);
  }
}

/**
 * Called to obtain element information based on a specified node.
 * NOTE: 该arkui接口需要在系统无障碍服务开启时，才能触发调用
 * TODO: 当前实现版本为实现简易功能，仅对flutter语义focused节点进行交互，后续完善并优化
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosById(
    int64_t elementId,
    ArkUI_AccessibilitySearchMode mode,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList) {
  if (elementList == nullptr) {
    FML_DLOG(INFO) << "OhosAccessibilityBridge::FindAccessibilityNodeInfosById "
                      "elementList is null";
    return OH_ARKUI_ACCESSIBILITY_RESULT_FAILED;
  }
  //创建elementinfo
  ArkUI_AccessibilityElementInfo* focusedElementInfo =
      OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
  if (focusedElementInfo == nullptr) {
    FML_DLOG(INFO) << "OhosAccessibilityBridge::FindAccessibilityNodeInfosById "
                      "elementInfo is null";
    return OH_ARKUI_ACCESSIBILITY_RESULT_FAILED;
  }

   /** Search for current nodes. */
  if(mode == ArkUI_AccessibilitySearchMode::NATIVE_SEARCH_MODE_PREFETCH_CURRENT) {
     flutterInitElementInfo(focusedElementInfo);
  } else if(ArkUI_AccessibilitySearchMode::NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS) {
    /** Search for parent nodes. */
    flutterInitElementInfo(focusedElementInfo);
  } else if(ArkUI_AccessibilitySearchMode::NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS) {
     /** Search for sibling nodes. */
     flutterInitElementInfo(focusedElementInfo);
  } else if(ArkUI_AccessibilitySearchMode::NATIVE_SEARCH_MODE_PREFETCH_CHILDREN) {
    /** Search for child nodes at the next level. */
     flutterInitElementInfo(focusedElementInfo);
  } else { //ArkUI_AccessibilitySearchMode::NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN
    /** Search for all child nodes. */
     flutterInitElementInfo(focusedElementInfo);
  }
  FML_DLOG(INFO) << "OhosAccessibilityBridge::FindAccessibilityNodeInfosById is succeed!";
  return OH_ARKUI_ACCESSIBILITY_RESULT_SUCCESS;
}

/** Called to obtain element information based on a specified node and text
 * content. */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosByText(
    int64_t elementId,
    const char* text,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList) {

  return 0;
}
int32_t OhosAccessibilityBridge::FindFocusedAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusType focusType,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementinfo) {
    //todo ...

  return 0;
}
int32_t OhosAccessibilityBridge::FindNextFocusAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementList) {
    //todo ...

  return 0;
}
int32_t OhosAccessibilityBridge::ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments *actionArguments,
    int32_t requestId) {
  //todo ...

  return 0;
}
int32_t OhosAccessibilityBridge::ClearFocusedFocusAccessibilityNode() {
  //todo ...

  return 0;
}
int32_t OhosAccessibilityBridge::GetAccessibilityNodeCursorPosition(int64_t elementId,
                                           int32_t requestId,
                                           int32_t* index) {
  //todo ...
  return 0;
}

void OhosAccessibilityBridge::ArkUI_SendAccessibilityAsyncEvent(ArkUI_AccessibilityProvider* provider) {
  if(provider == nullptr) {
    FML_DLOG(ERROR)<<"OhosAccessibilityBridge::ArkUI_SendAccessibilityAsyncEvent provider = null";
    return;
  }
  //创建eventInfo对象
  ArkUI_AccessibilityEventInfo *eventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
  if(eventInfo == nullptr) {
    FML_DLOG(ERROR)<<"OH_ArkUI_CreateAccessibilityEventInfo eventInfo = null";
    return;
  }
  auto callback = [](int32_t errorCode) {
    if(errorCode == 0) {
      FML_DLOG(ERROR)<<"OhosAccessibilityBridge::ArkUI_SendAccessibilityAsyncEvent errorCode = 0";
    } 
  };
  //发送event到OH侧
  OH_ArkUI_SendAccessibilityAsyncEvent(provider, eventInfo, callback);

  FML_DLOG(ERROR)<<"OhosAccessibilityBridge::ArkUI_SendAccessibilityAsyncEvent is succeed";
  return;
}


/**
 * 从dart侧传递到c++侧的flutter语义树节点更新过程
 */
void OhosAccessibilityBridge::updateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions) {
  FML_DLOG(INFO)
      << ("Native C++ OhosAccessibilityBridge::updateSemantics is called");

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

    if (node.HasFlag(FLAGS_::kIsHidden)) {  // 判断当前更新节点是隐藏的
      continue;
    }
    if (node.HasFlag(FLAGS_::kIsFocused)) {  // 判断当前更新节点是否获焦
      FML_DLOG(INFO) <<"OhosAccessibilityBridge::updateSemantics node.HasFlag(FLAGS_::kIsFocused) node.id="<<node.id;
      //将获焦语义节点加入到flutterSemanticsTree
      flutterSemanticsTree_.insert({node.id, node});
      // flutterSemanticsTreeVec.push_back(node);
      FML_DLOG(INFO) <<"flutterSemanticsTree_.insert is succeed";

    }

    // todo：根据nodeId获取当前os对应的真实节点
    //  currentNode =
    //  ArkUI_AccessibilityElementInfo::createAccessibilityElementInfo(node.id);

    // todo：获取当前节点的全部子节点数量，并构建当前节点的全部更新子节点
    int32_t newChildCount = node.childrenInTraversalOrder.size();
    // todo：声明并创建当前节点的新的子节点
    //  declare一个newChildren
    for (int32_t i = 0; i < newChildCount; i++) {
      // todo：通过遍历当前节点的子节点，并对所有子节点进行逐一构建os对应的elementinfo
      // AccessibilityElementInfo child =
      // createAccessibilityElementInfo(node.childrenInTraversalOrder[i]);
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
}

// 获取根节点
flutter::SemanticsNode OhosAccessibilityBridge::getRootSemanticsNode() {
  if(!flutterSemanticsTree_.size()) {
     FML_DLOG(ERROR)<<"OhosAccessibilityBridge::flutterInitElementInfo -> flutterSemanticsTree_.size()=0";
    return flutter::SemanticsNode{};
  }
  return flutterSemanticsTree_.at(0);
}

void OhosAccessibilityBridge::onWindowNameChange(flutter::SemanticsNode route) {

}

void OhosAccessibilityBridge::removeSemanticsNode(
    flutter::SemanticsNode nodeToBeRemoved) {
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
  FML_DLOG(INFO) << "node.id=" << node.id;
  FML_DLOG(INFO) << "node.flags=" << node.flags;
  FML_DLOG(INFO) << "node.actions=" << node.actions;
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
