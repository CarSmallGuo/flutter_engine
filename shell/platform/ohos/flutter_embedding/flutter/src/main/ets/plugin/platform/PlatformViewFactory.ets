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

import MessageCodec from '../common/MessageCodec';
import PlatformView from './PlatformView'
import common from '@ohos.app.ability.common';
import Any from '../common/Any';

export default abstract class PlatformViewFactory {
  private createArgsCodec: MessageCodec<Any>;

  /** @param createArgsCodec the codec used to decode the args parameter of {@link #create}. */
  constructor(createArgsCodec: MessageCodec<Any>) {
    this.createArgsCodec = createArgsCodec;
  }

  /**
   * Creates a new Dynamic be embedded in the Flutter hierarchy.
   *
   * @param context the context to be used when creating the view, this is different than
   *     FlutterView's context.
   * @param viewId unique identifier for the created instance, this value is known on the Dart side.
   * @param args arguments sent from the Flutter app. The bytes for this value are decoded using the
   *     createArgsCodec argument passed to the constructor. This is null if createArgsCodec was
   *     null, or no arguments were sent from the Flutter app.
   */
  public abstract create(context: common.Context, viewId: number, args: Any): PlatformView;

  /** Returns the codec to be used for decoding the args parameter of {@link #create}. */
  getCreateArgsCodec(): MessageCodec<Any> {
    return this.createArgsCodec;
  }
}