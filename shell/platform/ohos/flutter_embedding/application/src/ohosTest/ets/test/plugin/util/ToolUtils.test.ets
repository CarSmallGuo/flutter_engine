// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { ToolUtils } from '@ohos/flutter_ohos';

export default function ToolUtilsTest() {
  describe('ToolUtilsTest', () => {
    it('isObj', 0, () => {
      const json = JSON.stringify('object');
      expect(ToolUtils.isObj(json)).assertFalse();
    });
  })
}