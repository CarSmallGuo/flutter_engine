
import { describe, it, expect } from '@ohos/hypium';
import FlutterEngine from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngine';
import FlutterEngineCache from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterEngineCache';

export default function FlutterEngineCacheTest() {
  const flutterEngineCache: FlutterEngineCache = new FlutterEngineCache();

  describe('FlutterEngineCacheTest', () => {

    it('getInstance', 0, async (done: Function) => {
      const getInstance: FlutterEngineCache = FlutterEngineCache.getInstance();
      expect(getInstance).not().assertNull();
      done();
    })

    it('contains', 0, async (done: Function) => {
      const contains: boolean = flutterEngineCache.contains('test');
      expect(contains).not().assertNull();
      done();
    })

    it('get', 0, async (done: Function) => {
      const get: FlutterEngine | null = flutterEngineCache.get('test');
      if(get) {
        expect(get).not().assertNull();
      }
      done();
    })

    it('put', 0, async (done: Function) => {
      flutterEngineCache.put('test',flutterEngineCache.get('test'));
      done();
    })

    it('remove', 0, async (done: Function) => {
      flutterEngineCache.remove('test');
      done();
    })

    it('clear', 0, async (done: Function) => {
      flutterEngineCache.clear();
      done();
    })

  })
}