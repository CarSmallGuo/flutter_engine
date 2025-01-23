/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

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

import LocalizationChannel, {
  LocalizationMessageHandler
} from '../../embedding/engine/systemchannels/LocalizationChannel'
import common from '@ohos.app.ability.common';
import intl from '@ohos.intl';
import Log from '../../util/Log';
import i18n from '@ohos.i18n';

const TAG = "LocalizationPlugin";

export default class LocalizationPlugin {
  private localizationChannel: LocalizationChannel;
  private context: common.Context;

  localeFromString(localeString: string): intl.Locale {
    localeString = localeString.replace('_', '-');
    let parts: string[] = localeString.split('-', -1);
    let languageCode = parts[0];
    let scriptCode = "";
    let countryCode = "";
    let index: number = 1;

    if (parts.length > index && parts[index].length == 4) {
      scriptCode = parts[index];
      index++;
    }

    if (parts.length > index && parts[index].length >= 2 && parts[index].length <= 3) {
      countryCode = parts[index];
      index++;
    }
    return new intl.Locale(languageCode + '-' + countryCode + '-' + scriptCode);
  }

  private localizationMessageHandler: LocalizationMessageHandler =
    new enterGetStringResource((key: string, localeString: string | null) => {

      Log.i(TAG, "getStringResource,key: " + key + ",localeString: " + localeString);
      let localContext: common.Context = this.context;
      let stringToReturn: string | null = null;
      // 获取资源管理器
      let resMgr = localContext.resourceManager;

      try {
        // 如果localeString不为空，则更新为指定地区的资源管理器
        if (localeString) {
          let overrideConfig = resMgr.getOverrideConfiguration();
          overrideConfig.locale = localeString;
          let overrideResMgr = resMgr.getOverrideResourceManager(overrideConfig);
          stringToReturn = overrideResMgr.getStringByNameSync(key);
        } else {
          stringToReturn = resMgr.getStringByNameSync(key);
        }
      } catch (e) {
        Log.e(TAG, e);
        return null;
      }

      return stringToReturn;
    })

  constructor(context: common.Context, localizationChannel: LocalizationChannel) {
    this.context = context;
    this.localizationChannel = localizationChannel;
    this.localizationChannel.setLocalizationMessageHandler(this.localizationMessageHandler);
  }

  sendLocaleToFlutter(): void {
    let systemLocale: string = i18n.System.getSystemLocale();
    let data: Array<string> = [];
    data.push(systemLocale);
    this.localizationChannel.sendLocales(data);
  }
}

class enterGetStringResource {
  getStringResource: (key: string, localeString: string | null) => string | null

  constructor(getStringResource: (key: string, localeString: string | null) => string | null) {
    this.getStringResource = getStringResource
  }
}