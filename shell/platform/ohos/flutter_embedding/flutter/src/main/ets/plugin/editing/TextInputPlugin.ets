/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on TextInputPlugin.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/

import TextInputChannel, {
  Configuration,
  TextEditState,
  TextInputMethodHandler,
} from '../../embedding/engine/systemchannels/TextInputChannel';
import inputMethod from '@ohos.inputMethod';
import Log from '../../util/Log';
import { EditingStateWatcher, ListenableEditingState } from './ListenableEditingState';
import Any from '../common/Any';
import { inputDevice } from '@kit.InputKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { deviceInfo } from '@kit.BasicServicesKit';

/// 临时规避缺少newline对应枚举问题
const NEWLINE_KEY_TYPE: number = 8;
const sdkApiVersion: number = deviceInfo.sdkApiVersion;

export default class TextInputPlugin implements EditingStateWatcher {
  private static TAG = "TextInputPlugin";
  private textInputChannel: TextInputChannel;
  private mTextInputHandler: TextInputMethodHandlerImpl;

  constructor(textInputChannel: TextInputChannel) {
    this.textInputChannel = textInputChannel;
    this.mTextInputHandler = new TextInputMethodHandlerImpl(this);
    this.textInputChannel.setTextInputMethodHandler(this.mTextInputHandler);
  }

  public clearTextInputClient() {
    this.textInputChannel.textInputMethodHandler?.clearClient();
  }

  setTextInputEditingState(state: TextEditState) {

  }

  getEditingState() {
    return this.mTextInputHandler.mEditable;
  }

  // do preview the text in the editing widget
  didChangePreviewTextEditingState(inputTarget: InputTarget, editable: ListenableEditingState): void {
    let stringCache: string = editable.getStringCache();
    // obtain the start index of editing preview text
    let startIdx: number = editable.getLeftIdxOfPreviewTextRange();
    let leftStringCache  = stringCache.substring(0, startIdx);
    let rightStringCache = stringCache.substring(startIdx, stringCache.length);
    // concat the string cache and preview text and show on the TextField
    let finalStringCache = leftStringCache + editable.getPreviewText() + rightStringCache;
    // update the selection range
    let newSelectionStart: number = startIdx + editable.getPreviewText().length;
    let newSelectionEnd: number = startIdx + editable.getPreviewText().length;

    this.textInputChannel.updateEditingState(inputTarget.id, finalStringCache,
      newSelectionStart, newSelectionEnd,
      editable.getComposingStart(), editable.getComposingEnd())
  }

  didChangeEditingState(textChanged: boolean, selectionChanged: boolean, composingRegionChanged: boolean): void {
    let editable = this.mTextInputHandler.mEditable;
    let inputTarget = this.mTextInputHandler.inputTarget;
    let configuration = this.mTextInputHandler.configuration;
    if (configuration != null && configuration.enableDeltaModel) {
      this.textInputChannel.updateEditingStateWithDeltas(inputTarget.id, editable.extractBatchTextEditingDeltas());
      editable.clearBatchDeltas();
    } else if (sdkApiVersion >= 17 && editable.getPreviewText() != "") {
      this.didChangePreviewTextEditingState(inputTarget, editable);
    } else {
      this.textInputChannel.updateEditingState(inputTarget.id, editable.getStringCache(),
        editable.getSelectionStart(), editable.getSelectionEnd(),
        editable.getComposingStart(), editable.getComposingEnd())
    }
  }

  detach(): void {
    this.mTextInputHandler.inputMethodController.detach((err) => {
      if (err) {
        Log.e(TextInputPlugin.TAG, "Failed to detach: " + JSON.stringify(err));
      }
    })
  }

  destroy() {
    this.textInputChannel.setTextInputMethodHandler(null);
  }
}

