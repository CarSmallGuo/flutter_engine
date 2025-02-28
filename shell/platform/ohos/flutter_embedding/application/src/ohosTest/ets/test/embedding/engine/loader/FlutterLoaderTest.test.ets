
import common from '@ohos.app.ability.common';
import FlutterLoader from '@ohos/flutter_ohos/src/main/ets/embedding/engine/loader/FlutterLoader'
import { describe,it, expect } from '@ohos/hypium';
import FlutterNapi from '@ohos/flutter_ohos/src/main/ets/embedding/engine/FlutterNapi'
export default function FlutterLoaderTest() {
  describe('FlutterLoaderTest', () => {
    it('initializationTest', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterLoader: FlutterLoader = new FlutterLoader(flutterNapi);
      expect(flutterLoader.initialized).assertFalse();
      done();
    })

    it('initResultTest', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterLoader: FlutterLoader = new FlutterLoader(flutterNapi);
      expect(flutterLoader.initResult).assertNull();
      done();
    })

    it('flutterNapiTest', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterLoader: FlutterLoader = new FlutterLoader(flutterNapi);
      expect(flutterLoader.flutterNapi).not().assertNull();
      done();
    })

    it('startInitializationTest', 0, async (done: Function) => {
      let flutterNapi: FlutterNapi = new FlutterNapi();
      let flutterLoader: FlutterLoader = new FlutterLoader(flutterNapi);
      let context: common.Context = getContext();
      expect(flutterLoader.initialized).assertFalse();
      flutterLoader.startInitialization(context);
      done();
    })
  })
}
