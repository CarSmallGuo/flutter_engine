import { describe, it, expect } from '@ohos/hypium';
import { FlutterEngine, FlutterPlugin, FlutterEntry } from "@ohos/flutter_ohos";
import { EmbeddingNodeController } from '@ohos/flutter_ohos/src/main/ets/embedding/ohos/EmbeddingNodeController';
import { common } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const controller:EmbeddingNodeController = new EmbeddingNodeController()

export default function EmbeddingNodeControllerTest() {
  describe('EmbeddingNodeControllerTest', () => {

    it('getBuilderNode', 0, async (done: Function) => {
      const res = controller.getBuilderNode()
      expect(res).assertNull()
      done();
    })

    it('updateNode', 0, async (done: Function) => {
      const arg:Object = []
      controller.updateNode(arg)
      done();
    })

    it('getEmbedId', 0, async (done: Function) => {
      const res = controller.getEmbedId()
      expect(res).not().assertNull()
      done();
    })

    it('setDestroy true', 0, async (done: Function) => {
      controller.setDestroy(true)
      done();
    })

    it('setDestroy false', 0, async (done: Function) => {
      controller.setDestroy(false)
      done();
    })

    it('disposeFrameNode', 0, async (done: Function) => {
      controller.disposeFrameNode()
      done();
    })

    it('postEvent', 0, async (done: Function) => {
      const res:boolean = controller.postEvent(undefined,false)
      expect(res).assertFalse()
      done();
    })




  })
}

