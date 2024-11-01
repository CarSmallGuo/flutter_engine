// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_context_gl_impeller.h"

namespace flutter {

OHOSContextGLImpeller::OHOSContextGLImpeller()
    : OHOSContext(OHOSRenderingAPI::kOpenGLES) {}

OHOSContextGLImpeller::~OHOSContextGLImpeller() {}

bool OHOSContextGLImpeller::IsValid() const {
  return true;
}

}  // namespace flutter