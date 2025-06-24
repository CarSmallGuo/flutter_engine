import { describe, it, expect } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterAbilityAndEntryDelegate } from "@ohos/flutter_ohos";
import { Options } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineGroup';
import { Want } from '@kit.AbilityKit';
import { launchParam, plugin } from './FlutterAbilityTest.test';

const bundleName = abilityDelegatorRegistry.getArguments().bundleName;
const want: Want = {
  bundleName: bundleName,
  abilityName: 'EntryAbility'
}

export default function FlutterAbilityAndEntryDelegateTest() {
  describe('FlutterAbilityAndEntryDelegateTest', () => {

    it('onAttach', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        await flutterAbilityAndEntryDelegate.onAttach(getContext());
      } catch (e) {
      }
      done();
    })


    it('onRestoreInstanceState', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onRestoreInstanceState(want);
      } catch (e) {
      }
      done();
    })

    it('setupFlutterEngine', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.setupFlutterEngine();
      } catch (e) {
      }
      done();
    })

    it('TestAddEntrypointOptions', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const options: Options = new Options(getContext());
      const result = flutterAbilityAndEntryDelegate.addEntrypointOptions(options);
      expect(result).not().assertNull();
      done();
    })

    it('createView', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.createView(getContext());
      expect(result).not().assertNull();
      done();
    })

    it('release', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.release();
      } catch (e) {
      }
      done();
    })

    it('onDetach', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onDetach();
      } catch (e) {
      }
      done();
    })

    it('onLowMemory', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onLowMemory();
      } catch (e) {
      }
      done();
    })

    it('onCreate', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onCreate();
      } catch (e) {
      }
      done();
    })

    it('onWindowStageCreate', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onWindowStageCreate();
      } catch (e) {
      }
      done();
    })

    it('onWindowStageDestroy', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onWindowStageDestroy();
      } catch (e) {
      }
      done();
    })

    it('onWindowFocusChanged', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onWindowFocusChanged(true);
      } catch (e) {
      }
      done();
    })

    it('onForeground', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onForeground();
      } catch (e) {
      }
      done();
    })

    it('onBackground', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onBackground();
      } catch (e) {
      }
      done();
    })

    it('TestShouldDispatchAppLifecycleState', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.shouldDispatchAppLifecycleState();
      expect(result).not().assertNull();
      done();
    })

    it('ensureAlive', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.ensureAlive();
      } catch (e) {
      }
      done();
    })

    it('TestGetFlutterNapi', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.getFlutterNapi();
      expect(result).assertNull();
      done();
    })

    it('detachFromFlutterEngine', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.detachFromFlutterEngine();
      } catch (e) {
      }
      done();
    })

    it('onNewWant', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.onNewWant(want, launchParam);
      } catch (e) {
      }
      done();
    })

    it('TestOnSaveState', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const reason: AbilityConstant.StateType = AbilityConstant.StateType.CONTINUATION
      const want: Want = {
        bundleName: bundleName,
        abilityName: 'EntryAbility'
      }
      const wantParam: Record<string, Object> = {
        "testParams": want
      }
      const result = flutterAbilityAndEntryDelegate.onSaveState(reason, wantParam);
      expect(result).assertEqual(5);
      done();
    })

    it('addPlugin', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.addPlugin(plugin);
      } catch (e) {
      }
      done();
    })

    it('removePlugin', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.removePlugin(plugin);
      } catch (e) {
      }
      done();
    })

    it('TestIsFlutterEngineFromHost', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.isFlutterEngineFromHost();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetFlutterEngine', 0, async (done: Function) => {
      const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
      const result = flutterAbilityAndEntryDelegate.getFlutterEngine();
      expect(result).assertNull();
      done();
    })

    it('initWindow', 0, async (done: Function) => {
      try {
        const flutterAbilityAndEntryDelegate: FlutterAbilityAndEntryDelegate = new FlutterAbilityAndEntryDelegate();
        flutterAbilityAndEntryDelegate.initWindow();
      } catch (e) {
      }
      done();
    })
  })
}