class TextInputMethodHandlerImpl implements TextInputMethodHandler {
  private static TAG = "TextInputMethodHandlerImpl";
  private textConfig: inputMethod.TextConfig;
  inputMethodController: inputMethod.InputMethodController;
  inputTarget: InputTarget;
  public configuration: Configuration | null = null;
  mEditable: ListenableEditingState;
  private mRestartInputPending: boolean = false;
  private plugin: EditingStateWatcher | Any;
  private imcFlag: boolean = false;
  private inputTypeNone: string = 'NONE';
  private keyboardStatus: inputMethod.KeyboardStatus = inputMethod.KeyboardStatus.HIDE;
  private inputAttribute: inputMethod.InputAttribute = { textInputType: 0, enterKeyType: 1 };
  private keyboardFocusState: boolean = false

  constructor(plugin: TextInputPlugin | Any) {
    this.textConfig = {
      inputAttribute: this.inputAttribute
    };
    this.plugin = plugin;
    this.mEditable = new ListenableEditingState(null, 0);
    this.inputMethodController = inputMethod.getController();
    this.inputTarget = new InputTarget(Type.NO_TARGET, 0);
  }

  /// 通过判断是否是TextInputType.none来决定是否弹出键盘
  show(): void {
    if (this.canShowTextInput()) {
      this.keyboardFocusState = true;
      this.showTextInput();
    } else {
      this.hide();
    }
  }

  hide(): void {
    this.keyboardFocusState = false;
    this.hideTextInput();
  }

  requestAutofill(): void {

  }

  finishAutofillContext(shouldSave: boolean): void {

  }

  setClient(textInputClientId: number, configuration: Configuration | null): void {
    Log.d(TextInputMethodHandlerImpl.TAG, "textInputClientId: " + textInputClientId);
    this.setTextInputClient(textInputClientId, configuration);
  }

  setPlatformViewClient(id: number, usesVirtualDisplay: boolean): void {

  }

  setEditableSizeAndTransform(width: number, height: number, transform: number[]): void {

  }

  setCursorSizeAndPosition(cursorInfo: inputMethod.CursorInfo) {
    this.inputMethodController.updateCursor(cursorInfo)
  }

  setEditingState(editingState: TextEditState): void {
    Log.d(TextInputMethodHandlerImpl.TAG,
      "text:" + editingState.text + " selectionStart:" + editingState.selectionStart + " selectionEnd:"
        + editingState.selectionEnd + " composingStart:" + editingState.composingStart + " composingEnd" +
      editingState.composingEnd);
    this.mEditable.setPreviewText("");
    this.mEditable.setIsCursorIdxOutOfPreviewTextRange(false);
    this.mEditable.updateTextInputState(editingState);
    if (sdkApiVersion >= 17) {
      this.previewTextChangeSelection(editingState);
    }
  }

  previewTextChangeSelection(editingState: TextEditState): void {
    // if users only modify the cursor index without typing new preview text,
    // clear the current preview text cache for avoiding inserting redundant preview text
    // to insert at the current cursor position
    let currCursorIndex: number = editingState.selectionStart;
    let leftIdxOfPreviewText: number = this.mEditable.getLeftIdxOfPreviewTextRange();
    let rightIdxOfPreviewText: number = this.mEditable.getRightIdxOfPreviewTextRange();

    let isCursorIdxOutOfPreviewTextRange = (leftIdxOfPreviewText != rightIdxOfPreviewText) &&
      (currCursorIndex < leftIdxOfPreviewText || currCursorIndex > rightIdxOfPreviewText)
    // if user move the current cursor index out of editing preview text range,
    // immediately inserting the first candidate word into the editing widget
    if (isCursorIdxOutOfPreviewTextRange) {
      try {
        // change selection async callback
        this.inputMethodController.changeSelection(editingState.text, leftIdxOfPreviewText, rightIdxOfPreviewText)
          .then(() => {
            // force the cursor position at the end of editing preview text
            editingState.selectionStart = rightIdxOfPreviewText;
            editingState.selectionEnd = rightIdxOfPreviewText;
            // update TextInputState and reset the preview text state
            this.mEditable.updateTextInputState(editingState);
            this.mEditable.setPreviewText("");
          })
          .catch((err: BusinessError) => {
            console.error(`Failed to previewTextChangeSelection: ${JSON.stringify(err)}`);
          })
      } catch (err) {
        console.error(`Failed to previewTextChangeSelection: ${JSON.stringify(err)}`);
      }
    }
  }

  clearClient(): void {
    this.clearTextInputClient();
  }

