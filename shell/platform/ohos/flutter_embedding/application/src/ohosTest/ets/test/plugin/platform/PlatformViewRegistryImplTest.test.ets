
import { describe, it, expect } from '@ohos/hypium';
import { PlatformViewRegistryImpl, PlatformViewFactory } from "@ohos/flutter_ohos";

const platformViewRegistryImpl: PlatformViewRegistryImpl = new PlatformViewRegistryImpl();

export default function PlatformViewRegistryImplTest() {
  describe('PlatformViewRegistryImplTest', () => {
    it('TestPlatformViewRegistryImplGetFactory', 0, async (done: Function) => {
      const platformViewFactory: PlatformViewFactory = platformViewRegistryImpl.getFactory('test');
      expect(platformViewFactory).not().assertNull();
      done();
    })

    it('TestPlatformViewRegistryImplRegisterViewFactory', 0, async (done: Function) => {
      const platformViewFactory: PlatformViewFactory = platformViewRegistryImpl.getFactory('test');
      expect(platformViewFactory).not().assertNull();
      const result = platformViewRegistryImpl.registerViewFactory('test', platformViewFactory);
      expect(result).assertTrue();
      done();
    })
  })
}