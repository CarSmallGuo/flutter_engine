/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef OHOS_DISPLAY_H
#define OHOS_DISPLAY_H

#include <cstdint>

#include "flutter/fml/macros.h"
#include "flutter/shell/common/display.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
namespace flutter {

class OHOSDisplay : public Display {
 public:
  explicit OHOSDisplay(std::shared_ptr<PlatformViewOHOSNapi> napi_facade_);
  ~OHOSDisplay() = default;

  double GetRefreshRate() const override;

 private:
  std::shared_ptr<PlatformViewOHOSNapi> napi_facade_;
  FML_DISALLOW_COPY_AND_ASSIGN(OHOSDisplay);
};
}  // namespace flutter
#endif