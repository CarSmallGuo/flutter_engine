/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "flutter/shell/platform/ohos/context/ohos_context.h"

namespace flutter {

OHOSContext::OHOSContext(OHOSRenderingAPI rendering_api)
    : rendering_api_(rendering_api) {}

OHOSContext::~OHOSContext() {
  if (main_context_) {
    main_context_->releaseResourcesAndAbandonContext();
  }
}

OHOSRenderingAPI OHOSContext::RenderingApi() const {
  return rendering_api_;
}

bool OHOSContext::IsValid() const {
  return true;
}

void OHOSContext::SetMainSkiaContext(
    const sk_sp<GrDirectContext>& main_context) {
  main_context_ = main_context;
}

sk_sp<GrDirectContext> OHOSContext::GetMainSkiaContext() const {
  return main_context_;
}

}  // namespace flutter