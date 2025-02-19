/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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

  std::unique_ptr<OhosEGLSurface> CreateOnscreenSurface(
      const fml::RefPtr<OHOSNativeWindow>& window) const;

  std::unique_ptr<OhosEGLSurface> CreateOffscreenSurface() const;

  std::unique_ptr<OhosEGLSurface> CreatePbufferSurface() const;

  //----------------------------------------------------------------------------
  /// @return     The Ohos environment that contains a reference to the
  /// display.
  ///
  fml::RefPtr<OhosEnvironmentGL> Environment() const;

  bool IsValid() const override;

  bool ClearCurrent() const;

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
