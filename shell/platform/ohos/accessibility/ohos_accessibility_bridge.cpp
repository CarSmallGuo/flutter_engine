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

#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"
#include "flutter/fml/logging.h"
// #include
// "flutter/shell/platform/darwin/ios/framework/Source/accessibility_bridge.h"

namespace flutter {

OhosAccessibilityBridge::OhosAccessibilityBridge() = default;
OhosAccessibilityBridge::~OhosAccessibilityBridge() = default;

OhosAccessibilityBridge& OhosAccessibilityBridge::GetInstance()
{
    static OhosAccessibilityBridge ohosAccessibilityBridge;
    return ohosAccessibilityBridge;
}

void OhosAccessibilityBridge::announce(std::unique_ptr<char[]>& message) {
  FML_DLOG(INFO) << ("Native C++ OhosAccessibilityBridge::announce message: ") << (message.get());
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

void OhosAccessibilityBridge::updateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions) {
  FML_DLOG(INFO) << ("Native C++ OhosAccessibilityBridge::updateSemantics");

  // 遍历更新的actions，并将所有的actions的id添加进actionMap
  for (const auto& item : actions) {
    const flutter::CustomAccessibilityAction action = item.second;
    actions_mp_[action.id] = action;
  }

  for (auto& item : update) {
    const flutter::SemanticsNode& node = item.second;
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
      if(node.HasFlag(FLAGS_::kIsLiveRegion)) {
      // sendWindowContentChangeEvent(object.id);
      }

      //todo：当前焦点语义节点
      bool isHadFlag = false; //这里判断previousFlag和当前flag是否相同 
      std::shared_ptr<flutter::SemanticsNode> accessibilityFocusedSemanticsNode;
      if (accessibilityFocusedSemanticsNode != nullptr
          && accessibilityFocusedSemanticsNode->id == node.id
          && !isHadFlag
          && node.HasFlag(FLAGS_::kIsSelected)) {
      // todo：创建并发送事件
      // AccessibilityEvent event = obtainAccessibilityEvent(
      //     node.id, AccessibilityEvent.TYPE_VIEW_SELECTED);
      // event.getText().add(object.label);
      // sendAccessibilityEvent(event);
      }

      //todo: 若该对象是输入焦点节点，且发生更新变化，则发送给os有关它的信息
      std::shared_ptr<flutter::SemanticsNode> inputFocusedSemanticsNode; //当前输入焦点节点
      std::shared_ptr<flutter::SemanticsNode> lastInputFocusedSemanticsNode; //上一个输入焦点节点
      if (inputFocusedSemanticsNode != nullptr
          && inputFocusedSemanticsNode->id == node.id
          && (lastInputFocusedSemanticsNode == nullptr
              || lastInputFocusedSemanticsNode->id != inputFocusedSemanticsNode->id)) {
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

      if (inputFocusedSemanticsNode != nullptr
          && inputFocusedSemanticsNode->id == node.id
          && isHadFlag
          && node.HasFlag(FLAGS_::kIsTextField)
          // If we have a TextField that has InputFocus, we should avoid announcing it if something
          // else we track has a11y focus. This needs to still work when, e.g., IME has a11y focus
          // or the "PASTE" popup is used though.
          // See more discussion at https://github.com/flutter/flutter/issues/23180
          && (accessibilityFocusedSemanticsNode == nullptr
              || (accessibilityFocusedSemanticsNode->id == inputFocusedSemanticsNode->id))) {
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

void OhosAccessibilityBridge::performAction(int32_t virtualViewId,
                                            int32_t inputAction) {
  // TODO 根据输入的action进行相应的响应操作
  switch (inputAction) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    // ...
    default:
      break;
  }
}

flutter::SemanticsNode OhosAccessibilityBridge::getRootSemanticsNode() {
  return flutterSemanticsTree_.at(0);
}
// 找到当前焦点触发节点
int32_t OhosAccessibilityBridge::FindFocusedAccessibilityNode(
    int64_t elementId,
    int32_t focusType,
    int32_t requestId,
    int32_t elementinfo) {
  return 0;
}
// 找到下一个焦点触发节点
int32_t OhosAccessibilityBridge::FindNextFocusAccessibilityNode(
    int64_t elementId,
    int32_t direction,
    int32_t requestId,
    int32_t elementList) {
  return 0;
}

void ArkUI_AccessibilityElementInfo::createAccessibilityElementInfo(int vid) {
  // TODO ohos和flutter虚拟节点交互，根据虚拟节点id创建真实elementinfo
  // ArkUI_AccessibilityElementInfo相当于安卓的view
}

}  // namespace flutter
