// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_features.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"
#include "flutter/fml/logging.h"

namespace flutter {

  OhosAccessibilityFeatures OhosAccessibilityFeatures::instance;

  OhosAccessibilityFeatures::OhosAccessibilityFeatures() {};
  OhosAccessibilityFeatures::~OhosAccessibilityFeatures() {};

  OhosAccessibilityFeatures* OhosAccessibilityFeatures::GetInstance() {
    return &OhosAccessibilityFeatures::instance;
  }

  /**
   * bold text for AccessibilityFeature
   */
  void OhosAccessibilityFeatures::SetBoldText(double fontWeightScale, int64_t shell_holder_id) {
    bool shouldBold = fontWeightScale > 1.0;

    if (shouldBold) {
      accessibilityFeatureFlags |= static_cast<int32_t>(flutter::AccessibilityFeatureFlag::kBoldText);
      FML_DLOG(INFO) << "SetBoldText -> accessibilityFeatureFlags: "<<accessibilityFeatureFlags;

    } else {
      accessibilityFeatureFlags &= static_cast<int32_t>(flutter::AccessibilityFeatureFlag::kBoldText);
    }

    SendAccessibilityFlags(shell_holder_id);
  }
  
   /**
    * send the accessibility flags to flutter dart sdk 
    */
   void OhosAccessibilityFeatures::SendAccessibilityFlags(int64_t shell_holder_id) {
     auto ohos_shell_holder = reinterpret_cast<OHOSShellHolder*>(shell_holder_id);
     ohos_shell_holder->GetPlatformView()->PlatformView::SetAccessibilityFeatures(accessibilityFeatureFlags);
     FML_DLOG(INFO) << "SendAccessibilityFlags -> accessibilityFeatureFlags = "
                    << accessibilityFeatureFlags;
     // set accessibility feature flag to 0
     accessibilityFeatureFlags = 0;
   }

}