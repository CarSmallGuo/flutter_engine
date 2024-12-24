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

import FlutterEngine from '../engine/FlutterEngine';
import PlatformPlugin from '../../plugin/PlatformPlugin';
import Want from '@ohos.app.ability.Want';
import FlutterShellArgs from '../engine/FlutterShellArgs';
import UIAbility from '@ohos.app.ability.UIAbility';
import ExclusiveAppComponent from './ExclusiveAppComponent';
import { FlutterAbilityAndEntryDelegate, Host } from './FlutterAbilityAndEntryDelegate';
import FlutterAbilityLaunchConfigs from './FlutterAbilityLaunchConfigs';
import Log from '../../util/Log';
import { FlutterView } from '../../view/FlutterView';
import FlutterManager from './FlutterManager';
import window from '@ohos.window';
import FlutterEngineConfigurator from './FlutterEngineConfigurator';
import { FlutterPlugin } from '../engine/plugins/FlutterPlugin';

const TAG = "FlutterEntry";

export default class FlutterEntry implements Host {
  private static ARG_SHOULD_ATTACH_ENGINE_TO_ABILITY: string =  "should_attach_engine_to_ability";

  protected uiAbility: UIAbility | null = null
  protected delegate: FlutterAbilityAndEntryDelegate | null = null
  protected flutterView: FlutterView | null = null
  protected context: Context;
  protected windowStage: window.WindowStage | null = null
  private parameters: Record<string, Object> = {};
  protected engineConfigurator: FlutterEngineConfigurator | null = null
  protected hasInit: boolean = false;

  constructor(context: Context, params: Record<string, Object> = {}) {
    this.context = context;
    this.uiAbility = FlutterManager.getInstance().getUIAbility(context);
    this.parameters = params;
    this.windowStage = FlutterManager.getInstance().getWindowStage(this.uiAbility);
    this.hasInit = false;
  }

  async aboutToAppear() {
    Log.i(TAG, 'aboutToAppear');
    if (this.hasInit == false) {
      this.delegate = new FlutterAbilityAndEntryDelegate(this);
      this.flutterView = this.delegate?.createView(this.context);
      this.flutterView?.onWindowCreated();
      await this?.delegate?.onAttach(this.context);
      Log.i(TAG, 'onAttach end');
      this?.delegate?.platformPlugin?.setUIAbilityContext(this.uiAbility!!.context);
      this.delegate?.onCreate();
      this.delegate?.onWindowStageCreate()
      this.windowStage?.on('windowStageEvent', this.windowStageEventCallback);
      this.hasInit = true;
      this.delegate?.initWindow();
    }
  }

  protected windowStageEventCallback = (data: window.WindowStageEventType) => {
    let stageEventType: window.WindowStageEventType = data;
    switch (stageEventType) {
      case window.WindowStageEventType.SHOWN: // 切到前台
        Log.i(TAG, 'windowStage foreground.');
        break;
      case window.WindowStageEventType.ACTIVE: // 获焦状态
        Log.i(TAG, 'windowStage active.');
        this.delegate?.getFlutterEngine()?.getTextInputChannel()?.textInputMethodHandler?.handleChangeFocus(true);
        this?.delegate?.onWindowFocusChanged(true);
        break;
      case window.WindowStageEventType.INACTIVE: // 失焦状态
        Log.i(TAG, 'windowStage inactive.');
        this?.delegate?.onWindowFocusChanged(false);
        break;
      case window.WindowStageEventType.HIDDEN: // 切到后台
        Log.i(TAG, 'windowStage background.');
        break;
    }
  }

  setFlutterEngineConfigurator(configurator: FlutterEngineConfigurator) {
    this.engineConfigurator = configurator;
  }

  getFlutterView(): FlutterView {
    return this.flutterView!!
  }

  getFlutterEngine(): FlutterEngine | null {
    return this.delegate?.flutterEngine!
  }

  aboutToDisappear() {
    Log.d(TAG, "FlutterEntry aboutToDisappear===");
    try {
      this.windowStage?.off('windowStageEvent', this.windowStageEventCallback);
    } catch (err) {

    }
    if (this.flutterView != null) {
      this.flutterView.onDestroy();
      this.flutterView = null;
    }
    if (this.delegate != null) {
      this.delegate?.onDetach();
      this.delegate?.release()
    }
  }

  onPageShow() { //生命周期
    Log.d(TAG, "FlutterEntry onPageShow===");
    this?.delegate?.onForeground();
  }

  onPageHide() { //生命周期
    Log.d(TAG, "FlutterEntry onPageHide===");
    this?.delegate?.onBackground();
  }

