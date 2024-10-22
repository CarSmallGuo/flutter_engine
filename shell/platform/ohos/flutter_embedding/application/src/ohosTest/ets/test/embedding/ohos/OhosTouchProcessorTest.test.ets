/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
import { describe, it } from '@ohos/hypium';
import { TouchEvent, Action, Touch, ToolType, SourceType } from '@ohos.multimodalInput.touchEvent';
import { OhosTouchProcessor } from "@ohos/flutter_ohos";

export default function OhosTouchProcessorTest() {
  describe('OhosTouchProcessorTest', () => {
    it('FlutterOnTouchEvent', 0, async (done: Function) => {
      const ohosTouchProcessor: OhosTouchProcessor = new OhosTouchProcessor();
      const touch: Touch = {
        id: 0,
        pressedTime: 0,
        screenX: 0,
        screenY: 0,
        windowX: 0,
        windowY: 0,
        pressure: 0,
        width: 0,
        height: 0,
        tiltX: 0,
        tiltY: 0,
        toolX: 0,
        toolY: 0,
        toolWidth: 0,
        toolHeight: 0,
        rawX: 0,
        rawY: 0,
        toolType: ToolType.FINGER
      }
      const event: TouchEvent = {
        action: Action.DOWN,
        touch: touch,
        touches: [],
        sourceType: SourceType.TOUCH_SCREEN,
        id: 0,
        deviceId: 0,
        actionTime: 0,
        screenId: 0,
        windowId: 0
      }
      ohosTouchProcessor.onTouchEvent(event, null)
      done();
    })
  })
}
