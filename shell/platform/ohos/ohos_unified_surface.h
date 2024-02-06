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

#ifndef OHOS_UNIFIED_SURFACE_H
#define OHOS_UNIFIED_SURFACE_H

#include <memory>
#include "flutter/flow/surface.h"
#include "flutter/shell/platform/ohos/context/ohos_context.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "flutter/shell/platform/ohos/ohos_surface_gl_skia.h"
#include "third_party/skia/include/core/SkSize.h"

namespace impeller {
class Context;
}  // namespace impeller

namespace flutter {

class OHOSUnifiedSurface : public GPUSurfaceGLDelegate,
                           public OHOSSurface {
//  public:
//   virtual ~OHOSUnifiedSurface();
//   virtual bool IsValid() const = 0;
//   virtual void TeardownOnScreenContext() = 0;

//   virtual bool OnScreenSurfaceResize(const SkISize& size) = 0;

//   virtual bool ResourceContextMakeCurrent() = 0;

//   virtual bool ResourceContextClearCurrent() = 0;

//   virtual bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window) = 0;

//   virtual std::unique_ptr<Surface> CreateSnapshotSurface();

//   virtual std::unique_ptr<Surface> CreateGPUSurface(
//       GrDirectContext* gr_context = nullptr) = 0;

//   virtual std::shared_ptr<impeller::Context> GetImpellerContext();

//   virtual std::unique_ptr<GLContextResult> GLContextMakeCurrent() = 0;

//   virtual bool GLContextClearCurrent() = 0;

//   virtual void GLContextSetDamageRegion(const std::optional<SkIRect>& region) {}

//   virtual bool GLContextPresent(const GLPresentInfo& present_info) = 0;

//   virtual GLFBOInfo GLContextFBO(GLFrameInfo frame_info) const = 0;

//   virtual bool GLContextFBOResetAfterPresent() const;

//   virtual SurfaceFrame::FramebufferInfo GLContextFramebufferInfo() const;

//   virtual SkMatrix GLContextSurfaceTransformation() const;

//   virtual sk_sp<const GrGLInterface> GetGLInterface() const;

//   static sk_sp<const GrGLInterface> GetDefaultPlatformGLInterface();

//   using GLProcResolver =
//       std::function<void* /* proc name */ (const char* /* proc address */)>;
 
//   virtual GLProcResolver GetGLProcResolver() const;

//   virtual bool AllowsDrawingWhenGpuDisabled() const;

};


}  // namespace flutter

#endif