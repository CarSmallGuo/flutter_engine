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
#include "flutter/shell/common/thread_host.h"
#include "flutter/shell/platform/ohos/ohos_context_gl_skia.h"
#include "flutter/shell/platform/ohos/ohos_egl_surface.h"
#include "flutter/shell/platform/ohos/surface/ohos_native_window.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {
namespace {
TaskRunners MakeTaskRunners(const std::string& threadLabel)
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

std::unique_ptr<OhosContextGLSkia> CreateOhosContext()
{
    auto environment = fml::MakeRefCounted<OhosEnvironmentGL>();
    std::string threadLabel = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    TaskRunners taskRunners = MakeTaskRunners(threadLabel);
    return std::make_unique<OhosContextGLSkia>(OHOSRenderingAPI::kOpenGLES, environment, taskRunners, 0);
}
} // namespace
TEST(OHOSContextGlSkiaTest, CreateOnscreenSurface)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    auto window = fml::MakeRefCounted<OHOSNativeWindow>(nullptr);
    std::unique_ptr<OhosEGLSurface> surface = context->CreateOnscreenSurface(window);
    EXPECT_TRUE(surface.get() != nullptr);
}

TEST(OHOSContextGlSkiaTest, CreateOffscreenSurface)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());
}

TEST(OHOSContextGlSkiaTest, CreatePbufferSurface)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreatePbufferSurface();
    EXPECT_TRUE(surface->IsValid());
}

TEST(OHOSContextGlSkiaTest, Environment)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    fml::RefPtr<OhosEnvironmentGL> environment = context->Environment();
    EXPECT_TRUE(environment.get() != nullptr);
}

TEST(OHOSContextGlSkiaTest, ClearCurrent)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);
    EXPECT_TRUE(context->ClearCurrent());
}

TEST(OHOSContextGlSkiaTest, CreateNewContext)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    EGLContext newContext = context->CreateNewContext();
    EXPECT_TRUE(newContext != EGL_NO_CONTEXT);
}
}
}

