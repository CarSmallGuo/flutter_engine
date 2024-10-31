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