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
#include "types.h"

#define EGL_PLATFORM_OHOS_KHR             0x34E0

const int PIXEL_SIZE = 4; // 像素点占用4个字节

namespace flutter {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr uint32_t WHITE_COLOR = 0xFFFFFFFF;

const SkScalar DEFAULT_MATRIX[] = {1, 0, 0, 0, -1, 1, 0, 0, 1};
const int UPDATE_FRAME_COUNT = 2;

static int PixelMapToWindowFormat(PIXEL_FORMAT pixel_format)
{
  switch (pixel_format) {
    case PIXEL_FORMAT_RGB_565:
      return NATIVEBUFFER_PIXEL_FMT_RGB_565;
    case PIXEL_FORMAT_RGBA_8888:
      return NATIVEBUFFER_PIXEL_FMT_RGBA_8888;
    case PIXEL_FORMAT_BGRA_8888:
      return NATIVEBUFFER_PIXEL_FMT_BGRA_8888;
    case PIXEL_FORMAT_RGB_888:
      return NATIVEBUFFER_PIXEL_FMT_RGB_888;
    case PIXEL_FORMAT_NV21:
      return NATIVEBUFFER_PIXEL_FMT_YCRCB_420_SP;
    case PIXEL_FORMAT_NV12:
      return NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP;
    case PIXEL_FORMAT_RGBA_1010102:
      return NATIVEBUFFER_PIXEL_FMT_RGBA_1010102;
    case PIXEL_FORMAT_YCBCR_P010:
      return NATIVEBUFFER_PIXEL_FMT_YCBCR_P010;
    case PIXEL_FORMAT_YCRCB_P010:
      return NATIVEBUFFER_PIXEL_FMT_YCRCB_P010;
    case PIXEL_FORMAT_ALPHA_8:
    case PIXEL_FORMAT_RGBA_F16:
    case PIXEL_FORMAT_UNKNOWN:
    default:
      // no support/unknow format: cannot copy
      return 0;
  }
  return 0;
}

static bool IsPixelMapYUVFormat(PIXEL_FORMAT format)
{
  return format == PIXEL_FORMAT_NV21 || format == PIXEL_FORMAT_NV12 ||
         format == PIXEL_FORMAT_YCBCR_P010 || format == PIXEL_FORMAT_YCRCB_P010;
}

OHOSExternalTextureGL::OHOSExternalTextureGL(
    int64_t id,
    const std::shared_ptr<OHOSSurface>& ohos_surface)
    : Texture(id),
      ohos_surface_(std::move(ohos_surface)),
      transform(SkMatrix::I())
{
    state_ = AttachmentState::uninitialized;
    nativeImage_ = nullptr;
    backGroundNativeImage_ = nullptr;
    nativeWindow_ = nullptr;
    backGroundNativeWindow_ = nullptr;
    eglContext_ = EGL_NO_CONTEXT;
    eglDisplay_ = EGL_NO_DISPLAY;
    pixelMap_ = nullptr;
    backGroundPixelMap_ = nullptr;
    lastImage_ = nullptr;
    isEmulator_ = OhosMain::IsEmulator();
}

OHOSExternalTextureGL::~OHOSExternalTextureGL()
{
  FML_DLOG(INFO) << "~OHOSExternalTextureGL, texture_name_=" << texture_name_ << ", Id()=" << Id();
  if (state_ == AttachmentState::attached) {
    Detach();
  }
  state_ = AttachmentState::uninitialized;
  nativeImage_ = nullptr;
  backGroundNativeImage_ = nullptr;
  nativeWindow_ = nullptr;
  backGroundNativeWindow_ = nullptr;
  eglContext_ =  EGL_NO_CONTEXT;
  eglDisplay_ = EGL_NO_DISPLAY;
  pixelMap_ = nullptr;
  backGroundPixelMap_ = nullptr;
  lastImage_ = nullptr;
}

void OHOSExternalTextureGL::Attach()
{
  FML_DLOG(INFO) << "Attach, texture_name_=" << texture_name_
                 << ", Id()=" << Id();
  if (state_ != AttachmentState::uninitialized) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::Attach, the current status is not uninitialized";
    return;
  }
  OHOSSurface* ohos_surface_ptr = ohos_surface_.get();
  OhosSurfaceGLSkia* ohosSurfaceGLSkia_ = (OhosSurfaceGLSkia*)ohos_surface_ptr;
  auto result = ohosSurfaceGLSkia_->GLContextMakeCurrent();
  if (result->GetResult()) {
    FML_DLOG(INFO) << "ResourceContextMakeCurrent successed";
    glGenTextures(1, &texture_name_);
    FML_DLOG(INFO) << "OHOSExternalTextureGL::Attach, glGenTextures texture_name_="
      << texture_name_ << ", Id()=" << Id();
    if (nativeImage_ == nullptr) {
      nativeImage_ = OH_NativeImage_Create(texture_name_, GL_TEXTURE_EXTERNAL_OES);
      if (nativeImage_ == nullptr) {
        FML_LOG(ERROR) << "Error with OH_NativeImage_Create";
        return;
      }
    }

    nativeWindow_ = OH_NativeImage_AcquireNativeWindow(nativeImage_);
    if (nativeWindow_ == nullptr) {
      FML_LOG(ERROR) << "Error with OH_NativeImage_AcquireNativeWindow";
      return;
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
    FML_LOG(ERROR) << "Paint, the current status is detached";
    return;
  }
  if (state_ == AttachmentState::uninitialized) {
    Attach();
    if (pixelMap_ != nullptr) {
      // 外接纹理图片场景
      ProducePixelMapToNativeImage();
      newFrameCount++;
    }
  }
  if (!freeze && newFrameCount > 0) {
    Update();
    new_frame_ready_ = false;
    newFrameCount--;
  }

  GrGLTextureInfo textureInfo;
  if (!freeze && !first_update_ && !isEmulator_ && !new_frame_ready_ && pixelMap_ == nullptr) {
    setBackground(bounds.width(), bounds.height());
    textureInfo = {GL_TEXTURE_EXTERNAL_OES, backGroundTextureName_, GL_RGBA8_OES};
  } else {
    textureInfo = {GL_TEXTURE_EXTERNAL_OES, texture_name_, GL_RGBA8_OES};
  }

  GrBackendTexture backendTexture(1, 1, GrMipMapped::kNo, textureInfo);
  GrSurfaceOrigin grOrigin = isEmulator_ ? kBottomLeft_GrSurfaceOrigin : kTopLeft_GrSurfaceOrigin;
  sk_sp<SkImage> image = SkImage::MakeFromTexture(
      context.gr_context, backendTexture, grOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
  if (image) {
    SkAutoCanvasRestore autoRestore(context.canvas, true);

    // The incoming texture is vertically flipped, so we flip it
    // back. OpenGL's coordinate system has Positive Y equivalent to up, while
    // Skia's coordinate system has Negative Y equvalent to up.
    // 模拟器和真机在外接纹理功能的表现不一致，需要进行适配
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

void OHOSExternalTextureGL::OnGrContextCreated()
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::OnGrContextCreated"
                 << ", texture_name_=" << texture_name_
                 << ", Id()=" << Id();
  state_ = AttachmentState::uninitialized;
}

void OHOSExternalTextureGL::OnGrContextDestroyed()
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::OnGrContextDestroyed"
                 << ", texture_name_=" << texture_name_
                 << ", Id()=" << Id();
  if (state_ == AttachmentState::attached) {
    Detach();
  }
  state_ = AttachmentState::detached;
}

void OHOSExternalTextureGL::MarkNewFrameAvailable()
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::MarkNewFrameAvailable";
  new_frame_ready_ = true;
  newFrameCount = UPDATE_FRAME_COUNT;
}

void OHOSExternalTextureGL::OnTextureUnregistered()
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::OnTextureUnregistered, texture_name_=" << texture_name_
    << ", Id()=" << Id()
    << ", nativeImage_=" << nativeImage_
    << ", backGroundNativeImage_=" << backGroundNativeImage_;
  first_update_ = false;
  if (state_ == AttachmentState::attached) {
    Detach();
    state_ = AttachmentState::detached;
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
  if (nativeImage_ != nullptr) {
    OH_NativeImage_DetachContext(nativeImage_);
    OH_NativeImage_UnsetOnFrameAvailableListener(nativeImage_);
    OH_NativeImage_Destroy(&nativeImage_);
    nativeImage_ = nullptr;
  }
  if (nativeWindow_ != nullptr) {
    OH_NativeWindow_DestroyNativeWindow(nativeWindow_);
    nativeWindow_ = nullptr;
  }

  if (backGroundNativeImage_ != nullptr) {
    OH_NativeImage_DetachContext(backGroundNativeImage_);
    OH_NativeImage_Destroy(&backGroundNativeImage_);
    backGroundNativeImage_ = nullptr;
  }
  if (backGroundNativeWindow_ != nullptr) {
    OH_NativeWindow_DestroyNativeWindow(backGroundNativeWindow_);
    backGroundNativeWindow_ = nullptr;
  }
  glDeleteTextures(1, &texture_name_);
  glDeleteTextures(1, &backGroundTextureName_);
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
    FML_LOG(ERROR) << "OHOSExternalTextureGL UpdateTransform matrix error";

    transform.set9(DEFAULT_MATRIX);
    if (!transform.invert(&inverted)) {
      FML_LOG(ERROR) << "UpdateTransform matrix error again";
    }
  }
  transform = inverted;
}

