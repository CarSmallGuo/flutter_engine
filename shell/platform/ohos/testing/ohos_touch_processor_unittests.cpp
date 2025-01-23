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

#include "flutter/shell/platform/ohos/ohos_touch_processor.h"
#include "gtest/gtest.h"

namespace flutter {
namespace testing {

TEST(OhosTouchProcessorTest, getPointerChangeForAction)
{
    OhosTouchProcessor touchProcessor;
    int maskedAction = OH_NATIVEXCOMPONENT_DOWN;
    EXPECT_EQ(touchProcessor.getPointerChangeForAction(maskedAction), PointerData::Change::kDown);
    maskedAction = OH_NATIVEXCOMPONENT_UP;
    EXPECT_EQ(touchProcessor.getPointerChangeForAction(maskedAction), PointerData::Change::kUp);
    maskedAction = OH_NATIVEXCOMPONENT_MOVE;
    EXPECT_EQ(touchProcessor.getPointerChangeForAction(maskedAction), PointerData::Change::kMove);
    maskedAction = OH_NATIVEXCOMPONENT_CANCEL;
    EXPECT_EQ(touchProcessor.getPointerChangeForAction(maskedAction), PointerData::Change::kCancel);
}

TEST(OhosTouchProcessorTest, getPointerDeviceTypeForToolType)
{
    OhosTouchProcessor touchProcessor;
    int toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_FINGER;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kTouch);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_PEN;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kStylus);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_RUBBER;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kInvertedStylus);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_BRUSH;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kStylus);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_PENCIL;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kStylus);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_AIRBRUSH;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kStylus);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_MOUSE;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kMouse);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_LENS;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kTouch);
    toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN;
    EXPECT_EQ(touchProcessor.getPointerDeviceTypeForToolType(toolType), PointerData::DeviceKind::kTouch);
}
}
}