
import { describe, it, expect } from '@ohos/hypium';
import SendableJSONMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableJSONMessageCodec';
import StringUtils from '@ohos/flutter_ohos/src/main/ets/util/StringUtils'
import { Any } from '@ohos/flutter_ohos';

export default function SendableJSONMessageCodecTest() {
  describe('SendableJSONMessageCodecTest', () => {
    const codec = SendableJSONMessageCodec.INSTANCE;
    it('encodeMessage and decodeMessage with a null message', 0, ()  => {
      const originalMessage = null;
      const encodedMessage = codec.encodeMessage(originalMessage);
      const decodedMessage = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedMessage).assertEqual("");
    })

    it('decodeMessage', 0, ()  => {
      const originalMessage = null;
      codec.decodeMessage(originalMessage);
    })

    it('toBaseData null', 0, ()  => {
      const originalMessage = null;
      codec.toBaseData(originalMessage);
    })

    it('toBaseData List', 0, ()  => {
      const originalMessage = [1,1];
      codec.toBaseData(originalMessage);
    })

    it('toBaseData Array', 0, ()  => {
      const originalMessage:Array<Any> = ['1','a'];
      codec.toBaseData(originalMessage);
    })

    it('toBaseData Object', 0, ()  => {
      const originalMessage:Object = [1,1];
      codec.toBaseData(originalMessage);
    })

  })
}