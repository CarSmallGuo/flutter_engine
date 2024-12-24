// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import { RawPointerCoords } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/RawPointerCoord';

const orientation: number = 100;
const pressure: number = 100;
const size: number = 100;
const toolMajor: number = 100;
const toolMinor: number = 100;
const touchMajor: number = 100;
const touchMinor: number = 100;
const x: number = 100;
const y: number = 100;
const rawPointerCoords: RawPointerCoords =
  new RawPointerCoords(orientation, pressure, size, toolMajor, toolMinor, touchMajor, touchMinor, x, y);

export default function RawPointerCoordsTest() {
  describe('RawPointerCoordsTest', () => {
    it('TestRawPointerCoordsGetX', 0, async (done: Function) => {
      const pointX = rawPointerCoords.getX();
      expect(pointX).not().assertNull();
      done();
    })

    it('TestRawPointerCoordsGetY', 0, async (done: Function) => {
      const pointY = rawPointerCoords.getY();
      expect(pointY).not().assertNull();
      done();
    })
  })
}