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

#include <GLES2/gl2ext.h>
#include <sys/mman.h>
#include <utility>

#include "ohos_main.h"
#include "third_party/skia/include/core/SkAlphaType.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/core/SkColorType.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/gpu/GrBackendSurface.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

#define EGL_PLATFORM_OHOS_KHR             0x34E0

const int PIXEL_SIZE = 4; // 像素点占用4个字节

namespace flutter {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";

OHOSExternalTextureGL::OHOSExternalTextureGL(int64_t id, const std::shared_ptr<OHOSSurface>& ohos_surface)
  : Texture(id), ohos_surface_(std::move(ohos_surface)), transform(SkMatrix::I())
{
    state_ = AttachmentState::uninitialized;
    nativeImage_ = nullptr;
    backGroundNativeImage_ = nullptr;
    nativeWindow_ = nullptr;
    backGroundNativeWindow_ = nullptr;
    eglContext_ =  EGL_NO_CONTEXT;
    eglDisplay_ = EGL_NO_DISPLAY;
    buffer_ = nullptr;
    backGroundBuffer_ = nullptr;
    pixelMap_ = nullptr;
    backGroundPixelMap_ = nullptr;
    lastImage_ = nullptr;
    isEmulator_ = OhosMain::IsEmulator();
}

OHOSExternalTextureGL::~OHOSExternalTextureGL()
{
  FML_DLOG(INFO) << "~OHOSExternalTextureGL, texture_name_=" << texture_name_ << ", Id()=" << Id();
  if (state_ == AttachmentState::attached) {
    if (texture_name_ != 0) {
      glDeleteTextures(1, &texture_name_);
      texture_name_ = 0;
    }
    if (backGroundTextureName_ != 0) {
      glDeleteTextures(1, &backGroundTextureName_);
      backGroundTextureName_ = 0;
    }
  }
  state_ = AttachmentState::uninitialized;
  nativeImage_ = nullptr;
  backGroundNativeImage_ = nullptr;
  nativeWindow_ = nullptr;
  backGroundNativeWindow_ = nullptr;
  eglContext_ =  EGL_NO_CONTEXT;
  eglDisplay_ = EGL_NO_DISPLAY;
  buffer_ = nullptr;
  backGroundBuffer_ = nullptr;
  pixelMap_ = nullptr;
  backGroundPixelMap_ = nullptr;
  lastImage_ = nullptr;
}

void OHOSExternalTextureGL::Attach()
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::Attach, Id()=" << Id();
  if (state_ != AttachmentState::uninitialized) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::Attach, the current status is not uninitialized";
    return;
  }
  OHOSSurface* ohos_surface_ptr = ohos_surface_.get();
  OhosSurfaceGLSkia* ohosSurfaceGLSkia_ = (OhosSurfaceGLSkia*)ohos_surface_ptr;
  auto result = ohosSurfaceGLSkia_->GLContextMakeCurrent();
  if (result->GetResult()) {
    FML_DLOG(INFO)<<"ResourceContextMakeCurrent successed";
    glGenTextures(1, &texture_name_);
    FML_DLOG(INFO) << "OHOSExternalTextureGL::Paint, glGenTextures texture_name_=" << texture_name_ << ", Id()=" << Id();
    if (nativeImage_ == nullptr) {
      nativeImage_ = OH_NativeImage_Create(texture_name_, GL_TEXTURE_EXTERNAL_OES);
      if (nativeImage_ == nullptr) {
        FML_LOG(ERROR) << "Error with OH_NativeImage_Create";
        return;
      }
      nativeWindow_ = OH_NativeImage_AcquireNativeWindow(nativeImage_);
      if (nativeWindow_ == nullptr) {
        FML_LOG(ERROR) << "Error with OH_NativeImage_AcquireNativeWindow";
        return;
      }
    }

    int32_t ret = OH_NativeImage_AttachContext(nativeImage_, texture_name_);
    if (ret != 0) {
      FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeImage_AttachContext err code:" << ret;
    }
    state_ = AttachmentState::attached;
  } else {
    FML_LOG(ERROR) << "ResourceContextMakeCurrent failed";
  }
}

