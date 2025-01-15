/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import FlutterApplicationInfo from '@ohos/flutter_ohos/src/main/ets/embedding/engine/loader/FlutterApplicationInfo'
import { describe, it, expect } from '@ohos/hypium';
export default function FlutterApplicationInfoTest() {
  describe('FlutterApplicationInfoTest', () => {
    it('TestLoad', 0, async (done: Function) => {
      let flutterApplicationInfo = new FlutterApplicationInfo("test", "test", "test", "test", "test", "test", false)
      expect(flutterApplicationInfo).not().assertNull();
      done();
    })
  })
}
