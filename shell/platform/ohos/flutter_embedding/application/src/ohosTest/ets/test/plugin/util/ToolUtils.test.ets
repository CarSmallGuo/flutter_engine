

import { describe, it, expect } from '@ohos/hypium'
import { ToolUtils } from '@ohos/flutter_ohos';

export default function ToolUtilsTest() {
  describe('ToolUtilsTest', () => {
    it('isObj', 0, () => {
      const json = JSON.stringify('object');
      expect(ToolUtils.isObj(json)).assertFalse();
    });
  })
}