/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import { describe, it, expect } from '@ohos/hypium'
import { MessageChannelUtils } from '@ohos/flutter_ohos';

export default function MessageChannelUtilsTest() {
  describe('MessageChannelUtilsTest', () => {
    it('resizeChannelBuffer', 0, () => {
      expect(MessageChannelUtils.toString()).not().assertNull();
    });
  })
}