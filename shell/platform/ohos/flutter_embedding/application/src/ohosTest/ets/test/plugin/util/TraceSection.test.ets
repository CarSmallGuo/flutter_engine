// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { describe, it, expect } from '@ohos/hypium'
import { TraceSection } from '@ohos/flutter_ohos';

export default function TraceSectionTest() {
  describe('TraceSectionTest', () => {
    it('begin', 0, () => {
      const sectionName = "DartExecutor";
      expect(TraceSection.begin(sectionName)).assertLarger(0);
    });
    it('end', 0, () => {
      const sectionName = "DartExecutor";
      TraceSection.end(sectionName);
    });
    it('endWithId', 0, () => {
      const sectionName = "DartExecutor";
      TraceSection.endWithId(sectionName, 1);
    });
  })
}