// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { MethodCall } from "@ohos/flutter_ohos";
import { describe, expect, it } from "@ohos/hypium";

export default function MethodCallUnitTest() {
  describe('MethodCall', () => {
    it("MethodCall_argument", 0, argument);

    it("MethodCall_hasArgument", 0, hasArgument);
  })
}

function argument() {
  let ob = new MethodCall("get", null);
  expect(ob.argument("key")).assertNull();
}

function hasArgument() {
  let ob = new MethodCall("get", null);
  expect(ob.hasArgument("key")).assertFalse();
}