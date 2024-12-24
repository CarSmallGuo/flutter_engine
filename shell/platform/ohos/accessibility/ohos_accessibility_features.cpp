// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "ohos_accessibility_features.h"
#include "flutter/fml/logging.h"
#include "flutter/shell/platform/ohos/ohos_shell_holder.h"

namespace flutter {

OhosAccessibilityFeatures::OhosAccessibilityFeatures()
{
    nativeAccessibilityChannel_ = std::make_shared<NativeAccessibilityChannel>();
};

OhosAccessibilityFeatures::~OhosAccessibilityFeatures() {};

/**
 * 无障碍特征之无障碍导航
 */
void OhosAccessibilityFeatures::SetAccessibleNavigation(
    bool isAccessibleNavigation,
    int64_t shell_holder_id)
{
  if (ACCESSIBLE_NAVIGATION == isAccessibleNavigation) {
    return;
  }
  ACCESSIBLE_NAVIGATION = isAccessibleNavigation;
  if (ACCESSIBLE_NAVIGATION) {
    accessibilityFeatureFlags |=
        static_cast<int32_t>(AccessibilityFeatures::AccessibleNavigation);
    FML_DLOG(INFO) << "SetAccessibleNavigation -> accessibilityFeatureFlags: "
                   << accessibilityFeatureFlags;
  } else {
    accessibilityFeatureFlags &=
        ~static_cast<int32_t>(AccessibilityFeatures::AccessibleNavigation);
  }
  SendAccessibilityFlags(shell_holder_id);
}

/**
 * 无障碍特征之字体加粗
 */
void OhosAccessibilityFeatures::SetBoldText(double fontWeightScale,
                                            int64_t shell_holder_id) {
  bool shouldBold = fontWeightScale > 1.0;
  if (shouldBold) {
    accessibilityFeatureFlags |=
        static_cast<int32_t>(AccessibilityFeatures::BoldText);
    FML_DLOG(INFO) << "SetBoldText -> accessibilityFeatureFlags: "
                   << accessibilityFeatureFlags;
  } else {
    accessibilityFeatureFlags &=
        static_cast<int32_t>(AccessibilityFeatures::BoldText);
  }
  SendAccessibilityFlags(shell_holder_id);
}

/**
 * send the accessibility flags to flutter dart sdk
 */
void OhosAccessibilityFeatures::SendAccessibilityFlags(
    int64_t shell_holder_id) {
  nativeAccessibilityChannel_->SetAccessibilityFeatures(shell_holder_id, accessibilityFeatureFlags);
  FML_DLOG(INFO) << "SendAccessibilityFlags -> accessibilityFeatureFlags = "
                 << accessibilityFeatureFlags;
  accessibilityFeatureFlags = 0;
}

}  // namespace flutter