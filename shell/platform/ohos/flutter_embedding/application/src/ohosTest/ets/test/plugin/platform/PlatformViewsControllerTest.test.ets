/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { describe, it, expect, beforeAll } from '@ohos/hypium';
import {
  PlatformViewsController,
  ByteBuffer,
  MethodResult,
  Any,
  DartExecutor,
  TextureRegistry,
  FlutterRenderer,
  FlutterNapi,
  FlutterView,
  TextInputPlugin,
  TextInputChannel,
  PlatformOverlayView,
  AccessibilityEventsDelegate,
  FlutterOverlaySurface,
} from "@ohos/flutter_ohos";
import {
  PlatformViewBufferResized,
  PlatformViewBufferSize,
  PlatformViewCreationRequest,
  PlatformViewResizeRequest,
  PlatformViewTouch
} from '@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/PlatformViewsChannel';
import { resourceManager } from '@kit.LocalizationKit';

const context = getContext();
const platformViewsController: PlatformViewsController = new PlatformViewsController();
const viewId: number = 1;
const viewType: string = 'test';
const logicalTop: number = 100;
const logicalLeft: number = 100;
const logicalWidth: number = 100;
const logicalHeight: number = 100;
const direction: Direction = 100;
const byteBuffer = new ByteBuffer();
const request: PlatformViewCreationRequest =
  new PlatformViewCreationRequest(viewId, viewType, logicalTop, logicalLeft, logicalWidth, logicalHeight, direction,
    byteBuffer);

const resizeRequest: PlatformViewResizeRequest =
  new PlatformViewResizeRequest(viewId, logicalWidth, logicalHeight);

const downTime: number = 100;
const eventTime: number = 100;
const action: number = 100;
const pointerCount: number = 100;
const rawPointerPropertiesList: Any = 100;
const rawPointerCoords: Any = 100;
const metaState: number = 100;
const buttonState: number = 100;
const xPrecision: number = 100;
const yPrecision: number = 100;
const deviceId: number = 100;
const edgeFlags: number = 100;
const source: number = 100;
const flags: number = 100;
const motionEventId: number = 100;
const touch: PlatformViewTouch =
  new PlatformViewTouch(viewId, downTime, eventTime, action, pointerCount, rawPointerPropertiesList,
    rawPointerCoords, metaState, buttonState, xPrecision, yPrecision, deviceId, edgeFlags, source, flags,
    motionEventId);
const flutterNapi: FlutterNapi = new FlutterNapi();
const textureRegistry: TextureRegistry = new FlutterRenderer(flutterNapi);
let dartExecutor: DartExecutor;

class ResizeCallback extends PlatformViewBufferResized {
  result: MethodResult | null = null;

  run(bufferSize: PlatformViewBufferSize) {
    if (bufferSize == null) {
      this.result?.error("error", "Failed to resize the platform view", null);
    } else {
      const response: Map<string, Any> = new Map();
      response.set("width", bufferSize.width);
      response.set("height", bufferSize.height);
      this.result?.success(response);
    }
  }
}

export default function PlatformViewsControllerTest() {
  describe('PlatformViewsControllerTest', () => {
    beforeAll(() => {
      const assetManager: resourceManager.ResourceManager = context.resourceManager;
      dartExecutor = new DartExecutor(flutterNapi, assetManager);
    })

    it('TestPlatformViewRegistryImplGetRegistry', 0, async (done: Function) => {
      const result = platformViewsController.getRegistry();
      expect(result).not().assertNull();
      done();
    })
    it('TestCreateForPlatformViewLayer', 0, async (done: Function) => {
      platformViewsController.createForPlatformViewLayer(request);
      done();
    })

    it('TestPlatformViewRegistryImplDispose', 0, async (done: Function) => {
      platformViewsController.dispose(viewId);
      done();
    })

    it('TestPlatformViewRegistryImplResize', 0, async (done: Function) => {
      const onComplete: PlatformViewBufferResized = new ResizeCallback();
      platformViewsController.resize(resizeRequest, onComplete);
      done();
    })

    it('TestPlatformViewRegistryImplOffset', 0, async (done: Function) => {
      platformViewsController.offset(viewId, logicalTop, logicalLeft);
      done();
    })

    it('TestPlatformViewRegistryImplOnBeginFrame', 0, async (done: Function) => {
      platformViewsController.onBeginFrame();
      done();
    })

    it('TestCreateOverlaySurfaceByPlatformOverlayView', 0, async (done: Function) => {
      const accessibilityEventsDelegate: AccessibilityEventsDelegate = new AccessibilityEventsDelegate();
      const imageView: PlatformOverlayView =
        new PlatformOverlayView(context, logicalWidth, logicalHeight, accessibilityEventsDelegate);
      platformViewsController.createOverlaySurfaceByPlatformOverlayView(imageView);
      done();
    })

    it('TestCreateOverlaySurface', 0, async (done: Function) => {
      const result: FlutterOverlaySurface = platformViewsController.createOverlaySurface();
      expect(result).not().assertNull();
      done();
    })

    it('TestPlatformViewRegistryImplOnTouch', 0, async (done: Function) => {
      platformViewsController.onTouch(touch);
      done();
    })

    it('TestPlatformViewRegistryImplSetDirection', 0, async (done: Function) => {
      platformViewsController.setDirection(viewId, direction);
      done();
    })

    it('TestPlatformViewRegistryImplValidateDirection', 0, async (done: Function) => {
      const result = platformViewsController.validateDirection(direction);
      expect(result).assertInstanceOf('boolean');
      done();
    })

    it('TestPlatformViewRegistryImplClearFocus', 0, async (done: Function) => {
      platformViewsController.clearFocus(viewId);
      done();
    })

    it('TestPlatformViewRegistryImplCreateForTextureLayer', 0, async (done: Function) => {
      const result = platformViewsController.createForTextureLayer(request);
      expect(result).not().assertNull();
      done();
    })

    it('TestPlatformViewRegistryImplAttach', 0, async (done: Function) => {
      platformViewsController.attach(context, textureRegistry, dartExecutor);
      done();
    })

    it('TestPlatformViewRegistryImplAttachTextInputPlugin', 0, async (done: Function) => {
      const textInputChannel: TextInputChannel = new TextInputChannel(dartExecutor);
      const textInputPlugin: TextInputPlugin = new TextInputPlugin(textInputChannel);
      platformViewsController.attachTextInputPlugin(textInputPlugin);
      done();
    })

    it('TestPlatformViewRegistryImplAttachToView', 0, async (done: Function) => {
      const newFlutterView: FlutterView = new FlutterView(viewId + '', context);
      platformViewsController.attachToView(newFlutterView);
      done();
    })

    it('TestPlatformViewRegistryImplGetFlutterView', 0, async (done: Function) => {
      const result = platformViewsController.getFlutterView();
      expect(result).not().assertNull();
      done();
    })

    it('TestPlatformViewRegistryImplDetach', 0, async (done: Function) => {
      platformViewsController.detach();
      done();
    })

    it('TestPlatformViewRegistryImplDetachFromView', 0, async (done: Function) => {
      platformViewsController.detachFromView();
      done();
    })

    it('TestPlatformViewRegistryImplDetachTextInputPlugin', 0, async (done: Function) => {
      platformViewsController.detachTextInputPlugin();
      done();
    })
  })

}
