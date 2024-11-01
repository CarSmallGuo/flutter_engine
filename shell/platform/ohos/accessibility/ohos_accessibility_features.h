// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef OHOS_ACCESSIBILITY_FEATURES_H
#define OHOS_ACCESSIBILITY_FEATURES_H
#include <cstdint>
#include "flutter/lib/ui/window/platform_configuration.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"

namespace flutter {

class OhosAccessibilityFeatures {
  public:
    OhosAccessibilityFeatures();
    ~OhosAccessibilityFeatures();

    static OhosAccessibilityFeatures* GetInstance();

    void SetBoldText(double fontWeightScale, int64_t shell_holder_id);
    void SendAccessibilityFlags(int64_t shell_holder_id);

  private:
    static OhosAccessibilityFeatures instance;

    // Font weight adjustment (FontWeight.Bold - FontWeight.Normal = w700 - w400 = 300)
    static const int32_t BOLD_TEXT_WEIGHT_ADJUSTMENT = 300;
    int32_t accessibilityFeatureFlags = 0;
};
  
}
#endif 