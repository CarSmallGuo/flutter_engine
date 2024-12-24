// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { PathUtils } from '@ohos/flutter_ohos';

export default function PathUtilsTest() {
  describe('PathUtilsTest', () => {
    it('getFilesDir', 0, () => {
      expect(PathUtils.getFilesDir(getContext())).not().assertNull();
    });
    it('getCacheDirectory', 0, () => {
      expect(PathUtils.getCacheDirectory(getContext())).not().assertNull();
    });
    it('getDataDirectory', 0, () => {
      const name = "flutter";
      expect(PathUtils.getDataDirectory(getContext())).assertContain(name);
    });
  })
}