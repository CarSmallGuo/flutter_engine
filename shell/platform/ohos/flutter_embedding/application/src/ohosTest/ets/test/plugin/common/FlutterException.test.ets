// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { describe, it, expect } from '@ohos/hypium';
import FlutterException from '@ohos/flutter_ohos/src/main/ets/plugin/common/FlutterException';

interface keyValue {
  key: string;
}

interface anotherKey {
  anotherKey: string;
}

interface defaultKey {
  defaultKey: string;
}

export default function FlutterExceptionTest() {
  describe('FlutterExceptionTest', () => {
    it('should initialize with correct values', 0, () => {
      const code = 'ERROR_CODE_123';
      const message = 'This is an error message';
      const details:keyValue = { key: 'value' };
      const exception = new FlutterException(code, message, details);
      expect(exception.message).assertEqual(message);
      expect(exception.code).assertEqual(code);
      expect(exception.name).assertEqual('');
    })
    it('should have a stack property if provided', 0, () => {
      const code = 'ERROR_CODE_456';
      const message = 'Another error message';
      const details: anotherKey = { anotherKey: 'anotherValue' };
      const stack = 'Error stack trace';
      const exception = new FlutterException(code, message, details);
      exception.stack = stack;
      expect(exception.stack).assertEqual(stack);
    })
    it('should default name to an empty string if not provided', 0, () => {
      const code = 'ERROR_CODE_789';
      const message = 'Default name test';
      const details: defaultKey = { defaultKey: 'defaultValue' };
      const exception = new FlutterException(code, message, details);
      expect(exception.name).assertEqual('');
    })
  })
}