void OHOSExternalTextureGL::DispatchImage(ImageNative* image)
{
  lastImage_ = image;
}

void OHOSExternalTextureGL::setBackground(int32_t width, int32_t height)
{
  FML_DLOG(INFO) << " OHOSExternalTextureGL::setBackground";
  if (backGroundNativeImage_ != nullptr) {
    return;
  }

  OHOSSurface* ohos_surface_ptr = ohos_surface_.get();
  OhosSurfaceGLSkia* ohosSurfaceGLSkia_ = (OhosSurfaceGLSkia*)ohos_surface_ptr;
  auto result = ohosSurfaceGLSkia_->GLContextMakeCurrent();
  if (result->GetResult()) {
    FML_DLOG(INFO) << "ResourceContextMakeCurrent successed";
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
  if (nativeWindow_ == nullptr && nativeImage_) {
    nativeWindow_ = OH_NativeImage_AcquireNativeWindow(nativeImage_);
  }

  if (nativeWindow_ == nullptr) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL::SetTextureBufferSize nativeWindow_ is nullptr";
    return;
  }
  
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

  uint64_t usage = 0;
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, GET_USAGE, &usage);
  usage |= NATIVEBUFFER_USAGE_CPU_READ | (BUFFER_USAGE_HW_COMPOSER);
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, SET_USAGE, usage);

  OHNativeWindowBuffer *backGroundBuffer_;
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

  for (int32_t x = 0; x < handle->size / PIXEL_SIZE; x++) {
    *destAddr++ = WHITE_COLOR;
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
  
  int windowFormat = PixelMapToWindowFormat((PIXEL_FORMAT)pixelMapInfo.pixelFormat);
  ret = OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, SET_FORMAT, windowFormat);

  uint64_t usage = 0;
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, GET_USAGE, &usage);
  usage |= NATIVEBUFFER_USAGE_CPU_READ | (BUFFER_USAGE_HW_COMPOSER);
  OH_NativeWindow_NativeWindowHandleOpt(backGroundNativeWindow_, SET_USAGE, usage);

  OHNativeWindowBuffer *backGroundBuffer_;
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

  uint32_t real_height = pixelMapInfo.height;
  if (IsPixelMapYUVFormat((PIXEL_FORMAT)pixelMapInfo.pixelFormat)) {
    // y is height, uv is height/2
    real_height = pixelMapInfo.height + (pixelMapInfo.height + 1) / 2;
  }

  // 复制图片纹理数据到内存中，需要处理DMA内存补齐相关的逻辑
  if (pixelMapInfo.width * PIXEL_SIZE != pixelMapInfo.rowSize) {
    // 直接复制整块内存
    memcpy(destAddr, pixel, real_height * pixelMapInfo.rowSize);
  } else {
    // 需要处理DMA内存补齐相关的逻辑
    for (uint32_t i = 0; i < real_height; i++) {
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

  ret = OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, SET_TIMEOUT, 0);
  if (ret != 0) {
    FML_LOG(ERROR) << "OHOSExternalTextureGL SET_TIMEOUT err:" << ret;
    return;
  }

  uint64_t usage = 0;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, GET_USAGE, &usage);
  usage |= NATIVEBUFFER_USAGE_CPU_READ | (BUFFER_USAGE_HW_COMPOSER);
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, SET_USAGE, usage);

  OHNativeWindowBuffer *buffer_;
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