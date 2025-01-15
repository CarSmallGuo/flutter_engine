/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
 */

import { describe, expect, it } from "@ohos/hypium";
import { resourceManager } from "@kit.LocalizationKit";
import { DartExecutor, FlutterNapi, SettingsChannel } from "@ohos/flutter_ohos";

export default function SettingsChannelUnitTest() {
  describe('SettingsChannelTest', () => {
    it("SettingsChannel_startMessage", 0, startMessage);
  })
}

function startMessage() {
  let channel = getSettingsChannel();
  let result = channel.startMessage();
  expect(result).assertInstanceOf("Object");
}

function getSettingsChannel(): SettingsChannel {
  let flutterNapi = new FlutterNapi();
  let manager: resourceManager.ResourceManager = getContext().resourceManager;
  let dartExecutor = new DartExecutor(flutterNapi, manager);
  let channel = new SettingsChannel(dartExecutor);
  return channel;
}