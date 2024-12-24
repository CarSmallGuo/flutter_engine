// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { createDVModelFromJson } from '@ohos/flutter_ohos/src/main/ets/view/DynamicView/dynamicViewJson';

const TAG: string = "dynamicViewJsonTest";

export default function dynamicViewJsonTest() {
  describe('dynamicViewJsonTest', () => {
    it('createDVModelFromJson', 0, () => {
      let dvModel = createDVModelFromJson(JSON.stringify(TAG));
      expect(dvModel)
        .not()
        .assertNull();
    })
  })
}