  private async showTextInput(): Promise<void> {
    if (this.keyboardStatus == inputMethod.KeyboardStatus.SHOW) {
      return;
    }
    await this.attach(true);
    if (!this.imcFlag) {
      this.listenKeyBoardEvent();
    }
  }

  private async hideTextInput(): Promise<void> {
    this.inputMethodController.detach();
    this.keyboardStatus = inputMethod.KeyboardStatus.NONE;
    this.cancelListenKeyBoardEvent();
  }

  async attach(showKeyboard: boolean): Promise<void> {
    try {
      // must register previewtext callbacks before attachment
      if (sdkApiVersion >= 17) {
        this.registerPreviewTextCallbacks();
      } else {
        Log.w(TextInputMethodHandlerImpl.TAG, "The preview text feature of InputMethod is only supported at API-17+")
      }
      await this.inputMethodController.attach(showKeyboard, this.textConfig);
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to attach:" + JSON.stringify(err));
    }
  }

  handleChangeFocus(focusState: boolean) {
    try {
      inputDevice.getDeviceList((Error: Error, ids: Array<number>) => {
        let isPhysicalKeyboard = false;
        for (let i = 0; i < ids.length; i++) {
          const type = inputDevice.getKeyboardTypeSync(ids[i]);
          if (type == inputDevice.KeyboardType.ALPHABETIC_KEYBOARD ||
            type == inputDevice.KeyboardType.DIGITAL_KEYBOARD) {
            isPhysicalKeyboard = true;
            break;
          }
        }

        if (focusState && isPhysicalKeyboard && this.keyboardFocusState) {
          this.cancelListenKeyBoardEvent();
          this.inputMethodController.detach().then(async () => {
            await this.attach(true);
            this.listenKeyBoardEvent();
          })
        }
      })
    } catch (error) {
      Log.e(TextInputMethodHandlerImpl.TAG,
        `Failed to query device. Code is ${error.code}, message is ${error.message}`)
    }
  }

  async updateAttribute(): Promise<void> {
    if (this.keyboardStatus != inputMethod.KeyboardStatus.SHOW) {
      return;
    }
    try {
      await this.inputMethodController.updateAttribute(this.inputAttribute);
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to updateAttribute:" + JSON.stringify(err));
    }
  }

  setTextInputClient(client: number, configuration: Configuration | null): void {
    const INPUT_TYPE_NAME =
      ['NONE', 'TEXT', 'MULTILINE', 'NUMBER', 'PHONE', 'DATETIME', 'EMAIL_ADDRESS', 'URL', 'VISIBLE_PASSWORD']
    if (configuration) {
      this.configuration = configuration;
      if (configuration.inputType) {
        this.textConfig.inputAttribute.textInputType = INPUT_TYPE_NAME.indexOf(configuration.inputType.type) - 1
        this.textConfig.inputAttribute.enterKeyType = configuration.inputAction as Any;
      }
    }
    if (this.canShowTextInput()) {
      this.inputTarget = new InputTarget(Type.FRAMEWORK_CLIENT, client);
    } else {
      this.inputTarget = new InputTarget(Type.NO_TARGET, client);
    }
    this.mEditable.removeEditingStateListener(this.plugin);

    this.mEditable = new ListenableEditingState(this.plugin.textInputChannel, this.inputTarget.id);

    this.mRestartInputPending = true;
    this.mEditable.addEditingStateListener(this.plugin);

    this.inputAttribute = this.textConfig.inputAttribute;

    this.updateAttribute();
  }

  canShowTextInput(): boolean {
    if (this.configuration == null || this.configuration.inputType == null) {
      return true;
    }
    return this.configuration.inputType.type != this.inputTypeNone;
  }

