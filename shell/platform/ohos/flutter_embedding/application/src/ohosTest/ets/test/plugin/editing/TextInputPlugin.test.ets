
import { describe,  beforeEach, it, expect } from '@ohos/hypium'
import TextInputChannel from '@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/TextInputChannel';
import TextInputPlugin from '@ohos/flutter_ohos/src/main/ets/plugin/editing/TextInputPlugin';
import TextInputMethodHandlerImpl from '@ohos/flutter_ohos/src/main/ets/plugin/editing/TextInputPlugin';
import common from '@ohos.app.ability.common';
import DartExecutor from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartExecutor';
import FlutterInjector from '@ohos/flutter_ohos/src/main/ets/FlutterInjector';

export default function TextInputPluginTest() {
  describe('TextInputPluginTest', () => {
    let textInputPlugin: TextInputPlugin;
    let textInputChannelMock: TextInputChannel;
    let textInputMethodHandlerImpl: TextInputMethodHandlerImpl;
    let dartExecutor: DartExecutor;
    beforeEach(() => {
      let context: common.Context = getContext();
      const assetManager = context.resourceManager;
      const flutterNapi = FlutterInjector.getInstance().getFlutterNapi();
      dartExecutor = new DartExecutor(flutterNapi, assetManager);
      textInputChannelMock = new TextInputChannel(dartExecutor);
      textInputPlugin = new TextInputPlugin(textInputChannelMock);
      textInputMethodHandlerImpl = new TextInputMethodHandlerImpl(textInputChannelMock);
    })
    it('should initialize correctly', 0, ()=> {
      expect(textInputMethodHandlerImpl).not().assertNull();
    })
    it('Test clearTextInputClient', 0, () => {
      textInputPlugin.clearTextInputClient();
      expect(textInputPlugin).not().assertNull();
    })
  })
}