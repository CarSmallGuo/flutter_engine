/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "ohos_accessibility_bridge.h"
#include <limits>
#include <cstring>
#include <deviceinfo.h>
#include "flutter/fml/logging.h"
#include "flutter/shell/platform/ohos/ohos_logging.h"
#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/platform/embedder/embedder.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include "flutter/shell/platform/ohos/ohos_xcomponent_adapter.h"

namespace flutter {

const int32_t OhosAccessibilityBridge::OHOS_API_VERSION = OH_GetSdkApiVersion();

std::unique_ptr<OhosAccessibilityBridge> OhosAccessibilityBridge::bridgeInstance_ = nullptr;

/**
 * flutter无障碍相关语义树、句柄指针provider等参数
 * 跟随xcomponentid显示切换，而加载对应xcomponent的语义树和provider指针
 * @NOTE: 当屏幕同时显示多个xcomponent时，无法通过聚焦事件而触发xcomponentid改变
 */
void OhosAccessibilityBridge::AccessibiltiyChangesWithXComponentId()
{   
    auto xcompMap = XComponentAdapter::GetInstance()->xcomponetMap_;
    // 获取当前显示的xcomponetid
    std::string currXcompId = XComponentAdapter::GetInstance()->currentXComponentId_;
    if (xcomponentId_ == currXcompId) { return; }

    auto it = xcompMap.find(currXcompId);
    if (!xcompMap.empty() && it != xcompMap.end()) {
        // 更新xcompid，shellholderId，provider指针
        xcomponentId_ = currXcompId;
        native_shell_holder_id_ = std::stoll(it->second->shellholderId_);
        g_flutterSemanticsTree = g_flutterSemanticsTreeXComponents[xcomponentId_];
        g_parentChildIdVec = g_parentChildIdVecXComponents[xcomponentId_];
        FML_DLOG(INFO) << "AccessibiltiyChangesWithXComponentId -> xcomponentid:" << xcomponentId_;
    } else {
        xcomponentId_ = "oh_flutter_1";
        g_flutterSemanticsTree = g_flutterSemanticsTreeXComponents[xcomponentId_];
        g_parentChildIdVec = g_parentChildIdVecXComponents[xcomponentId_];
        FML_DLOG(INFO) << "AccessibiltiyChangesWithXComponentId -> xcomponentid:" << xcomponentId_;
    }
}

/**
 * 采用局部静态变量配合call-once特性，实现线程安全的单例模式
 */
OhosAccessibilityBridge* OhosAccessibilityBridge::GetInstance() 
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        bridgeInstance_.reset(new OhosAccessibilityBridge());
    });
    return bridgeInstance_.get();
}

OhosAccessibilityBridge::OhosAccessibilityBridge()
    : isFlutterNavigated_(false),
      isAccessibilityEnabled_(false) {}

/**
 * 监听当前ohos平台是否开启无障碍屏幕朗读服务
 */
void OhosAccessibilityBridge::OnOhosAccessibilityStateChange(
    bool ohosAccessibilityEnabled, int64_t shellholderId)
{
    native_shell_holder_id_ = shellholderId;
    AccessibiltiyChangesWithXComponentId();
    nativeAccessibilityChannel_ = std::make_shared<NativeAccessibilityChannel>();
    accessibilityFeatures_ = std::make_shared<OhosAccessibilityFeatures>();

    if (ohosAccessibilityEnabled) {
        isAccessibilityEnabled_ = ohosAccessibilityEnabled;
        nativeAccessibilityChannel_->OnOhosAccessibilityEnabled(native_shell_holder_id_);
    } else {
        accessibilityFeatures_->SetAccessibleNavigation(false, native_shell_holder_id_);
        nativeAccessibilityChannel_->OnOhosAccessibilityDisabled(native_shell_holder_id_);
    }
}

/**
 * build the id mapping betwween parent node and its children nodes
 */
void OhosAccessibilityBridge::BuildParentChildNodeIdRelation(
    const SemanticsNodeExtent& node)
{
    if (!IsNodeVisible(node)) { return; }
    for (const auto& childId : node.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childId);
        if (!IsNodeVisible(childNode)) { continue; }
        g_parentChildIdVec.emplace_back(std::make_pair(node.id, childId));
    }
}

/**
 * 从dart侧传递到c++侧的flutter无障碍语义树节点更新过程，
 * 路由新页面、滑动页面等操作会自动触发该语义树的更新
 */
void OhosAccessibilityBridge::UpdateSemantics(
    flutter::SemanticsNodeUpdates update,
    flutter::CustomAccessibilityActionUpdates actions)
{
    FML_DLOG(INFO) << "OhosAccessibilityBridge::UpdateSemantics()";
    std::vector<SemanticsNodeExtent> updatedFlutterNodes;

    // 当flutter页面状态更新（路由新页面）时，自动请求root节点组件获焦（规避滑动组件更新干扰）
    if (isFlutterNavigated_) {
        RequestFocusWhenPageUpdate(0);
        isFlutterNavigated_ = false;
    }

    /** 获取并分析每个语义节点的更新属性 */
    for (const auto& item : update) {
        // 获取当前更新的节点node
        const auto& node = item.second;
        FML_DLOG(INFO) << "*#*#* node.id=" << node.id;
        // 更新扩展的SemanticsNode信息
        auto nodeEx = UpdatetSemanticsNodeExtent(node);

        // 构建flutter无障碍语义节点树
        g_flutterSemanticsTree[nodeEx.id] = nodeEx;
        // 构建flutter节点的父子id映射关系
        BuildParentChildNodeIdRelation(nodeEx);

        //print semantics node and flags info for debugging
        GetSemanticsNodeDebugInfo(nodeEx);
        GetSemanticsFlagsDebugInfo(nodeEx);

        if (!IsNodeVisible(nodeEx)) { continue; }

        // 若当前节点为获焦
        if (IsNodeFocused(nodeEx)) {
            inputFocusedNode = nodeEx;
        }
        // 若当前节点和更新前节点信息不同，则加入更新节点数组
        if (nodeEx.hadPreviousConfig) {
            updatedFlutterNodes.emplace_back(nodeEx);
            FML_DLOG(INFO) << "updatedFlutterNodes -> node.id=" << nodeEx.id;
        }
    }

    // 将更新后的flutter语义树和父子节点id映射缓存，保存到相应的xcomponent里面
    g_flutterSemanticsTreeXComponents[xcomponentId_] = g_flutterSemanticsTree;
    g_parentChildIdVecXComponents[xcomponentId_] = g_parentChildIdVec;

    // 页面内容更新事件
    Flutter_SendAccessibilityAsyncEvent(0,
        ArkUI_AccessibilityEventType::
            ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);
    LOGD("Flutter_SendAccessibilityAsyncEvent -> PAGE_CONTENT_UPDATE");

    /* 针对更新后的节点进行事件处理 */
    for (auto& nodeEx: updatedFlutterNodes) {
        FML_DLOG(INFO) << "*#*#* updated node.id=" << nodeEx.id;

        // 当滑动节点产生滑动，并执行滑动处理
        if (HasScrolled(nodeEx)) {
            LOGD("UpdateSemantics -> nodeId = %{public}d has scrolled", nodeEx.id);
            auto OH_ArkUI_CreateAccessibilityElementInfo =
                OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_NODE);
            CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityElementInfo);
            auto* _elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();

            FlutterSetElementInfoProperties(_elementInfo, static_cast<int64_t>(nodeEx.id));
            // flutter滑动组件滑动处理逻辑
            FlutterScrollExecution(nodeEx, _elementInfo);

            // 屏幕朗读状态下双指滑动，获焦节点绿框实时跟随节点滑动
            Flutter_SendAccessibilityAsyncEvent(
                static_cast<int64_t>(accessibilityFocusedNode.id),
                ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_FOCUS_NODE_UPDATE);

            auto OH_ArkUI_DestoryAccessibilityElementInfo =
                OhosAccessibilityDDL::DLLoadDestroyElemFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_NODE);
            CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityElementInfo);
            OH_ArkUI_DestoryAccessibilityElementInfo(_elementInfo);
            _elementInfo = nullptr;
        }

        // 判断是否触发liveRegion活动区，当前节点是否活跃 nodeEx.HasFlag(FLAGS_::kIsLiveRegion) 
        if (nodeEx.HasFlag(FLAGS_::kIsLiveRegion) && HasChangedLabel(nodeEx)) {
            FML_DLOG(INFO) << "liveRegion -> page content update, nodeEx.id=" << nodeEx.id;
            Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeEx.id),
                                                ArkUI_AccessibilityEventType::
                                                    ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);
        }
    }
    // calculate the global tranfomr matrix for each node
    ComputeGlobalTransform();
    // 输出flutter语义树相关重要语义信息debug日志
    GetSemanticsDebugInfo();
    FML_DLOG(INFO) << "=== UpdateSemantics() is finished ===";
}

/**
 * flutter可滑动组件的滑动逻辑处理实现
 */
