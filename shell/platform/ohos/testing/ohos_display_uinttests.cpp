// Copyright (C) 2024 Huawei Device Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_display.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {

const double DEFAULT_FPS = 60;
TEST(OHOSDisplayTest, GetRefreshRate)
{
    std::shared_ptr<flutter::OHOSDisplay> display = std::make_shared<flutter::OHOSDisplay>(nullptr);
    EXPECT_TRUE(display.get() != nullptr);
    EXPECT_EQ(display->GetRefreshRate(), DEFAULT_FPS);
}
}
}