
import { describe, it, expect } from '@ohos/hypium';
import { PlatformViewWrapper, OhosTouchProcessor } from "@ohos/flutter_ohos";
import { DVModel, DVModelParameters } from '@ohos/flutter_ohos/src/main/ets/view/DynamicView/dynamicView';

function sleep(time: number) {
  return new Promise<void>((resolve: Function) => setTimeout(resolve, time));
}

const platformViewWrapper: PlatformViewWrapper = new PlatformViewWrapper();

export default function PlatformViewWrapperTest() {
  describe('PlatformViewWrapperTest', () => {
    it('TestPlatformViewWrapperDvModel', 0, async (done: Function) => {
      const newTouchProcessor: OhosTouchProcessor = new OhosTouchProcessor();
      platformViewWrapper.setTouchProcessor(newTouchProcessor);
      await sleep(1000);
      const dvModel: DVModel = platformViewWrapper.getDvModel();
      expect(dvModel).not().assertNull();
      await sleep(1000);
      platformViewWrapper.addDvModel(dvModel);
      await sleep(1000);
      const parameters: DVModelParameters = new DVModelParameters();
      platformViewWrapper.setLayoutParams(parameters);
      done();
    })
  })
}
