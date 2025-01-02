/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef SHELL_PLATFORM_OHOS_PLATFORM_VIEW_OHOS_DELEGATE
#define SHELL_PLATFORM_OHOS_PLATFORM_VIEW_OHOS_DELEGATE

#include <memory>
#include <string>
#include <vector>

#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/accessibility/ohos_accessibility_bridge.h"

namespace flutter {

class PlatformViewOHOSDelegate {
 public:
  explicit PlatformViewOHOSDelegate(
      std::shared_ptr<PlatformViewOHOSNapi> napi_facade);

  void UpdateSemantics(
      const flutter::SemanticsNodeUpdates& update,
      const flutter::CustomAccessibilityActionUpdates& actions);

 private:
  const std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;

};
}  // namespace flutter
#endif 