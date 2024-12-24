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


import {
  DartExecutor,
  FlutterNapi,
  MethodCall,
  MethodCallHandler,
  MethodChannel,
  MethodResult,
  RestorationChannel,
  StandardMethodCodec
} from "@ohos/flutter_ohos";
import { describe, expect, it } from "@ohos/hypium";
import { resourceManager } from "@kit.LocalizationKit";
import { util } from "@kit.ArkTS";

let TAG = "RestorationChannel";

export default function RestorationChannelUnitTest() {

  describe('RestorationChannelTest', () => {
    it("RestorationChannel_getRestorationData", 0, getRestorationData);
  })
}

function getRestorationData() {
  let channel = getRestorationChannel();
  expect(channel.getRestorationData()).assertInstanceOf("Uint8Array")
}

function getRestorationChannel(): RestorationChannel {
  let flutterNapi = new FlutterNapi();
  let manager: resourceManager.ResourceManager = getContext().resourceManager;
  let dartExecutor = new DartExecutor(flutterNapi, manager);
  let rawChannel = new MethodChannel(dartExecutor, "flutter/mousecursor", StandardMethodCodec.INSTANCE);
  let restorationChannel = new RestorationChannel(rawChannel, false);
  return restorationChannel;
}

class TestMethodCallHandler implements MethodCallHandler {
  onMethodCall(call: MethodCall, result: MethodResult): void {
  }
}

/**
 * string转Uint8Array
 * @param value
 * @returns
 */
export function stringToUint8Array(str: string): Uint8Array {
  try {
    let textEncoder = new util.TextEncoder("utf-8");
    let array: Uint8Array = textEncoder.encodeInto(str);
    return array
  } catch (err) {
    return new Uint8Array()
  }
}
