import { describe, it, expect } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { common, UIAbility, Want } from '@kit.AbilityKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterView, FlutterAbility, FlutterEngine, FlutterPlugin, FlutterShellArgs } from "@ohos/flutter_ohos";
import FlutterWindow from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/FlutterWindow';
import { Configuration } from '@ohos.app.ability.Configuration';



function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const context:Context = getContext()

const parameters: Record<string, Object> = {};

const flutterWindow:FlutterWindow = new FlutterWindow(context,parameters);

export default function FlutterWindowTest() {
  describe('FlutterWindowTest', () => {

    it('aboutToInit', 0, async (done: Function) => {
      flutterWindow.aboutToInit()
      done();
    })

    it('getFlutterView', 0, async (done: Function) => {
      const res:FlutterView = flutterWindow.getFlutterView()
      expect(res).not().assertNull()
      done();
    })

    it('getFlutterEngine', 0, async (done: Function) => {
      const res: FlutterEngine | null= flutterWindow.getFlutterEngine()
      expect(res).not().assertNull()
      done();
    })

    it('aboutToDestroy', 0, async (done: Function) => {
      flutterWindow.aboutToDestroy()
      done();
    })

    it('onPageHide', 0, async (done: Function) => {
      flutterWindow.onPageHide()
      done();
    })

    it('onBackPress', 0, async (done: Function) => {
      flutterWindow.onBackPress()
      done();
    })

    it('shouldDispatchAppLifecycleState', 0, async (done: Function) => {
      const res:boolean =  flutterWindow.shouldDispatchAppLifecycleState()
      expect(res).assertTrue()
      done();
    })

    it('detachFromFlutterEngine', 0, async (done: Function) => {
      flutterWindow.detachFromFlutterEngine()
      done();
    })

    it('getAbility', 0, async (done: Function) => {
      const res:UIAbility = flutterWindow.getAbility()
      expect(res).not().assertNull()
      done();
    })

    it('loadContent', 0, async (done: Function) => {
      flutterWindow.loadContent()
      done();
    })

    it('shouldAttachEngineToAbility', 0, async (done: Function) => {
      const res:boolean = flutterWindow.shouldAttachEngineToAbility()
      expect(res).not().assertNull()
      done();
    })

    it('getCachedEngineId', 0, async (done: Function) => {
      const res:string = flutterWindow.getCachedEngineId()
      expect(res).not().assertNull()
      done();
    })

    it('getCachedEngineGroupId', 0, async (done: Function) => {
      const res:string | null= flutterWindow.getCachedEngineGroupId()
      expect(res).assertNull()
      done();
    })

    it('shouldDestroyEngineWithHost', 0, async (done: Function) => {
      const res:boolean= flutterWindow.shouldDestroyEngineWithHost()
      expect(res).not().assertNull()
      done();
    })

    it('attachToEngineAutomatically', 0, async (done: Function) => {
      const res:boolean= flutterWindow.attachToEngineAutomatically()
      expect(res).assertTrue()
      done();
    })

    it('getFlutterShellArgs', 0, async (done: Function) => {
      const res:FlutterShellArgs= flutterWindow.getFlutterShellArgs()
      expect(res).not().assertNull()
      done();
    })

    it('getDartEntrypointArgs', 0, async (done: Function) => {
      const res:string[]= flutterWindow.getDartEntrypointArgs()
      expect(res).not().assertNull()
      done();
    })

    it('getDartEntrypointLibraryUri', 0, async (done: Function) => {
      const res:string= flutterWindow.getDartEntrypointLibraryUri()
      expect(res).not().assertNull()
      done();
    })

    it('getAppBundlePath', 0, async (done: Function) => {
      const res:string= flutterWindow.getAppBundlePath()
      expect(res).not().assertNull()
      done();
    })

    it('getDartEntrypointFunctionName', 0, async (done: Function) => {
      const res:string= flutterWindow.getDartEntrypointFunctionName()
      expect(res).not().assertNull()
      done();
    })

    it('getInitialRoute', 0, async (done: Function) => {
      const res:string= flutterWindow.getInitialRoute()
      expect(res).not().assertNull()
      done();
    })


    it('shouldRestoreAndSaveState', 0, async (done: Function) => {
      const res:boolean= flutterWindow.shouldRestoreAndSaveState()
      expect(res).not().assertNull()
      done();
    })

    it('getExclusiveAppComponent', 0, async (done: Function) => {
      flutterWindow.getExclusiveAppComponent()
      done();
    })

    it('provideFlutterEngine', 0, async (done: Function) => {
      const res:FlutterEngine | null= flutterWindow.provideFlutterEngine(context)
      expect(res).assertNull()
      done();
    })

    it('popSystemNavigator', 0, async (done: Function) => {
      const res:boolean= flutterWindow.popSystemNavigator()
      expect(res).assertFalse()
      done();
    })


  })

}
