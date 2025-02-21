/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

#include "flutter/fml/platform/ohos/paths_ohos.h"
#include "gtest/gtest.h"

namespace fml {
namespace {
TEST(PathsTest, PathsOhosTest01) {
    fml::paths::InitializeOhosCachesPath("test");
    fml::UniqueFD resultFD = fml::paths::GetCachesDirectory();
    EXPECT_FALSE(resultFD.is_valid());
}


}  // namespace
}  // namespace fml