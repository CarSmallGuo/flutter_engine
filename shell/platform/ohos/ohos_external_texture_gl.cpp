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

#include "ohos_external_texture_gl.h"

#include <utility>

#include "third_party/skia/include/core/SkAlphaType.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/core/SkColorType.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

#include <GLES2/gl2ext.h>
#include <sys/mman.h>

namespace flutter {

OHOSExternalTextureGL::OHOSExternalTextureGL(int64_t id)
  : Texture(id),transform(SkMatrix::I()) {
    nativeImage = nullptr;
}

OHOSExternalTextureGL::~OHOSExternalTextureGL() {
  if (state_ == AttachmentState::attached) {
    glDeleteTextures(1, &texture_name_);
  }
}

void OHOSExternalTextureGL::Paint(PaintContext& context,
                                  const SkRect& bounds,
                                  bool freeze,
                                  const SkSamplingOptions& sampling) {
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::Paint";
  if (state_ == AttachmentState::detached) {
    return;
  }
  if (state_ == AttachmentState::uninitialized) {
    glGenTextures(1, &texture_name_);
    Attach(static_cast<int>(texture_name_));
    state_ = AttachmentState::attached;
  }
  if (!freeze && new_frame_ready_) {
    Update();
    new_frame_ready_ = false;
  }
  GrGLTextureInfo textureInfo = {GL_TEXTURE_EXTERNAL_OES, texture_name_,
                                 GL_RGBA8_OES};
  GrBackendTexture backendTexture(1, 1, GrMipMapped::kNo, textureInfo);
  sk_sp<SkImage> image = SkImage::MakeFromTexture(
      context.gr_context, backendTexture, kTopLeft_GrSurfaceOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
  if (image) {
    SkAutoCanvasRestore autoRestore(context.canvas, true);

    // The incoming texture is vertically flipped, so we flip it
    // back. OpenGL's coordinate system has Positive Y equivalent to up, while
    // Skia's coordinate system has Negative Y equvalent to up.
    context.canvas->translate(bounds.x(), bounds.y() + bounds.height());
    context.canvas->scale(bounds.width(), -bounds.height());

    if (!transform.isIdentity()) {
      sk_sp<SkShader> shader = image->makeShader(
          SkTileMode::kRepeat, SkTileMode::kRepeat, sampling, transform);

      SkPaint paintWithShader;
      if (context.sk_paint) {
        paintWithShader = *context.sk_paint;
      }
      paintWithShader.setShader(shader);
      context.canvas->drawRect(SkRect::MakeWH(1, 1), paintWithShader);
    } else {
      context.canvas->drawImage(image, 0, 0, sampling, context.sk_paint);
    }
  }
}

void OHOSExternalTextureGL::OnGrContextCreated() {
  state_ = AttachmentState::uninitialized;
}

void OHOSExternalTextureGL::OnGrContextDestroyed() {
  if (state_ == AttachmentState::attached) {
    Detach();
    glDeleteTextures(1, &texture_name_);
    OH_NativeImage_Destroy(&nativeImage);
  }
  state_ = AttachmentState::detached;
}

void OHOSExternalTextureGL::MarkNewFrameAvailable() {
  new_frame_ready_ = true;
}

void OHOSExternalTextureGL::OnTextureUnregistered() {
  // do nothing
}

void OHOSExternalTextureGL::Attach(int textureName) {
  OH_NativeImage_AttachContext(nativeImage, textureName);
}

void OHOSExternalTextureGL::Update() {
  OH_NativeImage_UpdateSurfaceImage(nativeImage);
  UpdateTransform();
}

void OHOSExternalTextureGL::Detach() {
  OH_NativeImage_DetachContext(nativeImage);
}

void OHOSExternalTextureGL::UpdateTransform() {
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::UpdateTransform";
  float m[16] = { 0.0f };
  OH_NativeImage_GetTransformMatrix(nativeImage, m);
  //transform ohos 4x4 matrix to skia 3x3 matrix
  SkScalar matrix3[] = {
    m[0], m[4], m[12],  //
    m[1], m[5], m[13],  //
    m[3], m[7], m[15],  //
  };
  transform.set9(matrix3);
  SkMatrix inverted;
  if (!transform.invert(&inverted)) {
    FML_LOG(FATAL) << "Invalid SurfaceTexture transformation matrix";
  }
  transform = inverted;
}

void OHOSExternalTextureGL::DispatchImage(ImageNative* image)
{
  if(image == nullptr) {
    return;
  }

  if (state_ == AttachmentState::detached) {
    return;
  }
  if(nativeImage == nullptr) {
    glGenTextures(1, &texture_name_);
    nativeImage = OH_NativeImage_Create(texture_name_, GL_TEXTURE_2D);
  }
  if (state_ == AttachmentState::uninitialized) {
    // glGenTextures(1, &texture_name_);
    Attach(static_cast<int>(texture_name_));
    state_ = AttachmentState::attached;
  }
  OhosImageComponent componentNative;
  if (IMAGE_RESULT_SUCCESS !=
        OH_Image_GetComponent(image, OHOS_IMAGE_COMPONENT_FORMAT_JPEG, &componentNative)) {
        FML_DLOG(FATAL)<<"get native component failed";
        return;
    }
  OHNativeWindow *nativeWindow = OH_NativeImage_AcquireNativeWindow(nativeImage);
  int code = SET_BUFFER_GEOMETRY;
  uint32_t width = componentNative.rowStride;
  uint32_t height = componentNative.size / componentNative.rowStride;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, width, height);
  //get NativeWindowBuffer from NativeWindow
  OHNativeWindowBuffer *buffer = nullptr;
  int fenceFd;
  OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &buffer, &fenceFd);
  BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(buffer);
  //get virAddr of bufferHandl by mmap sys interface
  void *mappedAddr = mmap(handle->virAddr, handle->size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    FML_DLOG(FATAL)<<"mmap failed";
    return;
  }

