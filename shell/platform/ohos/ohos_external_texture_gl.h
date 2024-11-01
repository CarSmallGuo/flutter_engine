// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

// maybe now unused
namespace flutter {

class OHOSExternalTextureGL : public flutter::Texture {
 public:
  explicit OHOSExternalTextureGL(int64_t id, const std::shared_ptr<OHOSSurface>& ohos_surface);

  ~OHOSExternalTextureGL() override;

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

  bool new_frame_ready_ = false;

  bool texture_update_ = false;

  GLuint texture_name_ = 0;

  GLuint backGroundTextureName_ = 0;

  std::shared_ptr<OHOSSurface> ohos_surface_;

  SkMatrix transform;

  OHNativeWindow *nativeWindow_;

  OHNativeWindow *backGroundNativeWindow_;

  OHNativeWindowBuffer *buffer_;

  OHNativeWindowBuffer *backGroundBuffer_;

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
};
}  // namespace flutter
#endif