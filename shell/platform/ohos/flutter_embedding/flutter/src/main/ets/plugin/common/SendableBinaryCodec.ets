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

import SendableMessageCodec from './SendableMessageCodec';

/**
 * A {@link MessageCodec} using unencoded binary messages, represented as {@link ByteBuffer}s.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/BinaryCodec-class.html">BinaryCodec</a> on the
 * Dart side. These parts of the Flutter SDK are evolved synchronously.
 *
 * <p>On the Dart side, messages are represented using {@code ByteData}.
 */

@Sendable
export default class SendableBinaryCodec implements SendableMessageCodec<ArrayBuffer> {
  private returnsDirectByteBufferFromDecoding: boolean = false;
  static readonly INSTANCE_DIRECT: SendableBinaryCodec = new SendableBinaryCodec(true);

  constructor(returnsDirectByteBufferFromDecoding: boolean) {
    this.returnsDirectByteBufferFromDecoding = returnsDirectByteBufferFromDecoding;
  }

  encodeMessage(message: ArrayBuffer): ArrayBuffer {
    return message
  }

  decodeMessage(message: ArrayBuffer | null): ArrayBuffer {
    if (message == null) {
      return new ArrayBuffer(0);
    } else if (this.returnsDirectByteBufferFromDecoding) {
      return message;
    } else {
      return message.slice(0, message.byteLength);
    }
  }
}
