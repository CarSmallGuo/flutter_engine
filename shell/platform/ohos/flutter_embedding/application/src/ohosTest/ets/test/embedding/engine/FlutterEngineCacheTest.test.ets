
import { describe, it, expect } from '@ohos/hypium';
import FlutterEngine from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngine';
import FlutterEngineCache from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineCache';

export default function FlutterEngineCacheTest() {
  const flutterEngineCache: FlutterEngineCache = new FlutterEngineCache();

  describe('FlutterEngineCacheTest', () => {

    it('getInstance', 0, async (done: Function) => {
      console.info("uitest: getInstance begin");
      const getInstance: FlutterEngineCache = FlutterEngineCache.getInstance();
      expect(getInstance).not().assertNull();
      done();
    })

    it('contains', 0, async (done: Function) => {
      console.info("uitest: contains begin");
      const contains: boolean = flutterEngineCache.contains('test');
      expect(contains).not().assertNull();
      done();
    })

    it('get', 0, async (done: Function) => {
      console.info("uitest: contains begin");
      const get: FlutterEngine | null = flutterEngineCache.get('test');
      if(get) {
        expect(get).not().assertNull();
      }
      done();
    })
  })
}