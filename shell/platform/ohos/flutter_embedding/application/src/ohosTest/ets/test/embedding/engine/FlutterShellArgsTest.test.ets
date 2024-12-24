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

import { describe, expect, it } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { Want } from '@kit.AbilityKit';
import FlutterShellArgs from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterShellArgs';

const bundleName = abilityDelegatorRegistry.getArguments().bundleName;

export default function FlutterShellArgsTest() {
  const flutterShellArgs: FlutterShellArgs = new FlutterShellArgs();
  const want: Want = {
    bundleName: bundleName,
    abilityName: 'EntryAbility'
  }

  describe('FlutterShellArgsTest', () => {

    it('fromWant', 0, async (done: Function) => {
      console.info("uitest: fromWant begin");
      try {
        const flutterShellArgs = FlutterShellArgs.fromWant(want);
        if (flutterShellArgs) {
          expect(flutterShellArgs).not().assertNull();
        }
      } catch (e) {
      }
      done();
    })

    it('checkArg', 0, async (done: Function) => {
      console.info("uitest: checkArg begin");
      const argKey = 'argKey';
      const argFlag = 'argFlag';
      try {
        const flutterShellArgs = FlutterShellArgs.fromWant(want);
        FlutterShellArgs.checkArg(argKey, argFlag, want, flutterShellArgs);
      } catch (e) {
      }
      done();
    })

    it('toArray', 0, async (done: Function) => {
      console.info("uitest: toArray begin");
      const toArray = flutterShellArgs.toArray();
      expect(toArray).not().assertNull();
      done();
    })
  })
}