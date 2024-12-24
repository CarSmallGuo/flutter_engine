/*
Copyright (C) 2024 Huawei Device Co., Ltd.

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

#include "native_accessibility_channel.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"

namespace flutter {
  
  NativeAccessibilityChannel::NativeAccessibilityChannel() {}

  NativeAccessibilityChannel::~NativeAccessibilityChannel() {}
  
  /**
   * 通知flutter框架ohos平台无障碍屏幕朗读已开启
   */
  void NativeAccessibilityChannel::OnOhosAccessibilityEnabled(int64_t shellHolderId)
  {
    FML_DLOG(INFO) << "NativeAccessibilityChannel -> OnOhosAccessibilityEnabled";
    this->SetSemanticsEnabled(shellHolderId, true);
  }

  /**
   * 通知flutter框架ohos平台无障碍屏幕朗读未开启
   */
  void NativeAccessibilityChannel::OnOhosAccessibilityDisabled(int64_t shellHolderId)
  {
    FML_DLOG(INFO) << "NativeAccessibilityChannel -> OnOhosAccessibilityDisabled";
    this->SetSemanticsEnabled(shellHolderId, false);
  }
  
  /**
   * Native无障碍通道传递语义感知，若开启则实时更新语义树信息
   */
  void NativeAccessibilityChannel::SetSemanticsEnabled(int64_t shellHolderId,
                                                       bool enabled)
  {
    auto ohos_shell_holder =
        reinterpret_cast<OHOSShellHolder*>(shellHolderId);
    ohos_shell_holder->GetPlatformView()->SetSemanticsEnabled(enabled);
  }

  /**
   * Native无障碍通道设置无障碍特征类型，如:无障碍导航、字体加粗等
   */
  void NativeAccessibilityChannel::SetAccessibilityFeatures(int64_t shellHolderId,
                                                            int32_t flags)
  {
    auto ohos_shell_holder =
        reinterpret_cast<OHOSShellHolder*>(shellHolderId);
    ohos_shell_holder->GetPlatformView()->SetAccessibilityFeatures(flags);
  }

  /**
   * Native无障碍通道分发flutter屏幕语义动作，如:点击、滑动等
   */
  void NativeAccessibilityChannel::DispatchSemanticsAction(
      int64_t shellHolderId,
      int32_t id, 
      flutter::SemanticsAction action, 
      fml::MallocMapping args)
  {
    auto ohos_shell_holder =
        reinterpret_cast<OHOSShellHolder*>(shellHolderId);
    ohos_shell_holder->GetPlatformView()->PlatformView::DispatchSemanticsAction(id, action, fml::MallocMapping());
  }

  /**
   * 更新flutter无障碍相关语义信息
   */
  void NativeAccessibilityChannel::UpdateSemantics(
      flutter::SemanticsNodeUpdates update,
      flutter::CustomAccessibilityActionUpdates actions)
  {
    auto ohos_a11y_bridge = OhosAccessibilityBridge::GetInstance();
    ohos_a11y_bridge->UpdateSemantics(update, actions);
  }  

  /**
   * 设置无障碍消息处理器，通过无障碍通道发送处理dart侧传递的相关信息
   */
  void NativeAccessibilityChannel::SetAccessibilityMessageHandler(
      std::shared_ptr<AccessibilityMessageHandler> handler)
  {
    this->handler = handler;
  }

  /**
   * 利用通道内部类AccessibilityMessageHandler处理主动播报事件
   */
  void NativeAccessibilityChannel::AccessibilityMessageHandler::Announce(
      std::unique_ptr<char[]>& message)
  {
    auto ohos_a11y_bridge = OhosAccessibilityBridge::GetInstance();
    ohos_a11y_bridge->Announce(message);
  }
}