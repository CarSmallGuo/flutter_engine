
import { describe, it, expect } from '@ohos/hypium'
import Any from '@ohos/flutter_ohos/src/main/ets/plugin/common/Any';
import StandardMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/StandardMessageCodec';
import { ByteBuffer } from '@ohos/flutter_ohos';

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
    it('should encode and decode bigint', 0, () => {
      const bigint =9123372036854775807;
      const encoded = codec.encodeMessage(bigint);
      const decoded:Any = codec.decodeMessage(encoded);
        expect(decoded).assertEqual(bigint);
    })
    it('should encode and decode Array', 0, () => {
      const array = new Array(1.1, 2.2, 3.3, 4.4, 5.5);
      const encoded = codec.encodeMessage(array);
      const decoded:Any = codec.decodeMessage(encoded);
      for (let i = 0; i < array.length; i++) {
        expect(decoded[i]).assertEqual(array[i]);
      }
    })
    it('writeAlignment', 0, () => {
      const buffer = ByteBuffer.from(new ArrayBuffer(16))
      buffer.writeInt32(0);
      codec.writeAlignment(buffer, 4);
      expect(buffer.byteOffset).assertEqual(4);
    })
    it('writeSize', 0, () => {
      const testBytes = new Uint8Array([0x10, 0x20, 0x30]);
      const buffer = ByteBuffer.from(new ArrayBuffer(64));
      codec.writeBytes(buffer, testBytes);
      const emptyBytes = new Uint8Array(0);
      const emptyBuffer = ByteBuffer.from(new ArrayBuffer(8));
      codec.writeBytes(emptyBuffer, emptyBytes);
    })
    it('readSize', 0, () => {
      const buf1 = ByteBuffer.from(new ArrayBuffer(1));
      buf1.writeUint8(100);
      codec.readSize(ByteBuffer.from(buf1.buffer));

      const buf2 = ByteBuffer.from(new ArrayBuffer(3));
      buf2.writeUint8(254);
      buf2.writeUint16(0, true);
      codec.readSize(ByteBuffer.from(buf2.buffer));

      const buf3 = ByteBuffer.from(new ArrayBuffer(5));
      buf3.writeUint8(255);
      buf3.writeUint32(0, true);
      codec.readSize(ByteBuffer.from(buf3.buffer));
    })
    it('readAlignment', 0, () => {
      const buf = ByteBuffer.from(new ArrayBuffer(8));
      buf.writeUint8(0);
      codec.readAlignment(buf, 4);
      expect(buf.byteOffset).assertEqual(4);
    });

    it('readValue', 0, () => {
      const originalBuf = ByteBuffer.from(new ArrayBuffer(1));
      originalBuf.writeUint8(0);
      const buf = ByteBuffer.from(originalBuf.buffer);
      const result:Any= codec.readValue(buf);
      expect(result).assertEqual(null);
    });

    it('readBytes', 0, () => {
      const originalBuf = ByteBuffer.from(new ArrayBuffer(5));
      originalBuf.writeUint8(3);
      originalBuf.writeUint8(1);
      originalBuf.writeUint8(2);
      originalBuf.writeUint8(3);
      const buf = ByteBuffer.from(originalBuf.buffer);
      const result = codec.readBytes(buf);
      expect(result.length).assertEqual(3);
      expect(result[0]).assertEqual(1);
      expect(result[1]).assertEqual(2);
      expect(result[2]).assertEqual(3);
    });
    const createBuffer = (bytes: number[]): ByteBuffer => {
      const buf = ByteBuffer.from(new ArrayBuffer(bytes.length));
      bytes.forEach(b => buf.writeUint8(b));
      return ByteBuffer.from(buf.buffer);
    };
    it('should read NULL', 0, () => {
      const result:Any = codec.readValueOfType(0, createBuffer([]));
      expect(result).assertEqual(null);
    });

    it('should read TRUE', 0, () => {
      const result:Any = codec.readValueOfType(1, createBuffer([]));
      expect(result).assertEqual(true);
    });

    it('should read FALSE', 0, () => {
      const result:Any = codec.readValueOfType(2, createBuffer([]));
      expect(result).assertEqual(false);
    });

    it('should read INT32', 0, () => {
      const buf = createBuffer([1, 0, 0, 0]);
      const result:Any = codec.readValueOfType(3, buf);
      expect(result).assertEqual(1);
    });

    it('should read INT64', 0, () => {
      const bigValue = 1n;
      const bytes = new Uint8Array(8);
      new DataView(bytes.buffer).setBigInt64(0, bigValue, true);
      const buf = createBuffer(Array.from(bytes));
      const result: Any = codec.readValueOfType(4, buf);
      expect(result).assertEqual(1);
    });

    it('should read UINT8_ARRAY', 0, () => {
      const buf = createBuffer([2, 65, 66]);
      const result: Any = codec.readValueOfType(8, buf);
      const actual = Array.from<number>(result);
      const expectArr = Array.from<number>(new Uint8Array([65, 66]));
      expect(actual.length).assertEqual(expectArr.length);
      actual.forEach((val, idx) => {
        expect(val).assertEqual(expectArr[idx]);
      });
    });

    it('should read INT32_ARRAY', 0, () => {
      const buf = createBuffer([1, 0, 0, 0, 100, 0, 0, 0]);
      const result: Any = codec.readValueOfType(9, buf);
      const actual = Array.from<number>(result);
      const expected = [100];
      expect(actual.length).assertEqual(expected.length);
      for (let i = 0; i < actual.length; i++) {
        expect(actual[i]).assertEqual(expected[i]);
      }
    });

  })
}