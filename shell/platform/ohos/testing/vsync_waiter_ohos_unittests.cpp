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