  registerPreviewTextCallbacks(): void {
    try {
      this.inputMethodController.on("setPreviewText", this.setPreviewTextCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe setPreviewText:" + JSON.stringify(err));
      this.unregisterPreviewTextCallbacks();
      return;
    }

    try {
      this.inputMethodController.on("finishTextPreview", this.finishPreviewTextCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe finishTextPreview:" + JSON.stringify(err));
      this.unregisterPreviewTextCallbacks();
      return;
    }
  }

  unregisterPreviewTextCallbacks(): void {
    this.inputMethodController?.off("setPreviewText", this.setPreviewTextCallback)
    this.inputMethodController?.off("finishTextPreview", this.finishPreviewTextCallback);
  }

  listenKeyBoardEvent(): void {
    try {
      this.inputMethodController.on('insertText', this.insertTextCallback);
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe insertText:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }

    try {
      this.inputMethodController.on('deleteLeft', this.deleteLeftCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe deleteLeft:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }

    try {
      this.inputMethodController.on('deleteRight', this.deleteRightCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe deleteRight:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }

    try {
      this.inputMethodController.on('sendFunctionKey', this.sendFunctionKeyCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe sendFunctionKey:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }

    try {
      this.inputMethodController.on('sendKeyboardStatus', this.sendKeyboardStatusCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe sendKeyboardStatus:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }

    try {
      this.inputMethodController.on('selectByRange', this.selectByRangeCallback)
    } catch (err) {
      Log.e(TextInputMethodHandlerImpl.TAG, "Failed to subscribe selectByRange:" + JSON.stringify(err));
      this.cancelListenKeyBoardEvent();
      return;
    }
    Log.d(TextInputMethodHandlerImpl.TAG, "listenKeyBoardEvent success");
    this.imcFlag = true;
  }

  private setPreviewTextCallback = (text: string, range: inputMethod.Range) => {
    this.mEditable.handleInsertPreviewTextEvent(text, range);
  }
  private finishPreviewTextCallback = () => {
    this.mEditable.clearPreviewTextContents();
    Log.d(TextInputMethodHandlerImpl.TAG, "finishPreviewTextCallback");
  }
  private insertTextCallback = (text: string) => {
    this.mEditable.handleInsertTextEvent(text);
  }
  private deleteLeftCallback = (length: number) => {
    this.mEditable.handleDeleteEvent(false, length);
  }
  private deleteRightCallback = (length: number) => {
    this.mEditable.handleDeleteEvent(true, length);
  }
  private sendFunctionKeyCallback = (functionKey: inputMethod.FunctionKey) => {
    /// 临时规避缺少newline对应枚举类型问题
    if (functionKey.enterKeyType == NEWLINE_KEY_TYPE) {
      this.mEditable.handleNewlineEvent();
    }
    this.mEditable.handleFunctionKey(functionKey);
  }
  private sendKeyboardStatusCallback = (state: inputMethod.KeyboardStatus) => {
    this.keyboardStatus = state;
  }
  private selectByRangeCallback = (range: inputMethod.Range) => {
    this.mEditable.handleSelectByRange(range);
  }

  cancelListenKeyBoardEvent(): void {
    this.inputMethodController?.off('insertText', this.insertTextCallback);
    this.inputMethodController?.off('deleteLeft', this.deleteLeftCallback);
    this.inputMethodController?.off('deleteRight', this.deleteRightCallback);
    this.inputMethodController?.off('sendFunctionKey', this.sendFunctionKeyCallback);
    this.inputMethodController?.off('sendKeyboardStatus', this.sendKeyboardStatusCallback);
    this.inputMethodController?.off('selectByRange', this.selectByRangeCallback);
    this.imcFlag = false;
  }

  public clearTextInputClient(): void {
    if (this.inputTarget.type == Type.VIRTUAL_DISPLAY_PLATFORM_VIEW) {
      return;
    }
    this.mEditable.removeEditingStateListener(this.plugin);
    this.configuration = null;
    this.inputTarget = new InputTarget(Type.NO_TARGET, 0);
  }
}

enum Type {
  NO_TARGET,
  // InputConnection is managed by the TextInputPlugin, and events are forwarded to the Flutter
  // framework.
  FRAMEWORK_CLIENT,
  // InputConnection is managed by a platform view that is presented on a virtual display.
  VIRTUAL_DISPLAY_PLATFORM_VIEW,
  PHYSICAL_DISPLAY_PLATFORM_VIEW,
}

export class InputTarget {
  type: Type;
  id: number;

  constructor(type: Type, id: number) {
    this.type = type;
    this.id = id;
  }
}