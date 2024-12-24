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
import { abilityDelegatorRegistry } from '@kit.TestKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterAbilityAndEntryDelegate } from "@ohos/flutter_ohos";
import { Options } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroup';
import { Want } from '@kit.AbilityKit';

const bundleName = abilityDelegatorRegistry.getArguments().bundleName;

export default function FlutterAbilityAndEntryDelegateTest() {
  describe('FlutterAbilityAndEntryDelegateTest', () => {

    it('TestAddEntrypointOptions', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const options: Options = new Options(getContext());
      const result = flutterAbilityAndEntryDelegate.addEntrypointOptions(options);
      expect(result).not().assertNull();
      done();
    })

    it('TestShouldDispatchAppLifecycleState', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.shouldDispatchAppLifecycleState();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetFlutterNapi', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.getFlutterNapi();
      expect(result).assertNull();
      done();
    })

    it('TestOnSaveState', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const reason: AbilityConstant.StateType = AbilityConstant.StateType.CONTINUATION
      const want: Want = {
        bundleName: bundleName,
        abilityName: 'EntryAbility'
      }
      const wantParam: Record<string, Object> = {
        "testParams": want
      }
      const result = flutterAbilityAndEntryDelegate.onSaveState(reason, wantParam);
      expect(result).assertEqual(5);
      done();
    })

    it('TestIsFlutterEngineFromHost', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.isFlutterEngineFromHost();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetFlutterEngine', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.getFlutterEngine();
      expect(result).assertNull();
      done();
    })
  })
}