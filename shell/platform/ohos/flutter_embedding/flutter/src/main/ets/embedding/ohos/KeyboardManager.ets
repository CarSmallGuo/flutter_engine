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
*/
import TextInputPlugin from '../../plugin/editing/TextInputPlugin';
import FlutterEngine from '../engine/FlutterEngine';
import KeyEventChannel, { FlutterKeyEvent } from '../engine/systemchannels/KeyEventChannel';
import { KeyEventHandlerForEmulator } from './KeyEventHandlerForEmulator';

export default class KeyboardManager {
  private keyEventChannel: KeyEventChannel | null = null
  private keyEventHandlerForEmulator: KeyEventHandlerForEmulator;

  constructor(engine: FlutterEngine, textInputPlugin: TextInputPlugin) {
    this.keyEventChannel = new KeyEventChannel(engine.dartExecutor)
    this.keyEventHandlerForEmulator = new KeyEventHandlerForEmulator(textInputPlugin);
  }

  handleKeyEvent(event: KeyEvent) {
    this.keyEventChannel?.sendFlutterKeyEvent(new FlutterKeyEvent(event), event.type == KeyType.Up, {
      onFrameworkResponse: (isEventHandled: boolean): void => {

      }
    })
    this.keyEventHandlerForEmulator.handleKeyEvent(event);
  }
}