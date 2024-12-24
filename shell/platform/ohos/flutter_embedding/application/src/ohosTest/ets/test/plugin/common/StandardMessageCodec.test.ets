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
import StandardMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/StandardMessageCodec';

export default function StandardMessageCodecTest() {
  describe('StandardMessageCodecTest', () => {
    const codec = StandardMessageCodec.INSTANCE;
    it('should encode and decode null', 0, () => {
      const encoded = codec.encodeMessage(null);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertNull();
    })
    it('should encode and decode boolean', 0, () => {
      const encodedTrue = codec.encodeMessage(true);
      const decodedTrue:Any = codec.decodeMessage(encodedTrue);
      expect(decodedTrue).assertTrue();

      const encodedFalse = codec.encodeMessage(false);
      const decodedFalse:Any = codec.decodeMessage(encodedFalse);
      expect(decodedFalse).assertFalse();
    })
    it('should encode and decode number (int32)', 0, () => {
      const number = 123456;
      const encoded = codec.encodeMessage(number);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertEqual(number);
    })
    it('should encode and decode number (int64)', 0, () => {
      const number = BigInt(1234567890123456789);
      const encoded = codec.encodeMessage(number);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertEqual(number);
    })
    it('should encode and decode number (float64)', 0, ()=> {
      const number = 3.14159;;
      const encoded = codec.encodeMessage(number);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertClose(number,5)
    })
    it('should encode and decode Uint8Array', 0, () => {
      const array = new Uint8Array([1, 2, 3, 4, 5]);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertDeepEquals(array);
    })
    it('should encode and decode Int32Array', 0, () => {
      const array = new Int32Array([1, 2, 3, 4, 5]);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded).assertDeepEquals(array);
    })
    it('should encode and decode BigInt64Array', 0, () => {
      const array = new BigInt64Array([BigInt(1), BigInt(2), BigInt(3), BigInt(4), BigInt(5)]);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      expect(decoded.length).assertEqual(array.length);
      for (let i = 0; i < array.length; i++) {
        expect(decoded[i]).assertEqual(array[i]);
      }
    })
    it('should encode and decode Float32Array', 0, () => {
      const array = new Float32Array([1.1, 2.2, 3.3, 4.4, 5.5]);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      for (let i = 0; i < array.length; i++) {
        expect(decoded[i]).assertEqual(array[i]);
      }
    })
    it('should encode and decode Float64Array', 0, () => {
      const array = new Float64Array([1.1, 2.2, 3.3, 4.4, 5.5]);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      for (let i = 0; i < array.length; i++) {
        expect(decoded[i]).assertEqual(array[i]);
      }
    })
  })
}