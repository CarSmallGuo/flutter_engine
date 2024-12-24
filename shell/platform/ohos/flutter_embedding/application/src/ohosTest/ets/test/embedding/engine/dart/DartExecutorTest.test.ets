// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, beforeAll, expect, it } from '@ohos/hypium';
import { common } from '@kit.AbilityKit';
import { Options } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroup';
import { DartEntrypoint } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartExecutor';
import { resourceManager } from '@kit.LocalizationKit';
import DartExecutor from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartExecutor';
import FlutterNapi from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterNapi';

export default function DartExecutorTest() {
  const flutterNapi = new FlutterNapi();
  let context: common.UIAbilityContext;
  let dartExecutor: DartExecutor;
  let options: Options;
  let assetManager: resourceManager.ResourceManager;
  let dartEntrypoint: DartEntrypoint | null = null;
  let resourceManager: resourceManager.ResourceManager;

  describe('DartExecutorTest', () => {

    beforeAll(() => {
      context = getContext() as common.UIAbilityContext;
      assetManager = context.resourceManager;
      resourceManager = context.resourceManager;
      dartExecutor = new DartExecutor(flutterNapi, assetManager);
      options = new Options(context);
      dartEntrypoint = options.getDartEntrypoint();
    })

    it('isExecutingDart', 0, async (done: Function) => {
      console.info("uitest: isExecutingDart begin");
      if (dartExecutor) {
        const isExecutingDart = dartExecutor.isExecutingDart();
        expect(isExecutingDart).not().assertNull();
      }
      done();
    })

    it('getBinaryMessenger', 0, async (done: Function) => {
      console.info("uitest: getBinaryMessenger begin");
      if (dartExecutor) {
        const binaryMessenger = dartExecutor.getBinaryMessenger();
        expect(binaryMessenger).not().assertNull();
      }
      done();
    })

    it('makeBackgroundTaskQueue', 0, async (done: Function) => {
      console.info("uitest: makeBackgroundTaskQueue begin");
      if (dartExecutor) {
        const taskQueue = dartExecutor.makeBackgroundTaskQueue();
        expect(taskQueue).not().assertNull();
      }
      done();
    })
  })
}