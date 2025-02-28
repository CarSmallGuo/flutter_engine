/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
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