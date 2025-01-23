/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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