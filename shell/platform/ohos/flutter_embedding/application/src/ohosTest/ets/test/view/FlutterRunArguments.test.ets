/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import { describe, it, expect } from '@ohos/hypium'
import { FlutterRunArguments } from '@ohos/flutter_ohos';

const TAG: string = "FlutterRunArgumentsTest";

export default function FlutterRunArgumentsTest() {
  describe('FlutterRunArgumentsTest', () => {
    it('flutterRunArguments', 0, () => {
      const flutterRunArguments = new FlutterRunArguments(TAG, TAG, TAG)
      expect(flutterRunArguments.bundlePath).assertEqual(TAG)
    })
  })
}