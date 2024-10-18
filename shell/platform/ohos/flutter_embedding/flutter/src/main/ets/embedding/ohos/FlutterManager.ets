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
import { common } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import { FlutterView } from '../../view/FlutterView';
import Log from '../../util/Log';

const TAG = "FlutterManager"

export default class FlutterManager {
  private static instance: FlutterManager;

  static getInstance(): FlutterManager {
    if (FlutterManager.instance == null) {
      FlutterManager.instance = new FlutterManager();
    }
    return FlutterManager.instance;
  }

  private flutterViewList = new Map<String, FlutterView>();
  private flutterViewIndex = 1;
  private mFullScreenListener: FullScreenListener = new DefaultFullScreenListener();

  hasFlutterView(viewId: string): boolean {
    return this.flutterViewList.has(viewId);
  }

  getFlutterView(viewId: string): FlutterView | null {
    return this.flutterViewList.get(viewId) ?? null;
  }

  getFlutterViewList(): Map<String, FlutterView> {
    return this.flutterViewList;
  }

  private putFlutterView(viewId: string, flutterView?: FlutterView): void {
    if (flutterView != null) {
      this.flutterViewList.set(viewId, flutterView);
    } else {
      this.flutterViewList.delete(viewId);
    }
  }

  /**
   * It's suggested to keep 'oh_flutter_' as the prefix for xcomponent_id.
   * Otherwise it might affect the performance.
   */
  createFlutterView(context: common.UIAbilityContext): FlutterView {
    let flutterView = new FlutterView(`oh_flutter_${this.flutterViewIndex++}`, context);
    this.putFlutterView(flutterView.getId(), flutterView);
    return flutterView;
  }

  clear(): void {
    this.flutterViewList.clear();
  }

  setFullScreenListener(listener: FullScreenListener) {
    this.mFullScreenListener = listener
  }

  getFullScreenListener(): FullScreenListener {
    return this.mFullScreenListener;
  }

  setUseFullScreen(use: boolean) {
    this.mFullScreenListener.setUseFullScreen(use)
  }

  useFullScreen(): boolean {
    return this.mFullScreenListener.useFullScreen();
  }
}

export interface FullScreenListener {
  useFullScreen(): boolean;
  setUseFullScreen(useFullScreen: boolean): void;
  onScreenStateChanged(data: window.WindowStatusType): void;
}

export class DefaultFullScreenListener implements FullScreenListener {
  private fullScreen: boolean = true;
  private skipCheck: boolean = false;

  useFullScreen(): boolean {
    return this.fullScreen;
  }

  setUseFullScreen(useFullScreen: boolean): void {
    this.fullScreen = useFullScreen;
    this.skipCheck = true;

    const windowStage = (getContext(this) as common.UIAbilityContext).windowStage;
    windowStage.getMainWindowSync().setWindowLayoutFullScreen(useFullScreen);
    Log.i(TAG, "WindowLayoutFullScreen is on")
  }

  onScreenStateChanged(data: window.WindowStatusType): void {
    if (this.skipCheck) {
      Log.i(TAG, "onScreenStateChanged: skipCheck is on, WindowStatusType = " + data)
      return;
    }
    switch (data) {
      case window.WindowStatusType.FULL_SCREEN:
      case window.WindowStatusType.SPLIT_SCREEN:
      case window.WindowStatusType.FLOATING:
      case window.WindowStatusType.MAXIMIZE:
        this.fullScreen = true;
        Log.i(TAG, "onScreenStateChanged: fullScreen = true")
        break;
      default:
        this.fullScreen = false;
        Log.i(TAG, "onScreenStateChanged: fullScreen = false")
        break;
    }
  }

}
