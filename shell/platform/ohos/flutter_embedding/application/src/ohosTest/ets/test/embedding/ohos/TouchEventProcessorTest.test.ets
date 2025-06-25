
import { describe, it, expect } from '@ohos/hypium';
import TouchEventProcessor from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/TouchEventProcessor';
import { CustomTouchEvent } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/CustomTouchEvent';
import { util } from '@kit.ArkTS';

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

    it('postTouchEvent', 0, async (done: Function) => {
      console.info("uitest: decodeTouchPacket begin");
      const strings = ['test1', 'test2', 'test3'];
      touchEventProcessor.postTouchEvent(strings);
      done();
    })

    it('checkHitPlatformView False', 0, async (done: Function) => {
      const res:boolean = touchEventProcessor.checkHitPlatformView(2,2,1,1,0,0);
      expect(res).assertFalse();
      done();
    })

    it('checkHitPlatformView True', 0, async (done: Function) => {
      const res:boolean = touchEventProcessor.checkHitPlatformView(1,1,3,3,2,2);
      expect(res).assertTrue();
      done();
    })


  })
}

