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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_xcomponent_adapter.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {
TEST(XComponentAdapterTest, SetNativeXComponent)
{
    XComponentAdapter* xcomponentAdapter = XComponentAdapter::GetInstance();
    std::string id = "test";
    OH_NativeXComponent* nativeXComponent = nullptr;
    xcomponentAdapter->SetNativeXComponent(id, nativeXComponent);

    EXPECT_TRUE(xcomponentAdapter->xcomponetMap_.size() > 0);
}
}
}
