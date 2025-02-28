/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
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