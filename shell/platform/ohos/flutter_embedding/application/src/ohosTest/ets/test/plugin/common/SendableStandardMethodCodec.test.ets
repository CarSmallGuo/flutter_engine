// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import SendableStandardMethodCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableStandardMethodCodec';

export default function SendableStandardMethodCodecTest() {
  describe('SendableStandardMethodCodecTest', () => {
    const codec = SendableStandardMethodCodec.INSTANCE;
    it('decodeMethodCall_shouldThrowErrorOnCorruptedMethodCall', 0, () => {
      const corruptedBuffer = new ArrayBuffer(2);
      const bufferView = new Uint8Array(corruptedBuffer);
      bufferView[0] = 0;
      bufferView[1] = 1;

      try {
        codec.decodeMethodCall(corruptedBuffer);
        expect(false).assertEqual(true);
      } catch (e) {
        expect(e.message).assertEqual('Method call corrupted');
      }
    })
  })
}