void OHOSExternalTextureGL::Paint(PaintContext& context,
                                  const SkRect& bounds,
                                  bool freeze,
                                  const SkSamplingOptions& sampling)
{
  if (state_ == AttachmentState::detached) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::Paint, the current status is detached";
    return;
  }
  if (state_ == AttachmentState::uninitialized) {
    Attach();
    if (!freeze && new_frame_ready_ && pixelMap_ != nullptr) {
      ProducePixelMapToNativeImage();
      Update();
    }
    new_frame_ready_ = false;
  }

  GrGLTextureInfo textureInfo;

  if (!freeze && !first_update_ && !isEmulator_ && !new_frame_ready_ && pixelMap_ == nullptr) {
    setBackground(bounds.width(), bounds.height());
    textureInfo = {GL_TEXTURE_EXTERNAL_OES, backGroundTextureName_, GL_RGBA8_OES};
  } else {
    textureInfo = {GL_TEXTURE_EXTERNAL_OES, texture_name_, GL_RGBA8_OES};
  }

  GrBackendTexture backendTexture(1, 1, GrMipMapped::kNo, textureInfo);
  sk_sp<SkImage> image = SkImage::MakeFromTexture(
      context.gr_context, backendTexture, kTopLeft_GrSurfaceOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
  if (image) {
    SkAutoCanvasRestore autoRestore(context.canvas, true);

    // The incoming texture is vertically flipped, so we flip it
    // back. OpenGL's coordinate system has Positive Y equivalent to up, while
    // Skia's coordinate system has Negative Y equvalent to up.
    // 模拟器和真机在外接纹理功能的表现不一致，需要进行适配
    if (isEmulator_) {
      context.canvas->translate(bounds.x(), bounds.y());
      context.canvas->scale(bounds.width(), bounds.height());
    } else {
      context.canvas->translate(bounds.x(), bounds.y() + bounds.height());
      context.canvas->scale(bounds.width(), -bounds.height());
    }

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

void OHOSExternalTextureGL::OnGrContextCreated()
{
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::OnGrContextCreated";
  state_ = AttachmentState::uninitialized;
}

void OHOSExternalTextureGL::OnGrContextDestroyed()
{
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::OnGrContextDestroyed";
  if (state_ == AttachmentState::attached) {
    Detach();
    glDeleteTextures(1, &texture_name_);
  }
  state_ = AttachmentState::detached;
  if (backGroundTextureName_ != 0) {
    glDeleteTextures(1, &backGroundTextureName_);
  }
}

void OHOSExternalTextureGL::MarkNewFrameAvailable()
{
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::MarkNewFrameAvailable";
  new_frame_ready_ = true;
  if (pixelMap_ == nullptr) {
    Update();
  } else {
    FML_DLOG(INFO) << "pixelMap_ is nullptr, texture_name_=" << texture_name_;
  }
}

void OHOSExternalTextureGL::OnTextureUnregistered()
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::OnTextureUnregistered, texture_name_=" << texture_name_
    << ", Id()=" << Id()
    << ", nativeImage_=" << nativeImage_
    << ", backGroundNativeImage_=" << backGroundNativeImage_;
  first_update_ = false;
  if (nativeImage_ != nullptr) {
    OH_NativeImage_UnsetOnFrameAvailableListener(nativeImage_);
    OH_NativeImage_Destroy(&nativeImage_);
    nativeImage_ = nullptr;
  }
  if (backGroundNativeImage_ != nullptr) {
    OH_NativeImage_Destroy(&backGroundNativeImage_);
    backGroundNativeImage_ = nullptr;
  }
}

void OHOSExternalTextureGL::Update()
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::Update, texture_name_=" << texture_name_;
  if (nativeImage_ == nullptr) {
    FML_LOG(ERROR) << "Update, nativeImage_ is nullptr, texture_name_=" << texture_name_;
    return;
  }
  int32_t ret = OH_NativeImage_UpdateSurfaceImage(nativeImage_);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeImage_UpdateSurfaceImage err code:" << ret;
    return;
  }
  first_update_ = true;
  UpdateTransform(nativeImage_);
}

void OHOSExternalTextureGL::Detach()
{
  FML_LOG(INFO) << "OHOSExternalTextureGL::Detach, texture_name_=" << texture_name_;
  if (state_ != AttachmentState::attached) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::Detach, the current status is not attached";
    return;
  }
  OH_NativeImage_DetachContext(nativeImage_);
  OH_NativeImage_DetachContext(backGroundNativeImage_);
  OH_NativeWindow_DestroyNativeWindow(nativeWindow_);
  OH_NativeWindow_DestroyNativeWindow(backGroundNativeWindow_);
  nativeWindow_ = nullptr;
  backGroundNativeWindow_ = nullptr;
}

