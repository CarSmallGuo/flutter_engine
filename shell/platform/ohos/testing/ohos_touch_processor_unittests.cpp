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