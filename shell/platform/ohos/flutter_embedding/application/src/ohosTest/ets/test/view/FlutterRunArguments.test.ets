
import { describe, it, expect } from '@ohos/hypium'
import { FlutterRunArguments } from '@ohos/flutter_ohos';

const TAG: string = "FlutterRunArgumentsTest";

export default function FlutterRunArgumentsTest() {
  describe('FlutterRunArgumentsTest', () => {
    it('flutterRunArguments', 0, () => {
      const flutterRunArguments = new FlutterRunArguments(TAG, TAG, TAG)
      expect(flutterRunArguments.bundlePath).assertEqual(TAG)
    })
  })
}