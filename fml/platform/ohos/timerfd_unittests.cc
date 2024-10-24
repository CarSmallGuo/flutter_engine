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
    MockFileDescriptor() {
        pipe(fd_);
    }

    ~MockFileDescriptor() {
        close(fd_[0]);
        close(fd_[1]);
    }

    int get_read_fd() const { return fd_[0]; }
    int get_write_fd() const { return fd_[1]; }

    void write_fire_count(uint64_t count) {
        write(fd_[1], &count, sizeof(count));
    }

private:
    int fd_[2];
};
TEST(TimerfdTest, TimerRearmTest) {
  TimePoint timeP;
  EXPECT_EQ(TimerRearm(0, timeP), false);
}

TEST(TimerDrainTest, ReadsFireCountSuccessfully) {
  {
    MockFileDescriptor mock_fd;
    uint64_t expected_count = 1;
    mock_fd.write_fire_count(expected_count);
    EXPECT_TRUE(TimerDrain(mock_fd.get_read_fd()));
  } 
}

TEST(TimerfdTest, TimerFdCreateTest) {
  EXPECT_TRUE(timerfd_create(0, 0));
}

TEST(TimerfdTest, TimerFdSettimeTest) {
  EXPECT_EQ(timerfd_settime(0, 0, nullptr, nullptr), -1);
}

}  // namespace
}  // namespace fml