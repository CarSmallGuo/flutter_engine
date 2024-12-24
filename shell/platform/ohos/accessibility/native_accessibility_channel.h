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

#ifndef OHOS_NATIVE_ACCESSIBILITY_CHANNEL_H
#define OHOS_NATIVE_ACCESSIBILITY_CHANNEL_H
#include <memory>
#include "flutter/fml/mapping.h"
#include "flutter/lib/ui/semantics/semantics_node.h"
#include "flutter/lib/ui/semantics/custom_accessibility_action.h"
namespace flutter {

class NativeAccessibilityChannel {
 public:
   NativeAccessibilityChannel();
   ~NativeAccessibilityChannel();

   void OnOhosAccessibilityEnabled(int64_t shellHolderId);

   void OnOhosAccessibilityDisabled(int64_t shellHolderId);

   void SetSemanticsEnabled(int64_t shellHolderId, bool enabled);

   void SetAccessibilityFeatures(int64_t shellHolderId, int32_t flags);

   void DispatchSemanticsAction(int64_t shellHolderId,
                                int32_t id, 
                                flutter::SemanticsAction action, 
                                fml::MallocMapping args);

   void UpdateSemantics(flutter::SemanticsNodeUpdates update,
                        flutter::CustomAccessibilityActionUpdates actions);

   class AccessibilityMessageHandler {
    public:
      void Announce(std::unique_ptr<char[]>& message);

      void OnTap(int32_t nodeId);

      void OnLongPress(int32_t nodeId);

      void OnTooltip(std::unique_ptr<char[]>& message);
   };

   void SetAccessibilityMessageHandler(
      std::shared_ptr<AccessibilityMessageHandler> handler);
   
 private:
    std::shared_ptr<AccessibilityMessageHandler> handler;
};
}

#endif 