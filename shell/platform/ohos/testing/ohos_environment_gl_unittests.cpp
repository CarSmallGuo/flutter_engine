// Copyright (C) 2024 Huawei Device Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_environment_gl.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {

TEST(OHOSEnvironmentGLTest, Create)
{
    fml::RefPtr<OhosEnvironmentGL> environment = fml::MakeRefCounted<OhosEnvironmentGL>();
    EXPECT_TRUE(environment->IsValid());
}
}
}