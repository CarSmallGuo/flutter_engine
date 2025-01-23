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

import { TouchEvent } from '@ohos.multimodalInput.touchEvent';
import Any from '../../plugin/common/Any';

export default class OhosTouchProcessor {
  private static POINTER_DATA_FIELD_COUNT: number = 35;

  static BYTES_PER_FIELD: number = 8;

  private static POINTER_DATA_FLAG_BATCHED: number = 1;

  public onTouchEvent(event: TouchEvent, transformMatrix: Any): void {

  }
}

export enum PointerChange {
  CANCEL = 0,
  ADD = 1,
  REMOVE = 2,
  HOVER = 3,
  DOWN = 4,
  MOVE = 5,
  UP = 6,
  PAN_ZOOM_START = 7,
  PAN_ZOOM_UPDATE = 8,
  PAN_ZOOM_END = 9
}

export enum PointerDeviceKind {
  TOUCH = 0,
  MOUSE = 1,
  STYLUS = 2,
  INVERTED_STYLUS = 3,
  TRACKPAD = 4,
  UNKNOWN = 5
}

export enum PointerSignalKind {
  NONE = 0,
  SCROLL = 1,
  SCROLL_INERTIA_CANCEL = 2,
  SCALE = 3,
  UNKNOWN = 4
}