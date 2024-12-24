// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { StringUtils } from '@ohos/flutter_ohos';

const TAG: string = "stringUtilsTest";

export default function StringUtilsTest() {
  describe('StringUtilsTest', () => {
    it('isNotEmpty', 0, () => {
      expect(StringUtils.isNotEmpty(TAG)).assertTrue();
    });
    it('isEmpty', 0, () => {
      expect(StringUtils.isEmpty("")).assertTrue();
    });
  })
}