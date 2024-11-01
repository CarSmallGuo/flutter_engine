// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import FlutterEngine from '../engine/FlutterEngine';
import KeyEventChannel, { FlutterKeyEvent } from '../engine/systemchannels/KeyEventChannel';

export default class KeyboardManager {
  private keyEventChannel: KeyEventChannel | null = null

  constructor(engine: FlutterEngine) {
    this.keyEventChannel = new KeyEventChannel(engine.dartExecutor)
  }

  handleKeyEvent(event: KeyEvent) {
    this.keyEventChannel?.sendFlutterKeyEvent(new FlutterKeyEvent(event), event.type == KeyType.Up, {
      onFrameworkResponse: (isEventHandled: boolean): void => {

      }
    })
  }
}