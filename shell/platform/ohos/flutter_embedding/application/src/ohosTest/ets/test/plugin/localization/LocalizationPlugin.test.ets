/*
Copyright (C) 2024 Huawei Device Co., Ltd.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of pngout nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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