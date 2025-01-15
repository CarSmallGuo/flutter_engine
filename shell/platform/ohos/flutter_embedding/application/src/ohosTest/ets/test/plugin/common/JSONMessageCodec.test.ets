/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */
import { describe, it, expect } from '@ohos/hypium'
import JSONMessageCodec from '@ohos/flutter_ohos/src/main/ets/plugin/common/JSONMessageCodec';
import StringUtils from '@ohos/flutter_ohos/src/main/ets/util/StringUtils';

export default function JSONMessageCodecTest() {
  describe('JSONMessageCodecTest', () => {
    const codec = JSONMessageCodec.INSTANCE;
    it('should encode and decode a null message', 0, () => {
      const originalMessage = null;
      const encodedMessage = codec.encodeMessage(originalMessage);
      const decodedMessage = StringUtils.arrayBufferToString(encodedMessage);
      expect(decodedMessage).assertEqual("");
    })
  })
}