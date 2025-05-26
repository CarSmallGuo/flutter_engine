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
import { describe, it, expect } from '@ohos/hypium';
import TouchEventProcessor from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/TouchEventProcessor';
import { CustomTouchEvent } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/CustomTouchEvent';

export default function TouchEventProcessorTest() {
  const touchEventProcessor: TouchEventProcessor = new TouchEventProcessor();

  describe('TouchEventProcessorTest', () => {

    it('getInstance', 0, async (done: Function) => {
      console.info("uitest: getInstance begin");
      const getInstance: TouchEventProcessor = TouchEventProcessor.getInstance();
      expect(getInstance).not().assertNull();
      done();
    })

    it('decodeTouchPacket', 0, async (done: Function) => {
      console.info("uitest: decodeTouchPacket begin");
      const strings = ['test1', 'test2', 'test3'];
      const top = 10;
      const left = 10;
      const decodeTouchPacket: CustomTouchEvent = touchEventProcessor.constructCustomTouchEvent(strings, top, left);
      expect(decodeTouchPacket).not().assertNull();
      done();
    })
  })
}