  onBackPress() {
    Log.d(TAG, "FlutterEntry onBackPress===");
    this?.delegate?.flutterEngine?.getNavigationChannel()?.popRoute();
    this?.delegate?.flutterEngine?.getTextInputChannel()?.textInputMethodHandler?.hide();
  }

  shouldDispatchAppLifecycleState(): boolean {
    return true;
  }

  detachFromFlutterEngine() {
    if (this?.delegate != null) {
      this?.delegate?.onDetach();
    }
  }

  getAbility(): UIAbility {
    return this.uiAbility!!
  }

  loadContent() {

  }

  shouldAttachEngineToAbility(): boolean {
    let param = this.parameters![FlutterEntry.ARG_SHOULD_ATTACH_ENGINE_TO_ABILITY];
    if (!param) {
      return true;
    }
    return param as boolean
  }

  getCachedEngineId(): string {
    let param = this.parameters![FlutterAbilityLaunchConfigs.EXTRA_CACHED_ENGINE_ID];
    if (!param) {
      return "";
    }
    return param as string
  }

  getCachedEngineGroupId(): string | null {
    let param = this.parameters![FlutterAbilityLaunchConfigs.EXTRA_CACHED_ENGINE_GROUP_ID];
    if (!param) {
      return null;
    }
    return param as string
  }

  shouldDestroyEngineWithHost(): boolean {
    if ((this.getCachedEngineId() != null && this.getCachedEngineId().length > 0) || this.delegate!!.isFlutterEngineFromHost()) {
      // Only destroy a cached engine if explicitly requested by app developer.
      return false;
    }
    return true;
  }

  attachToEngineAutomatically(): boolean {
    return true;
  }

  getFlutterShellArgs(): FlutterShellArgs {
    return new FlutterShellArgs();
  }

  getDartEntrypointArgs(): string[] {
    if (this.parameters![FlutterAbilityLaunchConfigs.EXTRA_DART_ENTRYPOINT_ARGS]) {
      return this.parameters![FlutterAbilityLaunchConfigs.EXTRA_DART_ENTRYPOINT_ARGS] as Array<string>;
    }
    return new Array<string>()
  }

  getDartEntrypointLibraryUri(): string {
    return "";
  }

  getAppBundlePath(): string {
    return "";
  }

  getDartEntrypointFunctionName(): string {
    if (this.parameters![FlutterAbilityLaunchConfigs.EXTRA_DART_ENTRYPOINT]) {
      return this.parameters![FlutterAbilityLaunchConfigs.EXTRA_DART_ENTRYPOINT] as string;
    }
    return FlutterAbilityLaunchConfigs.DEFAULT_DART_ENTRYPOINT
  }

  getInitialRoute(): string {
    if (this.parameters![FlutterAbilityLaunchConfigs.EXTRA_INITIAL_ROUTE]) {
      return this.parameters![FlutterAbilityLaunchConfigs.EXTRA_INITIAL_ROUTE] as string
    }
    return "";
  }

  getWant(): Want {
    return new Want();
  }

  shouldRestoreAndSaveState(): boolean {
    if (this.parameters![FlutterAbilityLaunchConfigs.EXTRA_CACHED_ENGINE_ID] != undefined) {
      return this.parameters![FlutterAbilityLaunchConfigs.EXTRA_CACHED_ENGINE_ID] as boolean;
    }
    if (this.getCachedEngineId() != null && this.getCachedEngineId().length > 0) {
      // Prevent overwriting the existing state in a cached engine with restoration state.
      return false;
    }
    return true;
  }

  getExclusiveAppComponent(): ExclusiveAppComponent<UIAbility> | null {
    return this.delegate ? this.delegate : null
  }

  provideFlutterEngine(context: Context): FlutterEngine | null {
    return null;
  }

  providePlatformPlugin(flutterEngine: FlutterEngine): PlatformPlugin | undefined {
    return new PlatformPlugin(flutterEngine.getPlatformChannel()!, this.context, this);
  }

  configureFlutterEngine(flutterEngine: FlutterEngine) {
    if (this.engineConfigurator) {
      this.engineConfigurator.configureFlutterEngine(flutterEngine)
    }
  }

  cleanUpFlutterEngine(flutterEngine: FlutterEngine) {
    if (this.engineConfigurator) {
      this.engineConfigurator.cleanUpFlutterEngine(flutterEngine)
    }
  }

  popSystemNavigator(): boolean {
    return false;
  }

  addPlugin(plugin: FlutterPlugin): void {
    this.delegate?.addPlugin(plugin)
  }

  removePlugin(plugin: FlutterPlugin): void {
    this.delegate?.removePlugin(plugin)
  }
}