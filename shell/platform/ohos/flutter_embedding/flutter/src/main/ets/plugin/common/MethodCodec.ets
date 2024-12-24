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
import Any from './Any';

import MethodCall from './MethodCall';
/**
 * A codec for method calls and enveloped results.
 *
 * <p>Method calls are encoded as binary messages with enough structure that the codec can extract a
 * method name String and an arguments Object. These data items are used to populate a {@link
 * MethodCall}.
 *
 * <p>All operations throw {@link IllegalArgumentException}, if conversion fails.
 */
export default interface MethodCodec {
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

  /**
   * Encodes an error result into a binary envelope message.
   *
   * @param errorCode An error code String.
   * @param errorMessage An error message String, possibly null.
   * @param errorDetails Error details, possibly null. Consider supporting {@link Throwable} in your
   *     codec. This is the most common value passed to this field.
   * @return a {@link ByteBuffer} containing the encoding between position 0 and the current
   *     position.
   */
  encodeErrorEnvelope(errorCode: string, errorMessage: string, errorDetails: Any): ArrayBuffer;

  /**
   * Encodes an error result into a binary envelope message with the native stacktrace.
   *
   * @param errorCode An error code String.
   * @param errorMessage An error message String, possibly null.
   * @param errorDetails Error details, possibly null. Consider supporting {@link Throwable} in your
   *     codec. This is the most common value passed to this field.
   * @param errorStacktrace Platform stacktrace for the error. possibly null.
   * @return a {@link ByteBuffer} containing the encoding between position 0 and the current
   *     position.
   */
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