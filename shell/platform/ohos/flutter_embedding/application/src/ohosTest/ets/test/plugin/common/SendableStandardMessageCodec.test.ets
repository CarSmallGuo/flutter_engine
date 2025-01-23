/*
Copyright (C) 2024 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import { describe, it, expect } from '@ohos/hypium'
import Any from '@ohos/flutter_ohos/src/main/ets/plugin/common/Any';
import SendableStandardMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableStandardMessageCodec';

export default function SendableStandardMessageCodecTest() {
  describe('SendableStandardMessageCodecTest', () => {
    const codec = SendableStandardMessageCodec.INSTANCE;

    it('testEncodeDecodeNull', 0, () => {
      const encoded = codec.encodeMessage(null);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertNull();
    })

    it('testEncodeDecodeBoolean', 0, () => {
      const encodedTrue = codec.encodeMessage(true);
      const decodedTrue:Any = codec.decodeMessage(encodedTrue);
      expect(decodedTrue).assertTrue();

      const encodedFalse = codec.encodeMessage(false);
      const decodedFalse:Any = codec.decodeMessage(encodedFalse);
      expect(decodedFalse).assertFalse();
    })

    it('testEncodeDecodeNumber', 0, () => {
      const int32Value = 123456;
      const encodedInt32 = codec.encodeMessage(int32Value);
      const decodedInt32:Any = codec.decodeMessage(encodedInt32);
      expect(decodedInt32).assertEqual(int32Value);

      const int64Value = BigInt(9007199254740991); // Max safe integer
      const encodedInt64 = codec.encodeMessage(int64Value);
      const decodedInt64:Any  = codec.decodeMessage(encodedInt64);
      expect(decodedInt64).assertEqual(int64Value);

      const float64Value = 3.14159;
      const encodedFloat64 = codec.encodeMessage(float64Value);
      const decodedFloat64:Any = codec.decodeMessage(encodedFloat64);
      expect(decodedFloat64).assertClose(float64Value, 5);

    })

    it('testEncodeDecodeUint8Array', 0, () => {
      const uint8Array = new Uint8Array([1, 2, 3, 4, 5]);
      const encodedArray = codec.encodeMessage(uint8Array);
      const decodedArray:Any  = codec.decodeMessage(encodedArray);
      expect(decodedArray).assertDeepEquals(uint8Array);
    })

    it('testEncodeDecodeInt32Array', 0, () => {
      const int32Array = new Int32Array([1, 2, 3, 4, 5]);
      const encodedArray = codec.encodeMessage(int32Array);
      const decodedArray:Any  = codec.decodeMessage(encodedArray);
      expect(decodedArray).assertDeepEquals(int32Array);
    })

    it('testEncodeDecodeBigInt64Array', 0, () => {
      const bigInt64Array = new BigInt64Array([BigInt(1), BigInt(2), BigInt(3), BigInt(4), BigInt(5)]);
      const encodedArray = codec.encodeMessage(bigInt64Array);
      const decodedArray:Any  = codec.decodeMessage(encodedArray);
      expect(decodedArray.length).assertEqual(bigInt64Array.length);
      for (let i = 0; i < bigInt64Array.length; i++) {
        expect(decodedArray[i]).assertEqual(bigInt64Array[i]);
      }
    })

    it('testEncodeDecodeFloat64Array', 0, () => {
      const float64Array = new Float64Array([1.1, 2.2, 3.3, 4.4, 5.5]);
      const encodedArray = codec.encodeMessage(float64Array);
      const decodedArray:Any  = codec.decodeMessage(encodedArray);
      expect(decodedArray).assertDeepEquals(float64Array);
    })
  })
}