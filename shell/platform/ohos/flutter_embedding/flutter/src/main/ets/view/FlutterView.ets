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
import util from '@ohos.util';
import FlutterEngine from '../embedding/engine/FlutterEngine';
import FlutterNapi from '../embedding/engine/FlutterNapi';
import { RootDvModeManager } from '../plugin/platform/RootDvModelManager';
import { BuilderParams, DVModel, DVModelChildren, DVModelEvents, DVModelParameters } from './DynamicView/dynamicView';


@Component
struct XComponentStruct {
  private context: ESObject;
  dvModelParams: DVModelParameters = new DVModelParameters();

  build() {
    XComponent({
      id: (this.dvModelParams as Record<string, ESObject>)["xComponentId"],
      type: 'texture',
      libraryname: 'flutter'
    })
      .onLoad((context) => {
        this.context = context;
      })
      .onDestroy(() => {
      })
  }
}

@Builder
function BuildXComponentStruct(buildParams: BuilderParams) {
  XComponentStruct({ dvModelParams: buildParams.params });
}

export class FlutterView {
  private xcomponentId: number
  private flutterEngine?: FlutterEngine | null
  private flutterNapi: FlutterNapi
  private isAttach: boolean

  constructor(flutterNapi: FlutterNapi) {
    this.flutterNapi = flutterNapi
    this.isAttach = false
    this.xcomponentId = Math.floor(Math.random() * (Number.MAX_SAFE_INTEGER + 1));
  }

  public getXComponentId(): number {
    return this.xcomponentId;
  }

  public getAttachedFlutterEngine(): FlutterEngine | null {
    return this.flutterEngine ? this.flutterEngine : null;
  }

  public attachToFlutterEngine(flutterEngine: FlutterEngine) {
    this.flutterEngine = flutterEngine;

    // 1.给XComponent赋值id
    let params: DVModelParameters = new DVModelParameters();
    (params as Record<string, ESObject>)["xComponentId"] = this.xcomponentId.toString()
    let xComponentModel: DVModel =
      new DVModel("xComponent", params, new DVModelEvents(), new DVModelChildren(), BuildXComponentStruct);
    RootDvModeManager.addDvModel(xComponentModel);

    //2.xcomponent绑定engine
    // 将shellHolder传递到C++侧，用于将Xcomponent和FlutterEngine绑定
    this.flutterNapi.xComponentAttachFlutterEngine(this.xcomponentId);
    this.isAttach = true;
  }

  public isAttachedToFlutterEngine(): boolean {
    return this.isAttach;
  }

  public detachFromFlutterEngine() {
    this.flutterNapi.xComponentDetachFlutterEngine(this.xcomponentId);
    this.flutterEngine = null
    this.isAttach = false
  }
}