void OhosAccessibilityBridge::FlutterScrollExecution(
    SemanticsNodeExtent node,
    ArkUI_AccessibilityElementInfo* elementInfoFromList)
{
    if (OHOS_API_VERSION < 13) { return; }
    double nodePosition = node.scrollPosition;
    double nodeScrollExtentMax = node.scrollExtentMax;
    double nodeScrollExtentMin = node.scrollExtentMin;
    double infinity = std::numeric_limits<double>::infinity();

    // 设置flutter可滑动的最大范围值
    if (nodeScrollExtentMax == infinity) {
        nodeScrollExtentMax = SCROLL_EXTENT_FOR_INFINITY;
        if (nodePosition > SCROLL_POSITION_CAP_FOR_INFINITY) {
            nodePosition = SCROLL_POSITION_CAP_FOR_INFINITY;
        }
    }
    if (nodeScrollExtentMin == infinity) {
        nodeScrollExtentMax += SCROLL_EXTENT_FOR_INFINITY;
        if (nodePosition < -SCROLL_POSITION_CAP_FOR_INFINITY) {
            nodePosition = -SCROLL_POSITION_CAP_FOR_INFINITY;
        }
        nodePosition += SCROLL_EXTENT_FOR_INFINITY;
    } else {
        nodeScrollExtentMax -= node.scrollExtentMin;
        nodePosition -= node.scrollExtentMin;
    }

    if (node.HasAction(ACTIONS_::kScrollUp) ||
        node.HasAction(ACTIONS_::kScrollDown)) {
    } else if (node.HasAction(ACTIONS_::kScrollLeft) ||
               node.HasAction(ACTIONS_::kScrollRight)) {
        LOGD("current flutterNode has scroll up/down/left/right");
    }

    // 当可滑动组件存在滑动子节点
    if (node.scrollChildren > 0) {
        // 配置当前滑动组件的子节点总数
        int32_t itemCount = node.scrollChildren;
        auto OH_ArkUI_AccessibilityElementInfoSetItemCount =
            OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_ITEM_COUNT);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetItemCount);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetItemCount(elementInfoFromList, itemCount));

        // 设置当前页面可见的起始滑动index
        int32_t startItemIndex = node.scrollIndex;
        auto OH_ArkUI_AccessibilityElementInfoSetStartItemIndex =
            OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_START_ITEM_IDX);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetStartItemIndex);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetStartItemIndex(elementInfoFromList, startItemIndex));

        // 设置当前获焦节点的当前index
        int32_t currItemIndex = accessibilityFocusedNode.id;
        auto OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex =
            OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_CURR_ITEM_IDX);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetCurrentItemIndex(elementInfoFromList, currItemIndex));

        // 计算当前滑动位置页面的可见子滑动节点数量
        int visibleChildren = 0;
        // handle hidden children at the beginning and end of the list.
        for (const auto& childId : node.childrenInHitTestOrder) {
            auto childNode = GetFlutterSemanticsNode(childId);
            if (!childNode.HasFlag(FLAGS_::kIsHidden)) {
                visibleChildren += 1;
            }
        }
        // 当可见滑动子节点数量超过滑动组件总子节点数量
        if (node.scrollIndex + visibleChildren > node.scrollChildren) {
            FML_DLOG(WARNING)
                << "FlutterScrollExecution -> Scroll index is out of bounds";
        }
        // 当滑动击中子节点数量为0
        if (!node.childrenInHitTestOrder.size()) {
            FML_DLOG(WARNING) << "FlutterScrollExecution -> Had scrollChildren but no "
                                "childrenInHitTestOrder";
        }
        // 设置当前页面可见的末尾滑动index
        int32_t endItemIndex = node.scrollIndex + visibleChildren - 1;
        auto OH_ArkUI_AccessibilityElementInfoSetEndItemIndex =
            OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_END_ITEM_IDX);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEndItemIndex);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEndItemIndex(elementInfoFromList, endItemIndex));
    }
}

/**
 * 特定节点的焦点请求 (当页面更新时自动请求id=0节点获焦)
 */
void OhosAccessibilityBridge::RequestFocusWhenPageUpdate(int32_t requestFocusId)
{
    if (OHOS_API_VERSION < 13) { return; }
    std::lock_guard<std::mutex> lock(XComponentAdapter::GetInstance()->mutex_);
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider(xcomponentId_);
    CHECK_NULL_PTR_RET_VOID(provider_, RequestFocusWhenPageUpdate);
    
    auto OH_ArkUI_CreateAccessibilityEventInfo = 
        OhosAccessibilityDDL::DLLoadCreateEventInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityEventInfo);
    auto* reqFocusEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();

    auto OH_ArkUI_CreateAccessibilityElementInfo = 
        OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityElementInfo);
    auto* elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();

    auto OH_ArkUI_AccessibilityEventSetEventType =
        OhosAccessibilityDDL::DLLoadSetEventFunc(ArkUIAccessibilityConstant::ARKUI_SET_EVENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetEventType);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetEventType(
            reqFocusEventInfo,
            ArkUI_AccessibilityEventType::
                ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_REQUEST_ACCESSIBILITY_FOCUS)
    );

    auto OH_ArkUI_AccessibilityEventSetRequestFocusId =
        OhosAccessibilityDDL::DLLoadSetReqFocusFunc(ArkUIAccessibilityConstant::ARKUI_SET_REQ_FOCUSED_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetRequestFocusId);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetRequestFocusId(reqFocusEventInfo, requestFocusId)
    );

    auto OH_ArkUI_AccessibilityEventSetElementInfo =
        OhosAccessibilityDDL::DLLoadSetEventElemFunc(ArkUIAccessibilityConstant::ARKUI_EVENT_SET_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetElementInfo);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetElementInfo(reqFocusEventInfo, elementInfo)
    );

    auto callback = [](int32_t errorCode) {
        FML_DLOG(WARNING) << "PageStateUpdate callback-> errorCode =" << errorCode;
    };

    auto OH_ArkUI_SendAccessibilityAsyncEvent =
        OhosAccessibilityDDL::DLLoadSendAsyncEventFunc(ArkUIAccessibilityConstant::ARKUI_SEND_A11Y_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_SendAccessibilityAsyncEvent);
    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, reqFocusEventInfo, callback);

    auto OH_ArkUI_DestoryAccessibilityEventInfo =
        OhosAccessibilityDDL::DLLoadDestroyEventFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityEventInfo);
    OH_ArkUI_DestoryAccessibilityEventInfo(reqFocusEventInfo);
    reqFocusEventInfo = nullptr;

    auto OH_ArkUI_DestoryAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadDestroyElemFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityElementInfo);
    OH_ArkUI_DestoryAccessibilityElementInfo(elementInfo);
    elementInfo = nullptr;
}

/**
 * 业务侧通过无障碍通道主动播报自定义文本内容
 */
void OhosAccessibilityBridge::Announce(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("Announce -> message: %{public}s", message.get());
}

/**
 * 业务侧通过无障碍通道主动点击给定id的组件节点
 */
void OhosAccessibilityBridge::OnTap(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeId),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED);
    LOGD("OnTap -> nodeId: %{public}d", nodeId);
}

/**
 * 业务侧通过无障碍通道主动长按给定id的组件节点
 */
void OhosAccessibilityBridge::OnLongPress(int32_t nodeId)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(nodeId),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED);
    LOGD("OnLongPress -> nodeId: %{public}d", nodeId);
}

/**
 * 业务侧通过无障碍通道主动长按给定id的组件节点
 */
void OhosAccessibilityBridge::OnTooltip(std::unique_ptr<char[]>& message)
{
    if (!isAccessibilityEnabled_) { return; }
    Flutter_SendAccessibilityAsyncEvent(static_cast<int64_t>(ROOT_NODE_ID),
                                        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_STATE_UPDATE);
    Flutter_SendAccessibilityAnnounceEvent(
        message, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY);
    LOGD("OnTooltip -> message: %{public}s", message.get());
}

//获取根节点
SemanticsNodeExtent OhosAccessibilityBridge::GetFlutterRootSemanticsNode()
{
    if (!g_flutterSemanticsTree.size()) {
        LOGE("GetFlutterRootSemanticsNode: g_flutterSemanticsTree.size()=0");
        return {};
    }
    if (g_flutterSemanticsTree.find(0) == g_flutterSemanticsTree.end()) {
        LOGE("GetFlutterRootSemanticsNod: g_flutterSemanticsTree has no root id");
        return {};
    }
    return g_flutterSemanticsTree.at(0);
}

/**
 * 根据nodeid获取或创建flutter语义节点
 */
SemanticsNodeExtent OhosAccessibilityBridge::GetFlutterSemanticsNode(
    int32_t id)
{
    if (g_flutterSemanticsTree.count(id) > 0) {
        return g_flutterSemanticsTree.at(id);
    } else {
        LOGE("GetFlutterSemanticsNode g_flutterSemanticsTree = null");
        return {};
    }
}

/**
 * 获取当前elementid的父节点id
 */
int32_t OhosAccessibilityBridge::GetParentId(int64_t elementId)
{
    if (!g_parentChildIdVec.size()) {
        FML_DLOG(WARNING) << "OhosAccessibilityBridge::GetParentId parentChildIdMap.size()=0";
        return ARKUI_ACCESSIBILITY_ROOT_PARENT_ID;
    }
    if (elementId == -1 || elementId == 0) {
        return ARKUI_ACCESSIBILITY_ROOT_PARENT_ID;
    }
    int32_t childElementId = static_cast<int32_t>(elementId);
    for (const auto& item : g_parentChildIdVec) {
        if (item.second == childElementId) {
            return item.first;
        }
    }
    return RET_ERROR_STATE_CODE;
}

/**
 * 设置并获取xcomponet上渲染的组件的屏幕绝对坐标rect
 */
void OhosAccessibilityBridge::SetAbsoluteScreenRect(SemanticsNodeExtent& flutterNode,
                                                    float left,
                                                    float top,
                                                    float right,
                                                    float bottom)
{
    g_screenRectMap[flutterNode.id] = AbsoluteRect{left, top, right, bottom};
    FML_DLOG(INFO) << "SetAbsoluteScreenRect -> id=" << flutterNode.id
                   << ", {" << left << ", " << top << ", " << right << ", "<< bottom << "> }";
}

AbsoluteRect OhosAccessibilityBridge::GetAbsoluteScreenRect(SemanticsNodeExtent& flutterNode)
{
    if (!g_screenRectMap.empty() && g_screenRectMap.count(flutterNode.id) > 0) {
        return g_screenRectMap.at(flutterNode.id);
    } else {
        FML_DLOG(ERROR) << "GetAbsoluteScreenRect -> flutterNodeId="
                        << flutterNode.id << " is not found !";
        return {};
    }
}

/**
 * 获取flutter相对-绝对坐标映射的真实缩放系数
 */
std::pair<float, float> OhosAccessibilityBridge::GetRealScaleFactor()
{
    auto secondNode = GetFlutterSemanticsNode(1);
    SkMatrix transform = secondNode.transform.asM33();
    auto scaleX = transform.get(SkMatrix::kMScaleX);
    auto scaleY = transform.get(SkMatrix::kMScaleY);
    return std::make_pair(scaleX, scaleY);
}

/**
 * calculate the global transform matrix for each node
 */
void OhosAccessibilityBridge::ComputeGlobalTransform()
{
  std::queue<SemanticsNodeExtent> semanticsQue;

  auto root = GetFlutterSemanticsNode(0);
  semanticsQue.push(root);
  g_globalTransformMap[root.id] = root.transform;

  while (!semanticsQue.empty()) {
    uint32_t queSize = semanticsQue.size();
    for (uint32_t i=0; i<queSize; i++) {
      auto currNode = semanticsQue.front();
      semanticsQue.pop();

      for (const auto& childId: currNode.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childId);
        semanticsQue.push(childNode);
        g_globalTransformMap[childId] = g_globalTransformMap[currNode.id] * childNode.transform;
      }
    }
  }
}

SkPoint OhosAccessibilityBridge::ApplyTransform(
    SkPoint& point, const SkM44& transform) {
  SkV4 vector = transform.map(point.x(), point.y(), 0, 1);
  return SkPoint::Make(vector.x / vector.w, vector.y / vector.w);
}

