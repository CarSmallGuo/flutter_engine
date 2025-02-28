/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "flutter/fml/message_loop.h"

#include <iostream>
#include <thread>

#include "flutter/fml/build_config.h"
#include "flutter/fml/concurrent_message_loop.h"
#include "flutter/fml/synchronization/count_down_latch.h"
#include "flutter/fml/synchronization/waitable_event.h"
#include "flutter/fml/task_runner.h"
#include "flutter/fml/time/chrono_timestamp_provider.h"
#include "gtest/gtest.h"

namespace fml {
namespace message_loop_test {

#define PLATFORM_SPECIFIC_CAPTURE(...) [__VA_ARGS__]

/**
 *   breif:  注册异步任务
 *
 */
void MessageLoopTestPostTask(void) {
  bool started = false;
  bool terminated = false;
  std::thread thread([&started, &terminated]() {
    fml::MessageLoop::EnsureInitializedForCurrentThread();
    auto& loop = fml::MessageLoop::GetCurrent();
    ASSERT_TRUE(loop.GetTaskRunner());
    loop.GetTaskRunner()->PostTask([&terminated]() {
      std::cout << "MessageLoopTestPostTask running ..." << std::endl;
      fml::MessageLoop::GetCurrent().Terminate();
      terminated = true;
    });
    loop.Run();
    started = true;
  });
  thread.join();
  ASSERT_TRUE(started);
  ASSERT_TRUE(terminated);
}


}  // namespace message_loop_test
}  // namespace fml