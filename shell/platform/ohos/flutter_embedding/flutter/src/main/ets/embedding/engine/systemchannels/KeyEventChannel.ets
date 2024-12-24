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

import BasicMessageChannel from '../../../plugin/common/BasicMessageChannel';
import { BinaryMessenger } from '../../../plugin/common/BinaryMessenger';
import Log from '../../../util/Log';
import JSONMessageCodec from '../../../plugin/common/JSONMessageCodec';

export default class KeyEventChannel {
  private static TAG = "KeyEventChannel";
  private static CHANNEL_NAME = "flutter/keyevent";
  private channel: BasicMessageChannel<Map<String, Object>>;

  constructor(binaryMessenger: BinaryMessenger) {
    this.channel = new BasicMessageChannel<Map<String, Object>>(binaryMessenger, KeyEventChannel.CHANNEL_NAME, JSONMessageCodec.INSTANCE);
  }

  sendFlutterKeyEvent(keyEvent: FlutterKeyEvent,
                      isKeyUp: boolean,
                      responseHandler: EventResponseHandler): void {
    this.channel.send(this.encodeKeyEvent(keyEvent, isKeyUp),
      (message: Map<String, Object>) => {
        let isEventHandled = false;
        try {
          if (message != null) {
            isEventHandled = message.get("handled") as boolean;
          }
        } catch (e) {
          Log.e(KeyEventChannel.TAG, "Unable to unpack JSON message: " + e);
        }
        responseHandler.onFrameworkResponse(isEventHandled);
      }
    );
  }

  private encodeKeyEvent(keyEvent: FlutterKeyEvent, isKeyUp: boolean): Map<String, Object> {
    let message: Map<String, Object> = new Map();
    message.set("type", isKeyUp ? "keyup" : "keydown");
    message.set("keymap", "ohos");
    message.set("keyCode", keyEvent.event.keyCode);
    message.set("deviceId", keyEvent.event.deviceId);
    message.set("flags", keyEvent.event.keyText);
    message.set("metaState", keyEvent.event.metaKey);
    message.set("source", keyEvent.event.keySource);
    message.set("intentionCode", keyEvent.event.intentionCode)
    return message;
  }
}

export interface EventResponseHandler {
  onFrameworkResponse: (isEventHandled: boolean) => void;
}

export class FlutterKeyEvent {
  event: KeyEvent;

  constructor(ohosKeyEvent: KeyEvent) {
    this.event = ohosKeyEvent;
  }
}