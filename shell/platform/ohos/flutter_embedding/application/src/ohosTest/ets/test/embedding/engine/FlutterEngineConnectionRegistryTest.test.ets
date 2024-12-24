// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, beforeAll, it, expect } from '@ohos/hypium';
import { FlutterPlugin, } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/plugins/FlutterPlugin';
import FlutterEngine from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngine';
import PlatformViewsController from '@ohos/flutter_ohos/src/main/ets/plugin/platform/PlatformViewsController'
import common from '@ohos.app.ability.common';
import FlutterEngineConnectionRegistry from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineConnectionRegistry';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';

export default function FlutterEngineConnectionRegistryTest() {
  let flutterEngineConnectionRegistry: FlutterEngineConnectionRegistry;
  let flutterEngine: FlutterEngine;
  let context: common.UIAbilityContext;
  const platformViewsController = new PlatformViewsController();
  const pluginClassName = 'test';

  describe('FlutterEngineConnectionRegistryTest', () => {

    beforeAll(() => {
      context = getContext() as common.UIAbilityContext;
      flutterEngine = new FlutterEngine(context, null, null, platformViewsController);
      const flutterLoader = flutterEngine.getFlutterLoader();
      flutterEngineConnectionRegistry = new FlutterEngineConnectionRegistry(context, flutterEngine, flutterLoader);
    })

    it('has', 0, async (done: Function) => {
      console.info("uitest: has begin");
      if (flutterEngineConnectionRegistry) {
        const hasFlag = flutterEngineConnectionRegistry.has(pluginClassName);
        expect(hasFlag).not().assertNull();
      }
      done();
    })

    it('get', 0, async (done: Function) => {
      console.info("uitest: get begin");
      if (flutterEngineConnectionRegistry) {
        const name: FlutterPlugin = flutterEngineConnectionRegistry.get(pluginClassName);
        expect(name).not().assertNull();
      }
      done();
    })

    it('onSaveState', 0, async (done: Function) => {
      console.info("uitest: onSaveState begin");
      const reason = AbilityConstant.StateType.CONTINUATION;
      const wantParam: Record<string, object> = {};
      if (flutterEngineConnectionRegistry) {
        const result: AbilityConstant.OnSaveResult = flutterEngineConnectionRegistry.onSaveState(reason, wantParam);
        expect(result).not().assertNull();
      }
      done();
    })
  })
}