void OHOSExternalTextureGL::UpdateTransform(OH_NativeImage *image)
{
  float m[16] = { 0.0f };
  int32_t ret = OH_NativeImage_GetTransformMatrixV2(image, m);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeImage_GetTransformMatrixV2 err code:" << ret;
  }
  // transform ohos 4x4 matrix to skia 3x3 matrix
  SkScalar matrix3[] = {
    m[0], m[4], m[12],  //
    m[1], m[5], m[13],  //
    m[3], m[7], m[15],  //
  };
  transform.set9(matrix3);
  SkMatrix inverted;
  if (!transform.invert(&inverted)) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL Invalid SurfaceTexture transformation matrix";
  }
  transform = inverted;
}

void OHOSExternalTextureGL::DispatchImage(ImageNative* image)
{
  lastImage_ = image;
}

void OHOSExternalTextureGL::setBackground(int32_t width, int32_t height)
{
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::setBackground";
  if (backGroundNativeImage_ != nullptr) {
    return;
  }

  OHOSSurface* ohos_surface_ptr = ohos_surface_.get();
  OhosSurfaceGLSkia* ohosSurfaceGLSkia_ = (OhosSurfaceGLSkia*)ohos_surface_ptr;
  auto result = ohosSurfaceGLSkia_->GLContextMakeCurrent();
  if (result->GetResult()) {
    FML_DLOG(INFO)<<"ResourceContextMakeCurrent successed";
    glGenTextures(1, &backGroundTextureName_);
    FML_DLOG(INFO) << "OHOSExternalTextureGL::setBackground, glGenTextures backGroundTextureName_=" << backGroundTextureName_;
    if (backGroundNativeImage_ == nullptr) {
      backGroundNativeImage_ = OH_NativeImage_Create(backGroundTextureName_, GL_TEXTURE_EXTERNAL_OES);
      if (backGroundNativeImage_ == nullptr) {
        FML_LOG(ERROR) << "Error with OH_NativeImage_Create";
        return;
      }
      backGroundNativeWindow_ = OH_NativeImage_AcquireNativeWindow(backGroundNativeImage_);
      if (backGroundNativeWindow_ == nullptr) {
        FML_LOG(ERROR) << "Error with OH_NativeImage_AcquireNativeWindow";
        return;
      }
    }
    int32_t ret = OH_NativeImage_AttachContext(backGroundNativeImage_, backGroundTextureName_);
    if (ret != 0) {
      FML_LOG(ERROR) << "OHOSExternalTextureGL::setBackground OH_NativeImage_AttachContext err code:" << ret;
    }
  } else {
    FML_LOG(ERROR) << "ResourceContextMakeCurrent failed";
  }
  if (backGroundPixelMap_ != nullptr) {
    ProducePixelMapToBackGroundImage();
  } else {
    ProduceColorToBackGroundImage(width, height);
  }
}

void OHOSExternalTextureGL::setTextureBufferSize(int32_t width, int32_t height)
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::SetTextureBufferSize";
  int code = SET_BUFFER_GEOMETRY;
  int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, code, width, height);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::SetTextureBufferSize OH_NativeWindow_NativeWindowHandleOpt err:" << ret;
    return;
  }
}

void OHOSExternalTextureGL::ProduceColorToBackGroundImage(int32_t width, int32_t height)
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::ProduceColorToBackGroundImage";
  int code = SET_BUFFER_GEOMETRY;
  int32_t ret = OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, code, width, height);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::setBackground OH_NativeWindow_NativeWindowHandleOpt err:" << ret;
    return;
  }

  ret = OH_NativeWindow_NativeWindowRequestBuffer(backGroundNativeWindow_, &backGroundBuffer_, &backGroundFenceFd);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::setBackground OH_NativeWindow_NativeWindowRequestBuffer err:" << ret;
    return;
  }

  BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(backGroundBuffer_);
  void *mappedAddr = mmap(handle->virAddr, handle->size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL::setBackground mmap failed";
    return;
  }

  uint32_t* destAddr = static_cast<uint32_t *>(mappedAddr);
  uint32_t value = 0xFFFFFFFF;

  for (int32_t x = 0; x < handle->width; x++) {
    for (int32_t y = 0; y < handle->height; y++) {
      *destAddr++ = value;
    }
  }

    // munmap after use
  ret = munmap(mappedAddr, handle->size);
  if (ret == -1) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL in setBackground munmap failed";
    return;
  }

  Region region{nullptr, 0};
  ret = OH_NativeWindow_NativeWindowFlushBuffer(backGroundNativeWindow_, backGroundBuffer_, backGroundFenceFd, region);
  if (ret != 0) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL::setBackground OH_NativeWindow_NativeWindowFlushBuffer err:"<< ret;
  }
  ret = OH_NativeImage_UpdateSurfaceImage(backGroundNativeImage_);
  if (ret != 0) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL::setBackground OH_NativeImage_UpdateSurfaceImage err code:"<< ret;
    return;
  }
}

