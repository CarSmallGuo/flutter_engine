// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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