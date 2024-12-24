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
import AccessibilityChannel, {AccessibilityMessageHandler} from '../embedding/engine/systemchannels/AccessibilityChannel';
import { ByteBuffer } from '../util/ByteBuffer';
import Log from '../util/Log';

const TAG = "AccessibilityBridge";

export default class AccessibilityBridge implements AccessibilityMessageHandler {

  private accessibilityChannel: AccessibilityChannel | null = null;

  constructor(){

  }

  announce(message: string): void {
    throw new Error('Method not implemented.');
    // android -> rootAccessibilityView.announceForAccessibility(message);
  }

  onTap(nodeId: number): void {
    throw new Error('Method not implemented.');
    // android -> sendAccessibilityEvent(nodeId, AccessibilityEvent.TYPE_VIEW_CLICKED);
  }

  onLongPress(nodeId: number): void {
    throw new Error('Method not implemented.');
    // android -> sendAccessibilityEvent(nodeId, AccessibilityEvent.TYPE_VIEW_LONG_CLICKED);
  }

  onTooltip(nodeId: string): void {
    throw new Error('Method not implemented.');
  }

  updateSemantics(buffer: ByteBuffer, strings: string[], stringAttributeArgs: ByteBuffer[]): void {
      Log.d(TAG, "AccessibilityBridge.ets updateSemantics is called");
  }

  updateCustomAccessibilityActions(buffer: ByteBuffer, strings: string[]): void {
    Log.d(TAG, "AccessibilityBridge.ets updateCustomAccessibilityActions is called");

  }

  accessibilityStateChange(state: Boolean): void{
    Log.d(TAG, "AccessibilityBridge.ets accessibilityStateChange is called");
  }

}

export class AccessibilityManager {
  private fontWeightScale: number | null = null;

  setFontWeightScale(fontWeightScale: number): void {
    this.fontWeightScale  = fontWeightScale;
    Log.i(TAG, 'setFontWeightScale: ' + JSON.stringify(this.fontWeightScale));
  }

  getFontWeightScale(): number {
    Log.i(TAG, 'getFontWeightScale: ' + JSON.stringify(this.fontWeightScale));
    return this.fontWeightScale!;
  }
}

export enum Action {
  TAP = 1 << 0,
  LONG_PRESS = 1 << 1,
  SCROLL_LEFT = 1 << 2,
  SCROLL_RIGHT = 1 << 3,
  SCROLL_UP = 1 << 4,
  SCROLL_DOWN = 1 << 5,
  INCREASE = 1 << 6,
  DECREASE = 1 << 7,
  SHOW_ON_SCREEN = 1 << 8,
  MOVE_CURSOR_FORWARD_BY_CHARACTER = 1 << 9,
  MOVE_CURSOR_BACKWARD_BY_CHARACTER = 1 << 10,
  SET_SELECTION = 1 << 11,
  COPY = 1 << 12,
  CUT = 1 << 13,
  PASTE = 1 << 14,
  DID_GAIN_ACCESSIBILITY_FOCUS = 1 << 15,
  DID_LOSE_ACCESSIBILITY_FOCUS = 1 << 16,
  CUSTOM_ACTION = 1 << 17,
  DISMISS = 1 << 18,
  MOVE_CURSOR_FORWARD_BY_WORD = 1 << 19,
  MOVE_CURSOR_BACKWARD_BY_WORD = 1 << 20,
  SET_NEXT = 1 << 21,
};