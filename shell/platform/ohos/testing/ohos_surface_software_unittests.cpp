/*
Copyright (C) 2024 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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