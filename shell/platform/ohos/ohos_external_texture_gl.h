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

#ifndef OHOS_EXTERNAL_TEXTURE_GL_H
#define OHOS_EXTERNAL_TEXTURE_GL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <multimedia/image_framework/image/pixelmap_native.h>
#include <multimedia/image_framework/image_mdk.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_buffer/native_buffer.h>
#include <native_window/external_window.h>
#include <native_image/native_image.h>

#include "flutter/common/graphics/texture.h"
#include "flutter/shell/platform/ohos/napi/platform_view_ohos_napi.h"
#include "flutter/shell/platform/ohos/ohos_surface_gl_skia.h"
#include "flutter/shell/platform/ohos/surface/ohos_surface.h"
#include "flutter/shell/common/platform_view.h"

// maybe now unused
namespace flutter {

// Represents an external texture for OHOS platform, using std::weak_ptr to avoid circular reference.
class OHOSExternalTextureGL : public flutter::Texture, public std::enable_shared_from_this<OHOSExternalTextureGL> {
 public:
  explicit OHOSExternalTextureGL(int64_t id, const std::shared_ptr<OHOSSurface>& ohos_surface);
  explicit OHOSExternalTextureGL(int64_t id, const std::shared_ptr<OHOSSurface>& ohos_surface,
    PlatformView::Delegate& delegate, const TaskRunners& task_runners);

  ~OHOSExternalTextureGL() override;

  PlatformView::Delegate& delegate_;

  const TaskRunners& task_runners_;

  OH_NativeImage *nativeImage_;

  OH_NativeImage *backGroundNativeImage_;

  bool first_update_ = false;

  void Paint(PaintContext& context,
             const SkRect& bounds,
             bool freeze,
             const SkSamplingOptions& sampling) override;

  void OnGrContextCreated() override;

  void OnGrContextDestroyed() override;

  void MarkNewFrameAvailable() override;

  void OnTextureUnregistered() override;

  void DispatchImage(ImageNative* image);

  void setBackground(int32_t width, int32_t height);

  GrGLTextureInfo GetGrGLTextureInfo();

  void setTextureBufferSize(int32_t width, int32_t height);

  void DispatchPixelMap(NativePixelMap* pixelMap);

  void DispatchBackGroundPixelMap(NativePixelMap* pixelMap);

 private:
  void Attach();

  void Update();

  void Detach();

  void UpdateTransform(OH_NativeImage *image);

  EGLDisplay GetPlatformEglDisplay(EGLenum platform, void *native_display, const EGLint *attrib_list);

  bool CheckEglExtension(const char *extensions, const char *extension);

  void HandlePixelMapBuffer(NativePixelMap* pixelMap, OHNativeWindowBuffer* buffer);

  void ProducePixelMapToNativeImage();

  void ProduceColorToBackGroundImage(int32_t width, int32_t height);

  void ProducePixelMapToBackGroundImage();

  enum class AttachmentState { uninitialized, attached, detached };

  AttachmentState state_;

  GLuint texture_name_ = 0;

  GLuint backGroundTextureName_ = 0;

  std::shared_ptr<OHOSSurface> ohos_surface_;

  SkMatrix transform;

  OHNativeWindow *nativeWindow_;

  OHNativeWindow *backGroundNativeWindow_;

  NativePixelMap* backGroundPixelMap_;

  NativePixelMap* pixelMap_;

  ImageNative* lastImage_;

  bool isEmulator_;

  OhosPixelMapInfos pixelMapInfo;

  int fenceFd = -1;

  int backGroundFenceFd = -1;

  EGLContext eglContext_;
  EGLDisplay eglDisplay_;

  FML_DISALLOW_COPY_AND_ASSIGN(OHOSExternalTextureGL);

  void* display_;
  void* draw_surface_;
  void* read_surface_;
  void* context_;

  bool IsContextCurrent();
};

}  // namespace flutter
#endif