/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Google Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_SURFACE_GL_IMPELLER_H
#define OHOS_SURFACE_GL_IMPELLER_H

#include "flutter/fml/macros.h"
#include "flutter/impeller/renderer/context.h"
#include "flutter/impeller/toolkit/egl/display.h"
#include "flutter/shell/gpu/gpu_surface_gl_delegate.h"
#include "surface/ohos_native_window.h"
#include "surface/ohos_surface.h"

namespace flutter {

class OHOSSurfaceGLImpeller final : public GPUSurfaceGLDelegate,
                                    public OHOSSurface {
 public:
  OHOSSurfaceGLImpeller(const std::shared_ptr<OHOSContext>& ohos_context);

  ~OHOSSurfaceGLImpeller() override;

  // OHOSSurface
  bool IsValid() const override;

  // OHOSSurface
  std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context) override;

  // OHOSSurface
  void TeardownOnScreenContext() override;

  // OHOSSurface
  bool OnScreenSurfaceResize(const SkISize& size) override;

  // OHOSSurface
  bool ResourceContextMakeCurrent() override;

  // OHOSSurface
  bool ResourceContextClearCurrent() override;

  // OHOSSurface
  bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window) override;

  // OHOSSurface
  std::unique_ptr<Surface> CreateSnapshotSurface() override;

  // OHOSSurface
  std::shared_ptr<impeller::Context> GetImpellerContext() override;

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

 private:
  class ReactorWorker;
  std::shared_ptr<ReactorWorker> reactor_worker_;
  std::unique_ptr<impeller::egl::Display> display_;
  std::unique_ptr<impeller::egl::Config> onscreen_config_;
  std::unique_ptr<impeller::egl::Config> offscreen_config_;
  std::unique_ptr<impeller::egl::Surface> onscreen_surface_;
  std::unique_ptr<impeller::egl::Surface> offscreen_surface_;
  std::unique_ptr<impeller::egl::Context> onscreen_context_;
  std::unique_ptr<impeller::egl::Context> offscreen_context_;
  std::shared_ptr<impeller::Context> impeller_context_;
  fml::RefPtr<OHOSNativeWindow> native_window_;

  bool is_valid_ = false;

  bool OnGLContextMakeCurrent();

  bool RecreateOnscreenSurfaceAndMakeOnscreenContextCurrent();

  FML_DISALLOW_COPY_AND_ASSIGN(OHOSSurfaceGLImpeller);
};

}  // namespace flutter
#endif