/**
 * convert local(relative) rect to global(absolut) rect
 * @param node flutter semantics node
 */
void OhosAccessibilityBridge::RelativeRectToScreenRect(SemanticsNodeExtent& node)
{
    auto [left, top, right, bottom] = node.rect;
    SkM44 globalTransform = g_globalTransformMap[node.id];

    SkPoint points[4] = {
        SkPoint::Make(left, top),     // top-left point
        SkPoint::Make(right, top),    // top-right point
        SkPoint::Make(right, bottom), // bottom-right point
        SkPoint::Make(left, bottom)   // bottom-left point
    };

    for (auto& point : points) {
        point = ApplyTransform(point, globalTransform);
    }

    SkRect globalRect;
    bool checkResult = globalRect.setBoundsCheck(points, 4);
    if (!checkResult) {
        FML_DLOG(WARNING) << "RelativeRectToScreenRect -> Transformed points can't make a rect ";
    }
    globalRect.setBounds(points, 4);

    SetAbsoluteScreenRect(node, globalRect.left(),  globalRect.top(),
                                globalRect.right(), globalRect.bottom());
}

/**
 * 配置arkui节点的可操作动作类型
 */
void OhosAccessibilityBridge::FlutterSetElementInfoOperationActions(
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    std::string widget_type)
{
    if (OHOS_API_VERSION < 13) { return; }
    auto OH_ArkUI_AccessibilityElementInfoSetOperationActions = 
        OhosAccessibilityDDL::DLLoadSetElemOperActionsFunc(ArkUIAccessibilityConstant::ARKUI_SET_ACTIONS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetOperationActions);
    if (OHOSUtils::Contains(widget_type, EDIT_TEXT_WIDGET_NAME) ||
        OHOSUtils::Contains(widget_type, EDIT_MULTILINE_TEXT_WIDGET_NAME)) {
        // set elementinfo action types
        int32_t actionTypeNum = 10;
        ArkUI_AccessibleAction actions[actionTypeNum];
        int32_t idx = 0; 
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "获取焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "清除焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
        actions[idx++].description = "点击操作";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK;
        actions[idx++].description = "长按操作";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY;
        actions[idx++].description = "文本复制";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE;
        actions[idx++].description = "文本粘贴";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT;
        actions[idx++].description = "文本剪切";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT;
        actions[idx++].description = "文本选择";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT;
        actions[idx++].description = "文本内容设置";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_CURSOR_POSITION;
        actions[idx].description = "光标位置设置";

        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, actionTypeNum, actions)
        );
    } else if (OHOSUtils::Contains(widget_type, SCROLL_WIDGET_NAME)) {
        // if node is a scrollable component
        int32_t actionTypeNum = 5;
        ArkUI_AccessibleAction actions[actionTypeNum];
        int32_t idx = 0; 
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "获取焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "清除焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
        actions[idx++].description = "点击动作";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD;
        actions[idx++].description = "向上滑动";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD;
        actions[idx].description = "向下滑动";

        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, actionTypeNum, actions)
        );
    } else {
        // set common component action types
        int32_t actionTypeNum = 3;
        ArkUI_AccessibleAction actions[actionTypeNum];
        int32_t idx = 0; 
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "获取焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS;
        actions[idx++].description = "清除焦点";
        actions[idx].actionType = ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK;
        actions[idx].description = "点击动作";

        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetOperationActions(elementInfoFromList, actionTypeNum, actions)
        );
    }
}

/**
 * 实现对特定id的flutter节点到arkui的elementinfo节点转化,
 * 根据flutter节点信息配置elementinfo无障碍属性
 */
void OhosAccessibilityBridge::FlutterSetElementInfoProperties(
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    int64_t elementId)
{
    if (OHOS_API_VERSION < 13) { return; }
    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId > 0 ? elementId : 0));
    if (!g_flutterSemanticsTree.count(flutterNode.id)) {
        LOGE("FlutterSetElementInfoProperties: GetFlutterSemanticsNode id=%{public}ld null", elementId);
    }

    // 设置当前节点id
    auto OH_ArkUI_AccessibilityElementInfoSetElementId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_NODE_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetElementId);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetElementId(elementInfoFromList, flutterNode.id)
    );

    // 相对-绝对坐标映射
    ArkUI_AccessibleRect rect;
    if (elementId < 1) { // 若当前节点为root
        int32_t left = flutterNode.rect.fLeft;
        int32_t top = flutterNode.rect.fTop;
        int32_t right = flutterNode.rect.fRight;
        int32_t bottom = flutterNode.rect.fBottom;
        SetAbsoluteScreenRect(flutterNode, left, top, right, bottom);
        rect = {static_cast<int32_t>(left), static_cast<int32_t>(top),
                static_cast<int32_t>(right), static_cast<int32_t>(bottom)};
    } else { // 若当前节点为id >= 1的节点
        RelativeRectToScreenRect(flutterNode);
        auto [left, top, right, bottom] = GetAbsoluteScreenRect(flutterNode);
        rect = {static_cast<int32_t>(left), static_cast<int32_t>(top),
                static_cast<int32_t>(right), static_cast<int32_t>(bottom)};
    }
    auto OH_ArkUI_AccessibilityElementInfoSetScreenRect = 
        OhosAccessibilityDDL::DLLoadSetElemSreenRectFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCREEN_RECT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScreenRect);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetScreenRect(elementInfoFromList, &rect)
    );
    
    // 配置arkui的elementinfo可操作动作属性
    // 设置root节点的action类型
    std::string widgeType = GetNodeComponentType(flutterNode);
    if (elementId < 1) {
        FlutterSetElementInfoOperationActions(elementInfoFromList, OTHER_WIDGET_NAME);
    } else {
        FlutterSetElementInfoOperationActions(elementInfoFromList, widgeType);
    }

    // 设置当前节点的父节点id
    int32_t parentId = GetParentId(elementId);
    auto OH_ArkUI_AccessibilityElementInfoSetParentId =
        OhosAccessibilityDDL::DLLoadSetElemIntFunc(ArkUIAccessibilityConstant::ARKUI_SET_PARENT_ID);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetParentId);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetParentId(elementInfoFromList, parentId)
    );
    FML_DLOG(INFO) << "FlutterSetElementInfoProperties GetParentId = " << parentId;
    
    // 设置可朗读文本
    std::string text = flutterNode.label + flutterNode.value;
    auto OH_ArkUI_AccessibilityElementInfoSetAccessibilityText =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_TEXT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityText);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityText(elementInfoFromList, text.c_str())
    );
    FML_DLOG(INFO) << "FlutterSetElementInfoProperties SetAccessibilityText = " << text;

    // 设置无障碍content文本
    auto OH_ArkUI_AccessibilityElementInfoSetContents =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_CONTENTS);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetContents);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetContents(elementInfoFromList, text.c_str())
    );
    // 设置hint提示文本
    std::string hint = flutterNode.hint;
    auto OH_ArkUI_AccessibilityElementInfoSetHintText =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_HINT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetHintText);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetHintText(elementInfoFromList, hint.c_str())
    );

    // 设置当前节点的全部孩子节点
    int32_t childCount = flutterNode.childrenInTraversalOrder.size();
    if (childCount > 0) {
        auto childrenIdsVec = flutterNode.childrenInTraversalOrder;
        std::sort(childrenIdsVec.begin(), childrenIdsVec.end());
        int64_t childNodeIds[childCount];
        for (int32_t i = 0; i < childCount; i++) {
            childNodeIds[i] = static_cast<int64_t>(childrenIdsVec[i]);
            FML_DLOG(INFO) << "FlutterSetElementInfoProperties -> elementid=" << elementId
                           << " childCount=" << childCount
                           << " childNodeIds=" << childNodeIds[i];
        }
        auto OH_ArkUI_AccessibilityElementInfoSetChildNodeIds =
            OhosAccessibilityDDL::DLLoadSetElemChildFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHILD_IDS);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetChildNodeIds); 
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(elementInfoFromList, childCount, childNodeIds)
        );
    }

    // 判断当前节点组件是否enabled
    if (IsNodeEnabled(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetEnabled =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_ENABLED);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEnabled); 
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEnabled(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetEnabled -> true";
    }
    // 判断当前节点是否可点击
    if (IsNodeClickable(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetClickable =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CLICKABLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetClickable); 
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetClickable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetClickable -> true";
    }
    // 判断当前节点是否可获焦点
    if (IsNodeFocusable(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetFocusable =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_FOCUSABLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetFocusable); 
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetFocusable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetFocusable -> true";
    }
    if (IsNodeFocused(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_FOCUSED);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetAccessibilityFocused -> true";
    }
    // 判断当前节点是否为密码输入框
    if (IsNodePassword(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetIsPassword = 
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_IS_PASSWORD);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetIsPassword);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetIsPassword(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetIsPassword -> true";
    }
    // 判断当前节点是否具备checkable状态 (如：checkbox, radio button)
    if (IsNodeCheckable(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetCheckable = 
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHECKABLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetCheckable);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetCheckable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetCheckable -> true";
    }
    // 判断当前节点(check box/radio button)是否checked/unchecked
    if (IsNodeChecked(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetChecked = 
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_CHECKED);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetChecked);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetChecked(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetChecked -> true";
    }
    // 判断当前节点组件是否可显示
    if (IsNodeVisible(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetVisible =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_VISIBLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetVisible);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetVisible(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                        << " OH_ArkUI_AccessibilityElementInfoSetVisible -> true";
    }
    // 判断当前节点组件是否选中
    if (IsNodeSelected(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetSelected =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_SELECTED);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetSelected);
        OH_ArkUI_AccessibilityElementInfoSetSelected(elementInfoFromList, true);
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                       << " OH_ArkUI_AccessibilityElementInfoSetSelected -> true";
    }
    // 判断当前节点组件是否可滑动
    if (IsNodeScrollable(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetScrollable =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_SCROLLABLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetScrollable);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetScrollable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                        << " OH_ArkUI_AccessibilityElementInfoSetScrollable -> true";
    }
    // 判断当前节点组件是否可编辑（文本输入框）
    if (IsTextField(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetEditable =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_EDITABLE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetEditable);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetEditable(elementInfoFromList, true)
        );
        FML_DLOG(INFO) << "flutterNode.id=" << flutterNode.id
                        << " OH_ArkUI_AccessibilityElementInfoSetEditable -> true";
    }
    // 判断当前节点组件是否支持长按
    if (IsNodeHasLongPress(flutterNode)) {
        auto OH_ArkUI_AccessibilityElementInfoSetLongClickable =
            OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_LONG_PRESS);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetLongClickable);
        ARKUI_ACCESSIBILITY_CALL_CHECK(
            OH_ArkUI_AccessibilityElementInfoSetLongClickable(elementInfoFromList, true)
        );
        FML_DLOG(INFO)
            << "flutterNode.id=" << flutterNode.id
            << " OH_ArkUI_AccessibilityElementInfoSetLongClickable -> true";
    }

    // 获取当前节点的组件类型
    std::string componentTypeName = GetNodeComponentType(flutterNode);
    FML_DLOG(INFO) << "FlutterSetElementInfoProperties componentTypeName = "
                    << componentTypeName;
    // flutter节点对应elementinfo所属的组件类型（如：root， button，text等）
    auto OH_ArkUI_AccessibilityElementInfoSetComponentType =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_COMPONENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetComponentType);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetComponentType(
            elementInfoFromList, elementId < 1 ? "root" : componentTypeName.c_str())
    );
    FML_DLOG(INFO) << "FlutterSetElementInfoProperties SetComponentType: "
                   << componentTypeName;

    /**
     * 无障碍重要性，用于控制某个组件是否可被无障碍辅助服务所识别。支持的值为（默认值：“auto”）：
     * “auto”：根据组件不同会转换为“yes”或者“no”
     * “yes”：当前组件可被无障碍辅助服务所识别
     * “no”：当前组件不可被无障碍辅助服务所识别
     * “no-hide-descendants”：当前组件及其所有子组件不可被无障碍辅助服务所识别
     */
    auto OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel =
        OhosAccessibilityDDL::DLLoadSetElemStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_LEVEL);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityLevel(
            elementInfoFromList, componentTypeName != OTHER_WIDGET_NAME ? "yes" : "no"); 
    );
    // 无障碍组，设置为true时表示该组件及其所有子组件为一整个可以选中的组件，无障碍服务将不再关注其子组件内容。默认值：false
    auto OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup =
        OhosAccessibilityDDL::DLLoadSetElemBoolFunc(ArkUIAccessibilityConstant::ARKUI_SET_A11Y_GROUP);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityElementInfoSetAccessibilityGroup(elementInfoFromList, false);
    );
}

