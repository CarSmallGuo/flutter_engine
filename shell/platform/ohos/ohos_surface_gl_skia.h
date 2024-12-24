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

#ifndef OHOS_SURFACE_GL_SKIA_H
#define OHOS_SURFACE_GL_SKIA_H

#include <memory>

#include "flutter/fml/macros.h"
#include "flutter/shell/gpu/gpu_surface_gl_skia.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/ohos_context_gl_skia.h"
#include "flutter/shell/platform/ohos/ohos_environment_gl.h"
#include "flutter/shell/platform/ohos/surface/ohos_surface.h"

namespace flutter {

class OhosSurfaceGLSkia final : public GPUSurfaceGLDelegate,
                                public OHOSSurface {
 public:
  OhosSurfaceGLSkia(const std::shared_ptr<OHOSContext>& ohos_context);

  ~OhosSurfaceGLSkia() override;

  // |OhosSurface|
  bool IsValid() const override;

  // |OhosSurface|
  std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context) override;

  // |OhosSurface|
  void TeardownOnScreenContext() override;

  // |OhosSurface|
  bool OnScreenSurfaceResize(const SkISize& size) override;

  // |OhosSurface|
  bool ResourceContextMakeCurrent() override;

  // |OhosSurface|
  bool ResourceContextClearCurrent() override;

  // |OhosSurface|
  bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window) override;

  // |OhosSurface|
  virtual std::unique_ptr<Surface> CreateSnapshotSurface() override;

  // |GPUSurfaceGLDelegate|
  std::unique_ptr<GLContextResult> GLContextMakeCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent() override;

  // |GPUSurfaceGLDelegate|
  SurfaceFrame::FramebufferInfo GLContextFramebufferInfo() const override;

  // |GPUSurfaceGLDelegate|
  void GLContextSetDamageRegion(const std::optional<SkIRect>& region) override;

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent(const GLPresentInfo& present_info) override;

  // |GPUSurfaceGLDelegate|
  GLFBOInfo GLContextFBO(GLFrameInfo frame_info) const override;

  // |GPUSurfaceGLDelegate|
  sk_sp<const GrGLInterface> GetGLInterface() const override;

  // Obtain a raw pointer to the on-screen OhosEGLSurface.
  //
  // This method is intended for use in tests. Callers must not
  // delete the returned pointer.
  OhosEGLSurface* GetOnscreenSurface() const { return onscreen_surface_.get(); }

 private:
  fml::RefPtr<OHOSNativeWindow> native_window_;
  std::unique_ptr<OhosEGLSurface> onscreen_surface_;
  std::unique_ptr<OhosEGLSurface> offscreen_surface_;

  //----------------------------------------------------------------------------
  /// @brief      Takes the super class OhosSurface's OhosContext and
  ///             return a raw pointer to an OhosContextGL.
  ///
  OhosContextGLSkia* GLContextPtr() const;

  FML_DISALLOW_COPY_AND_ASSIGN(OhosSurfaceGLSkia);
};

}  // namespace flutter

#endif
