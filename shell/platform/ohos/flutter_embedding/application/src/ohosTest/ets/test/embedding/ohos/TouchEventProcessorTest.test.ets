/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
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
      const decodeTouchPacket: CustomTouchEvent = touchEventProcessor.constureCustomTouchEvent(strings, top, left);
      expect(decodeTouchPacket).not().assertNull();
      done();
    })
  })
}