/**
 * 将flutter无障碍语义树的转化为层次遍历顺序存储，
 * 并按该顺序构建arkui语义树，以实现DevEco Testing
 * UIViewer、Hypium自动化测试工具对flutter组件树的可视化
 */
std::vector<int64_t> OhosAccessibilityBridge::GetLevelOrderTraversalTree(int32_t rootId)
{
  std::vector<int64_t> levelOrderTraversalTree;
  std::queue<SemanticsNodeExtent> semanticsQue;

  auto root = GetFlutterSemanticsNode(rootId);
  semanticsQue.push(root);

  while (!semanticsQue.empty()) {
    uint32_t queSize = semanticsQue.size();
    for (uint32_t i=0; i<queSize; i++) {
      auto currNode = semanticsQue.front();

      semanticsQue.pop();
      levelOrderTraversalTree.emplace_back(static_cast<int64_t>(currNode.id));

      std::sort(currNode.childrenInTraversalOrder.begin(), 
                currNode.childrenInTraversalOrder.end());
      for (const auto& childId: currNode.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childId);

        semanticsQue.push(childNode);
      }
    }
  }
  return levelOrderTraversalTree;
}

/**
 * 创建并配置完整arkui无障碍语义树
 */
void OhosAccessibilityBridge::BuildArkUISemanticsTree(
    int64_t elementId,
    ArkUI_AccessibilityElementInfo* elementInfoFromList,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    if (OHOS_API_VERSION < 13) { return; }
    //配置root节点信息
    FlutterSetElementInfoProperties(elementInfoFromList, elementId);
    //获取flutter无障碍语义树的节点总数
    auto levelOrderTreeVec = GetLevelOrderTraversalTree(0);
    int64_t elementInfoCount = levelOrderTreeVec.size();
    //创建并配置节点id >= 1的全部节点
    for (int64_t i = 1; i < elementInfoCount; i++) {
        int64_t levelOrderId = levelOrderTreeVec[i];
        auto newNode = GetFlutterSemanticsNode(levelOrderId);
        if (g_flutterSemanticsTree.count(newNode.id)) {
            LOGE("BuildArkUISemanticsTree: GetFlutterSemanticsNode id=%{public}ld null", levelOrderId);
        }
        //当节点为隐藏状态时，自动规避
        auto OH_ArkUI_AddAndGetAccessibilityElementInfo =
            OhosAccessibilityDDL::DLLoadGetElemFunc(ArkUIAccessibilityConstant::ARKUI_GET_A11Y_NODE);
        CHECK_DLL_NULL_PTR(OH_ArkUI_AddAndGetAccessibilityElementInfo);
        auto* newElementInfo = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
        //配置当前子节点信息
        FlutterSetElementInfoProperties(newElementInfo, levelOrderId);
    }
}

/**
 * Called to obtain element information based on a specified node.
 * NOTE:该arkui接口需要在系统无障碍服务开启时，才能触发调用
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosById(
    int64_t elementId,
    ArkUI_AccessibilitySearchMode mode,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    if (OHOS_API_VERSION < 13) { return ARKUI_FAILED_CODE; }
    FML_DLOG(INFO)
        << "#### FindAccessibilityNodeInfosById input-params ####: elementId = "
        << elementId << " mode=" << mode;
    CHECK_NULL_PTR_WITH_RET(elementList, FindAccessibilityNodeInfosById);

    AccessibiltiyChangesWithXComponentId();

    if (g_flutterSemanticsTree.size() == 0) {
        FML_DLOG(INFO)
            << "FindAccessibilityNodeInfosById g_flutterSemanticsTree is null";
        return ARKUI_ACCESSIBILITY_NATIVE_RESULT_FAILED;
    }
    
    // 获取当前对应id的flutter节点，若为空则返回错误编码
    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId));
    if (!g_flutterSemanticsTree.count(flutterNode.id)) {
        LOGE("FindAccessibilityNodeInfosById: GetFlutterSemanticsNode id=%{public}ld is null", elementId);
    }

    // 开启无障碍导航功能
    if(elementId == -1 || elementId == 0) {
        accessibilityFeatures_->SetAccessibleNavigation(true, native_shell_holder_id_);
    }

    // 从elementinfolist中获取elementinfo
    auto OH_ArkUI_AddAndGetAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadGetElemFunc(ArkUIAccessibilityConstant::ARKUI_GET_A11Y_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AddAndGetAccessibilityElementInfo);
    auto* elementInfoFromList = OH_ArkUI_AddAndGetAccessibilityElementInfo(elementList);
    CHECK_NULL_PTR_WITH_RET(elementInfoFromList, OH_ArkUI_AddAndGetAccessibilityElementInfo);

    if (mode == ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT) {
        /** Search for current nodes. (mode = 0) */
        BuildArkUISemanticsTree(elementId, elementInfoFromList, elementList);
    } else if (mode ==ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS) {
        /** Search for parent nodes. (mode = 1) */
        if (IsNodeVisible(flutterNode)) {
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
        }
    } else if (mode ==ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS) {
        /** Search for sibling nodes. (mode = 2) */
        if (IsNodeVisible(flutterNode)) {
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
        }
    } else if (mode ==ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CHILDREN) {
        /** Search for child nodes at the next level. (mode = 4) */
        if (IsNodeVisible(flutterNode)) {
            FlutterSetElementInfoProperties(elementInfoFromList, elementId);
        }
    } else if (mode ==ArkUI_AccessibilitySearchMode::ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN) {
        /** Search for all child nodes. (mode = 8) */
        BuildArkUISemanticsTree(elementId, elementInfoFromList, elementList);
    } else {
        FlutterSetElementInfoProperties(elementInfoFromList, elementId);
    }
    FML_DLOG(INFO) << "--- FindAccessibilityNodeInfosById is end ---";

    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

/**
 * 解析flutter语义动作，并通过NativAccessibilityChannel分发
 */
void OhosAccessibilityBridge::DispatchSemanticsAction(
    int32_t id,
    flutter::SemanticsAction action,
    fml::MallocMapping args)
{
    nativeAccessibilityChannel_->DispatchSemanticsAction(native_shell_holder_id_,
                                                         id,
                                                         action,
                                                         std::move(args));
}

/**
 * flutter按钮节点双击跳转新页面时，发送页面更新事件
 */
void OhosAccessibilityBridge::DoubleClickRouteToNewPage(SemanticsNodeExtent node)
{
    if (node.HasFlag(FLAGS_::kIsButton)) {
        RequestFocusWhenPageUpdate(0);
    }
}

/**
 * perform click action in accessibility status
 */
void OhosAccessibilityBridge::PerformClickAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    /** Click event, sent after the UI component responds. 1 */
    auto clickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_CLICKED;
    Flutter_SendAccessibilityAsyncEvent(elementId, clickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: click(" << action
                   << ")" << " event: click(" << clickEventType << ")";
    auto flutterTapAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterTapAction, {});
    // double click at button-like node for pushing page update
    DoubleClickRouteToNewPage(flutterNode);
}

/**
 * perform long-press action in accessibility status
 */
void OhosAccessibilityBridge::PerformLongClickAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    /** Long click event, sent after the UI component responds. 2 */
    auto longClickEventType = ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_LONG_CLICKED;
    Flutter_SendAccessibilityAsyncEvent(elementId, longClickEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: longclick("
                   << action << ")" << " event: longclick("
                   << longClickEventType << ")";
    auto flutterLongPressAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterLongPressAction, {});
}

/**
 * perform focus acquisition in accessibility status
 */
void OhosAccessibilityBridge::PerformGainFocusnAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    // 感知获焦flutter节点
    accessibilityFocusedNode = flutterNode;
    // 解析arkui的获焦 -> flutter对应节点的获焦
    auto flutterGainFocusAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId),
                            flutterGainFocusAction, {});
    // Accessibility focus event, sent after the UI component responds. 32768
    auto focusEventType = ArkUI_AccessibilityEventType::
        ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUSED;
    Flutter_SendAccessibilityAsyncEvent(elementId, focusEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: focus(" << action
                   << ")" << " event: focus(" << focusEventType << ")";

    if (flutterNode.HasAction(ACTIONS_::kIncrease) ||
        flutterNode.HasAction(ACTIONS_::kDecrease)) {
        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::
                ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
    }
}

/**
 * perform focus clearance in accessibility status
 */
void OhosAccessibilityBridge::PerformClearFocusAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    // 解析arkui的失焦 -> flutter对应节点的失焦
    auto flutterLoseFocusAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterLoseFocusAction, {});
    /** Accessibility focus cleared event, sent after the UI component
     * responds. 65536 */
    auto clearFocusEventType = ArkUI_AccessibilityEventType::
        ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED;
    Flutter_SendAccessibilityAsyncEvent(elementId, clearFocusEventType);
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: clearfocus("
                   << action << ")" << " event: clearfocus("
                   << clearFocusEventType << ")";
}

