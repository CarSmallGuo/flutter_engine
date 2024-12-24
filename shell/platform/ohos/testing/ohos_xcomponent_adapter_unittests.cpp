// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
