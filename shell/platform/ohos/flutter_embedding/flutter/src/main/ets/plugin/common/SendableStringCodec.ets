// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import SendableMessageCodec from './SendableMessageCodec';
import StringUtils from '../../util/StringUtils';

/**
 * A {@link MessageCodec} using UTF-8 encoded String messages.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/StringCodec-class.html">StringCodec</a> on the
 * Dart side. These parts of the Flutter SDK are evolved synchronously.
 */
@Sendable
export default class SendableStringCodec implements SendableMessageCodec<string> {
  static readonly INSTANCE: SendableStringCodec = new SendableStringCodec();

  encodeMessage(message: string): ArrayBuffer {
    if (message == null) {
      return StringUtils.stringToArrayBuffer("");
    }
    return StringUtils.stringToArrayBuffer(message);
  }

  decodeMessage(message: ArrayBuffer | null): string {
    if (message == null) {
      return "";
    }
    return StringUtils.arrayBufferToString(message);
  }
}
