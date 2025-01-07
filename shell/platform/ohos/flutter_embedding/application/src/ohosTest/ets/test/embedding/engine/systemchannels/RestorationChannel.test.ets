/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE_HW file.
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
