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

