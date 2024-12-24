// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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