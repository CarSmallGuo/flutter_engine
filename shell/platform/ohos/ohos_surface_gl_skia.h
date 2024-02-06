/*
 * Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
#include "flutter/shell/platform/ohos/ohos_unified_surface.h"

namespace flutter {

class OhosSurfaceGLSkia : public OHOSUnifiedSurface
                                 {
 public:
  OhosSurfaceGLSkia(const std::shared_ptr<OHOSContext>& ohos_context);

  ~OhosSurfaceGLSkia();



  // |OhosSurface|
  bool IsValid();




  // |OhosSurface|
  std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context);

  // |OhosSurface|
  void TeardownOnScreenContext();

  // |OhosSurface|
  bool OnScreenSurfaceResize(const SkISize& size);

  // |OhosSurface|
  bool ResourceContextMakeCurrent();

  // |OhosSurface|
  bool ResourceContextClearCurrent();

  // |OhosSurface|
  bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window);

  // |OhosSurface|
  virtual std::unique_ptr<Surface> CreateSnapshotSurface();

  // |GPUSurfaceGLDelegate|
  std::unique_ptr<GLContextResult> GLContextMakeCurrent();

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent();

  // |GPUSurfaceGLDelegate|
  SurfaceFrame::FramebufferInfo GLContextFramebufferInfo();

  // |GPUSurfaceGLDelegate|
  void GLContextSetDamageRegion(const std::optional<SkIRect>& region);

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent(const GLPresentInfo& present_info);

  // |GPUSurfaceGLDelegate|
  GLFBOInfo GLContextFBO(GLFrameInfo frame_info);

  // |GPUSurfaceGLDelegate|
  sk_sp<const GrGLInterface> GetGLInterface();

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
