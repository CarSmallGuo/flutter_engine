// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium'
import StringCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/SendableStringCodec'
import StringUtils from '@ohos/flutter_ohos/src/main/ets/util/StringUtils'

export default function SendableStringCodecTest() {
  describe('SendableStringCodecTest', () => {
    const codec = StringCodec.INSTANCE;
    it('encodeMessage_emptyString_ReturnEmptyString', 0, () => {
      const testString = "";
      const encodedMessage = codec.encodeMessage(testString);
      const decodedString = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedString).assertEqual(testString);
    })
    it('encodeMessage_null_ReturnEmptyString', 0, () => {
      const encodedMessage = codec.encodeMessage("");
      const decodedString = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedString).assertEqual("");
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