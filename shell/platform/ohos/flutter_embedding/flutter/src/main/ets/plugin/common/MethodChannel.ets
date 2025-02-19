/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on MethodChannel.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/

import Log from '../../util/Log';
import MessageChannelUtils from '../../util/MessageChannelUtils';
import StringUtils from '../../util/StringUtils';
import { BinaryMessageHandler, BinaryMessenger, BinaryReply } from './BinaryMessenger';
import Any from './Any';
import MethodCall from './MethodCall';
import MethodCodec from './MethodCodec';
import StandardMethodCodec from './StandardMethodCodec';


export default class MethodChannel {
  static TAG = "MethodChannel#";
  private messenger: BinaryMessenger;
  private name: string;
  private codec: MethodCodec;

  constructor(messenger: BinaryMessenger, name: string, codec: MethodCodec = StandardMethodCodec.INSTANCE) {
    this.messenger = messenger
    this.name = name
    this.codec = codec
  }

  invokeMethod(method: string, args: Any, callback?: MethodResult): void {
    this.messenger.send(this.name, this.codec.encodeMethodCall(new MethodCall(method, args)), callback == null ? null : new IncomingResultHandler(callback, this.codec));
  }

  setMethodCallHandler(handler: MethodCallHandler | null): void {
    this.messenger.setMessageHandler(this.name, handler == null ? null : new IncomingMethodCallHandler(handler, this.codec));
  }

  /**
   * Adjusts the number of messages that will get buffered when sending messages to channels that
   * aren't fully set up yet. For example, the engine isn't running yet or the channel's message
   * handler isn't set up on the Dart side yet.
   */
  resizeChannelBuffer(newSize: number): void {
    MessageChannelUtils.resizeChannelBuffer(this.messenger, this.name, newSize);
  }
}

/** A handler of incoming method calls. */
export interface MethodCallHandler {

  onMethodCall(call: MethodCall, result: MethodResult): void;

}

/**
 * Method call result callback. Supports dual use: Implementations of methods to be invoked by
 * Flutter act as clients of this interface for sending results back to Flutter. Invokers of
 * Flutter methods provide implementations of this interface for handling results received from
 * Flutter.
 *
 * <p>All methods of this class can be invoked on any thread.
 */
export interface MethodResult {
  /**
   * Handles a successful result.
   *
   * @param result The result, possibly null. The result must be an Object type supported by the
   *     codec. For instance, if you are using {@link StandardMessageCodec} (default), please see
   *     its documentation on what types are supported.
   */
  success: (result: Any) => void;

  error: (errorCode: string, errorMessage: string, errorDetails: Any) => void;

  /** Handles a call to an unimplemented method. */
  notImplemented: () => void;
}

export class IncomingResultHandler implements BinaryReply {
  private callback: MethodResult;
  private codec: MethodCodec;

  constructor(callback: MethodResult, codec: MethodCodec) {
    this.callback = callback;
    this.codec = codec
  }

  reply(reply: ArrayBuffer | null): void {
    try {
      if (reply == null) {
        this.callback.notImplemented();
      } else {
        try {
          this.callback.success(this.codec.decodeEnvelope(reply));
        } catch (e) {
          this.callback.error(e.code, e.getMessage(), e.details);
        }
      }
    } catch (e) {
      Log.e(MethodChannel.TAG, "Failed to handle method call result", e);
    }
  }
}

export class IncomingMethodCallHandler implements BinaryMessageHandler {
  private handler: MethodCallHandler;
  private codec: MethodCodec;

  constructor(handler: MethodCallHandler, codec: MethodCodec) {
    this.handler = handler;
    this.codec = codec
  }

  onMessage(message: ArrayBuffer, reply: BinaryReply): void {
    const call = this.codec.decodeMethodCall(message);
    try {
      this.handler.onMethodCall(
        call, {
        success: (result: Any): void => {
          reply.reply(this.codec.encodeSuccessEnvelope(result));
        },

        error: (errorCode: string, errorMessage: string, errorDetails: Any): void => {
          reply.reply(this.codec.encodeErrorEnvelope(errorCode, errorMessage, errorDetails));
        },

        notImplemented: (): void => {
          Log.w(MethodChannel.TAG,"method not implemented");
          reply.reply(StringUtils.stringToArrayBuffer(""));
        }
      });
    } catch (e) {
      Log.e(MethodChannel.TAG, "Failed to handle method call", e);
      reply.reply(this.codec.encodeErrorEnvelopeWithStacktrace("error", e.getMessage(), null, e));
    }
  }
}
