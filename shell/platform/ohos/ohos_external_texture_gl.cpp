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

#include <sys/mman.h>
#include <GLES2/gl2ext.h>

#define EGL_PLATFORM_OHOS_KHR             0x34E0

namespace flutter {
using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
constexpr char CHARACTER_WHITESPACE = ' ';
constexpr const char *CHARACTER_STRING_WHITESPACE = " ";
constexpr const char *EGL_EXT_PLATFORM_WAYLAND = "EGL_EXT_platform_wayland";
constexpr const char *EGL_KHR_PLATFORM_WAYLAND = "EGL_KHR_platform_wayland";
constexpr const char *EGL_GET_PLATFORM_DISPLAY_EXT = "eglGetPlatformDisplayEXT";
constexpr int32_t EGL_CONTEXT_CLIENT_VERSION_NUM = 2;

OHOSExternalTextureGL::OHOSExternalTextureGL(int64_t id, const std::shared_ptr<OHOSSurface>& ohos_surface)
  : Texture(id),ohos_surface_(std::move(ohos_surface)),transform(SkMatrix::I()) {
    nativeImage_ = nullptr;
    nativeWindow_ = nullptr;
    eglContext_ =  EGL_NO_CONTEXT;
    eglDisplay_ = EGL_NO_DISPLAY;
    buffer_ = nullptr;
    pixelMap_ = nullptr;
    lastImage_ = nullptr;
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
    FML_DLOG(ERROR) << "OHOSExternalTextureGL::Paint";
    return;
  }
  if (state_ == AttachmentState::uninitialized) {
    // Attach();
    // state_ = AttachmentState::attached;
    // InitEGLEnv();
    OHOSSurface* ohos_surface_ptr = ohos_surface_.get();
    OhosSurfaceGLSkia* ohosSurfaceGLSkia_ = (OhosSurfaceGLSkia*)ohos_surface_ptr;
    auto result = ohosSurfaceGLSkia_->GLContextMakeCurrent();
    if (result->GetResult()) {
    // bool result = ohosSurfaceGLSkia_->ResourceContextMakeCurrent();
    // if (result) {
      FML_DLOG(INFO)<<"ResourceContextMakeCurrent successed";
      glGenTextures(1, &texture_name_);
      glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_name_);
      glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      int32_t ret = OH_NativeImage_AttachContext(nativeImage_, texture_name_);
      
      if(ret != 0) {
        FML_DLOG(FATAL)<<"OHOSExternalTextureGL OH_NativeImage_AttachContext err code:"<< ret;
      }
      state_ = AttachmentState::attached;
    } else {
      FML_DLOG(FATAL)<<"ResourceContextMakeCurrent failed";
    }
  }
  if (!freeze && new_frame_ready_) {
    Update();
    new_frame_ready_ = false;
  }
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::Paint, texture_name_=" << texture_name_;
  PaintOrigin(context, bounds, freeze, sampling);
}

