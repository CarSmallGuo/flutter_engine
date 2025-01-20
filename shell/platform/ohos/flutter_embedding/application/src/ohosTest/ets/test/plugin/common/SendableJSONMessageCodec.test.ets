
import { describe, it, expect } from '@ohos/hypium';
import SendableJSONMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableJSONMessageCodec';
import StringUtils from '@ohos/flutter_ohos/src/main/ets/util/StringUtils'

export default function SendableJSONMessageCodecTest() {
  describe('SendableJSONMessageCodecTest', () => {
    const codec = SendableJSONMessageCodec.INSTANCE;
    it('encodeMessage and decodeMessage with a null message', 0, ()  => {
      const originalMessage = null;
      const encodedMessage = codec.encodeMessage(originalMessage);
      const decodedMessage = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedMessage).assertEqual("");
    })
  })
}