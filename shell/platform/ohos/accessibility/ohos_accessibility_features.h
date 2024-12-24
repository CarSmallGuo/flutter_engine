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

#ifndef OHOS_ACCESSIBILITY_FEATURES_H
#define OHOS_ACCESSIBILITY_FEATURES_H
#include <cstdint>
#include "flutter/lib/ui/window/platform_configuration.h"
#include "native_accessibility_channel.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
namespace flutter {

class OhosAccessibilityFeatures {
 public:
  OhosAccessibilityFeatures();
  ~OhosAccessibilityFeatures();
  
  bool ACCESSIBLE_NAVIGATION = false;

  void SetAccessibleNavigation(bool isAccessibleNavigation,
                                 int64_t shell_holder_id);
  void SetBoldText(double fontWeightScale, int64_t shell_holder_id);

  void SendAccessibilityFlags(int64_t shell_holder_id);

 private:
  std::shared_ptr<NativeAccessibilityChannel> nativeAccessibilityChannel_;
  int32_t accessibilityFeatureFlags = 0;
};

/**
 * 无障碍特征枚举类（flutter平台通用）
 * 注意：必须同src/flutter/lib/ui/window/platform_configuration.h
 * 中的`AccessibilityFeatureFlag`枚举类保持一致
 */
enum AccessibilityFeatures : int32_t {
  AccessibleNavigation = 1 << 0, 
  InvertColors = 1 << 1, 
  DisableAnimations = 1 << 2,
  BoldText = 1 << 3,
  ReduceMotion = 1 << 4,
  HighContrast = 1 << 5,
  OnOffSwitchLabels = 1 << 6,
};

}  // namespace flutter

#endif