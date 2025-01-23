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

import { describe, expect, it, } from "@ohos/hypium";
import { AsyncCallback as _AsyncCallback } from '@ohos.base';
import PlatformChannel, {
  Brightness,
  ClipboardContentFormat,
  DeviceOrientation,
  HapticFeedbackType,
  SystemUiMode,
  SystemUiOverlay
} from "@ohos/flutter_ohos/src/main/ets/embedding/engine/systemchannels/PlatformChannel";
import { Any, DartExecutor, FlutterNapi, MethodResult } from "@ohos/flutter_ohos";
import { resourceManager } from "@kit.LocalizationKit";

export default function PlatformChannelUnitTest() {
  describe('PlatformChannelTest', () => {
    it('platformChannel_decodeOrientations', 0, decodeOrientations);

    it('platformChannel_getFeedbackTypeFromValue', 0, getFeedbackTypeFromValue);

    it('platformChannel_getClipboardContentFormatFromValue', 0, getClipboardContentFormatFromValue);

    it('platformChannel_getSystemUiOverlayFromValue', 0, getSystemUiOverlayFromValue);

    it('platformChannel_getSystemUiModeFromValue', 0, getSystemUiModeFromValue);

    it('platformChannel_getBrightnessFromValue', 0, getBrightnessFromValue);

    it('platformChannel_getDeviceOrientationFromValue', 0, getDeviceOrientationFromValue);
  })
}

function decodeOrientations() {
  let channel = getPlatformChannel();
  expect(channel.decodeOrientations([DeviceOrientation.PORTRAIT_UP])).assertInstanceOf("Number")
}

function getFeedbackTypeFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getFeedbackTypeFromValue(HapticFeedbackType.STANDARD);
  expect(result).assertInstanceOf("String")
}

function getClipboardContentFormatFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getClipboardContentFormatFromValue(ClipboardContentFormat.PLAIN_TEXT);
  expect(result).assertInstanceOf("String")
}

function getSystemUiOverlayFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getSystemUiOverlayFromValue(SystemUiOverlay.BOTTOM_OVERLAYS);
  expect(result).assertInstanceOf("String")
}

function getSystemUiModeFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getSystemUiModeFromValue(SystemUiMode.IMMERSIVE);
  expect(result).assertInstanceOf("String")
}

function getBrightnessFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getBrightnessFromValue(Brightness.LIGHT);
  expect(result).assertInstanceOf("String")
}

function getDeviceOrientationFromValue() {
  let channel = getPlatformChannel();
  let result = channel.getDeviceOrientationFromValue(DeviceOrientation.PORTRAIT_UP);
  expect(result).assertInstanceOf("String")
}

function getPlatformChannel(): PlatformChannel {
  let flutterNapi = new FlutterNapi();
  let manager: resourceManager.ResourceManager = getContext().resourceManager;
  let dartExecutor = new DartExecutor(flutterNapi, manager);
  let channel = new PlatformChannel(dartExecutor);
  return channel;
}

export class MethodResultTest implements MethodResult {
  success = (result: Any): void => {
  };
  error = (errorCode: string, errorMessage: string, errorDetails: Any): void => {
  };
  notImplemented = (): void => {
  }
}