void OHOSExternalTextureGL::PaintOrigin(PaintContext& context,
             const SkRect& bounds,
             bool freeze,
             const SkSamplingOptions& sampling) {
  GrGLTextureInfo textureInfo = {GL_TEXTURE_EXTERNAL_OES, texture_name_,
                                GL_RGBA8_OES};
  GrBackendTexture backendTexture(720, 1280, GrMipMapped::kNo, textureInfo);
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::Paint, backendTexture.isValid=" << backendTexture.isValid();
  sk_sp<SkImage> image = SkImage::MakeFromTexture(
      context.gr_context, backendTexture, kTopLeft_GrSurfaceOrigin,
      kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
  // glFinish();
  if (image) {
    FML_DLOG(INFO)<<"OHOSExternalTextureGL begin draw 1";
    SkAutoCanvasRestore autoRestore(context.canvas, true);

    // The incoming texture is vertically flipped, so we flip it
    // back. OpenGL's coordinate system has Positive Y equivalent to up, while
    // Skia's coordinate system has Negative Y equvalent to up.
    // context.canvas->translate(bounds.x(), bounds.y() + bounds.height());
    // context.canvas->scale(bounds.width(), -bounds.height());

    if (!transform.isIdentity()) {
      FML_DLOG(INFO)<<"OHOSExternalTextureGL begin draw 2";
      sk_sp<SkShader> shader = image->makeShader(
          SkTileMode::kRepeat, SkTileMode::kRepeat, sampling, transform);

      SkPaint paintWithShader;
      if (context.sk_paint) {
        paintWithShader = *context.sk_paint;
      }
      paintWithShader.setShader(shader);
      context.canvas->drawRect(SkRect::MakeWH(1, 1), paintWithShader);
    } else {
      FML_DLOG(INFO)<<"OHOSExternalTextureGL begin draw 3";
      // todo 看这里对不对
      SkSamplingOptions samplingTemp;
      context.canvas->drawImageRect(image, bounds, samplingTemp, nullptr);
      // glFinish();
    }
  }
}

void OHOSExternalTextureGL::PaintOhImage(PaintContext& context,
             const SkRect& bounds,
             bool freeze,
             const SkSamplingOptions& sampling) {
  // todo 参考Demo中的方式，测试直接使用egl绘制视频纹理是否可行
}

void OHOSExternalTextureGL::OnGrContextCreated() {
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::OnGrContextCreated";
  state_ = AttachmentState::uninitialized;
}

void OHOSExternalTextureGL::OnGrContextDestroyed() {
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::OnGrContextDestroyed";
  if (state_ == AttachmentState::attached) {
    Detach();
    glDeleteTextures(1, &texture_name_);
    OH_NativeImage_Destroy(&nativeImage_);
  }
  state_ = AttachmentState::detached;
}

void OHOSExternalTextureGL::MarkNewFrameAvailable() {
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::MarkNewFrameAvailable";
  new_frame_ready_ = true;
}

void OHOSExternalTextureGL::OnTextureUnregistered() {
  FML_DLOG(INFO)<<" OHOSExternalTextureGL::OnTextureUnregistered";
  // do nothing
}

void OHOSExternalTextureGL::Attach() {
  if (eglContext_ == EGL_NO_CONTEXT) {
    FML_DLOG(INFO) << "OHOSExternalTextureGL eglContext_ no context, need init";
    InitEGLEnv();
  }

  if (texture_name_ == 0) {
    glGenTextures(1, &texture_name_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_name_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // 关联上下文
  if (!eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext_)) {
    EGLint surfaceId = -1;
    eglQuerySurface(eglDisplay_, EGL_NO_SURFACE, EGL_CONFIG_ID, &surfaceId);
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to call eglMakeCurrent, error:"
              << eglGetError();
  }

  int32_t ret = OH_NativeImage_AttachContext(nativeImage_, texture_name_);
  if (ret != 0) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL OH_NativeImage_AttachContext err code:" << ret;
  }

}

void OHOSExternalTextureGL::Update() {
  //ProducePixelMapToNativeImage();
  int32_t ret = OH_NativeImage_UpdateSurfaceImage(nativeImage_);
  if(ret != 0) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL OH_NativeImage_UpdateSurfaceImage err code:"<< ret;
  }
  UpdateTransform();
}

void OHOSExternalTextureGL::Detach() {
  OH_NativeImage_DetachContext(nativeImage_);
  OH_NativeWindow_DestroyNativeWindow(nativeWindow_);
}

void OHOSExternalTextureGL::UpdateTransform() {
  float m[16] = { 0.0f };
  int32_t ret = OH_NativeImage_GetTransformMatrix(nativeImage_, m);
  if(ret != 0) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL OH_NativeImage_GetTransformMatrix err code:"<< ret;
  }
  //transform ohos 4x4 matrix to skia 3x3 matrix
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::UpdateTransform "<<m[0]<<" "<<m[4]<<" "<<m[12];
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::UpdateTransform "<<m[1]<<" "<<m[5]<<" "<<m[13];
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::UpdateTransform "<<m[3]<<" "<<m[7]<<" "<<m[15];
  SkScalar matrix3[] = {
    m[0], m[4], m[12],  //
    m[1], m[5], m[13],  //
    m[3], m[7], m[15],  //
  };
  transform.set9(matrix3);
  SkMatrix inverted;
  if (!transform.invert(&inverted)) {
    FML_LOG(FATAL) << "OHOSExternalTextureGL Invalid SurfaceTexture transformation matrix";
  }
  transform = inverted;
}

void OHOSExternalTextureGL::DispatchImage(ImageNative* image)
{
  lastImage_ = image;
}

