import { describe, it, expect } from '@ohos/hypium';
import { abilityDelegatorRegistry } from '@kit.TestKit';
import { common, UIAbility, Want } from '@kit.AbilityKit';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import { FlutterView, FlutterAbility, FlutterEngine, FlutterPlugin, FlutterShellArgs } from "@ohos/flutter_ohos";
import { KeyEventHandler } from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/KeyEventHandler';
import { Configuration } from '@ohos.app.ability.Configuration';


function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const context: Context = getContext()

const parameters: Record<string, Object> = {};

const keyEventHandler: KeyEventHandler = new KeyEventHandler();

export default function KeyEventHandlerTest() {
  describe('KeyEventHandlerTest', () => {

    it('getCharByEvent', 0, async (done: Function) => {
      const key: KeyEvent = {
        type: KeyType.Down,
        keyCode: 1,
        keyText: '',
        keySource: 0,
        deviceId: 0,
        metaKey: 0,
        timestamp: 0,
        intentionCode: 0,
        stopPropagation:()=>{
        }
      }
      const res: string = keyEventHandler.getCharByEvent(key)
      expect(res).not().assertNull()
      done();
    })

    it('startDeleting', 0, async (done: Function) => {
      keyEventHandler.startDeleting(0)
      keyEventHandler.startDeleting(2071)
      done();
    })

    it('endDeletion', 0, async (done: Function) => {
      keyEventHandler.endDeletion(0)
      keyEventHandler.endDeletion(2071)
      done();
    })

    it('handleKeyEvent', 0, async (done: Function) => {
      const key: KeyEvent = {
        type: KeyType.Down,
        keyCode: 1,
        keyText: '',
        keySource: 0,
        deviceId: 0,
        metaKey: 0,
        timestamp: 0,
        intentionCode: 0,
        stopPropagation:()=>{
        }
      }
      keyEventHandler.handleKeyEvent(key)
      done();
    })


  })

}
