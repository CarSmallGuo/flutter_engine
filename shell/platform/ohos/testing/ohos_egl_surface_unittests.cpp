/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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