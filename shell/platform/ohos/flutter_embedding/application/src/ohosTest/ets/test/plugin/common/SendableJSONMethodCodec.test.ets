
import { describe, it, expect } from '@ohos/hypium';
import SendableJSONMethodCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableJSONMethodCodec';

export default function SendableJSONMethodCodecTest() {
  describe('SendableJSONMethodCodecTest', () => {
    const codec = SendableJSONMethodCodec.INSTANCE;
    it('throws an error for invalid JSON', 0, () => {
      const invalidJSON = new ArrayBuffer(1);

      try {
        codec.decodeMethodCall(invalidJSON);
        //fail('Expected an error to be thrown');
        throw new Error('Expected an error to be thrown');
      } catch (e) {
        expect(e.message).assertContain('Invalid JSON')
      }

      try {
        codec.decodeEnvelope(invalidJSON);
        //fail('Expected an error to be thrown');
        throw new Error('Expected an error to be thrown');
      } catch (e) {
        expect(e.message).assertContain('Invalid JSON')
      }
    })
  })
}