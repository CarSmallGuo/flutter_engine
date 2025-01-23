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