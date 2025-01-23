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

#ifndef FLUTTER_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_
#define FLUTTER_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_

#include "flutter/fml/macros.h"
#include "flutter/fml/memory/ref_counted.h"
#include "flutter/fml/memory/ref_ptr.h"
#include "flutter/shell/common/platform_view.h"
#include "flutter/shell/platform/ohos/context/ohos_context.h"
#include "flutter/shell/platform/ohos/ohos_environment_gl.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "third_party/skia/include/core/SkSize.h"

namespace flutter {

class OhosEGLSurface;

//------------------------------------------------------------------------------
/// The Ohos context is used by `OhosSurfaceGL` to create and manage
/// EGL surfaces.
///
/// This context binds `EGLContext` to the current rendering thread and to the
/// draw and read `EGLSurface`s.
///
class OhosContextGLSkia : public OHOSContext {
 public:
  OhosContextGLSkia(OHOSRenderingAPI rendering_api,
                    fml::RefPtr<OhosEnvironmentGL> environment,
                    const TaskRunners& taskRunners,
                    uint8_t msaa_samples);

  ~OhosContextGLSkia();

  //----------------------------------------------------------------------------
  /// @brief      Allocates an new EGL window surface that is used for on-screen
  ///             pixels.
  ///
  /// @return     The window surface.
  ///
  std::unique_ptr<OhosEGLSurface> CreateOnscreenSurface(
      const fml::RefPtr<OHOSNativeWindow>& window) const;

  //----------------------------------------------------------------------------
  /// @brief      Allocates an 1x1 pbuffer surface that is used for making the
  ///             offscreen current for texture uploads.
  ///
  /// @return     The pbuffer surface.
  ///
  std::unique_ptr<OhosEGLSurface> CreateOffscreenSurface() const;

  //----------------------------------------------------------------------------
  /// @brief      Allocates an 1x1 pbuffer surface that is used for making the
  ///             onscreen context current for snapshotting.
  ///
  /// @return     The pbuffer surface.
  ///
  std::unique_ptr<OhosEGLSurface> CreatePbufferSurface() const;

  //----------------------------------------------------------------------------
  /// @return     The Ohos environment that contains a reference to the
  /// display.
  ///
  fml::RefPtr<OhosEnvironmentGL> Environment() const;

  //----------------------------------------------------------------------------
  /// @return     Whether the current context is valid. That is, if the EGL
  ///             contexts were successfully created.
  ///
  bool IsValid() const override;

  //----------------------------------------------------------------------------
  /// @return     Whether the current context was successfully clear.
  ///
  bool ClearCurrent() const;

  //----------------------------------------------------------------------------
  /// @brief      Create a new EGLContext using the same EGLConfig.
  ///
  /// @return     The EGLContext.
  ///
  EGLContext CreateNewContext() const;

  //----------------------------------------------------------------------------
  /// @brief      The EGLConfig for this context.
  ///
  EGLConfig Config() const { return config_; }

 private:
  fml::RefPtr<OhosEnvironmentGL> environment_;
  EGLConfig config_;
  EGLContext context_;
  EGLContext resource_context_;
  bool valid_ = false;
  TaskRunners task_runners_;

  FML_DISALLOW_COPY_AND_ASSIGN(OhosContextGLSkia);
};

}  // namespace flutter

#endif  // FLUTTER_SHELL_PLATFORM_ANDROID_ANDROID_CONTEXT_GL_SKIA_H_