void OHOSExternalTextureGL::ProducePixelMapToBackGroundImage()
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage";
  if (backGroundPixelMap_ == nullptr) {
    FML_LOG(ERROR) << "backGroundPixelMap_ is nullptr";
    return;
  }
  int32_t ret = -1;
  ret = OH_PixelMap_GetImageInfo(backGroundPixelMap_, &pixelMapInfo);
  if (ret != 0) {
    FML_LOG(ERROR)
        << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage "
           "OH_PixelMap_GetImageInfo err:"
        << ret;
    return;
  }
  int code = SET_BUFFER_GEOMETRY;
  ret = OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, code, pixelMapInfo.width, pixelMapInfo.height);
  if (ret != 0) {
    FML_LOG(ERROR)
        << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage "
           "OH_NativeWindow_NativeWindowHandleOpt err:"
        << ret;
    return;
  }
  uint64_t usage = 0;
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, GET_USAGE, &usage);
  usage |= NATIVEBUFFER_USAGE_CPU_READ;
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, SET_USAGE, usage);

  if (backGroundBuffer_ != nullptr) {
    OH_NativeWindow_NativeWindowAbortBuffer(backGroundNativeWindow_, backGroundBuffer_);
    backGroundBuffer_ = nullptr;
  }
  ret = OH_NativeWindow_NativeWindowRequestBuffer(backGroundNativeWindow_, &backGroundBuffer_, &backGroundFenceFd);
  if (ret != 0) {
    FML_LOG(ERROR)
        << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage "
           "OH_NativeWindow_NativeWindowRequestBuffer err:"
        << ret;
    return;
  }
  HandlePixelMapBuffer(backGroundPixelMap_, backGroundBuffer_);
  Region region{nullptr, 0};
  ret = OH_NativeWindow_NativeWindowFlushBuffer(backGroundNativeWindow_, backGroundBuffer_, backGroundFenceFd, region);
  if (ret != 0) {
    FML_LOG(ERROR)
        << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage "
           "OH_NativeWindow_NativeWindowFlushBuffer err:"
        << ret;
  }
  ret = OH_NativeImage_UpdateSurfaceImage(backGroundNativeImage_);
  if (ret != 0) {
    FML_LOG(ERROR)
        << "OHOSExternalTextureGL::ProducePixelMapToBackGroundImage "
           "OH_NativeImage_UpdateSurfaceImage err code:"
        << ret;
    return;
  }
  UpdateTransform(backGroundNativeImage_);
}

void OHOSExternalTextureGL::HandlePixelMapBuffer(NativePixelMap* pixelMap, OHNativeWindowBuffer* buffer)
{
  BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(buffer);
  if (handle == nullptr) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::HandlePixelMapBuffer, handle is nullptr.";
    return;
  }
  // get virAddr of bufferHandl by mmap sys interface
  uint32_t stride = handle->stride;
  FML_DLOG(INFO) << "OHOSExternalTextureGL stride:" << stride;
  void *mappedAddr = mmap(handle->virAddr, handle->size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL mmap failed";
    return;
  }

  void *pixelAddr = nullptr;
  int64_t ret = OH_PixelMap_AccessPixels(pixelMap, &pixelAddr);
  if (ret != IMAGE_RESULT_SUCCESS) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL OH_PixelMap_AccessPixels err:"<< ret;
    return;
  }

  uint32_t *pixel = static_cast<uint32_t *>(pixelAddr);
  uint32_t *destAddr = static_cast<uint32_t *>(mappedAddr);

  FML_DLOG(INFO) << "OHOSExternalTextureGL pixelMapInfo w:" << pixelMapInfo.width
    << " h:" << pixelMapInfo.height;
  FML_DLOG(INFO) << "OHOSExternalTextureGL pixelMapInfo rowSize:" << pixelMapInfo.rowSize
    << " format:" << pixelMapInfo.pixelFormat;

  // 复制图片纹理数据到内存中，需要处理DMA内存补齐相关的逻辑
  if (pixelMapInfo.width * PIXEL_SIZE != pixelMapInfo.rowSize) {
    // 直接复制整块内存
    memcpy(destAddr, pixel, pixelMapInfo.height * pixelMapInfo.rowSize);
  } else {
    // 需要处理DMA内存补齐相关的逻辑
    for (uint32_t i = 0; i < pixelMapInfo.height; i++) {
      memcpy(destAddr, pixel, pixelMapInfo.rowSize);
      destAddr += stride / PIXEL_SIZE;
      pixel += pixelMapInfo.width;
    }
  }
  OH_PixelMap_UnAccessPixels(pixelMap);
  // munmap after use
  ret = munmap(mappedAddr, handle->size);
  if (ret == -1) {
    FML_LOG(ERROR)<<"OHOSExternalTextureGL munmap failed";
    return;
  }
}

