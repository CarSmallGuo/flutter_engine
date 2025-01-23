/*
Copyright (C) 2024 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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