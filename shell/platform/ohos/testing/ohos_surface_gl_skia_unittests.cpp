/*
Copyright (C) 2024 Huawei Device Co., Ltd.

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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/common/task_runners.h"
#include "flutter/fml/message_loop.h"
#include "flutter/shell/common/thread_host.h"
#include "flutter/shell/platform/ohos/ohos_context_gl_skia.h"
#include "flutter/shell/platform/ohos/ohos_surface_gl_skia.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {
namespace {
TaskRunners CreateTaskRunners(const std::string& threadLabel)
{
    fml::MessageLoop::EnsureInitializedForCurrentThread();
    auto& loop = fml::MessageLoop::GetCurrent();
    return {
        threadLabel,
        loop.GetTaskRunner(),  // platform
        loop.GetTaskRunner(),  // raster
        loop.GetTaskRunner(),  // ui
        loop.GetTaskRunner()   // io
    };
}

std::unique_ptr<OHOSContext> CreateOhosContext()
{
    auto environment = fml::MakeRefCounted<OhosEnvironmentGL>();
    std::string threadLabel = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    TaskRunners task_runners = CreateTaskRunners(threadLabel);
    return std::make_unique<OhosContextGLSkia>(OHOSRenderingAPI::kOpenGLES, environment, task_runners, 0);
}
} // namespace

TEST(OhosSurfaceGLSkiaTest, Create)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);
    std::unique_ptr<OhosSurfaceGLSkia> surfaceGL = std::make_unique<OhosSurfaceGLSkia>(context);
    EXPECT_TRUE(surfaceGL != nullptr);
    EXPECT_TRUE(surfaceGL->IsValid());
}

TEST(OhosSurfaceGLSkiaTest, ResourceContext)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);

    std::unique_ptr<OhosSurfaceGLSkia> surfaceGL = std::make_unique<OhosSurfaceGLSkia>(context);
    EXPECT_TRUE(surfaceGL != nullptr);
    EXPECT_TRUE(surfaceGL->IsValid());

    EXPECT_TRUE(surfaceGL->ResourceContextMakeCurrent());
    EXPECT_TRUE(surfaceGL->ResourceContextClearCurrent());
}

TEST(OhosSurfaceGLSkiaTest, GetContext)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);
    std::unique_ptr<OhosSurfaceGLSkia> surfaceGL = std::make_unique<OhosSurfaceGLSkia>(context);
    EXPECT_TRUE(surfaceGL != nullptr);
    EXPECT_TRUE(surfaceGL->IsValid());

    EXPECT_TRUE(surfaceGL->CreateSnapshotSurface() != nullptr);

    std::unique_ptr<GLContextResult> result = surfaceGL->GLContextMakeCurrent();
    EXPECT_TRUE(result != nullptr);

    SurfaceFrame::FramebufferInfo info = surfaceGL->GLContextFramebufferInfo();
    EXPECT_TRUE(info.supports_readback == true);

    std::optional<SkIRect> frameDamage(SkIRect::MakeEmpty());
    std::optional<SkIRect> bufferDamage(SkIRect::MakeEmpty());
    GLPresentInfo presentInfo = {0, frameDamage, std::nullopt, bufferDamage};
    EXPECT_TRUE(surfaceGL->GLContextPresent(presentInfo));

    GLFrameInfo frameInfo;
    EXPECT_TRUE(surfaceGL->GLContextFBO(frameInfo).fbo_id == 0);

    sk_sp<const GrGLInterface> grInterface = surfaceGL->GetGLInterface();
    EXPECT_TRUE(grInterface != nullptr);
}
}
}