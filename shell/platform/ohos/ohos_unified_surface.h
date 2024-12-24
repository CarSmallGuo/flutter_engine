// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_UNIFIED_SURFACE_H
#define OHOS_UNIFIED_SURFACE_H

#include <memory>
#include "flutter/flow/surface.h"
#include "flutter/shell/platform/ohos/context/ohos_context.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "flutter/shell/platform/ohos/surface/ohos_surface.h"
#include "flutter/shell/platform/ohos/ohos_surface_gl_skia.h"
#include "third_party/skia/include/core/SkSize.h"

namespace flutter {

class OHOSUnifiedSurface : public GPUSurfaceGLDelegate,
                           public OHOSSurface {
 public:
   ~OHOSUnifiedSurface() {}
  bool IsValid() {}
  void TeardownOnScreenContext() {}

  bool OnScreenSurfaceResize(const SkISize& size) {}

  bool ResourceContextMakeCurrent() {}

  bool ResourceContextClearCurrent() {}

  bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window) {}

  std::unique_ptr<Surface> CreateSnapshotSurface()

  std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context = nullptr) {}

  std::shared_ptr<impeller::Context> GetImpellerContext() {}

  std::unique_ptr<GLContextResult> GLContextMakeCurrent() {}

  bool GLContextClearCurrent() {}

  void GLContextSetDamageRegion(const std::optional<SkIRect>& region) {}

  bool GLContextPresent(const GLPresentInfo& present_info) {}

  GLFBOInfo GLContextFBO(GLFrameInfo frame_info) {}

  bool GLContextFBOResetAfterPresent() {}

  SurfaceFrame::FramebufferInfo GLContextFramebufferInfo() {}

  SkMatrix GLContextSurfaceTransformation() {}

  sk_sp<const GrGLInterface> GetGLInterface() {}

  static sk_sp<const GrGLInterface> GetDefaultPlatformGLInterface() {}

  using GLProcResolver =
      std::function<void* (const char*)>;
 
  GLProcResolver GetGLProcResolver() {}

  bool AllowsDrawingWhenGpuDisabled() {}
};
}  // namespace flutter

#endif