  uint8_t *value = componentNative.byteBuffer;
  uint32_t *pixel = static_cast<uint32_t *>(mappedAddr);
  for (uint32_t x = 0; x < width; x++) {
    for (uint32_t y = 0; y < height; y++) {
      *pixel++ = *value++;
    }
  }

  //munmap after use
  int result = munmap(mappedAddr, handle->size);
  if (result == -1) {
    FML_DLOG(FATAL)<<"munmap failed";
  }

}

void OHOSExternalTextureGL::DispatchPixelMap(NativePixelMap* pixelMap)
{
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::DispatchPixelMap enter";
  if(pixelMap == nullptr) {
    FML_DLOG(FATAL)<<"pixelMap in null";
    return;
  }
  if (state_ == AttachmentState::detached) {
    FML_DLOG(FATAL)<<"DispatchPixelMap AttachmentState err";
    return;
  }
  if(nativeImage == nullptr) {
    glGenTextures(1, &texture_name_);
    nativeImage = OH_NativeImage_Create(texture_name_, GL_TEXTURE_2D);
    FML_DLOG(INFO)<<"texture_name_:"<<static_cast<int>(texture_name_);
    FML_DLOG(INFO)<<"nativeImage addr:"<<nativeImage;
  }
  if (state_ == AttachmentState::uninitialized) {
    // glGenTextures(1, &texture_name_);
    Attach(static_cast<int>(texture_name_));
    state_ = AttachmentState::attached;
  }
  FML_DLOG(INFO)<<"Attach after";
  OhosPixelMapInfos pixelMapInfo;
  OH_PixelMap_GetImageInfo(pixelMap, &pixelMapInfo);
  OHNativeWindow *nativeWindow = OH_NativeImage_AcquireNativeWindow(nativeImage);
  int code = SET_BUFFER_GEOMETRY;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, code, pixelMapInfo.width, pixelMapInfo.height);
  OHNativeWindowBuffer *buffer = nullptr;
  int fenceFd;
  OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow, &buffer, &fenceFd);
  BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(buffer);
  //get virAddr of bufferHandl by mmap sys interface
  void *mappedAddr = mmap(handle->virAddr, handle->size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    FML_DLOG(FATAL)<<"mmap failed";
    return;
  }
  void *pixelAddr = nullptr;
  OH_PixelMap_AccessPixels(pixelMap, &pixelAddr);
  uint8_t *value = static_cast<uint8_t*>(pixelAddr);
  uint32_t *pixel = static_cast<uint32_t *>(mappedAddr);
  for (uint32_t x = 0; x < pixelMapInfo.width; x++) {
    for (uint32_t y = 0; y < pixelMapInfo.height; y++) {
      *pixel++ = *value++;
    }
  }
  OH_PixelMap_UnAccessPixels(pixelMap);
  //munmap after use
  int result = munmap(mappedAddr, handle->size);
  if (result == -1) {
    FML_DLOG(FATAL)<<"munmap failed";
  }
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::DispatchPixelMap finish";
  
}

}  // namespace flutter