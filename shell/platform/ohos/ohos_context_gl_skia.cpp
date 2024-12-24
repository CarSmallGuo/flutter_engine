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

#include "flutter/shell/platform/ohos/ohos_context_gl_skia.h"

#include <utility>

#include "flutter/fml/trace_event.h"
#include "flutter/shell/platform/ohos/ohos_egl_surface.h"

namespace flutter {

template <class T>
using EGLResult = std::pair<bool, T>;

static EGLResult<EGLContext> CreateContext(EGLDisplay display,
                                           EGLConfig config,
                                           EGLContext share = EGL_NO_CONTEXT) {
  EGLint attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

  EGLContext context = eglCreateContext(display, config, share, attributes);

  return {context != EGL_NO_CONTEXT, context};
}

static EGLResult<EGLConfig> ChooseEGLConfiguration(EGLDisplay display,
                                                   uint8_t msaa_samples) {
  EGLint sample_buffers = msaa_samples > 1 ? 1 : 0;
  EGLint attributes[] = {
      // clang-format off
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
      EGL_RED_SIZE,        8,
      EGL_GREEN_SIZE,      8,
      EGL_BLUE_SIZE,       8,
      EGL_ALPHA_SIZE,      8,
      EGL_DEPTH_SIZE,      0,
      EGL_STENCIL_SIZE,    0,
      EGL_SAMPLES,         static_cast<EGLint>(msaa_samples),
      EGL_SAMPLE_BUFFERS,  sample_buffers,
      EGL_NONE,            // termination sentinel
      // clang-format on
  };

  EGLint config_count = 0;
  EGLConfig egl_config = nullptr;

  if (eglChooseConfig(display, attributes, &egl_config, 1, &config_count) !=
      EGL_TRUE) {
    return {false, nullptr};
  }

  bool success = config_count > 0 && egl_config != nullptr;

  return {success, success ? egl_config : nullptr};
}

static bool TeardownContext(EGLDisplay display, EGLContext context) {
  if (context != EGL_NO_CONTEXT) {
    return eglDestroyContext(display, context) == EGL_TRUE;
  }

  return true;
}

OhosContextGLSkia::OhosContextGLSkia(OHOSRenderingAPI rendering_api,
                                     fml::RefPtr<OhosEnvironmentGL> environment,
                                     const TaskRunners& task_runners,
                                     uint8_t msaa_samples)
    : OHOSContext(OHOSRenderingAPI::kOpenGLES),
      environment_(std::move(environment)),
      config_(nullptr),
      task_runners_(task_runners) {
  if (!environment_->IsValid()) {
    FML_LOG(ERROR) << "Could not create an Ohos GL environment.";
    return;
  }

  bool success = false;

  // Choose a valid configuration.
  std::tie(success, config_) =
      ChooseEGLConfiguration(environment_->Display(), msaa_samples);
  if (!success) {
    FML_LOG(ERROR) << "Could not choose an EGL configuration.";
    LogLastEGLError();
    return;
  }

  // Create a context for the configuration.
  std::tie(success, context_) =
      CreateContext(environment_->Display(), config_, EGL_NO_CONTEXT);
  if (!success) {
    FML_LOG(ERROR) << "Could not create an EGL context";
    LogLastEGLError();
    return;
  }

  std::tie(success, resource_context_) =
      CreateContext(environment_->Display(), config_, context_);
  if (!success) {
    FML_LOG(ERROR) << "Could not create an EGL resource context";
    LogLastEGLError();
    return;
  }

  // All done!
  valid_ = true;
}

OhosContextGLSkia::~OhosContextGLSkia() {
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());
  sk_sp<GrDirectContext> main_context = GetMainSkiaContext();
  SetMainSkiaContext(nullptr);
  fml::AutoResetWaitableEvent latch;
  // This context needs to be deallocated from the raster thread in order to
  // keep a coherent usage of egl from a single thread.
  fml::TaskRunner::RunNowOrPostTask(task_runners_.GetRasterTaskRunner(), [&] {
    if (main_context) {
      std::unique_ptr<OhosEGLSurface> pbuffer_surface = CreatePbufferSurface();
      auto status = pbuffer_surface->MakeCurrent();
      if (status != OhosEGLSurfaceMakeCurrentStatus::kFailure) {
        main_context->releaseResourcesAndAbandonContext();
        main_context.reset();
        ClearCurrent();
      }
    }
    latch.Signal();
  });
  latch.Wait();

  if (!TeardownContext(environment_->Display(), context_)) {
    FML_LOG(ERROR)
        << "Could not tear down the EGL context. Possible resource leak.";
    LogLastEGLError();
  }

  if (!TeardownContext(environment_->Display(), resource_context_)) {
    FML_LOG(ERROR) << "Could not tear down the EGL resource context. Possible "
                      "resource leak.";
    LogLastEGLError();
  }
}

std::unique_ptr<OhosEGLSurface> OhosContextGLSkia::CreateOnscreenSurface(
    const fml::RefPtr<OHOSNativeWindow>& window) const {
  if (window->IsFakeWindow()) {
    return CreatePbufferSurface();
  } else {
    EGLDisplay display = environment_->Display();

    const EGLint attribs[] = {EGL_NONE};

    EGLSurface surface = eglCreateWindowSurface(
        display, config_,
        reinterpret_cast<EGLNativeWindowType>(window->handle()), attribs);
    return std::make_unique<OhosEGLSurface>(surface, display, context_);
  }
}

std::unique_ptr<OhosEGLSurface> OhosContextGLSkia::CreateOffscreenSurface()
    const {
  // We only ever create pbuffer surfaces for background resource loading
  // contexts. We never bind the pbuffer to anything.
  FML_LOG(INFO) << "CreateOffscreenSurface 1";
  if (environment_) {
    FML_LOG(INFO) << "CreateOffscreenSurface environment_   ";
  } else {
    FML_LOG(INFO) << "CreateOffscreenSurface environment_  nullptr";
  }

  EGLDisplay display = environment_->Display();

  const EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
  FML_LOG(INFO) << "CreateOffscreenSurface 2";
  EGLSurface surface = eglCreatePbufferSurface(display, config_, attribs);
  FML_LOG(INFO) << "CreateOffscreenSurface 3";
  return std::make_unique<OhosEGLSurface>(surface, display, resource_context_);
}

std::unique_ptr<OhosEGLSurface> OhosContextGLSkia::CreatePbufferSurface()
    const {
  EGLDisplay display = environment_->Display();

  const EGLint attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};

  EGLSurface surface = eglCreatePbufferSurface(display, config_, attribs);
  return std::make_unique<OhosEGLSurface>(surface, display, context_);
}

fml::RefPtr<OhosEnvironmentGL> OhosContextGLSkia::Environment() const {
  return environment_;
}

bool OhosContextGLSkia::IsValid() const {
  return valid_;
}

bool OhosContextGLSkia::ClearCurrent() const {
  if (eglGetCurrentContext() != context_) {
    return true;
  }
  if (eglMakeCurrent(environment_->Display(), EGL_NO_SURFACE, EGL_NO_SURFACE,
                     EGL_NO_CONTEXT) != EGL_TRUE) {
    FML_LOG(ERROR) << "Could not clear the current context";
    LogLastEGLError();
    return false;
  }
  return true;
}

EGLContext OhosContextGLSkia::CreateNewContext() const {
  bool success;
  EGLContext context;
  std::tie(success, context) =
      CreateContext(environment_->Display(), config_, EGL_NO_CONTEXT);
  return success ? context : EGL_NO_CONTEXT;
}

}  // namespace flutter
