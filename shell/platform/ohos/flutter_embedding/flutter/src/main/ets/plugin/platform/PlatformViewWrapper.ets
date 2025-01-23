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

import OhosTouchProcessor from '../../embedding/ohos/OhosTouchProcessor';
import { DVModel, DVModelParameters } from '../../view/DynamicView/dynamicView';
import { createDVModelFromJson } from '../../view/DynamicView/dynamicViewJson';
import { RootDvModeManager } from './RootDvModelManager';
import matrix4 from '@ohos.matrix4'
import Log from '../../util/Log';
import Any from '../common/Any';

const TAG: string = "PlatformViewWrapper";
export class PlatformViewWrapper {
  private prevLeft: number = 0;
  private prevTop: number = 0;
  private left: number = 0;
  private top: number = 0;
  private bufferWidth: number = 0;
  private bufferHeight: number = 0;
  private touchProcessor: OhosTouchProcessor | null = null;
  private model: DVModel | undefined;

  public setTouchProcessor(newTouchProcessor: OhosTouchProcessor): void {
    this.touchProcessor = newTouchProcessor;
  }

  constructor() {
  }

  public getDvModel(): DVModel {
    return this.model!;
  }

  setParams: (params: DVModelParameters, key: string, element: Any ) => void = (params: DVModelParameters, key: string, element: Any): void => {
    let params2 = params as Record<string, Any>;
    params2[key] =element;
  }

  getParams: (params: DVModelParameters, element: string) => string | Any = (params: DVModelParameters, element: string): string | Any => {
    let params2 = params as Record<string, Any>;
    return params2[element];
  }

  public setLayoutParams(parameters : DVModelParameters): void {
    if (!this.model) {
      return;
    }
    if (this.model.params == null) {
      this.model.params = new DVModelParameters();
    }
    this.setParams(this.model.params, "marginLeft", this.getParams(parameters, "marginLeft"));
    this.setParams(this.model.params, "marginTop", this.getParams(parameters, "marginTop"));
    this.left =  this.getParams(parameters, "marginLeft");
    this.top = this.getParams(parameters, "marginTop");

    this.setParams(this.model.params, "width", this.getParams(parameters, "width"));
    this.setParams(this.model.params, "height", this.getParams(parameters, "height"));

    // this.model.params.marginLeft = parameters.marginLeft;
    // this.model.params.marginTop = parameters.marginTop;
    // this.left = parameters.marginLeft;
    // this.top = parameters.marginTop;;

    // this.model.params.width = parameters.width;
    // this.model.params.height = parameters.height;
  }

  public addDvModel(model: DVModel): void {
    this.model = model
  }
}

class DVModelParam {
  compType: string
  children: []

  constructor(compType: string, children: []) {
    this.compType = compType;
    this.children = children;
  }
};