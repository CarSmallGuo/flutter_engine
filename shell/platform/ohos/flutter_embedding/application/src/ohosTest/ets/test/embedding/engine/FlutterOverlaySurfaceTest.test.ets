
import { describe, beforeAll, expect, it } from '@ohos/hypium';
import { FlutterOverlaySurface } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterOverlaySurface';

export default function FlutterOverlaySurfaceTest() {
  let flutterOverlaySurface: FlutterOverlaySurface;

  describe('FlutterOverlaySurfaceTest', () => {

    beforeAll(() => {
      const id = 2356;
      flutterOverlaySurface = new FlutterOverlaySurface(id);
    })

    it('getId', 0, async (done: Function) => {
      console.info("uitest: getId begin");
      const id = flutterOverlaySurface.getId();
      expect(id).not().assertNull();
      done();
    })
  })
}