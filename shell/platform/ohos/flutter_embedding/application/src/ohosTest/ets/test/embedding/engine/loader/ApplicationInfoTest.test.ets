// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import ApplicationInfoLoader from '@ohos/flutter_ohos/src/main/ets/embedding/engine/loader/ApplicationInfoLoader'
import { describe,it, expect } from '@ohos/hypium';
export default function ApplicationInfoTest() {
  describe('ApplicationInfoTest', () => {
    it('TestLoad', 0, async (done: Function) => {
      let context: common.Context = getContext();
      let applicationInfo = ApplicationInfoLoader.load(context)
      expect(applicationInfo).not().assertNull();
      done();
    })
  })
}
