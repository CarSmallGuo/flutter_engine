/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on RestorationChannel.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/
import Any from '../../../plugin/common/Any';

import MethodCall from '../../../plugin/common/MethodCall';
import MethodChannel, { MethodCallHandler, MethodResult } from '../../../plugin/common/MethodChannel';
import StandardMethodCodec from '../../../plugin/common/StandardMethodCodec';
import Log from '../../../util/Log';
import StringUtils from '../../../util/StringUtils';
import DartExecutor from '../dart/DartExecutor';

export default class RestorationChannel {
  private static TAG = "RestorationChannel";

  private static CHANNEL_NAME = "flutter/restoration";

  public waitForRestorationData: boolean = false;

  public pendingFrameworkRestorationChannelRequest: MethodResult | null = null;

  public engineHasProvidedData: boolean = false;

  public frameworkHasRequestedData: boolean = false;

  // Holds the most current restoration data which may have been provided by the engine
  // via "setRestorationData" or by the framework via the method channel. This is the data the
  // framework should be restored to in case the app is terminated.
  private restorationData: Uint8Array;

  private channel: MethodChannel | null = null;

  private handler: MethodCallHandler;

  constructor(channelOrExecutor: MethodChannel | DartExecutor, waitForRestorationData: boolean) {
    if (channelOrExecutor instanceof MethodChannel) {
      this.channel = channelOrExecutor;
    } else {
      this.channel =
        new MethodChannel(channelOrExecutor, RestorationChannel.CHANNEL_NAME, StandardMethodCodec.INSTANCE);
    }
    this.waitForRestorationData = waitForRestorationData;
    this.restorationData = new Uint8Array(1).fill(0);
    this.handler = new RestorationChannelMethodCallHandler(this);
    this.channel.setMethodCallHandler(this.handler);
  }

  /** Obtain the most current restoration data that the framework has provided. */
  getRestorationData(): Uint8Array {
    return this.restorationData;
  }

  setRestorationDataOnly(data: Uint8Array) {
    this.restorationData = data;
  }

  /** Set the restoration data from which the framework will restore its state. */
  setRestorationData(data: Uint8Array) {
    this.engineHasProvidedData = true;
    if (this.pendingFrameworkRestorationChannelRequest != null) {
      // If their is a pending request from the framework, answer it.
      this.pendingFrameworkRestorationChannelRequest.success(RestorationChannelMethodCallHandler.packageData(data));
      this.pendingFrameworkRestorationChannelRequest = null;
      this.restorationData = data;
    } else if (this.frameworkHasRequestedData) {
      // If the framework has previously received the engine's restoration data, push the new data
      // directly to it. This case can happen when "waitForRestorationData" is false and the
      // framework retrieved the restoration state before it was set via this method.
      // Experimentally, this can also be used to restore a previously used engine to another state,
      // e.g. when the engine is attached to a new activity.
      this.channel?.invokeMethod(
        "push", RestorationChannelMethodCallHandler.packageData(data), {
        success: (result: Any): void => {
          this.restorationData = data;
        },

        error: (errorCode: string, errorMessage: string, errorDetails: Any): void => {
          Log.e(
            RestorationChannel.TAG,
            "Error " + errorCode + " while sending restoration data to framework: " + errorMessage
          );
        },

        notImplemented: (): void => {
          // do nothing
        }
      })
    } else {
      // Otherwise, just cache the data until the framework asks for it.
      this.restorationData = data;
    }
  }

  /**
   * Clears the current restoration data.
   *
   * <p>This should be called just prior to a hot restart. Otherwise, after the hot restart the
   * state prior to the hot restart will get restored.
   */
  clearData() {
    this.restorationData = new Uint8Array(1).fill(0);
  }
}

class RestorationChannelMethodCallHandler implements MethodCallHandler {
  private channel: RestorationChannel;

  constructor(channel: RestorationChannel) {
    this.channel = channel;
  }

  onMethodCall(call: MethodCall, result: MethodResult): void {
    const method = call.method;
    const args: Any = call.args;
    switch (method) {
      case "put": {
        this.channel.setRestorationDataOnly(args);
        result.success(null);
        break;
      }
      case "get": {
        this.channel.frameworkHasRequestedData = true;
        if (this.channel.engineHasProvidedData || !this.channel.waitForRestorationData) {
          result.success(RestorationChannelMethodCallHandler.packageData(this.channel.getRestorationData()));
          // Do not delete the restoration data on the engine side after sending it to the
          // framework. We may need to hand this data back to the operating system if the
          // framework never modifies the data (and thus doesn't send us any
          // data back).
        } else {
          this.channel.pendingFrameworkRestorationChannelRequest = result;
        }
        break;
      }
      default: {
        result.notImplemented();
        break;
      }
    }
  }

  static packageData(data: Uint8Array): Map<string, Any> {
    const packaged: Map<string, Any> = new Map();
    packaged.set("enabled", true);
    packaged.set("data", data);
    return packaged;
  }
}