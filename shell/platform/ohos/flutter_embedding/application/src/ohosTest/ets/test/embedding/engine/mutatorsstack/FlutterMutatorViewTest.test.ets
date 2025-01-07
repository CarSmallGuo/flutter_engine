/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import { FlutterMutatorView } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/mutatorsstack/FlutterMutatorView'
import { describe,it, expect } from '@ohos/hypium';
export default function FlutterMutatorViewTest() {
  describe('FlutterMutatorViewTest', () => {
    it('TestGetDvModel', 0, async (done: Function) => {
      let flutterMutatorView = new FlutterMutatorView();
      let dvModel = flutterMutatorView.getDvModel();
      expect(dvModel).not().assertNull();
      done();
    })
  })
}
