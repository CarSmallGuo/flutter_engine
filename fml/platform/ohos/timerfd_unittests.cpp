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
        pipe(m_fd_);
    }

    ~MockFileDescriptor()
    {
        close(m_fd_[0]);
        close(m_fd_[1]);
    }

    int getReadFd() const { return m_fd_[0]; }
    int getWriteFd() const { return m_fd_[1]; }

    void writeFireCount(uint64_t count)
    {
        write(m_fd_[1], &count, sizeof(count));
    }

private:
    int m_fd_[2];
};
TEST(TimerfdTest, TimerRearmTest) {
    TimePoint timeP;
    EXPECT_EQ(TimerRearm(0, timeP), false);
}

TEST(TimerDrainTest, ReadsFireCountSuccessfully) {
    MockFileDescriptor mockFd;
    uint64_t expected_count = 1;
    mockFd.writeFireCount(expected_count);
    EXPECT_TRUE(TimerDrain(mockFd.getReadFd()));
}

TEST(TimerfdTest, TimerFdCreateTest) {
    EXPECT_TRUE(timerfd_create(0, 0));
}

TEST(TimerfdTest, TimerFdSettimeTest) {
    EXPECT_EQ(timerfd_settime(0, 0, nullptr, nullptr), -1);
}

}  // namespace
}  // namespace fml