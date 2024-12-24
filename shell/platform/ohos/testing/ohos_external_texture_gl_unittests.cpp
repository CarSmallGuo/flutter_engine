// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_external_texture_gl.h"
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

std::shared_ptr<OhosContextGLSkia> CreateOhosContext()
{
    fml::RefPtr<OhosEnvironmentGL> environment = fml::MakeRefCounted<OhosEnvironmentGL>();
    std::string threadLabel = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    TaskRunners taskRunners = MakeTaskRunners(threadLabel);
    return std::make_shared<OhosContextGLSkia>(OHOSRenderingAPI::kOpenGLES, environment, taskRunners, 0);
}
} // namespace

TEST(OHOSExternalTextureGLTest, OnTextureUnregistered)
{
    std::shared_ptr<OhosContextGLSkia> context = CreateOhosContext();
    EXPECT_TRUE(context.get() != nullptr);
    std::shared_ptr<OhosSurfaceGLSkia> surfaceSkia = std::make_shared<OhosSurfaceGLSkia>(context);
    std::shared_ptr<OHOSExternalTextureGL> extTexture = std::make_shared<OHOSExternalTextureGL>(0, surfaceSkia);

    extTexture->OnTextureUnregistered();
    EXPECT_TRUE(extTexture->first_update_ == false);
}
}
}