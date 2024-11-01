// Copyright (C) 2024 Huawei Device Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "flutter/shell/platform/ohos/ohos_surface_software.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {
namespace {
std::shared_ptr<OHOSContext> CreateOhosContext()
{
    return std::make_shared<OHOSContext>(OHOSRenderingAPI::kSoftware);
}
} // namespace

TEST(OHOSSurfaceSoftwareTest, Create)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);
    std::unique_ptr<OHOSSurfaceSoftware> surfaceSoftWare = std::make_unique<OHOSSurfaceSoftware>(context);
    EXPECT_TRUE(surfaceSoftWare != nullptr);
    EXPECT_TRUE(surfaceSoftWare->IsValid());
}

TEST(OHOSSurfaceSoftwareTest, CreateGPUSurface)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);
    std::unique_ptr<OHOSSurfaceSoftware> surfaceSoftWare = std::make_unique<OHOSSurfaceSoftware>(context);
    EXPECT_TRUE(surfaceSoftWare != nullptr);

    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeMock(nullptr);
    std::unique_ptr<Surface> gpuSurface = surfaceSoftWare->CreateGPUSurface(grContext.get());
    EXPECT_TRUE(gpuSurface != nullptr);
}

TEST(OHOSSurfaceSoftwareTest, AcquireBackingStore)
{
    std::shared_ptr<OHOSContext> context = CreateOhosContext();
    EXPECT_TRUE(context != nullptr);
    std::unique_ptr<OHOSSurfaceSoftware> surfaceSoftWare = std::make_unique<OHOSSurfaceSoftware>(context);
    EXPECT_TRUE(surfaceSoftWare != nullptr);
    EXPECT_TRUE(surfaceSoftWare->IsValid());

    sk_sp<SkSurface> skSurface = surfaceSoftWare->AcquireBackingStore({100, 100});
    EXPECT_TRUE(skSurface != nullptr);
}
}
}