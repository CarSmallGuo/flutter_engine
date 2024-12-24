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
    fml::RefPtr<OhosEnvironmentGL> environment = fml::MakeRefCounted<OhosEnvironmentGL>();
    std::string threadLabel = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    TaskRunners taskRunners = MakeTaskRunners(threadLabel);
    return std::make_unique<OhosContextGLSkia>(OHOSRenderingAPI::kOpenGLES, environment, taskRunners, 0);
}
} // namespace

TEST(OHOSEGLSurfaceTest, MakeCurrent)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());

    OhosEGLSurfaceMakeCurrentStatus status = surface->MakeCurrent();
    EXPECT_EQ(status, OhosEGLSurfaceMakeCurrentStatus::kSuccessMadeCurrent);
}

TEST(OHOSEGLSurfaceTest, SupportsPartialRepaint)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());

    bool res = surface->SupportsPartialRepaint();
    EXPECT_EQ(res, false);
}

TEST(OHOSEGLSurfaceTest, InitialDamage)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());

    std::optional<SkIRect> rect = surface->InitialDamage();
    EXPECT_EQ(rect.has_value(), false);
}

TEST(OHOSEGLSurfaceTest, SetDamageRegion)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());

    std::optional<SkIRect> rect(SkIRect::MakeEmpty());
    surface->SetDamageRegion(rect);
    EXPECT_EQ(rect.has_value(), true);
    SkISize size = surface->GetSize();
    EXPECT_TRUE(size.width() != 0);
    EXPECT_TRUE(size.height() != 0);
}

TEST(OHOSEGLSurfaceTest, SetPresentationTime)
{
    std::unique_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);

    std::unique_ptr<OhosEGLSurface> surface = context->CreateOffscreenSurface();
    EXPECT_TRUE(surface->IsValid());

    fml::TimePoint timePoint = fml::TimePoint::CurrentWallTime();
    bool res = surface->SetPresentationTime(timePoint);
    EXPECT_EQ(res, false);
}
}
}