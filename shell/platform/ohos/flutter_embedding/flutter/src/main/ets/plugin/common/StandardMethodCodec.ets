/*
Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.

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

import { ByteBuffer } from '../../util/ByteBuffer';
import FlutterException from './FlutterException';
import Any from './Any';
import MethodCall from './MethodCall';
import MethodCodec from './MethodCodec';
import StandardMessageCodec from './StandardMessageCodec';

/**
 * A {@link MethodCodec} using the Flutter standard binary encoding.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/StandardMethodCodec-class.html">StandardMethodCodec</a>
 * on the Dart side. These parts of the Flutter SDK are evolved synchronously.
 *
 * <p>Values supported as method arguments and result payloads are those supported by {@link
 * StandardMessageCodec}.
 */
export default class StandardMethodCodec implements MethodCodec {
  private static TAG = "StandardMethodCodec";
  public static INSTANCE = new StandardMethodCodec(StandardMessageCodec.INSTANCE);

  private messageCodec: StandardMessageCodec;

  /** Creates a new method codec based on the specified message codec. */
  constructor(messageCodec: StandardMessageCodec) {
    this.messageCodec = messageCodec;
  }

  encodeMethodCall(methodCall: MethodCall): ArrayBuffer {
    const stream = ByteBuffer.from(new ArrayBuffer(1024));
    this.messageCodec.writeValue(stream, methodCall.method);
    this.messageCodec.writeValue(stream, methodCall.args);
    return stream.buffer;
  }

  decodeMethodCall(methodCall: ArrayBuffer): MethodCall {
    const buffer = ByteBuffer.from(methodCall);
    const method: Any = this.messageCodec.readValue(buffer);
    const args: Any = this.messageCodec.readValue(buffer);
    if (typeof method == 'string' && !buffer.hasRemaining()) {
      return new MethodCall(method, args);
    }
    throw new Error("Method call corrupted");
  }

  encodeSuccessEnvelope(result: Any): ArrayBuffer {
    const stream = ByteBuffer.from(new ArrayBuffer(1024));
    stream.writeInt8(0);
    this.messageCodec.writeValue(stream, result);
    return stream.buffer;
  }

  encodeErrorEnvelope(errorCode: string, errorMessage: string, errorDetails: Any): ArrayBuffer {
    const stream = ByteBuffer.from(new ArrayBuffer(1024));
    stream.writeInt8(1);
    this.messageCodec.writeValue(stream, errorCode);
    this.messageCodec.writeValue(stream, errorMessage);
    if (errorDetails instanceof Error) {
      this.messageCodec.writeValue(stream, errorDetails.stack);
    } else {
      this.messageCodec.writeValue(stream, errorDetails);
    }
    return stream.buffer;
  }

  encodeErrorEnvelopeWithStacktrace(errorCode: string, errorMessage: string, errorDetails: Any, errorStacktrace: string): ArrayBuffer {
    const stream = ByteBuffer.from(new ArrayBuffer(1024));
    stream.writeInt8(1);
    this.messageCodec.writeValue(stream, errorCode);
    this.messageCodec.writeValue(stream, errorMessage);
    if (errorDetails instanceof Error) {
      this.messageCodec.writeValue(stream, errorDetails.stack);
    } else {
      this.messageCodec.writeValue(stream, errorDetails);
    }
    this.messageCodec.writeValue(stream, errorStacktrace);
    return stream.buffer;
  }

  decodeEnvelope(envelope: ArrayBuffer): Any {
    const buffer = ByteBuffer.from(envelope);
    const flag = buffer.readInt8();
    switch (flag) {
      case 0: {
        const result: Any = this.messageCodec.readValue(buffer);
        if (!buffer.hasRemaining()) {
          return result;
        }
        // Falls through intentionally.
      }
      case 1: {
        const code: Any = this.messageCodec.readValue(buffer);
        const message: Any = this.messageCodec.readValue(buffer);
        const details: Any = this.messageCodec.readValue(buffer);
        if (typeof code == 'string' && (message == null || typeof message == 'string') && !buffer.hasRemaining()) {
          throw new FlutterException(code, message, details);
        }
      }
    }
    throw new Error("Envelope corrupted");
  }
}