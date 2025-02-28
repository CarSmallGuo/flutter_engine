/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef FLUTTER_FML_PLATFORM_OHOS_TIMER_FD_H_
#define FLUTTER_FML_PLATFORM_OHOS_TIMER_FD_H_

#include "flutter/fml/time/time_point.h"


#include <sys/timerfd.h>

#define FML_TIMERFD_AVAILABLE 1



#define FML_TIMERFD_AVAILABLE 0

#include <sys/types.h>
// Must come after sys/types
#include <linux/time.h>

#define TFD_TIMER_ABSTIME (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

#define TFD_CLOEXEC O_CLOEXEC
#define TFD_NONBLOCK O_NONBLOCK

int timerfd_create(int clockid, int flags);

namespace fml {

/// Rearms the timer to expire at the given time point.
bool TimerRearm(int fd, fml::TimePoint time_point);

/// Drains the timer FD and returns true if it has expired. This may be false in
/// case the timer read is non-blocking and this routine was called before the
/// timer expiry.
bool TimerDrain(int fd);

}  // namespace fml

#endif  // FLUTTER_FML_PLATFORM_OHOS_TIMER_FD_H_
