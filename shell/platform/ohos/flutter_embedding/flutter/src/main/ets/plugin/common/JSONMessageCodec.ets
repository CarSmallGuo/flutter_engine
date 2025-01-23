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
import StringUtils from '../../util/StringUtils';

import MessageCodec from './MessageCodec';
import StringCodec from './StringCodec';
import TreeMap from '@ohos.util.TreeMap';
import HashMap from '@ohos.util.HashMap';
import LightWeightMap from '@ohos.util.LightWeightMap';
import PlainArray from '@ohos.util.PlainArray';
import List from '@ohos.util.List';
import LinkedList from '@ohos.util.LinkedList';
import Any from './Any';

/**
 * A {@link MessageCodec} using UTF-8 encoded JSON method calls and result envelopes.
 *
 * <p>This codec is guaranteed to be compatible with the corresponding <a
 * href="https://api.flutter.dev/flutter/services/JSONMethodCodec-class.html">JSONMethodCodec</a> on
 * the Dart side. These parts of the Flutter SDK are evolved synchronously.
 *
 * <p>On the Dart side, JSON messages are handled by the JSON facilities of the <a
 * href="https://api.dartlang.org/stable/dart-convert/JSON-constant.html">dart:convert</a> package.
 */
export default class JSONMessageCodec implements MessageCodec<Object> {
  static INSTANCE = new JSONMessageCodec();

  encodeMessage(message: Any): ArrayBuffer {
    if (message == null) {
      return StringUtils.stringToArrayBuffer("");
    }
    return StringCodec.INSTANCE.encodeMessage(JSON.stringify(this.toBaseData(message)));
  }

  decodeMessage(message: ArrayBuffer | null): Any {
    if (message == null) {
      return StringUtils.stringToArrayBuffer("");
    }
    try {
      const jsonStr = StringCodec.INSTANCE.decodeMessage(message);
      let jsonObj: Record<string, Any> = JSON.parse(jsonStr);
      if (jsonObj instanceof Object) {
        const list = Object.keys(jsonObj);
        if (list.includes('args')) {
          let args: Any = jsonObj['args'];
          if (args instanceof Object && !(args instanceof Array)) {
            let argsMap: Map<string, Any> = new Map();
            Object.keys(args).forEach(key => {
              argsMap.set(key, args[key]);
            })
            jsonObj['args'] = argsMap;
          }
        }
      }
      return jsonObj;
    } catch (e) {
      throw new Error("Invalid JSON");
    }
  }

  toBaseData(message: Any): Any {
    if (message == null || message == undefined) {
      return null;
    } else if (message instanceof List || message instanceof LinkedList) {
      return this.toBaseData(message.convertToArray());
    } else if (message instanceof Map || message instanceof HashMap || message instanceof TreeMap
      || message instanceof LightWeightMap || message instanceof PlainArray) {
      let messageObj: Any = {};
      message.forEach((value: Any, key: Any) => {
        messageObj[this.toBaseData(key)] = this.toBaseData(value);
      });
      return messageObj;
    } else if (message instanceof Array) {
      let messageArr:Array<Any> = [];
      message.forEach((value:Any)=>{
        messageArr.push(this.toBaseData(value));
      })
      return messageArr;
    } else if (message instanceof Object) {
      let messageObj: Any = {};
      Object.keys(message).forEach((key:Any)=>{
        messageObj[this.toBaseData(key)] = this.toBaseData(message[key]);
      })
      return messageObj;
    } else {
      return message;
    }
  }
}