

import { describe, it, expect } from '@ohos/hypium'
import { PlatformView } from '@ohos/flutter_ohos';
import { Params } from '@ohos/flutter_ohos/src/main/ets/plugin/platform/PlatformView';

export default function PlatformViewTest() {
  describe('PlatformViewTest', () => {
    class TestPlatformView extends PlatformView {
      getView(): WrappedBuilder<[Params]> {
        throw new Error('Method not implemented.');
      }

      dispose(): void {
        throw new Error('Method not implemented.');
      }
    }

    const platformView: PlatformView = new TestPlatformView()
    it('getType', 0, () => {
      expect(platformView.getType()).assertEqual('default')
    })
  })
}