/**
 * perform scroll forward in accessibility status
 */
void OhosAccessibilityBridge::PerformScrollUpAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    // flutter scroll forward with different situations
    if (flutterNode.HasAction(ACTIONS_::kScrollUp)) {
        auto flutterScrollUpAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterScrollUpAction, {});
    } else if (flutterNode.HasAction(ACTIONS_::kScrollLeft)) {
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kScrollLeft, {});
    } else if (flutterNode.HasAction(ACTIONS_::kIncrease)) {
        flutterNode.value = flutterNode.increasedValue;
        flutterNode.valueAttributes = flutterNode.increasedValueAttributes;
        
        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kIncrease, {});
    }
    std::string currComponetType = GetNodeComponentType(flutterNode);
    if (OHOSUtils::Contains(currComponetType, SCROLL_WIDGET_NAME)) {
        /** Scrolled event, sent when a scrollable component experiences a scroll event. 4096 */
        ArkUI_AccessibilityEventType scrollEventType1 =
            ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SCROLLED;
        Flutter_SendAccessibilityAsyncEvent(elementId, scrollEventType1);
        FML_DLOG(INFO)
            << "ExecuteAccessibilityAction -> action: scroll forward(" << action
            << ")" << " event: scroll forward(" << scrollEventType1 << ")";
    }
}

/**
 * perform scroll backward in accessibility status
 */
void OhosAccessibilityBridge::PerformScrollDownAction(
    int64_t elementId, 
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    // flutter scroll down with different situations
    if (flutterNode.HasAction(ACTIONS_::kScrollDown)) {
        auto flutterScrollDownAction = ArkuiActionsToFlutterActions(action);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), flutterScrollDownAction, {});
    } else if (flutterNode.HasAction(ACTIONS_::kScrollRight)) {
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kScrollRight, {});
    } else if (flutterNode.HasAction(ACTIONS_::kDecrease)) {
        flutterNode.value = flutterNode.decreasedValue;
        flutterNode.valueAttributes = flutterNode.decreasedValueAttributes;

        Flutter_SendAccessibilityAsyncEvent(
            elementId, ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SELECTED);
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kDecrease, {});
    }
    std::string currComponetType = GetNodeComponentType(flutterNode);
    if (OHOSUtils::Contains(currComponetType, SCROLL_WIDGET_NAME)) {
      /** Scrolled event, sent when a scrollable component experiences a
       * scroll event. 4096 */
      ArkUI_AccessibilityEventType scrollEventType1 =
          ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_SCROLLED;
      Flutter_SendAccessibilityAsyncEvent(elementId, scrollEventType1);
      FML_DLOG(INFO)
          << "ExecuteAccessibilityAction -> action: scroll forward(" << action
          << ")" << " event: scroll forward(" << scrollEventType1 << ")";
    }
}
/**
 * perform invalid action in accessibility status
 */
void OhosAccessibilityBridge::PerformClipboardAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action)
{
    if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: copy(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kCopy, {});
    } else if (action == ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE) {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: paste(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kPaste, {});
    } else {
        FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: cut(" << action << ")";
        DispatchSemanticsAction(static_cast<int32_t>(elementId), ACTIONS_::kCut, {});
    }
}
/**
 * perform invalid action in accessibility status
 */
void OhosAccessibilityBridge::PerformInvalidAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    SemanticsNodeExtent flutterNode)
{
    /** Invalid event. 0 */
    ArkUI_AccessibilityEventType invalidEventType =
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_INVALID;
    Flutter_SendAccessibilityAsyncEvent(elementId, invalidEventType);
    FML_DLOG(ERROR) << "ExecuteAccessibilityAction -> action: invalid("
                    << action << ")" << " event: innvalid("
                    << invalidEventType << ")";
}
/**
 * 设置输入框文本
 */
void OhosAccessibilityBridge::PerformSetText(
    SemanticsNodeExtent flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    if (OHOS_API_VERSION < 13) { return; }
    auto OH_ArkUI_FindAccessibilityActionArgumentByKey =
        OhosAccessibilityDDL::DLLoadGetFindActionArgs(ArkUIAccessibilityConstant::ARKUI_FIND_ACTION_ARG_BY_KEY);
    CHECK_DLL_NULL_PTR(OH_ArkUI_FindAccessibilityActionArgumentByKey);
    
    char* newText;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(actionArguments, ARKUI_ACTION_ARG_SET_TEXT, &newText));
    CHECK_NULL_PTR(newText, PerformSetText);

    auto flutterSetTextAction = ArkuiActionsToFlutterActions(action);
    DispatchSemanticsAction(flutterNode.id,
                            flutterSetTextAction,
                            fml::MallocMapping::Copy(newText, strlen(newText)));
    flutterNode.value = newText;
    flutterNode.valueAttributes = {};
    LOGI("ExecuteAccessibilityAction -> action: set text(%{public}d), newText=%{public}s", action, newText);
}

/**
 * perform select text (from base to extent) in accessibility status
 */
void OhosAccessibilityBridge::PerformSelectText(
    SemanticsNodeExtent flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: select text(" << action << ")";
    auto OH_ArkUI_FindAccessibilityActionArgumentByKey =
        OhosAccessibilityDDL::DLLoadGetFindActionArgs(ArkUIAccessibilityConstant::ARKUI_FIND_ACTION_ARG_BY_KEY);
    CHECK_DLL_NULL_PTR(OH_ArkUI_FindAccessibilityActionArgumentByKey);

    auto flutterSelectTextAction = ArkuiActionsToFlutterActions(action);

    char* textSelectBase;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(
            actionArguments, ARKUI_ACTION_ARG_SELECT_TEXT_START, &textSelectBase)
    );
    CHECK_NULL_PTR(textSelectBase, PerformSelectText);

    char* textSelectExtent;
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_FindAccessibilityActionArgumentByKey(
            actionArguments, ARKUI_ACTION_ARG_SELECT_TEXT_END, &textSelectExtent)
    );
    CHECK_NULL_PTR(textSelectExtent, PerformSelectText);

    std::map<std::string, int32_t> selectionMap;
    bool hasSelected = actionArguments != nullptr &&
                       textSelectBase != nullptr &&
                       textSelectExtent != nullptr;
    if (hasSelected) {
        int32_t base;
        int32_t extent;
        OHOSUtils::CharArrayToInt32(textSelectBase, base);
        OHOSUtils::CharArrayToInt32(textSelectBase, extent);
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_START, base});
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_END, extent});
    } else {
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_START, flutterNode.textSelectionBase});
        selectionMap.insert({ARKUI_ACTION_ARG_SELECT_TEXT_END, flutterNode.textSelectionExtent});
    }
    // serialize map<string, int32_t> to byte vector
    std::vector<uint8_t> encodedData = OHOSUtils::SerializeStringIntMap(selectionMap);
    DispatchSemanticsAction(flutterNode.id,
                            flutterSelectTextAction,
                            fml::MallocMapping::Copy(encodedData.data(), encodedData.size() * sizeof(uint8_t)));
}

/**
 * perform cursor position setting in accessibility status
 */
void OhosAccessibilityBridge::PerformSetCursorPosition(
    SemanticsNodeExtent flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: set cursor position (" << action << ")";
    return;
}

/**
 * perform custom action in accessibility status
 */
void OhosAccessibilityBridge::PerformCustomAction(
    SemanticsNodeExtent flutterNode,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction -> action: custom action (" << action << ")";
    DispatchSemanticsAction(flutterNode.id, ACTIONS_::kCustomAction, {});
    return;
}

/**
 * perform show on screen action in accessibility status
 */
void OhosAccessibilityBridge::PerformShowOnScreenAction(SemanticsNodeExtent flutterNode)
{
    if (!IsNodeShowOnScreen(flutterNode)) {
        DispatchSemanticsAction(flutterNode.id, ACTIONS_::kShowOnScreen, {});
    }
}

/**
 * 执行语义动作解析，当FindAccessibilityNodeInfosById找到相应的elementinfo时才会触发该回调函数
 */
int32_t OhosAccessibilityBridge::ExecuteAccessibilityAction(
    int64_t elementId,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* actionArguments,
    int32_t requestId)
{
    FML_DLOG(INFO) << "ExecuteAccessibilityAction input-params-> elementId="
                   << elementId << " action=" << action;
    CHECK_NULL_PTR_WITH_RET(actionArguments, ExecuteAccessibilityAction);

    AccessibiltiyChangesWithXComponentId();

    // 获取当前elementid对应的flutter语义节点
    auto flutterNode = GetFlutterSemanticsNode(static_cast<int32_t>(elementId));
    if (!g_flutterSemanticsTree.count(flutterNode.id)) {
        LOGE("ExecuteAccessibilityAction: GetFlutterSemanticsNode id=%{public}ld is null", elementId);
    }

    /**
     * 将被遮挡的flutter节点显示在屏幕上
     * @NOTE: arkui无障碍缺少showOnScreen动作
     */
    PerformShowOnScreenAction(flutterNode);

    // 根据当前elementid和无障碍动作类型，发送无障碍事件
    switch (action) {
        /** Response to a click. 16 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
            PerformClickAction(elementId, action, flutterNode);
            break;
        
        /** Response to a long click. 32 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
            PerformLongClickAction(elementId, action, flutterNode);
            break;
        
        /** Accessibility focus acquisition. 64 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
            PerformGainFocusnAction(elementId, action, flutterNode);
            break;
        
        /** Accessibility focus clearance. 128 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
            PerformClearFocusAction(elementId, action, flutterNode);
            break;
        
        /** Forward scroll action. 256 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
            PerformScrollUpAction(elementId, action, flutterNode);
            break;
        
        /** Backward scroll action. 512 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
            PerformScrollDownAction(elementId, action, flutterNode);
            break;
        
        /** Copy action for text content. 1024 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
            PerformClipboardAction(elementId, action);
            break;
        
        /** Paste action for text content. 2048 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_PASTE:
            PerformClipboardAction(elementId, action);
            break;
        
        /** Cut action for text content. 4096 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
            PerformClipboardAction(elementId, action);
            break;
        
        /** Text selection action, requiring the setting of <b>selectTextBegin</b>,
         * <b>TextEnd</b>, and <b>TextInForward</b> parameters to select a text
         * segment in the text box. 8192 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT:
            PerformSelectText(flutterNode, action, actionArguments);
            break;
        
        /** Text content setting action. 16384 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT:
            PerformSetText(flutterNode, action, actionArguments);
            break;
        
        /** Cursor position setting action. 1048576 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_CURSOR_POSITION:
            PerformSetCursorPosition(flutterNode, action, actionArguments);
            break;
        
        /** Invalid action. 0 */
        case ArkUI_Accessibility_ActionType::ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_INVALID:
            PerformInvalidAction(elementId, action, flutterNode);
            break;
        
        default:
            /** custom semantics action */
            PerformCustomAction(flutterNode, action, actionArguments);
    }
    FML_DLOG(INFO) << "--- ExecuteAccessibilityAction is end ---";
    return ARKUI_ACCESSIBILITY_NATIVE_RESULT_SUCCESSFUL;
}

