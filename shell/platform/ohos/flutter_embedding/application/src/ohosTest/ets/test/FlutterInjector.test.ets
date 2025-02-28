
import { describe, it, expect } from '@ohos/hypium'
import { FlutterInjector } from "@ohos/flutter_ohos";

export default function FlutterInjectorTest() {
  describe('FlutterInjectorTest', () => {
    it('getInstance', 0, () => {
      expect(FlutterInjector.getInstance()).not().assertNull();
    })
    it('getFlutterLoader', 0, () => {
      expect(FlutterInjector.getInstance().getFlutterLoader()).not().assertNull();
    })
    it('getFlutterNapi', 0, () => {
      expect(FlutterInjector.getInstance().getFlutterNapi()).not().assertNull();
    })
  })
}