import { describe,it, expect } from '@ohos/hypium';
import { FlutterRenderer} from '@ohos/flutter_ohos/src/main/ets/embedding/engine/renderer/FlutterRenderer';
import FlutterNapi from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterNapi'
export default function FlutterRendererTest() {
  describe('FlutterRendererTest', () => {
    it('TestCreateSurfaceTexture', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterRenderer = new FlutterRenderer(flutterNapi);
      let surfaceTextureEntry = flutterRenderer.createSurfaceTexture();
      expect(surfaceTextureEntry).not().assertNull();;
      done();
    })

    it('TestGetTextureId', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterRenderer = new FlutterRenderer(flutterNapi);
      let textureId = flutterRenderer.getTextureId();
      expect(textureId == 0).assertFalse();
      done();
    })

    it('TestRegisterPixelMap', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterRenderer = new FlutterRenderer(flutterNapi);
      const pixelMap = {} as PixelMap;
      let textureId = flutterRenderer.registerPixelMap(pixelMap);
      expect(textureId).assertEqual(1);
      done();
    })

    it('TestRegisterPixelMapMore', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterRenderer = new FlutterRenderer(flutterNapi);
      const pixelMap1 = {} as PixelMap;
      const pixelMap2 = {} as PixelMap;
      const textureId1 = flutterRenderer.registerPixelMap(pixelMap1);
      const textureId2 = flutterRenderer.registerPixelMap(pixelMap2);
      expect(textureId1).assertEqual(1); // 第一次调用
      expect(textureId2).assertEqual(2); // 第二次调用
      done();
    })
  })
}
