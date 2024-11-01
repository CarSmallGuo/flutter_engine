// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/ohos/ohos_display.h"

namespace flutter {

const double defaultFPS = 60;

OHOSDisplay::OHOSDisplay(std::shared_ptr<PlatformViewOHOSNapi> napi_facade)
    : Display(defaultFPS), napi_facade_(std::move(napi_facade)) {}

double OHOSDisplay::GetRefreshRate() const {
  return defaultFPS;
}

}  // namespace flutter