void OHOSExternalTextureGL::ProducePixelMapToNativeImage()
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL::ProducePixelMapToNativeImage, pixelMap_=" << pixelMap_;
  if (state_ == AttachmentState::detached) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL AttachmentState err";
    return;
  }
  if (pixelMap_ == nullptr) {
    FML_LOG(ERROR) << "pixelMap_ is nullptr";
    return;
  }
  int32_t ret = -1;
  ret = OH_PixelMap_GetImageInfo(pixelMap_, &pixelMapInfo);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_PixelMap_GetImageInfo err:" << ret;
    return;
  }

  int code = SET_BUFFER_GEOMETRY;
  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, code, pixelMapInfo.width, pixelMapInfo.height);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeWindow_NativeWindowHandleOpt err:" << ret;
    return;
  }

  uint64_t usage = 0;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, GET_USAGE, &usage);
  usage |= NATIVEBUFFER_USAGE_CPU_READ;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, SET_USAGE, usage);

  if (buffer_ != nullptr) {
    OH_NativeWindow_NativeWindowAbortBuffer(nativeWindow_, buffer_);
    buffer_ = nullptr;
  }
  ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeWindow_NativeWindowRequestBuffer err:" << ret;
    return;
  }
  HandlePixelMapBuffer(pixelMap_, buffer_);
  Region region{nullptr, 0};
  ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd, region);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL OH_NativeWindow_NativeWindowFlushBuffer err:" << ret;
  }
}

EGLDisplay OHOSExternalTextureGL::GetPlatformEglDisplay(EGLenum platform, void *native_display,
    const EGLint *attrib_list)
{
  GetPlatformDisplayExt eglGetPlatformDisplayExt = NULL;

  if (!eglGetPlatformDisplayExt) {
    const char* extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (extensions &&
        (CheckEglExtension(extensions, EGL_EXT_PLATFORM_WAYLAND) ||
         CheckEglExtension(extensions, EGL_KHR_PLATFORM_WAYLAND))) {
      eglGetPlatformDisplayExt = (GetPlatformDisplayExt)eglGetProcAddress(EGL_GET_PLATFORM_DISPLAY_EXT);
    }
  }

  if (eglGetPlatformDisplayExt) {
    return eglGetPlatformDisplayExt(platform, native_display, attrib_list);
  }

  return eglGetDisplay((EGLNativeDisplayType)native_display);
}

bool OHOSExternalTextureGL::CheckEglExtension(const char *extensions, const char *extension)
{
  size_t extlen = strlen(extension);
  const char *end = extensions + strlen(extensions);
  while (extensions < end) {
    size_t n = 0;
    if (*extensions == CHARACTER_WHITESPACE) {
        extensions++;
        continue;
      }
      n = strcspn(extensions, CHARACTER_STRING_WHITESPACE);
      if (n == extlen && strncmp(extension, extensions, n) == 0) {
        return true;
    }
    extensions += n;
  }
  return false;
}

void OHOSExternalTextureGL::DispatchPixelMap(NativePixelMap* pixelMap)
{
  if (pixelMap != nullptr) {
    pixelMap_ = pixelMap;
  }
}

void OHOSExternalTextureGL::DispatchBackGroundPixelMap(NativePixelMap* pixelMap)
{
  if (pixelMap != nullptr) {
    backGroundPixelMap_ = pixelMap;
  }
}

}  // namespace flutter