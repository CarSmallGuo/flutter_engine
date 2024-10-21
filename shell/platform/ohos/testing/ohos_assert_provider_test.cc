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

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../ohos_asset_provider.h"

TEST(OHOSAssetProviderTest, Build001) {
    std::shared_ptr<flutter::OHOSAssetProvider> provider = std::make_shared<flutter::OHOSAssetProvider>(nullptr);
    EXPECT_EQ(provider, nullptr);
}
