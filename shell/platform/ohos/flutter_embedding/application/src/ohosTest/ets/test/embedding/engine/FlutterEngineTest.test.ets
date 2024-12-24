// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, beforeAll, it, expect } from '@ohos/hypium';
import DartExecutor, { DartEntrypoint } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartExecutor';
import { Options } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroup';
import {
  AccessibilityChannel,
  FlutterLoader,
  FlutterNapi,
  FlutterRenderer,
  LifecycleChannel,
  LocalizationChannel,
  LocalizationPlugin,
  MouseCursorChannel,
  NavigationChannel,
  PlatformChannel,
  PluginRegistry,
  RestorationChannel,
  SettingsChannel,
  SystemChannel,
  TextInputChannel
} from '@ohos/flutter_ohos';
import ActivityControlSurface from '@ohos/flutter_ohos/src/main/ets/embedding/engine/plugins/ability/AbilityControlSurface';
import PlatformViewsController from '@ohos/flutter_ohos/src/main/ets/plugin/platform/PlatformViewsController';
import FlutterEngine from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngine';
import common from '@ohos.app.ability.common';

export default function FlutterEngineTest() {
  const platformViewsController = new PlatformViewsController();
  let context: common.UIAbilityContext;
  let flutterEngine: FlutterEngine;

  describe('FlutterEngineTest', () => {

    beforeAll(() => {
      const context = getContext() as common.UIAbilityContext;
      flutterEngine = new FlutterEngine(context, null, null, platformViewsController);
    })

    it('spawn', 0, async (done: Function) => {
      console.info("uitest: spawn begin");
      const options = new Options(context);
      let dartEntrypoint: DartEntrypoint | null = options.getDartEntrypoint();
      let initialRoute: string = options.getInitialRoute();
      let dartEntrypointArgs: Array<string> = options.getDartEntrypointArgs();
      let eg: FlutterEngine | null = null;
      if (dartEntrypoint) {
        eg = await flutterEngine.spawn(context,
          dartEntrypoint,
          initialRoute,
          dartEntrypointArgs,
          platformViewsController,
          false);
        if (eg) {
          expect(eg).not().assertNull();
        }
      }
      done();
    })

    it('getLifecycleChannel', 0, async (done: Function) => {
      console.info("uitest: getLifecycleChannel begin");
      if (flutterEngine) {
        const lifecycleChannel: LifecycleChannel | null = flutterEngine.getLifecycleChannel();
        if (lifecycleChannel) {
          expect(lifecycleChannel).not().assertNull();
        }
      }
      done();
    })

    it('getNavigationChannel', 0, async (done: Function) => {
      console.info("uitest: getNavigationChannel begin");
      if (flutterEngine) {
        const navigationChannel: NavigationChannel | null = flutterEngine.getNavigationChannel();
        if (navigationChannel) {
          expect(navigationChannel).not().assertNull();
        }
      }
      done();
    })

    it('getTextInputChannel', 0, async (done: Function) => {
      console.info("uitest: getTextInputChannel begin");
      if (flutterEngine) {
        const textInputChannel: TextInputChannel | null = flutterEngine.getTextInputChannel();
        if (textInputChannel) {
          expect(textInputChannel).not().assertNull();
        }
      }
      done();
    })

    it('getPlatformChannel', 0, async (done: Function) => {
      console.info("uitest: getPlatformChannel begin");
      if (flutterEngine) {
        const platformChannel: PlatformChannel | null = flutterEngine.getPlatformChannel();
        if (platformChannel) {
          expect(platformChannel).not().assertNull();
        }
      }
      done();
    })

    it('getSystemChannel', 0, async (done: Function) => {
      console.info("uitest: getSystemChannel begin");
      if (flutterEngine) {
        const systemChannel: SystemChannel | null = flutterEngine.getSystemChannel();
        if (systemChannel) {
          expect(systemChannel).not().assertNull();
        }
      }
      done();
    })

    it('getLocaleChannel', 0, async (done: Function) => {
      console.info("uitest: getLocaleChannel begin");
      if (flutterEngine) {
        const localizationChannel: LocalizationChannel | null = flutterEngine.getLocaleChannel();
        if (localizationChannel) {
          expect(localizationChannel).not().assertNull();
        }
      }
      done();
    })

    it('getMouseCursorChannel', 0, async (done: Function) => {
      console.info("uitest: getMouseCursorChannel begin");
      if (flutterEngine) {
        const mouseCursorChannel: MouseCursorChannel | null = flutterEngine.getMouseCursorChannel();
        if (mouseCursorChannel) {
          expect(mouseCursorChannel).not().assertNull();
        }
      }
      done();
    })

    it('getFlutterNapi', 0, async (done: Function) => {
      console.info("uitest: getFlutterNapi begin");
      if (flutterEngine) {
        const flutterNapi: FlutterNapi = flutterEngine.getFlutterNapi();
        expect(flutterNapi).not().assertNull();
      }
      done();
    })

    it('getFlutterRenderer', 0, async (done: Function) => {
      console.info("uitest: getFlutterRenderer begin");
      if (flutterEngine) {
        const flutterRenderer: FlutterRenderer = flutterEngine.getFlutterRenderer();
        expect(flutterRenderer).not().assertNull();
      }
      done();
    })

    it('getDartExecutor', 0, async (done: Function) => {
      console.info("uitest: getDartExecutor begin");
      if (flutterEngine) {
        const dartExecutor: DartExecutor = flutterEngine.getDartExecutor();
        expect(dartExecutor).not().assertNull();
      }
      done();
    })

    it('getPlugins', 0, async (done: Function) => {
      console.info("uitest: getPlugins begin");
      if (flutterEngine) {
        const pluginRegistry: PluginRegistry | null = flutterEngine.getPlugins();
        if (pluginRegistry) {
          expect(pluginRegistry).not().assertNull();
        }
      }
      done();
    })

    it('getAbilityControlSurface', 0, async (done: Function) => {
      console.info("uitest: getAbilityControlSurface begin");
      if (flutterEngine) {
        const activityControlSurface: ActivityControlSurface | null = flutterEngine.getAbilityControlSurface();
        if (activityControlSurface) {
          expect(activityControlSurface).not().assertNull();
        }
      }
      done();
    })

    it('getSettingsChannel', 0, async (done: Function) => {
      console.info("uitest: getSettingsChannel begin");
      if (flutterEngine) {
        const settingsChannel: SettingsChannel | null = flutterEngine.getSettingsChannel();
        if (settingsChannel) {
          expect(settingsChannel).not().assertNull();
        }
      }
      done();
    })

    it('getFlutterLoader', 0, async (done: Function) => {
      console.info("uitest: getFlutterLoader begin");
      if (flutterEngine) {
        const flutterLoader: FlutterLoader = flutterEngine.getFlutterLoader();
        expect(flutterLoader).not().assertNull();
      }
      done();
    })

    it('getRestorationChannel', 0, async (done: Function) => {
      console.info("uitest: getRestorationChannel begin");
      if (flutterEngine) {
        const restorationChannel: RestorationChannel | null = flutterEngine.getRestorationChannel();
        if (restorationChannel) {
          expect(restorationChannel).not().assertNull();
        }
      }
      done();
    })

    it('getAccessibilityChannel', 0, async (done: Function) => {
      console.info("uitest: getAccessibilityChannel begin");
      if (flutterEngine) {
        const accessibilityChannel: AccessibilityChannel | null = flutterEngine.getAccessibilityChannel();
        if (accessibilityChannel) {
          expect(accessibilityChannel).not().assertNull();
        }
      }
      done();
    })

    it('getLocalizationPlugin', 0, async (done: Function) => {
      console.info("uitest: getLocalizationPlugin begin");
      if (flutterEngine) {
        const localizationChannel: LocalizationPlugin | null = flutterEngine.getLocalizationPlugin();
        if (localizationChannel) {
          expect(localizationChannel).not().assertNull();
        }
      }
      done();
    })

    it('getPlatformViewsController', 0, async (done: Function) => {
      console.info("uitest: getPlatformViewsController begin");
      if (flutterEngine) {
        const platformViewsController: PlatformViewsController | null = flutterEngine.getPlatformViewsController();
        if (platformViewsController) {
          expect(platformViewsController).not().assertNull();
        }
      }
      done();
    })
  })
}