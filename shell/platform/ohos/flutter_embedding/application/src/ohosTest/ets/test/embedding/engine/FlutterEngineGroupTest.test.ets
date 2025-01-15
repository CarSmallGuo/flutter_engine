/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
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