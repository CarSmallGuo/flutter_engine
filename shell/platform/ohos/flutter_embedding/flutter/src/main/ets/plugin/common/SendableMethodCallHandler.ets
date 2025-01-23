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
import { lang } from '@kit.ArkTS';

import MethodCall from './MethodCall';
import { MethodResult } from './MethodChannel';

/** A handler of incoming method calls. */
type ISendable = lang.ISendable;
export default interface SendableMethodCallHandler extends ISendable {
  // isSendableMethodCallHandler: () => boolean;
  /**
   * Handles the specified method call received from Flutter.
   *
   * <p>Handler implementations must submit a result for all incoming calls, by making a single
   * call on the given {@link Result} callback. Failure to do so will result in lingering Flutter
   * result handlers. The result may be submitted asynchronously and on any thread. Calls to
   * unknown or unimplemented methods should be handled using {@link Result#notImplemented()}.
   *
   * <p>Any uncaught exception thrown by this method will be caught by the channel implementation
   * and logged, and an error result will be sent back to Flutter.
   *
   * <p>The handler is called on the platform thread (Android main thread) by default, or
   * otherwise on the thread specified by the {@link BinaryMessenger.TaskQueue} provided to the
   * associated {@link MethodChannel} when it was created. See also <a
   * href="https://github.com/flutter/flutter/wiki/The-Engine-architecture#threading">Threading in
   * the Flutter Engine</a>.
   *
   * @param call A {@link MethodCall}.
   * @param result A {@link Result} used for submitting the result of the call.
   */
  onMethodCall(call: MethodCall, result: MethodResult, ...args: Object[]): void;
}