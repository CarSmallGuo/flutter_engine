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
import ToolUtils from '../../util/ToolUtils';
import FlutterException from './FlutterException';
import Any from './Any';
import SendableJSONMessageCodec from './SendableJSONMessageCodec';
import MethodCall from './MethodCall';
import SendableMethodCodec from './SendableMethodCodec';

/**
 * A {@link SendableMethodCodec} using UTF-8 encoded JSON method calls and result envelopes.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/JSONMethodCodec-class.html">JSONMethodCodec</a> on
 * the Dart side. These parts of the Flutter SDK are evolved synchronously.
 *
 * <p>Values supported as methods arguments and result payloads are those supported by {@link
 * SendableJSONMessageCodec}.
 */
@Sendable
export default class SendableJSONMethodCodec implements SendableMethodCodec {
  static INSTANCE: SendableJSONMethodCodec = new SendableJSONMethodCodec();

  encodeMethodCall(methodCall: MethodCall): ArrayBuffer {
    try {
      const map: Record<string, Any> = {
        "method": methodCall.method, "args": methodCall.args
      }

      return SendableJSONMessageCodec.INSTANCE.encodeMessage(map);
    } catch (e) {
      throw new Error("Invalid JSON");
    }
  }

  decodeMethodCall(message: ArrayBuffer): MethodCall {
    try {
      const json: Any = SendableJSONMessageCodec.INSTANCE.decodeMessage(message);
      if (ToolUtils.isObj(json)) {
        const method: string = json["method"];
        const args: Any = json["args"];
        if (typeof method == 'string') {
          return new MethodCall(method, args);
        }
      }
      throw new Error("Invalid method call: " + json);
    } catch (e) {
      throw new Error("Invalid JSON:" + JSON.stringify(e));
    }
  }

  encodeSuccessEnvelope(result: Any): ArrayBuffer {
    return SendableJSONMessageCodec.INSTANCE.encodeMessage([result]);
  }

  encodeErrorEnvelope(errorCode: Any, errorMessage: string, errorDetails: Any) {
    return SendableJSONMessageCodec.INSTANCE.encodeMessage([errorCode, errorMessage, errorDetails]);
  }

  encodeErrorEnvelopeWithStacktrace(errorCode: string, errorMessage: string, errorDetails: Any, errorStacktrace: string): ArrayBuffer {
    return SendableJSONMessageCodec.INSTANCE.encodeMessage([errorCode, errorMessage, errorDetails, errorStacktrace])
  }

  decodeEnvelope(envelope: ArrayBuffer): Any {
    try {
      const json: Any = SendableJSONMessageCodec.INSTANCE.decodeMessage(envelope);
      if (json instanceof Array) {
        if (json.length == 1) {
          return json[0];
        }
        if (json.length == 3) {
          const code: string = json[0];
          const message: string = json[1];
          const details: Any = json[2];
          if (typeof code == 'string' && (message == null || typeof message == 'string')) {
            throw new FlutterException(code, message, details);
          }
        }
      }
      throw new Error("Invalid envelope: " + json);
    } catch (e) {
      throw new Error("Invalid JSON");
    }
  }
}