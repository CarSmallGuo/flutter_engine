/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
#include <gmock/gmock.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "flutter/fml/platform/ohos/timerfd.h"

#include "gtest/gtest.h"

using fml::TimePoint;
namespace fml {
namespace {

class MockFileDescriptor {
public:
    MockFileDescriptor()
    {
        pipe(mFdArray);
    }

    ~MockFileDescriptor()
    {
        close(mFdArray[0]);
        close(mFdArray[1]);
    }

    int GetReadFd() const { return mFdArray[0]; }
    int GetWriteFd() const { return mFdArray[1]; }

    void WriteFireCount(uint64_t count)
    {
        write(mFdArray[1], &count, sizeof(count));
    }

private:
    int mFdArray[2];
};
TEST(TimerfdTest, TimerRearmTest) {
    TimePoint timeP;
    EXPECT_EQ(TimerRearm(0, timeP), false);
}

TEST(TimerDrainTest, ReadsFireCountSuccessfully) {
    MockFileDescriptor mockFd;
    uint64_t expected_count = 1;
    mockFd.WriteFireCount(expected_count);
    EXPECT_TRUE(TimerDrain(mockFd.GetReadFd()));
}

TEST(TimerfdTest, TimerFdCreateTest) {
    EXPECT_TRUE(timerfd_create(0, 0));
}

TEST(TimerfdTest, TimerFdSettimeTest) {
    EXPECT_EQ(timerfd_settime(0, 0, nullptr, nullptr), -1);
}

}  // namespace
}  // namespace fml