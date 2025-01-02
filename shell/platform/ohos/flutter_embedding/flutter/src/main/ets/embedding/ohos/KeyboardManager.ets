/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Based on KeyboardManager.java originally written by
* Copyright (C) 2021  LongCatIsLooong <31859944+longcatislooong@users.noreply.github.com>
*
*/
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