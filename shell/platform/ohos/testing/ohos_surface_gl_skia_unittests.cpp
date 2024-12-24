// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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