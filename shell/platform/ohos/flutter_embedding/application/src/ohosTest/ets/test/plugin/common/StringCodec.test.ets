/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
import { describe, it, expect } from '@ohos/hypium'
import StringCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/StringCodec'
import StringUtils from '@ohos/flutter_ohos/src/main/ets/util/StringUtils'

export default function StringCodecTest() {
  describe('StringCodecTest', () => {
    const codec = StringCodec.INSTANCE;
    it('encodeMessage_emptyString_ReturnEmptyString', 0, () => {
      const testString = "";
      const encodedMessage = codec.encodeMessage(testString);
      const decodedString = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedString).assertEqual(testString);
    })
    it('decodeMessage_emptyString_ReturnEmptyString', 0, () => {
      const encodedMessage = new ArrayBuffer(0);
      const decodedString = codec.decodeMessage(encodedMessage);
      expect(decodedString).assertEqual("");
    })
    it('decodeMessage_null_ReturnEmptyString', 0, () => {
      const decodedString = codec.decodeMessage(null);
      expect(decodedString).assertEqual("");
    })
  })
}