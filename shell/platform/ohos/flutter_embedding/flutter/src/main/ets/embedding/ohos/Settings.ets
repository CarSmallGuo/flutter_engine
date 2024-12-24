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

import SettingsChannel, { PlatformBrightness } from '../engine/systemchannels/SettingsChannel'
import I18n from '@ohos.i18n'
import Log from '../../util/Log';
import { MediaQuery } from '@ohos.arkui.UIContext';


const TAG = "Settings";
export default class Settings {
  settingsChannel: SettingsChannel | null;

  constructor(settingsChannel: SettingsChannel | null) {
    this.settingsChannel = settingsChannel;
  }

  sendSettings(mediaQuery: MediaQuery): void {
    this.settingsChannel?.startMessage()
      .setAlwaysUse24HourFormat(I18n.System.is24HourClock())
      .setNativeSpellCheckServiceDefined(false)
      .setBrieflyShowPassword(false)
      .setPlatformBrightness(this.getThemeMode(mediaQuery))
      .setTextScaleFactor(this.getTextScaleFactor())
      .send();
  }

  getThemeMode(mediaQuery: MediaQuery): PlatformBrightness {

    let listener = mediaQuery.matchMediaSync('(dark-mode: true)');
    if (listener.matches) {
      Log.i(TAG,"return dark");
      return PlatformBrightness.DARK;
    } else {
      Log.i(TAG,"return light");
      return PlatformBrightness.LIGHT;
    }
  }

  getTextScaleFactor() : number {
    let sysTextScaleFactor = AppStorage.get<number>('fontSizeScale');
    if(sysTextScaleFactor == undefined) {
      sysTextScaleFactor = 1.0;
      Log.e(TAG, 'get textScaleFactor error, it is assigned to ' + JSON.stringify(sysTextScaleFactor));
    }
    Log.i(TAG, "return textScaleFactor = " + JSON.stringify(sysTextScaleFactor))
    return sysTextScaleFactor;
  }

}