/**
 * Called to obtain element information based on a specified node and text
 * content.
 */
int32_t OhosAccessibilityBridge::FindAccessibilityNodeInfosByText(
    int64_t elementId,
    const char* text,
    int32_t requestId,
    ArkUI_AccessibilityElementInfoList* elementList)
{
    FML_DLOG(INFO) << "=== FindAccessibilityNodeInfosByText() ===";
    return 0;
}
int32_t OhosAccessibilityBridge::FindFocusedAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusType focusType,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementinfo)
{
    FML_DLOG(INFO) << "=== FindFocusedAccessibilityNode() ===";
    return 0;
}
int32_t OhosAccessibilityBridge::FindNextFocusAccessibilityNode(
    int64_t elementId,
    ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t requestId,
    ArkUI_AccessibilityElementInfo* elementList) {
    FML_DLOG(INFO) << "=== FindNextFocusAccessibilityNode() ===";
    return 0;
}

int32_t OhosAccessibilityBridge::ClearFocusedFocusAccessibilityNode()
{
    FML_DLOG(INFO) << "=== ClearFocusedFocusAccessibilityNode() ===";
    return 0;
}
int32_t OhosAccessibilityBridge::GetAccessibilityNodeCursorPosition(
    int64_t elementId,
    int32_t requestId,
    int32_t* index)
{
    FML_DLOG(INFO) << "=== GetAccessibilityNodeCursorPosition() ===";
    return 0;
}

/**
 * 将arkui的action类型转化为flutter的action类型
 */
flutter::SemanticsAction OhosAccessibilityBridge::ArkuiActionsToFlutterActions(
    ArkUI_Accessibility_ActionType arkui_action)
{
    switch (arkui_action) {
        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK:
            return ACTIONS_::kTap;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK:
            return ACTIONS_::kLongPress;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_FORWARD:
            return ACTIONS_::kScrollUp;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SCROLL_BACKWARD:
            return ACTIONS_::kScrollDown;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_COPY:
            return ACTIONS_::kCopy;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CUT:
            return ACTIONS_::kCut;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS:
            return ACTIONS_::kDidGainAccessibilityFocus;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS:
            return ACTIONS_::kDidLoseAccessibilityFocus;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SELECT_TEXT:
            return ACTIONS_::kSetSelection;

        case ArkUI_Accessibility_ActionType::
            ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_SET_TEXT:
            return ACTIONS_::kSetText;

        default:
            // might not match to the valid action in arkui
            return ACTIONS_::kCustomAction;
    }
}

/**
 * flutter发送无障碍自定义主动播报事件
 */
void OhosAccessibilityBridge::Flutter_SendAccessibilityAnnounceEvent(
    std::unique_ptr<char[]>& message,
    ArkUI_AccessibilityEventType eventType)
{
    if (OHOS_API_VERSION < 13) { return; }
    AccessibiltiyChangesWithXComponentId();
    std::lock_guard<std::mutex> lock(XComponentAdapter::GetInstance()->mutex_);
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider(xcomponentId_);
    CHECK_NULL_PTR_RET_VOID(provider_, Flutter_SendAccessibilityAnnounceEvent);

    // 创建并设置屏幕朗读事件
    auto OH_ArkUI_CreateAccessibilityEventInfo =
        OhosAccessibilityDDL::DLLoadCreateEventInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityEventInfo);
    auto* announceEventInfo = OH_ArkUI_CreateAccessibilityEventInfo();

    auto OH_ArkUI_AccessibilityEventSetEventType =
        OhosAccessibilityDDL::DLLoadSetEventFunc(ArkUIAccessibilityConstant::ARKUI_SET_EVENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetEventType);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetEventType(
            announceEventInfo,
            ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ANNOUNCE_FOR_ACCESSIBILITY));

    auto OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility =
        OhosAccessibilityDDL::DLLoadSetEventStringFunc(ArkUIAccessibilityConstant::ARKUI_SET_ANNOUNCED_TEXT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility);
    ARKUI_ACCESSIBILITY_CALL_CHECK(
        OH_ArkUI_AccessibilityEventSetTextAnnouncedForAccessibility(
            announceEventInfo, message.get()));
    FML_DLOG(INFO) << ("announce -> message: ") << (message.get());

    auto callback = [](int32_t errorCode) {
        FML_DLOG(WARNING) << "announce callback-> errorCode =" << errorCode;
    };

    auto OH_ArkUI_SendAccessibilityAsyncEvent =
        OhosAccessibilityDDL::DLLoadSendAsyncEventFunc(ArkUIAccessibilityConstant::ARKUI_SEND_A11Y_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_SendAccessibilityAsyncEvent);
    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, announceEventInfo, callback);

    auto OH_ArkUI_DestoryAccessibilityEventInfo = 
        OhosAccessibilityDDL::DLLoadDestroyEventFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityEventInfo);
    OH_ArkUI_DestoryAccessibilityEventInfo(announceEventInfo);
    announceEventInfo = nullptr;
}

/**
 * 自定义无障碍异步事件发送
 */
void OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent(
    int64_t elementId,
    ArkUI_AccessibilityEventType eventType)
{
    if (OHOS_API_VERSION < 13) { return; }
    AccessibiltiyChangesWithXComponentId();
    std::lock_guard<std::mutex> lock(XComponentAdapter::GetInstance()->mutex_);
    auto provider_ = XComponentAdapter::GetInstance()->GetAccessibilityProvider(xcomponentId_);
    CHECK_NULL_PTR_RET_VOID(provider_, Flutter_SendAccessibilityAsyncEvent);

    // 创建eventInfo对象
    auto OH_ArkUI_CreateAccessibilityEventInfo =
        OhosAccessibilityDDL::DLLoadCreateEventInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityEventInfo);
    auto* eventInfo = OH_ArkUI_CreateAccessibilityEventInfo();
    CHECK_NULL_PTR(eventInfo, Flutter_SendAccessibilityAsyncEvent);

    // 创建的elementinfo并根据对应id的flutternode进行属性初始化
    auto OH_ArkUI_CreateAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadCreateElemInfoFunc(ArkUIAccessibilityConstant::ARKUI_CREATE_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_CreateAccessibilityElementInfo);
    ArkUI_AccessibilityElementInfo* _elementInfo = OH_ArkUI_CreateAccessibilityElementInfo();
    FlutterSetElementInfoProperties(_elementInfo, elementId);

    // 将eventinfo事件和当前elementinfo进行绑定
    auto OH_ArkUI_AccessibilityEventSetElementInfo =
        OhosAccessibilityDDL::DLLoadSetEventElemFunc(ArkUIAccessibilityConstant::ARKUI_EVENT_SET_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetElementInfo);
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetElementInfo(eventInfo, _elementInfo));

    // 设置发送事件，如配置获焦、失焦、点击、滑动事件
    auto OH_ArkUI_AccessibilityEventSetEventType =
        OhosAccessibilityDDL::DLLoadSetEventFunc(ArkUIAccessibilityConstant::ARKUI_SET_EVENT_TYPE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_AccessibilityEventSetEventType);
    ARKUI_ACCESSIBILITY_CALL_CHECK(OH_ArkUI_AccessibilityEventSetEventType(eventInfo, eventType));

    // 调用接口发送到ohos侧
    auto callback = [](int32_t errorCode) {
        FML_DLOG(INFO)
            << "Flutter_SendAccessibilityAsyncEvent callback-> errorCode ="
            << errorCode;
    };

    // 发送event到OH侧
    auto OH_ArkUI_SendAccessibilityAsyncEvent =
        OhosAccessibilityDDL::DLLoadSendAsyncEventFunc(ArkUIAccessibilityConstant::ARKUI_SEND_A11Y_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_SendAccessibilityAsyncEvent);
    OH_ArkUI_SendAccessibilityAsyncEvent(provider_, eventInfo, callback);

    // 销毁新创建的elementinfo, eventinfo
    auto OH_ArkUI_DestoryAccessibilityElementInfo =
        OhosAccessibilityDDL::DLLoadDestroyElemFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_NODE);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityElementInfo);
    OH_ArkUI_DestoryAccessibilityElementInfo(_elementInfo);
    _elementInfo = nullptr;

    auto OH_ArkUI_DestoryAccessibilityEventInfo =
        OhosAccessibilityDDL::DLLoadDestroyEventFunc(ArkUIAccessibilityConstant::ARKUI_DESTORY_EVENT);
    CHECK_DLL_NULL_PTR(OH_ArkUI_DestoryAccessibilityEventInfo);
    OH_ArkUI_DestoryAccessibilityEventInfo(eventInfo);
    eventInfo = nullptr;
    
    FML_DLOG(INFO) << "OhosAccessibilityBridge::Flutter_SendAccessibilityAsyncEvent is end";
    return;
}

/**
 * 获取当前flutter节点的组件类型，并映射为arkui组件
 */
