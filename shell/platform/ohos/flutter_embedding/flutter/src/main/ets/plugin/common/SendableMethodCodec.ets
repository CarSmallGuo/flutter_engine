/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on MethodCodec.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/
import { lang } from '@kit.ArkTS';

import Any from './Any';
import MethodCall from './MethodCall';

type ISendable = lang.ISendable;
export default interface SendableMethodCodec extends ISendable {
  /**
   * Encodes a message call into binary.
   *
   * @param methodCall a {@link MethodCall}.
   * @return a {@link ByteBuffer} containing the encoding between position 0 and the current
   *     position.
   */
  encodeMethodCall(methodCall: MethodCall): ArrayBuffer;

  /**
   * Decodes a message call from binary.
   *
   * @param methodCall the binary encoding of the method call as a {@link ByteBuffer}.
   * @return a {@link MethodCall} representation of the bytes between the given buffer's current
   *     position and its limit.
   */
  decodeMethodCall(methodCall: ArrayBuffer): MethodCall;

  /**
   * Encodes a successful result into a binary envelope message.
   *
   * @param result The result value, possibly null.
   * @return a {@link ByteBuffer} containing the encoding between position 0 and the current
   *     position.
   */
  encodeSuccessEnvelope(result: Any): ArrayBuffer;

  encodeErrorEnvelope(errorCode: string, errorMessage: string, errorDetails: Any): ArrayBuffer;

  encodeErrorEnvelopeWithStacktrace(errorCode: string, errorMessage: string, errorDetails: Any, errorStacktrace: string): ArrayBuffer

  /**
   * Decodes a result envelope from binary.
   *
   * @param envelope the binary encoding of a result envelope as a {@link ByteBuffer}.
   * @return the enveloped result Object.
   * @throws FlutterException if the envelope was an error envelope.
   */
  decodeEnvelope(envelope: ArrayBuffer): Any
}
