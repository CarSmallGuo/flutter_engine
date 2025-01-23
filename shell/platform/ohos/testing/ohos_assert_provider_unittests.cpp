/*
 * Copyright 2013 The Flutter Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_asset_provider.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {
TEST(OHOSAssetProviderTest, Create001)
{
    std::shared_ptr<flutter::OHOSAssetProvider> provider = std::make_shared<flutter::OHOSAssetProvider>(nullptr);
    EXPECT_TRUE(provider.get() != nullptr);
    std::unique_ptr<flutter::OHOSAssetProvider> newProvider = provider->Clone();
    EXPECT_TRUE(newProvider.get() != nullptr);
}
}
}