std::string OhosAccessibilityBridge::GetNodeComponentType(
    const SemanticsNodeExtent& node)
{
    if (node.HasFlag(FLAGS_::kIsButton)) {
        return BUTTON_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsTextField)) {
        return EDIT_TEXT_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsMultiline)) {
        return EDIT_MULTILINE_TEXT_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsLink)) {
        return LINK_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsSlider) || node.HasAction(ACTIONS_::kIncrease) ||
        node.HasAction(ACTIONS_::kDecrease)) {
        return SLIDER_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsHeader)) {
        return HEADER_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kIsImage)) {
        return IMAGE_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kHasCheckedState)) {
        if (node.HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup)) {
            // arkui没有RadioButton，这里透传为RadioButton
            return RADIO_BUTTON_WIDGET_NAME;
        } else {
            return CHECK_BOX_WIDGET_NAME;
        }
    }
    if (node.HasFlag(FLAGS_::kHasToggledState)) {
        return SWITCH_WIDGET_NAME;
    }
    if (node.HasAction(ACTIONS_::kIncrease) || 
        node.HasAction(ACTIONS_::kDecrease)) {
        return SEEKBAR_WIDGET_NAME;
    }
    if (node.HasFlag(FLAGS_::kHasImplicitScrolling)) {
        if (node.HasAction(ACTIONS_::kScrollLeft) ||
            node.HasAction(ACTIONS_::kScrollRight)) {
            return SCROLL_WIDGET_NAME;
        } else {
            return SCROLL_WIDGET_NAME;
        }
    }
    if ((!node.label.empty() || !node.tooltip.empty() || !node.hint.empty())) {
        return TEXT_WIDGET_NAME;
    }
    return OTHER_WIDGET_NAME;
}

/**
 * 判断当前节点是否为textfield文本框
 */
bool OhosAccessibilityBridge::IsTextField(SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsTextField);
}
/**
 * 判断当前节点是否为滑动条slider类型
 */
bool OhosAccessibilityBridge::IsSlider(SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsSlider);
}
/**
 * 判断当前flutter节点组件是否可点击
 */
bool OhosAccessibilityBridge::IsNodeClickable(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kTap);
}
/**
 * 判断当前flutter节点组件是否可显示
 */
bool OhosAccessibilityBridge::IsNodeVisible(
    SemanticsNodeExtent flutterNode)
{
    return !flutterNode.HasFlag(FLAGS_::kIsHidden);
}
/**
 * 判断当前flutter节点组件是否具备checkable属性
 */
bool OhosAccessibilityBridge::IsNodeCheckable(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kHasCheckedState) ||
           flutterNode.HasFlag(FLAGS_::kHasToggledState);
}
/**
 * 判断当前flutter节点组件是否checked/unchecked（checkbox、radio button）
 */
bool OhosAccessibilityBridge::IsNodeChecked(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsChecked) ||
           flutterNode.HasFlag(FLAGS_::kIsToggled);
}
/**
 * 判断当前flutter节点组件是否选中
 */
bool OhosAccessibilityBridge::IsNodeSelected(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsSelected);
}
/**
 * 判断当前flutter节点组件是否为密码输入框
 */
bool OhosAccessibilityBridge::IsNodePassword(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsTextField) &&
           flutterNode.HasFlag(FLAGS_::kIsObscured);
}
/**
 * 判断当前flutter节点组件是否支持长按功能
 */
bool OhosAccessibilityBridge::IsNodeHasLongPress(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kLongPress);
}
/**
 * 判断当前flutter节点是否enabled
 */
bool OhosAccessibilityBridge::IsNodeEnabled(
    SemanticsNodeExtent flutterNode)
{
    return !flutterNode.HasFlag(FLAGS_::kHasEnabledState) ||
           flutterNode.HasFlag(FLAGS_::kIsEnabled);
}
/**
 * 判断当前flutter节点是否在当前屏幕上显示
 */
bool OhosAccessibilityBridge::IsNodeShowOnScreen(SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kShowOnScreen);
}
/**
 * 判断当前节点是否已经滑动
 */
bool OhosAccessibilityBridge::HasScrolled(
    const SemanticsNodeExtent& flutterNode)
{
    return flutterNode.scrollPosition != std::nan("") &&
           flutterNode.previousScrollPosition != std::nan("") &&
           flutterNode.previousScrollPosition != flutterNode.scrollPosition;
}
/**
 * 判断当前节点是否改变标签文本
 */
bool OhosAccessibilityBridge::HasChangedLabel(const SemanticsNodeExtent& flutterNode)
{
    if (flutterNode.label.empty() && flutterNode.previousLabel.empty()) {
        return false;
    }
    return flutterNode.label.empty() ||
           flutterNode.previousLabel.empty() ||
           flutterNode.label != flutterNode.previousLabel;
}
/**
 * 判断当前语义节点是否可获焦
 */
bool OhosAccessibilityBridge::IsNodeFocusable(
    const SemanticsNodeExtent& node)
{
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
    if ((node.flags & FOCUSABLE_FLAGS) != 0) {
        return true;
    }
    if ((node.actions & ~FOCUSABLE_FLAGS) != 0) {
        return true;
    }
    // Consider text nodes focusable.
    return !node.label.empty() || !node.value.empty() || !node.hint.empty();
}
/**
 * 判断当前flutter节点是否获焦
 */
bool OhosAccessibilityBridge::IsNodeFocused(const SemanticsNodeExtent& flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kIsFocused);
}
/**
 * 判断是否可滑动
 */
bool OhosAccessibilityBridge::IsNodeScrollable(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasAction(ACTIONS_::kScrollLeft) ||
           flutterNode.HasAction(ACTIONS_::kScrollRight) ||
           flutterNode.HasAction(ACTIONS_::kScrollUp) ||
           flutterNode.HasAction(ACTIONS_::kScrollDown);
}
/**
 * 判断当前节点组件是否是滑动组件，如: listview, gridview等
 */
bool OhosAccessibilityBridge::IsScrollableWidget(
    SemanticsNodeExtent flutterNode)
{
    return flutterNode.HasFlag(FLAGS_::kHasImplicitScrolling);
}

void OhosAccessibilityBridge::AddRouteNodes(
    std::vector<SemanticsNodeExtent> edges,
    SemanticsNodeExtent node)
{
    if (node.HasFlag(FLAGS_::kScopesRoute)) {
        edges.emplace_back(node);
    }
    for (auto& childNodeId : node.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childNodeId);
        AddRouteNodes(edges, childNode);
    }
}

std::string OhosAccessibilityBridge::GetRouteName(SemanticsNodeExtent node)
{
    if (node.HasFlag(FLAGS_::kNamesRoute) && !node.label.empty()) {
        return node.label;
    }
    for (auto& childNodeId : node.childrenInTraversalOrder) {
        auto childNode = GetFlutterSemanticsNode(childNodeId);
        std::string newName = GetRouteName(childNode);
        if (!newName.empty()) {
            return newName;
        }
    }
    return "";
}

void OhosAccessibilityBridge::OnWindowNameChange(SemanticsNodeExtent route)
{
    std::string routeName = GetRouteName(route);
    if (routeName.empty()) {
        routeName = " ";
    }
    Flutter_SendAccessibilityAsyncEvent(
        static_cast<int64_t>(route.id),
        ArkUI_AccessibilityEventType::
            ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_PAGE_CONTENT_UPDATE);
}

void OhosAccessibilityBridge::RemoveSemanticsNode(
    SemanticsNodeExtent nodeToBeRemoved)
{
    if (!g_flutterSemanticsTree.size()) {
        FML_DLOG(ERROR) << "OhosAccessibilityBridge::removeSemanticsNode -> "
                          "g_flutterSemanticsTree.szie()=0";
        return;
    }
    if (g_flutterSemanticsTree.find(nodeToBeRemoved.id) ==
        g_flutterSemanticsTree.end()) {
        FML_DLOG(INFO) << "Attempted to remove a node that is not in the tree.";
    }
    int32_t nodeToBeRemovedParentId = GetParentId(nodeToBeRemoved.id);
    for (auto it = g_parentChildIdVec.begin(); it != g_parentChildIdVec.end(); it++) {
        if (it->first == nodeToBeRemovedParentId &&
            it->second == nodeToBeRemoved.id) {
            g_parentChildIdVec.erase(it);
        }
    }
}

/**
 * when the system accessibility service is shut down,
 * clear all the flutter semantics-relevant caches like maps, vectors
 */
