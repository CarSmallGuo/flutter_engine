/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef OHOS_CONTEXT_H
#define OHOS_CONTEXT_H

#include "flutter/fml/macros.h"
#include "flutter/fml/task_runner.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

namespace flutter {

enum class OHOSRenderingAPI {
  kSoftware,
  kOpenGLES,
};

class OHOSContext {
 public:
  explicit OHOSContext(OHOSRenderingAPI rendering_api);

  virtual ~OHOSContext();

  OHOSRenderingAPI RenderingApi() const;

  virtual bool IsValid() const;

  void SetMainSkiaContext(const sk_sp<GrDirectContext>& main_context);

  sk_sp<GrDirectContext> GetMainSkiaContext() const;

 private:
  const OHOSRenderingAPI rendering_api_;

  // This is the Skia context used for on-screen rendering.
  sk_sp<GrDirectContext> main_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(OHOSContext);
};

}  // namespace flutter
#endif