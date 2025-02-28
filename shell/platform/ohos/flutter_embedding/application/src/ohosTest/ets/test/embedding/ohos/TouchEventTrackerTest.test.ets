
import { describe, it, expect } from '@ohos/hypium';
import { TouchEventId, TouchEventTracker }   from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/TouchEventTracker';
import { TouchEvent } from '@ohos.multimodalInput.touchEvent';

export default function TouchEventTrackerTest() {
  const touchEventTracker: TouchEventTracker = new TouchEventTracker();

  describe('TouchEventTrackerTest', () => {

    it('getInstance', 0, async (done: Function) => {
      console.info("uitest: getInstance begin");
      const getInstance: TouchEventTracker  = TouchEventTracker.getInstance();
      expect(getInstance).not().assertNull();
      done();
    })

    it('track', 0, async (done: Function) => {
      console.info("uitest: decodeTouchPacket begin");
      const event: TouchEvent | null = null;
      if(event) {
        const decodeTouchPacket: TouchEventId = touchEventTracker.track(event);
        expect(decodeTouchPacket).not().assertNull();
      }
      done();
    })

    it('pop', 0, async (done: Function) => {
      console.info("uitest: pop begin");
      const eventId: TouchEventId | null = null;
      if(eventId) {
        const tuchEvent: TouchEvent = touchEventTracker.pop(eventId);
        expect(tuchEvent).not().assertNull();
      }
      done();
    })
  })
}