// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import { Settings } from "@ohos/flutter_ohos";
import { MediaQuery } from '@kit.ArkUI';
import { PlatformBrightness } from '@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/SettingsChannel';

const settings: Settings = new Settings(null);

export default function SettingTest() {
  describe('SettingTest', () => {
    it('TestGetTextScaleFactor', 0, async (done: Function) => {
      const result: number = settings.getTextScaleFactor();
      expect(result).not().assertNull();
      done();
    })

    it('TestGetThemeMode', 0, async (done: Function) => {
      const mediaQuery: MediaQuery = new MediaQuery();
      settings.sendSettings(mediaQuery);
      const result: PlatformBrightness = settings.getThemeMode(mediaQuery);
      expect(result).not().assertNull();
      done();
    })
  })
}