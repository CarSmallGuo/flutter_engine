/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "flutter/shell/platform/ohos/surface/ohos_surface.h"
namespace flutter {

OHOSSurface::OHOSSurface(const std::shared_ptr<OHOSContext>& ohos_context)
    : ohos_context_(ohos_context) {
  FML_DCHECK(ohos_context->IsValid());
  ohos_context_ = ohos_context;
}

std::unique_ptr<Surface> OHOSSurface::CreateSnapshotSurface() {
  return nullptr;
}

OHOSSurface::~OHOSSurface() = default;

std::shared_ptr<impeller::Context> OHOSSurface::GetImpellerContext() {
  return nullptr;
}

}  // namespace flutter