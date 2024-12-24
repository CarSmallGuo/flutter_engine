// Copyright (C) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
import TextInputPlugin from '../../plugin/editing/TextInputPlugin';
import FlutterEngine from '../engine/FlutterEngine';
import KeyEventChannel, { FlutterKeyEvent } from '../engine/systemchannels/KeyEventChannel';
import { KeyEventHandler } from './KeyEventHandler';

export default class KeyboardManager {
  private keyEventChannel: KeyEventChannel | null = null
  private keyEventHandler: KeyEventHandler;

  constructor(engine: FlutterEngine, textInputPlugin: TextInputPlugin) {
    this.keyEventChannel = new KeyEventChannel(engine.dartExecutor)
    this.keyEventHandler = new KeyEventHandler(textInputPlugin);
  }

  onKeyPreIme(event: KeyEvent) : boolean {
    this.keyEventChannel?.sendFlutterKeyEvent(new FlutterKeyEvent(event), event.type == KeyType.Up, {
      onFrameworkResponse: (isEventHandled: boolean): void => {

      }
    })
    return false;
  }

  onKeyEvent(event: KeyEvent) : boolean {
    this.keyEventHandler.handleKeyEvent(event);
    return false;
  }
}