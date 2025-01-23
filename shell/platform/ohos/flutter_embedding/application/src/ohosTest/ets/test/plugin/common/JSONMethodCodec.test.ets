
import { Any, JSONMethodCodec, MethodCall } from "@ohos/flutter_ohos";
import { describe, expect, it, MockKit, when } from "@ohos/hypium";

export default function JSONMethodCodecUnitTest() {
  describe('JSONMethodCodec', () => {
    it("JSONMethodCodec_encodeMethodCall", 0, encodeMethodCall);

    it("JSONMethodCodec_encodeSuccessEnvelope", 0, encodeSuccessEnvelope);

    it("JSONMethodCodec_decodeEnvelope", 0, decodeEnvelope);

  })
}

function encodeMethodCall() {
  let mocker = new MockKit();
  let ob = JSONMethodCodec.INSTANCE;
  let mockFunc = mocker.mockFunc(ob, ob.encodeMethodCall);
  let params = new MethodCall("key", "value");
  let r = new ArrayBuffer(4096);
  when(mockFunc)(params).afterReturn(r);
  let result = ob.encodeMethodCall(params);
  expect(result).assertEqual(r);
}

function encodeSuccessEnvelope() {
  let mocker = new MockKit();
  let ob = JSONMethodCodec.INSTANCE;
  let mockFunc = mocker.mockFunc(ob, ob.encodeSuccessEnvelope);
  let r = new ArrayBuffer(4096);
  when(mockFunc)(null).afterReturn(r);
  expect(ob.encodeSuccessEnvelope(null)).assertEqual(r);
}

function decodeEnvelope() {
  let mocker = new MockKit();
  let ob = JSONMethodCodec.INSTANCE;
  let mockFunc = mocker.mockFunc(ob, ob.decodeEnvelope);
  let r = new ArrayBuffer(4096);
  when(mockFunc)(r).afterReturn("any");
  let result: Any = ob.decodeEnvelope(r);
  expect(result).assertEqual("any");
}