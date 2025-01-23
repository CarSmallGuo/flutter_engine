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

import FlutterNapi from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterNapi';
import ApplicationInfoLoader from '@ohos/flutter_ohos/src/main/ets/embedding/engine/loader/ApplicationInfoLoader';
import { FlutterRenderer } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/renderer/FlutterRenderer';
import { common } from '@kit.AbilityKit';
import { describe, beforeAll, it, expect } from '@ohos/hypium';
import { EngineLifecycleListener } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngine'
import { FlutterApplicationInfo, FlutterCallbackInformation } from '@ohos/flutter_ohos';
import { resourceManager } from '@kit.LocalizationKit';

//jit产物默认kenel文件
const DEFAULT_KERNEL_BLOB = "kernel_blob.bin";
//文件路径分隔符
const FILE_SEPARATOR = "/";

export default function FlutterNapiTest() {
  const flutterNapi = new FlutterNapi();
  const context = getContext() as common.UIAbilityContext;
  let textureId = 0;
  let initResult: InitResult;
  let snapshotAssetPath: string;
  let flutterApplicationInfo: FlutterApplicationInfo;
  let bundlePath: string;
  let event: PanGestureEvent;
  let assetManager: resourceManager.ResourceManager;

  describe('FlutterNapiTest', () => {

    beforeAll(() => {
      if (context) {
        initResult = new InitResult(
          `${context.filesDir}/`,
          `${context.cacheDir}/`,
          `${context.filesDir}`);
        snapshotAssetPath = initResult!.dataDirPath + FILE_SEPARATOR + flutterApplicationInfo!.flutterAssetsDir;
        bundlePath = snapshotAssetPath + FILE_SEPARATOR + DEFAULT_KERNEL_BLOB;
        flutterApplicationInfo = ApplicationInfoLoader.load(context);
        assetManager = context.resourceManager;
      }
      const target: EventTarget = new CustomEventTarget(new CustomArea(0, 0, {
        x: 0, y: 0
      }, {
        x: 0, y: 0
      }));
      const renderer = new FlutterRenderer(flutterNapi);
      textureId = renderer.getTextureId();
      event = {
        offsetX: 50,
        offsetY: 50,
        velocityX: 50,
        velocityY: 50,
        fingerList: [],
        velocity: 0,
        target: target,
        timestamp: 0,
        source: SourceType.Unknown,
        pressure: 0,
        tiltX: 0,
        tiltY: 0,
        sourceTool: SourceTool.Unknown
      }
    })

    it('checkImplemented', 0, async (done: Function) => {
      console.info("uitest: checkImplemented begin");
      const methodName = 'test';
      const flag = flutterNapi.checkImplemented(methodName);
      expect(flag).not().assertNull();
      done();
    })

    it('nativeLookupCallbackInformation', 0, async (done: Function) => {
      console.info("uitest: nativeLookupCallbackInformation begin");
      try {
        const handle = 1;
        const flutterCallbackInformation: FlutterCallbackInformation | null =
          FlutterNapi.nativeLookupCallbackInformation(handle);
        if (flutterCallbackInformation) {
          expect(flutterCallbackInformation).not().assertNull();
        }
      } catch (e) {
      }
      done();
    })

    it('isAttached', 0, async (done: Function) => {
      console.info("uitest: isAttached begin");
      const flag: boolean = flutterNapi.isAttached();
      expect(flag).not().assertNull();
      done();
    })

    it('spawn', 0, async (done: Function) => {
      console.info("uitest: spawn begin");
      const entrypointFunctionName = 'test1';
      const pathToEntrypointFunction = 'test2';
      const initialRoute = 'test3';
      const entrypointArgs = ['test4'];
      try {
        const fnapi: FlutterNapi =
          flutterNapi.spawn(entrypointFunctionName, pathToEntrypointFunction, initialRoute, entrypointArgs);
        expect(fnapi).not().assertNull();
      } catch (e) {
      }
      done();
    })

    it('computePlatformResolvedLocale', 0, async (done: Function) => {
      console.info("uitest: computePlatformResolvedLocale begin");
      const strings = [];
      const arr = flutterNapi.computePlatformResolvedLocale(strings);
      expect(arr).not().assertNull();
      done();
    })

    it('registerTexture', 0, async (done: Function) => {
      console.info("uitest: registerTexture begin");
      const id = flutterNapi.registerTexture(textureId);
      expect(id).not().assertNull();
      done();
    })
  })
}

class InitResult {
  appStoragePath: string;
  engineCachesPath: string;
  dataDirPath: string;

  constructor(appStoragePath: string,
    engineCachesPath: string,
    dataDirPath: string) {
    this.appStoragePath = appStoragePath;
    this.engineCachesPath = engineCachesPath;
    this.dataDirPath = dataDirPath;
  }
}

class CurEngineLifecycleListener implements EngineLifecycleListener {
  onPreEngineRestart() {

  }

  onEngineWillDestroy() {

  }
}

class CustomEventTarget implements EventTarget {
  area: Area = new CustomArea(0, 0, { x: 0, y: 0 }, { x: 0, y: 0 });

  constructor(area: Area) {
    this.area = area;
  }
}

class CustomArea implements Area {
  width: Length = 0;
  height: Length = 0;
  position: Position = { x: 0, y: 0 };
  globalPosition: Position = { x: 0, y: 0 };

  constructor(width: Length, height: Length, position: Position, globalPosition: Position) {
    this.width = width;
    this.height = height;
    this.position = position;
    this.globalPosition = globalPosition;
  }
}