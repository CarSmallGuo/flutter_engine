/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on KeyboardManager.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/
import TextInputPlugin from '../../plugin/editing/TextInputPlugin';
import FlutterEngine from '../engine/FlutterEngine';
import KeyEventChannel, { FlutterKeyEvent } from '../engine/systemchannels/KeyEventChannel';
import { KeyEventHandler } from './KeyEventHandler';
import { KeyCode } from '@kit.InputKit';

export default class KeyboardManager {
  private keyEventChannel: KeyEventChannel | null = null
  private keyEventHandler: KeyEventHandler;

  constructor(engine: FlutterEngine, textInputPlugin: TextInputPlugin) {
    this.keyEventChannel = new KeyEventChannel(engine.dartExecutor)
    this.keyEventHandler = new KeyEventHandler(textInputPlugin);
  }

  onKeyPreIme(event: KeyEvent): boolean {
    this.keyEventChannel?.sendFlutterKeyEvent(new FlutterKeyEvent(event), event.type == KeyType.Up, {
      onFrameworkResponse: (isEventHandled: boolean): void => {

      }
    })
    //因为onFrameworkResponse是异步返回，所以在外部判断FlutterEngine处理事件
    return event.type == KeyType.Down && FLUTTER_KEYCODE_HANDLE_SET.indexOf(event.keyCode) != -1;
  }

  onKeyEvent(event: KeyEvent): boolean {
    this.keyEventHandler.handleKeyEvent(event);
    return false;
  }
}

// 测试FlutterEngine会消费的KeyCode列表
const FLUTTER_KEYCODE_HANDLE_SET = [
  // 暂时不拦截注释类型的keyCode,避免未测试出的影响
  // KeyCode.KEYCODE_ESCAPE,
  // KeyCode.KEYCODE_DPAD_DOWN,
  // KeyCode.KEYCODE_DPAD_LEFT,
  // KeyCode.KEYCODE_DPAD_UP,
  // KeyCode.KEYCODE_DPAD_RIGHT,
  // KeyCode.KEYCODE_PAGE_UP,
  // KeyCode.KEYCODE_PAGE_DOWN,
  // KeyCode.KEYCODE_TAB,
  KeyCode.KEYCODE_DEL,
  KeyCode.KEYCODE_FORWARD_DEL,
]