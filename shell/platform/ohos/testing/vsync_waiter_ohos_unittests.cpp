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
#include "flutter/shell/platform/ohos/vsync_waiter_ohos.h"
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
}
TEST(VsyncWaiterOHOSTest, Create)
{
    std::string threadLabel = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    TaskRunners runners = CreateTaskRunners(threadLabel);
    std::shared_ptr<VsyncWaiterOHOS> waiter = std::make_shared<VsyncWaiterOHOS>(runners);
    EXPECT_TRUE(waiter != nullptr);
}
}
}