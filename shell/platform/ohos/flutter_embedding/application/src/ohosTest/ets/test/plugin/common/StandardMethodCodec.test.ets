// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium'
import StandardMethodCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/StandardMethodCodec';

export default function StandardMethodCodecTest() {
  describe('StandardMethodCodecTest', () => {
    const codec = StandardMethodCodec.INSTANCE;
    it('should throw an error for a corrupted method call', 0, () => {
      const corruptedMethodCall = new ArrayBuffer(2); // Just an invalid ArrayBuffer
      try {
        codec.decodeMethodCall(corruptedMethodCall);
        //fail('Expected an error to be thrown');
        throw new Error('Expected an error to be thrown');
      } catch (e) {
        expect(e.message).assertEqual('Method call corrupted');
      }
    });
  })
}