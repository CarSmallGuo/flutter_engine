/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import { describe, it, expect } from '@ohos/hypium'
import { SendableBinaryCodec } from '@ohos/flutter_ohos';
import { buffer } from '@kit.ArkTS';

export default function SendableBinaryCodecTest() {
  describe('SendableBinaryCodecTest', () => {
    it('encodeMessage', 0, async (done: Function) => {
      let blobValue: buffer.Blob = new buffer.Blob(['name', 'age', 'sex']);
      let message: ArrayBuffer = await blobValue.arrayBuffer();
      expect(SendableBinaryCodec.INSTANCE_DIRECT.encodeMessage(message))
        .not()
        .assertNull();
      done();
    })

    it('decodeMessage', 0, () => {
      expect(SendableBinaryCodec.INSTANCE_DIRECT.decodeMessage(null))
        .not()
        .assertNull();
    });
  })
}