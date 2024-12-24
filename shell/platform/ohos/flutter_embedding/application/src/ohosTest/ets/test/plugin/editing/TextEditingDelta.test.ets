// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import { TextEditingDelta } from '@ohos/flutter_ohos/src/main/ets/plugin/editing/TextEditingDelta';

export default function TextEditingDeltaTest() {
  describe('TextEditingDeltaTest', () => {
    it('testToJSON', 0, () => {
      const delta = new TextEditingDelta("Old Text", 0, 0, 0, 0, 2, 5, "Delta Text");
      const json = delta.toJSON();
      expect(json.oldText).assertEqual("Old Text");
      expect(json.deltaText).assertEqual("Delta Text");
      expect(json.deltaStart).assertEqual(2);
      expect(json.deltaEnd).assertEqual(5);
      expect(json.selectionBase).assertEqual(0);
      expect(json.selectionExtent).assertEqual(0);
      expect(json.composingBase).assertEqual(0);
      expect(json.composingExtent).assertEqual(0);
    })
  })
}