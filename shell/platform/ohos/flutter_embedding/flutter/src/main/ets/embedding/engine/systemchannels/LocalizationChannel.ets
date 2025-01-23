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

import DartExecutor from '../dart/DartExecutor';
import MethodChannel, { MethodCallHandler, MethodResult} from '../../../plugin/common/MethodChannel';
import MethodCall from '../../../plugin/common/MethodCall';
import List from '@ohos.util.List';
import JSONMethodCodec from '../../../plugin/common/JSONMethodCodec';
import intl from '@ohos.intl';
import Log from '../../../util/Log';

const TAG =  "LocalizationChannel";
export default class LocalizationChannel implements MethodCallHandler{
  private static TAG = "LocalizationChannel";
  private static CHANNEL_NAME = "flutter/localization";
  private channel: MethodChannel;
  private localizationMessageHandler: LocalizationMessageHandler | null = null;

  onMethodCall(call: MethodCall, result: MethodResult) :void {
    if (this.localizationMessageHandler == null) {
      Log.e(TAG, "localizationMessageHandler is null");
      return;
    }
    let method: string = call.method;
    switch (method) {
        case "Localization.getStringResource": {
          Log.i(TAG, "Localization.getStringResource enter");
          let key: string = call.argument("key");
          let localeString: string = "";
          if (call.hasArgument("locale")) {
            localeString  = call.argument("locale");
          }
          result.success(this.localizationMessageHandler?.getStringResource(key, localeString));
          break;
        }
        default: {
            result.notImplemented();
            break;
        }
    }
  }

  constructor(dartExecutor: DartExecutor) {
    this.channel = new MethodChannel(dartExecutor, LocalizationChannel.CHANNEL_NAME, JSONMethodCodec.INSTANCE);
    this.channel.setMethodCallHandler(this);
  }

  setLocalizationMessageHandler(localizationMessageHandler: LocalizationMessageHandler): void {
    this.localizationMessageHandler = localizationMessageHandler;
  }

  sendLocales(locales: Array<string>): void {
    let data: Array<string> = [];
    for (let i = 0 ; i < locales.length ; i++) {
      let locale = new intl.Locale(locales[i]);
      data.push(locale.language);
      data.push(locale.region);
      data.push(locale.script);
      data.push(''); // locale.getVariant locale的一种变体
    }
    this.channel.invokeMethod("setLocale", data);
  }
}

export interface LocalizationMessageHandler {
    getStringResource(key: string, local: string): void;
}