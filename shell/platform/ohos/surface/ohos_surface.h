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

#ifndef OHOS_SURFACE_H
#define OHOS_SURFACE_H

#include <memory>
#include "flutter/flow/surface.h"
#include "flutter/shell/platform/ohos/context/ohos_context.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "third_party/skia/include/core/SkSize.h"

namespace impeller {
class Context;
}  // namespace impeller

namespace flutter {

class OHOSSurface {
 public:
  virtual ~OHOSSurface();
  virtual bool IsValid() const = 0;
  virtual void TeardownOnScreenContext() = 0;

  virtual bool OnScreenSurfaceResize(const SkISize& size) = 0;

  virtual bool ResourceContextMakeCurrent() = 0;

  virtual bool ResourceContextClearCurrent() = 0;

  virtual bool SetNativeWindow(fml::RefPtr<OHOSNativeWindow> window) = 0;

  virtual std::unique_ptr<Surface> CreateSnapshotSurface();

  virtual std::unique_ptr<Surface> CreateGPUSurface(
      GrDirectContext* gr_context = nullptr) = 0;

  virtual std::shared_ptr<impeller::Context> GetImpellerContext();

 protected:
  explicit OHOSSurface(const std::shared_ptr<OHOSContext>& ohos_context);
  std::shared_ptr<OHOSContext> ohos_context_;
};

class OhosSurfaceFactory {
 public:
  OhosSurfaceFactory() = default;

  virtual ~OhosSurfaceFactory() = default;

  virtual std::unique_ptr<OHOSSurface> CreateSurface() = 0;
};
}  // namespace flutter

#endif