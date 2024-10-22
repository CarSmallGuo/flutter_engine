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
import { describe, it, expect } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { ConfigurationConstant, Want } from '@kit.AbilityKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterView, FlutterAbility, FlutterEngine, FlutterPlugin } from "@ohos/flutter_ohos";
import { Configuration } from '@ohos.app.ability.Configuration';
import { FlutterPluginBinding } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/plugins/FlutterPlugin';

const delegator = abilityDelegatorRegistry.getAbilityDelegator()
const bundleName = abilityDelegatorRegistry.getArguments().bundleName;

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

export default function FlutterAbilityTest() {
  describe('FlutterAbilityTest', () => {
    it('FlutterAbilityTest', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const flutterView: FlutterView | null = flutterAbility.getFlutterView();
      expect(flutterView).not().assertNull();
      await sleep(1000);
      const pagePath: string = flutterAbility.pagePath();
      expect(pagePath).assertEqual("pages/Index");
      done();
    })

    it('TestFlutterAbilityOnCreate', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const want: Want = {
        bundleName: bundleName,
        abilityName: 'EntryAbility'
      }

      const launchParam: AbilityConstant.LaunchParam = {
        launchReason: AbilityConstant.LaunchReason.UNKNOWN,
        lastExitReason: AbilityConstant.LastExitReason.UNKNOWN,
        lastExitMessage: "test"
      }
      await delegator.startAbility(want);
      await sleep(1000);
      flutterAbility.onCreate(want, launchParam);
      done();
    })

    it('TestFlutterAbilityOnSaveState', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const reason: AbilityConstant.StateType = AbilityConstant.StateType.CONTINUATION
      const want: Want = {
        bundleName: bundleName,
        abilityName: 'EntryAbility'
      }
      const wantParam: Record<string, Object> = {
        "testParams": want
      }
      await sleep(1000);
      const result = flutterAbility.onSaveState(reason, wantParam);
      expect(result).not().assertNull();
      done();
    })

    it('TestOnNewWant', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const want: Want = {
        bundleName: bundleName,
        abilityName: 'EntryAbility'
      }

      const launchParam: AbilityConstant.LaunchParam = {
        launchReason: AbilityConstant.LaunchReason.UNKNOWN,
        lastExitReason: AbilityConstant.LastExitReason.UNKNOWN,
        lastExitMessage: "test"
      }
      flutterAbility.onNewWant(want, launchParam);
      done();
    })

    it('TestOnForegroundAndOnBackground', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.onForeground();
      await sleep(1000);
      flutterAbility.onBackground();
      done();
    })

    it('TestGetAbility', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.getAbility();
      done();
    })

    it('TestGetFlutterAbilityAndEntryDelegate', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getFlutterAbilityAndEntryDelegate();
      expect(result).not().assertNull();
      done();
    })

    it('TestShouldDispatchAppLifecycleState', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.shouldDispatchAppLifecycleState();
      expect(result).assertTrue();
      done();
    })

    it('TestProvideFlutterEngine', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.provideFlutterEngine(getContext());
      expect(result).assertNull();
      done();
    })

    it('TestProvidePlatformPlugin', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const flutterEngine = new FlutterEngine(getContext(), null, null, null);
      const result = flutterAbility.providePlatformPlugin(flutterEngine);
      expect(result).assertInstanceOf("PlatformPlugin");
      done();
    })

    it('TestGetFlutterShellArgs', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getFlutterShellArgs();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetDartEntrypointArgs', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getDartEntrypointArgs();
      expect(result).not().assertNull();
      done();
    })

    it('TestDetachFromFlutterEngine', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.detachFromFlutterEngine();
      done();
    })

    it('TestPopSystemNavigator', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.popSystemNavigator();
      expect(result).assertFalse();
      done();
    })

    it('TestShouldAttachEngineToAbility', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.shouldAttachEngineToAbility();
      expect(result).assertTrue();
      done();
    })

    it('TestGetDartEntrypointLibraryUri', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getDartEntrypointLibraryUri();
      expect(result).assertEqual("");
      done();
    })

    it('TestGetAppBundlePath', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getAppBundlePath();
      expect(result).assertEqual("");
      done();
    })

    it('TestGetDartEntrypointFunctionName', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getDartEntrypointFunctionName();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetInitialRoute', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getInitialRoute();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetWant', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getWant();
      expect(result).assertInstanceOf("Want");
      done();
    })

    it('TestShouldDestroyEngineWithHost', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.shouldDestroyEngineWithHost();
      expect(result).not().assertNull();
      done();
    })

    it('TestAttachToEngineAutomatically', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.attachToEngineAutomatically();
      expect(result).assertTrue();
      done();
    })

    it('TestShouldRestoreAndSaveState', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.shouldRestoreAndSaveState();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetExclusiveAppComponent', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getExclusiveAppComponent();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetCachedEngineId', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getCachedEngineId();
      expect(result).assertEqual("cached_engine_id");
      done();
    })

    it('TestGetCachedEngineGroupId', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getCachedEngineGroupId();
      expect(result).assertEqual("cached_engine_group_id");
      done();
    })

    it('TestAddPlugin', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.addPlugin(plugin);
      done();
    })

    it('TestRemovePlugin', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.removePlugin(plugin);
      done();
    })

    it('TestOnMemoryLevel', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const level = AbilityConstant.MemoryLevel.MEMORY_LEVEL_MODERATE
      flutterAbility.onMemoryLevel(level);
      done();
    })

    it('TestOnConfigurationUpdated', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const config: Configuration = {
        colorMode: ConfigurationConstant.ColorMode.COLOR_MODE_DARK,
      }
      flutterAbility.onConfigurationUpdated(config);
      done();
    })

    it('TestGetFlutterEngine', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getFlutterEngine();
      expect(result).not().assertNull();
      done();
    })

    it('TestOnWindowStageDestroyAndRelease', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      flutterAbility.onWindowStageDestroy();
      await sleep(1000);
      flutterAbility.release();
      done();
    })
  })

}


class TestFlutterPlugin implements FlutterPlugin {
  getUniqueClassName(): string {
    return "engine test";
  }

  onAttachedToEngine(binding: FlutterPluginBinding): void {
    console.info("uitest: onAttachedToEngine");
  }

  onDetachedFromEngine(binding: FlutterPluginBinding): void {
    console.info("uitest: onAttachedToEngine");
  }
}

let plugin: FlutterPlugin = new TestFlutterPlugin();
