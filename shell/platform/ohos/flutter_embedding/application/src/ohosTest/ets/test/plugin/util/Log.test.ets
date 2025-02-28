
import { describe, it, expect } from '@ohos/hypium'
import { Log } from '@ohos/flutter_ohos';

export default function LogTest() {
  describe('LogTest', () => {
    it('Log', 0, () => {
      expect(Log.toString()).not().assertNull();
    });
  })
}