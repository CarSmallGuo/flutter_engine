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
import FlutterEngineGroup, { Options } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroup';
import { FlutterEngine } from '@ohos/flutter_ohos';
import PlatformViewsController from '@ohos/flutter_ohos/src/main/ets/plugin/platform/PlatformViewsController'
import common from '@ohos.app.ability.common';

export default function FlutterEngineGroupTest() {
  const flutterEngineGroup = new FlutterEngineGroup();
  const platformViewsController = new PlatformViewsController();
  const context = getContext() as common.UIAbilityContext;
  const args: Array<string> = [];

  describe('FlutterEngineGroupTest', () => {

    it('checkLoader', 0, async (done: Function) => {
      console.info("uitest: checkLoader begin");
      const checkLoaderCallBack = flutterEngineGroup.checkLoader(context, args);
      expect(checkLoaderCallBack).not().assertNull();
      done();
    })

    it('createAndRunEngineByOptions', 0, async (done: Function) => {
      console.info("uitest: createAndRunEngineByOptions begin");
      const options = new Options(context);
      const createAndRunEngineByOptionsCallBack = flutterEngineGroup.createAndRunEngineByOptions(options);
      expect(createAndRunEngineByOptionsCallBack).not().assertNull();
      done();
    })

    it('createEngine', 0, async (done: Function) => {
      console.info("uitest: createEngine begin");
      if(context) {
        const fng: FlutterEngine = flutterEngineGroup.createEngine(context, platformViewsController);
        expect(fng).not().assertNull();
      }
      done();
    })
  })
}