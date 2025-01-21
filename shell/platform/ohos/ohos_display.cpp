/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "flutter/shell/platform/ohos/ohos_display.h"

namespace flutter {

const double defaultFPS = 60;

OHOSDisplay::OHOSDisplay(std::shared_ptr<flutter::VsyncWaiterOHOS> vsync_waiter_ohos)
    : Display(defaultFPS), vsync_waiter_ohos_(std::move(vsync_waiter_ohos)) {}

double OHOSDisplay::GetRefreshRate() const {
    if (vsync_waiter_ohos_ != nullptr) {
      return (double)vsync_waiter_ohos_->GetRefreshRate();
    }
    return defaultFPS;
}

}  // namespace flutter