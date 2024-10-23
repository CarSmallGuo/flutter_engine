import { FlutterMutatorView } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/mutatorsstack/FlutterMutatorView'
import { describe,it, expect } from '@ohos/hypium';
export default function FlutterMutatorViewTest() {
  describe('FlutterMutatorViewTest', () => {
    it('TestGetDvModel', 0, async (done: Function) => {
      let flutterMutatorView = new FlutterMutatorView();
      let dvModel = flutterMutatorView.getDvModel();
      expect(dvModel).not().assertNull();
      done();
    })
  })
}
