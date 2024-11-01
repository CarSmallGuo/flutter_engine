// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import { lang } from '@kit.ArkTS';

type ISendable = lang.ISendable;
export default interface SendableMessageCodec<T> extends ISendable {
  /**
   * Encodes the specified message into binary.
   */
  encodeMessage(message: T) : ArrayBuffer;

  /**
   * Decodes the specified message from binary.
   *
   */
  decodeMessage(message: ArrayBuffer | null): T;
}
