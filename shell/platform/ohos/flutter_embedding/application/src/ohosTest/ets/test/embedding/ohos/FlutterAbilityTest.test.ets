import { describe, it, expect } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { Want } from '@kit.AbilityKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterView, FlutterAbility, FlutterEngine, FlutterPlugin } from "@ohos/flutter_ohos";
import { FlutterPluginBinding } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/plugins/FlutterPlugin';
import { Configuration } from '@ohos.app.ability.Configuration';

const bundleName = abilityDelegatorRegistry.getArguments().bundleName;
const want: Want = {
  bundleName: bundleName,
  abilityName: 'EntryAbility'
}

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

export default function FlutterAbilityTest() {
  describe('FlutterAbilityTest', () => {
    it('FlutterAbilityTest', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const flutterView: FlutterView | null = flutterAbility.getFlutterView();
      expect(flutterView).assertNull();
      await sleep(1000);
      const pagePath: string = flutterAbility.pagePath();
      expect(pagePath).assertEqual("pages/Index");
      done();
    })

    it('isDefaultFullScreen', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const isDefaultFullScreen: boolean = flutterAbility.isDefaultFullScreen();
      expect(isDefaultFullScreen).assertTrue();
      done();
    })

    it('onCreate', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onCreate(want, launchParam);
      } catch (e) {
      }
      done();
    })

    it('onDestroy', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onDestroy();
      } catch (e) {
      }
      done();
    })

    it('TestFlutterAbilityOnSaveState', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const reason: AbilityConstant.StateType = AbilityConstant.StateType.CONTINUATION
      const wantParam: Record<string, Object> = {
        "testParams": want
      }
      await sleep(1000);
      const result = flutterAbility.onSaveState(reason, wantParam);
      expect(result).not().assertNull();
      done();
    })

    it('onNewWant', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onNewWant(want, launchParam);
      } catch (e) {
      }
      done();
    })

    it('onWindowStageDestroy', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onWindowStageDestroy();
      } catch (e) {
      }
      done();
    })

    it('onForeground', 0, async (done: Function) => {

      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onForeground();
      } catch (e) {
      }
      done();
    })

    it('onBackground', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onBackground();
      } catch (e) {
      }
      done();
    })

    it('release', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.release();
      } catch (e) {
      }
      done();
    })

    it('getAbility', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getAbility();
      expect(result).not().assertUndefined();
      done();
    })

    it('TestGetWant', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getWant();
      expect(result).assertUndefined();
      done();
    })

    it('TestGetFlutterAbilityAndEntryDelegate', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getFlutterAbilityAndEntryDelegate();
      expect(result).assertNull();
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
      expect(result).not().assertNull();
      done();
    })

    it('configureFlutterEngine', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        const flutterEngine = new FlutterEngine(getContext(), null, null, null);
        flutterAbility.configureFlutterEngine(flutterEngine);
      } catch (e) {
      }
      done();
    })

    it('cleanUpFlutterEngine', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        const flutterEngine = new FlutterEngine(getContext(), null, null, null);
        flutterAbility.cleanUpFlutterEngine(flutterEngine);
      } catch (e) {
      }
      done();
    })

    it('detachFromFlutterEngine', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.detachFromFlutterEngine();
      } catch (e) {
      }
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

    it('TestAttachToEngineAutomatically', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.attachToEngineAutomatically();
      expect(result).assertTrue();
      done();
    })

    it('TestGetExclusiveAppComponent', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getExclusiveAppComponent();
      expect(result).assertNull();
      done();
    })

    it('addPlugin', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.addPlugin(plugin);
      } catch (e) {
      }
      done();
    })

    it('removePlugin', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.removePlugin(plugin);
      } catch (e) {
      }
      done();
    })

    it('onMemoryLevel', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onMemoryLevel(AbilityConstant.MemoryLevel.MEMORY_LEVEL_LOW);
      } catch (e) {
      }
      done();
    })

    it('onConfigurationUpdated', 0, async (done: Function) => {
      try {
        const flutterAbility: FlutterAbility = new FlutterAbility();
        flutterAbility.onConfigurationUpdated(configuration);
      } catch (e) {
      }
      done();
    })

    it('TestGetFlutterEngine', 0, async (done: Function) => {
      const flutterAbility: FlutterAbility = new FlutterAbility();
      const result = flutterAbility.getFlutterEngine();
      expect(result).assertNull();
      done();
    })
  })

}

export class TestFlutterPlugin implements FlutterPlugin {
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

export let plugin: FlutterPlugin = new TestFlutterPlugin();

export class TestLaunchParam implements AbilityConstant.LaunchParam {
  launchReason: AbilityConstant.LaunchReason;
  lastExitReason: AbilityConstant.LastExitReason;
  lastExitMessage: string;

  constructor(launchReason: AbilityConstant.LaunchReason, lastExitReason: AbilityConstant.LastExitReason,
    lastExitMessage: string) {
    this.launchReason = launchReason;
    this.lastExitReason = lastExitReason;
    this.lastExitMessage = lastExitMessage;
  }
}

export let launchParam: TestLaunchParam =
  new TestLaunchParam(AbilityConstant.LaunchReason.START_ABILITY, AbilityConstant.LastExitReason.NORMAL, '');

export class TestConfiguration implements Configuration {

}

let configuration: Configuration = new TestConfiguration();