void OHOSExternalTextureGL::ProducePixelMapToNativeImage()
{
  FML_DLOG(INFO)<<"OHOSExternalTextureGL::ProducePixelMapToNativeImage enter";
  if(pixelMap_ == nullptr) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL pixelMap in null";
    return;
  }
  if (state_ == AttachmentState::detached) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL ProducePixelMapToNativeImage AttachmentState err";
    return;
  }
  
  OH_PixelMap_GetImageInfo(pixelMap_, &pixelMapInfo);
  FML_DLOG(INFO)<<"OHOSExternalTextureGL pixelMapInfo w:"<<pixelMapInfo.width<<" h:"<<pixelMapInfo.height;
  
  int code = SET_BUFFER_GEOMETRY;
  OH_NativeWindow_NativeWindowHandleOpt(nativeWindow_, code, pixelMapInfo.width, pixelMapInfo.height);
  if(buffer_ != nullptr) {
    OH_NativeWindow_NativeWindowAbortBuffer(nativeWindow_, buffer_);
    buffer_ = nullptr;
  }
  OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd);
  BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
  //get virAddr of bufferHandl by mmap sys interface
  void *mappedAddr = mmap(handle->virAddr, handle->size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL mmap failed";
    return;
  }
  void *pixelAddr = nullptr;
  int32_t ret = OH_PixelMap_AccessPixels(pixelMap_, &pixelAddr);
  if(ret != IMAGE_RESULT_SUCCESS) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL OH_PixelMap_AccessPixels err:"<< ret;
    return;
  }
  FML_DLOG(INFO)<<"OHOSExternalTextureGL pixelAddr:"<<pixelAddr;
  // memcpy(mappedAddr, pixelAddr, pixelMapInfo.width*pixelMapInfo.height*4);
  uint8_t *value = static_cast<uint8_t*>(pixelAddr);
  uint32_t *pixel = static_cast<uint32_t *>(mappedAddr);
  for (uint32_t x = 0; x < pixelMapInfo.width; x++) {
    for (uint32_t y = 0; y < pixelMapInfo.height; y++) {
      *pixel++ = *value++;
    }
  }
  OH_PixelMap_UnAccessPixels(pixelMap_);
  //munmap after use
  ret = munmap(mappedAddr, handle->size);
  if (ret == -1) {
    FML_DLOG(FATAL)<<"OHOSExternalTextureGL munmap failed";
    return;
  }
  Region region{nullptr, 0};
  ret = OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd, region);
  if(ret != 0) {
    FML_DLOG(FATAL)<<"OH_NativeWindow_NativeWindowFlushBuffer err code:"<< ret;
  }
  
}

EGLDisplay OHOSExternalTextureGL::GetPlatformEglDisplay(EGLenum platform, void *native_display, const EGLint *attrib_list)
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

void OHOSExternalTextureGL::InitEGLEnv()
{
  FML_DLOG(INFO) << "OHOSExternalTextureGL InitEGLEnv";
  // 获取当前的显示设备
  eglDisplay_ =
      GetPlatformEglDisplay(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, NULL);
  if (eglDisplay_ == EGL_NO_DISPLAY) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to create EGLDisplay gl errno : "
                    << eglGetError();
  }
  EGLint major, minor;
  // 初始化EGLDisplay
  if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to initialize EGLDisplay";
  }

  // 绑定图形绘制的API为OpenGLES
  if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to bind OpenGL ES API";
  }
  unsigned int ret;
  EGLint count;
  EGLint config_attribs[] = {EGL_SURFACE_TYPE,
                             EGL_WINDOW_BIT,
                             EGL_RED_SIZE,
                             8,
                             EGL_GREEN_SIZE,
                             8,
                             EGL_BLUE_SIZE,
                             8,
                             EGL_ALPHA_SIZE,
                             8,
                             EGL_RENDERABLE_TYPE,
                             EGL_OPENGL_ES3_BIT,
                             EGL_NONE};
  // 获取一个有效的系统配置信息
  ret = eglChooseConfig(eglDisplay_, config_attribs, &config_, 1, &count);
  if (!(ret && static_cast<unsigned int>(count) >= 1)) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to eglChooseConfig";
  }

  const EGLint context_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, EGL_CONTEXT_CLIENT_VERSION_NUM, EGL_NONE};

  // 创建上下文
  eglContext_ =
      eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, context_attribs);
  if (eglContext_ == EGL_NO_CONTEXT) {
    FML_DLOG(FATAL) << "OHOSExternalTextureGL Failed to create egl context, error:"
              << eglGetError();
  }

  FML_DLOG(INFO) << "OHOSExternalTextureGL InitEGLEnv finish";
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
  if(pixelMap != nullptr) {
    pixelMap_ = pixelMap;
  }
}

}  // namespace flutter