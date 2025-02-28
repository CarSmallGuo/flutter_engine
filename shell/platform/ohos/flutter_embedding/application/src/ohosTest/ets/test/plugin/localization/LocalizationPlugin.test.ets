
import { describe, beforeEach, it, expect } from '@ohos/hypium';
import LocalizationPlugin from '@ohos/flutter_ohos/src/main/ets/plugin/localization/LocalizationPlugin';
import common from '@ohos.app.ability.common';
import LocalizationChannel,{LocalizationMessageHandler} from '@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/LocalizationChannel';
import intl from '@ohos.intl';
import DartExecutor from '@ohos/flutter_ohos/src/main/ets/embedding/engine/dart/DartExecutor';
import FlutterInjector from '@ohos/flutter_ohos/src/main/ets/FlutterInjector';
import Any from '@ohos/flutter_ohos/src/main/ets/plugin/common/Any';

export default function LocalizationPluginTest() {
  describe('LocalizationPluginTest', () => {
    let localizationChannel: LocalizationChannel;
    let localizationPlugin: LocalizationPlugin;
    let dartExecutor: DartExecutor;

    beforeEach(() => {
      let context: common.Context = getContext();
      const assetManager = context.resourceManager;
      const flutterNapi = FlutterInjector.getInstance().getFlutterNapi();
      dartExecutor = new DartExecutor(flutterNapi, assetManager);
      localizationChannel = new LocalizationChannel(dartExecutor); // 初始化LocalizationChannel
      localizationPlugin = new LocalizationPlugin(context, localizationChannel);
    });
    it('test_localeFromString', 0, () => {
      let localeString = "zh_CN";
      let expectedLocale = new intl.Locale("zh-Hans-CN");
      let actualLocale = localizationPlugin.localeFromString(localeString);

      expect(actualLocale.language).assertEqual(expectedLocale.language);
      expect(actualLocale.region).assertEqual(expectedLocale.region);
      expect(actualLocale.script).assertEqual(expectedLocale.script);
    })
    it('testSendLocaleToFlutter', 0, () => {
      let mockSystemLocale = 'zh-Hans-CN';
      (localizationChannel as Any).sendLocales = (locales:Any) => {
        expect(locales).assertContain(mockSystemLocale);
      };
      localizationPlugin.sendLocaleToFlutter();
    })
  })
}