void OhosAccessibilityBridge::ClearFlutterSemanticsCaches()
{
    g_flutterSemanticsTree.clear();
    g_parentChildIdVec.clear();
    g_screenRectMap.clear();
    Flutter_SendAccessibilityAsyncEvent(
        accessibilityFocusedNode.id,
        ArkUI_AccessibilityEventType::ARKUI_ACCESSIBILITY_NATIVE_EVENT_TYPE_ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusedNode = {};
}

/**
 * 更新扩展版flutter语义节点
 */
SemanticsNodeExtent OhosAccessibilityBridge::UpdatetSemanticsNodeExtent(
    flutter::SemanticsNode node)
{
    SemanticsNodeExtent nodeEx = SemanticsNodeExtent();
    // 获取更新前的flutter节点信息
    if (g_flutterSemanticsTree.size() > 0) {
        auto prevNode = GetFlutterSemanticsNode(node.id);
        nodeEx.hadPreviousConfig = true;
        nodeEx.previousFlags = prevNode.flags;
        nodeEx.previousActions = prevNode.actions;
        nodeEx.previousTextSelectionBase = prevNode.textSelectionBase;
        nodeEx.previousTextSelectionExtent = prevNode.textSelectionExtent;
        nodeEx.previousScrollPosition = prevNode.scrollPosition;
        nodeEx.previousScrollExtentMax = prevNode.scrollExtentMax;
        nodeEx.previousScrollExtentMin = prevNode.scrollExtentMin;
        nodeEx.previousValue = prevNode.value;
        nodeEx.previousLabel = prevNode.label;
    }
    // 更新当前flutter节点信息
    nodeEx.isNull = false;
    nodeEx.id = std::move(node.id);
    nodeEx.flags = std::move(node.flags);
    nodeEx.actions = std::move(node.actions);
    nodeEx.maxValueLength = std::move(node.maxValueLength);
    nodeEx.currentValueLength = std::move(node.currentValueLength);
    nodeEx.textSelectionBase = std::move(node.textSelectionBase);
    nodeEx.textSelectionExtent = std::move(node.textSelectionExtent);
    nodeEx.platformViewId = std::move(node.platformViewId);
    nodeEx.scrollChildren = std::move(node.scrollChildren);
    nodeEx.scrollIndex = std::move(node.scrollIndex);
    nodeEx.scrollPosition = std::move(node.scrollPosition);
    nodeEx.scrollExtentMax = std::move(node.scrollExtentMax);
    nodeEx.scrollExtentMin = std::move(node.scrollExtentMin);
    nodeEx.elevation = std::move(node.elevation);
    nodeEx.thickness = std::move(node.thickness);
    nodeEx.label = std::move(node.label);
    nodeEx.labelAttributes = std::move(node.labelAttributes);
    nodeEx.hint = std::move(node.hint);
    nodeEx.hintAttributes = std::move(node.hintAttributes);
    nodeEx.value = std::move(node.value);
    nodeEx.valueAttributes = std::move(node.valueAttributes);
    nodeEx.increasedValue = std::move(node.increasedValue);
    nodeEx.increasedValueAttributes = std::move(node.increasedValueAttributes);
    nodeEx.decreasedValue = std::move(node.decreasedValue);
    nodeEx.decreasedValueAttributes = std::move(node.decreasedValueAttributes);
    nodeEx.tooltip = std::move(node.tooltip);
    nodeEx.textDirection = std::move(node.textDirection);
    nodeEx.rect = std::move(node.rect);
    nodeEx.transform = std::move(node.transform);
    nodeEx.childrenInTraversalOrder = std::move(node.childrenInTraversalOrder);
    nodeEx.childrenInHitTestOrder = std::move(node.childrenInHitTestOrder);
    nodeEx.customAccessibilityActions = std::move(node.customAccessibilityActions);
    return nodeEx;
}

void OhosAccessibilityBridge::GetSemanticsNodeDebugInfo(
    SemanticsNodeExtent node)
{
    FML_DLOG(INFO) << "-------------------SemanticsNode------------------";
    SkMatrix _transform = node.transform.asM33();
    FML_DLOG(INFO) << "node.id=" << node.id;
    FML_DLOG(INFO) << "node.label=" << node.label;
    FML_DLOG(INFO) << "node.previousLabel=" << node.previousLabel;
    FML_DLOG(INFO) << "node.tooltip=" << node.tooltip;
    FML_DLOG(INFO) << "node.hint=" << node.hint;
    FML_DLOG(INFO) << "node.flags=" << node.flags;
    FML_DLOG(INFO) << "node.previousFlags=" << node.previousFlags;
    FML_DLOG(INFO) << "node.actions=" << node.actions;
    FML_DLOG(INFO) << "node.previousActions=" << node.previousActions;
    FML_DLOG(INFO) << "node.value=" << node.value;
    FML_DLOG(INFO) << "node.previousValue=" << node.previousValue;
    FML_DLOG(INFO) << "node.rect= {" << node.rect.fLeft << ", " << node.rect.fTop
                   << ", " << node.rect.fRight << ", " << node.rect.fBottom
                   << "}";
    FML_DLOG(INFO) << "node.transform -> kMScaleX="
                   << _transform.get(SkMatrix::kMScaleX);
    FML_DLOG(INFO) << "node.transform -> kMSkewX="
                   << _transform.get(SkMatrix::kMSkewX);
    FML_DLOG(INFO) << "node.transform -> kMTransX="
                   << _transform.get(SkMatrix::kMTransX);
    FML_DLOG(INFO) << "node.transform -> kMSkewY="
                   << _transform.get(SkMatrix::kMSkewY);
    FML_DLOG(INFO) << "node.transform -> kMScaleY="
                   << _transform.get(SkMatrix::kMScaleY);
    FML_DLOG(INFO) << "node.transform -> kMTransY="
                   << _transform.get(SkMatrix::kMTransY);
    FML_DLOG(INFO) << "node.transform -> kMPersp0="
                   << _transform.get(SkMatrix::kMPersp0);
    FML_DLOG(INFO) << "node.transform -> kMPersp1="
                   << _transform.get(SkMatrix::kMPersp1);
    FML_DLOG(INFO) << "node.transform -> kMPersp2="
                   << _transform.get(SkMatrix::kMPersp2);
    FML_DLOG(INFO) << "node.maxValueLength=" << node.maxValueLength;
    FML_DLOG(INFO) << "node.currentValueLength=" << node.currentValueLength;
    FML_DLOG(INFO) << "node.textSelectionBase=" << node.textSelectionBase;
    FML_DLOG(INFO) << "node.previousTextSelectionBase=" << node.previousTextSelectionBase;
    FML_DLOG(INFO) << "node.textSelectionExtent=" << node.textSelectionExtent;
    FML_DLOG(INFO) << "node.previousTextSelectionExtent=" << node.previousTextSelectionExtent;
    FML_DLOG(INFO) << "node.platformViewId=" << node.platformViewId;
    FML_DLOG(INFO) << "node.scrollChildren=" << node.scrollChildren;
    FML_DLOG(INFO) << "node.scrollIndex=" << node.scrollIndex;
    FML_DLOG(INFO) << "node.scrollPosition=" << node.scrollPosition;
    FML_DLOG(INFO) << "node.previousScrollPosition=" << node.previousScrollPosition;
    FML_DLOG(INFO) << "node.scrollIndex=" << node.scrollIndex;
    FML_DLOG(INFO) << "node.scrollExtentMax=" << node.scrollExtentMax;
    FML_DLOG(INFO) << "node.previousScrollExtentMax=" << node.previousScrollExtentMax;
    FML_DLOG(INFO) << "node.scrollExtentMin=" << node.scrollExtentMin;
    FML_DLOG(INFO) << "node.previousScrollExtentMin=" << node.previousScrollExtentMin;
    FML_DLOG(INFO) << "node.elevation=" << node.elevation;
    FML_DLOG(INFO) << "node.thickness=" << node.thickness;
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
    FML_DLOG(INFO) << "------------------SemanticsNode-----------------";
}

void OhosAccessibilityBridge::GetSemanticsFlagsDebugInfo(
    SemanticsNodeExtent node)
{
    FML_DLOG(INFO) << "----------------SemanticsFlags-------------------------";
    FML_DLOG(INFO) << "node.id=" << node.id;
    FML_DLOG(INFO) << "node.label=" << node.label;
    FML_DLOG(INFO) << "kHasCheckedState: "
                  << node.HasFlag(FLAGS_::kHasCheckedState);
    FML_DLOG(INFO) << "kIsChecked:" << node.HasFlag(FLAGS_::kIsChecked);
    FML_DLOG(INFO) << "kIsSelected:" << node.HasFlag(FLAGS_::kIsSelected);
    FML_DLOG(INFO) << "kIsButton:" << node.HasFlag(FLAGS_::kIsButton);
    FML_DLOG(INFO) << "kIsTextField:" << node.HasFlag(FLAGS_::kIsTextField);
    FML_DLOG(INFO) << "kIsFocused:" << node.HasFlag(FLAGS_::kIsFocused);
    FML_DLOG(INFO) << "kHasEnabledState:"
                  << node.HasFlag(FLAGS_::kHasEnabledState);
    FML_DLOG(INFO) << "kIsEnabled:" << node.HasFlag(FLAGS_::kIsEnabled);
    FML_DLOG(INFO) << "kIsInMutuallyExclusiveGroup:"
                  << node.HasFlag(FLAGS_::kIsInMutuallyExclusiveGroup);
    FML_DLOG(INFO) << "kIsHeader:" << node.HasFlag(FLAGS_::kIsHeader);
    FML_DLOG(INFO) << "kIsObscured:" << node.HasFlag(FLAGS_::kIsObscured);
    FML_DLOG(INFO) << "kScopesRoute:" << node.HasFlag(FLAGS_::kScopesRoute);
    FML_DLOG(INFO) << "kNamesRoute:" << node.HasFlag(FLAGS_::kNamesRoute);
    FML_DLOG(INFO) << "kIsHidden:" << node.HasFlag(FLAGS_::kIsHidden);
    FML_DLOG(INFO) << "kIsImage:" << node.HasFlag(FLAGS_::kIsImage);
    FML_DLOG(INFO) << "kIsLiveRegion:" << node.HasFlag(FLAGS_::kIsLiveRegion);
    FML_DLOG(INFO) << "kHasToggledState:"
                  << node.HasFlag(FLAGS_::kHasToggledState);
    FML_DLOG(INFO) << "kIsToggled:" << node.HasFlag(FLAGS_::kIsToggled);
    FML_DLOG(INFO) << "kHasImplicitScrolling:"
                  << node.HasFlag(FLAGS_::kHasImplicitScrolling);
    FML_DLOG(INFO) << "kIsMultiline:" << node.HasFlag(FLAGS_::kIsMultiline);
    FML_DLOG(INFO) << "kIsReadOnly:" << node.HasFlag(FLAGS_::kIsReadOnly);
    FML_DLOG(INFO) << "kIsFocusable:" << node.HasFlag(FLAGS_::kIsFocusable);
    FML_DLOG(INFO) << "kIsLink:" << node.HasFlag(FLAGS_::kIsLink);
    FML_DLOG(INFO) << "kIsSlider:" << node.HasFlag(FLAGS_::kIsSlider);
    FML_DLOG(INFO) << "kIsKeyboardKey:" << node.HasFlag(FLAGS_::kIsKeyboardKey);
    FML_DLOG(INFO) << "kIsCheckStateMixed:"
                  << node.HasFlag(FLAGS_::kIsCheckStateMixed);
    FML_DLOG(INFO) << "----------------SemanticsFlags--------------------";
  }

  void OhosAccessibilityBridge::GetCustomActionDebugInfo(
      flutter::CustomAccessibilityAction customAccessibilityAction)
  {
    FML_DLOG(INFO) << "--------------CustomAccessibilityAction------------";
    FML_DLOG(INFO) << "customAccessibilityAction.id="
                  << customAccessibilityAction.id;
    FML_DLOG(INFO) << "customAccessibilityAction.overrideId="
                  << customAccessibilityAction.overrideId;
    FML_DLOG(INFO) << "customAccessibilityAction.label="
                  << customAccessibilityAction.label;
    FML_DLOG(INFO) << "customAccessibilityAction.hint="
                  << customAccessibilityAction.hint;
    FML_DLOG(INFO) << "------------CustomAccessibilityAction--------------";
}

void OhosAccessibilityBridge::GetSemanticsDebugInfo()
{
    // 打印flutter语义树的不同节点的属性信息
    for (const auto& item : g_flutterSemanticsTree) {
        FML_DLOG(INFO) << "g_flutterSemanticsTree -> {" << item.first << ", "
                       << item.second.id << "}";
    }
    for (const auto& item : g_parentChildIdVec) {
        FML_DLOG(INFO) << "g_parentChildIdVec -> (" << item.first << ", "
                       << item.second << ")";
    }
    //打印按层次遍历排序的flutter语义树节点id数组
    std::vector<int64_t> levelOrderTraversalTree = GetLevelOrderTraversalTree(0);
    for (const auto& item: levelOrderTraversalTree) {
        FML_DLOG(INFO) << "LevelOrderTraversalTree: { " << item << " }";
    }
}

}  // namespace flutter
