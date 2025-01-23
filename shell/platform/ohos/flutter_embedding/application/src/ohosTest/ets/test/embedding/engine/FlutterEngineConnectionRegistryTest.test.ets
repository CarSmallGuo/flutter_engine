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