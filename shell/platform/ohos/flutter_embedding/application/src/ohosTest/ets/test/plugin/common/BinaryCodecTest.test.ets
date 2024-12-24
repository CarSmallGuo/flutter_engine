// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import { BinaryCodec } from "@ohos/flutter_ohos";
import { buffer } from '@kit.ArkTS';

const binaryCodec: BinaryCodec = new BinaryCodec(true);

export default function BinaryCodecTest() {
  describe('BinaryCodecTest', () => {
    it('TestBinaryCodecEncodeMessageSend', 0, async (done: Function) => {
      let blobValue: buffer.Blob = new buffer.Blob(['name', 'age', 'sex']);
      let message: ArrayBuffer = await blobValue.arrayBuffer();
      const result: ArrayBuffer = binaryCodec.encodeMessage(message);
      expect(result).not().assertNull();
      done();
    })

    it('TestBinaryCodecDecodeMessage', 0, async (done: Function) => {
      const result = binaryCodec.decodeMessage(null);
      expect(result).not().assertNull();
      done();
    })
  })
}