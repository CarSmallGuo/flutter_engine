/*
Copyright (c) 2022 Huawei Device Co., Ltd.

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
import Any from '../../plugin/common/Any';

import { DVModel, DVModelParameters, DVModelEvents, DVModelChildren } from "./dynamicView";
import Log from '../../util/Log';
const TAG = "dynamicViewJson";

export function createDVModelFromJson(json: Object): DVModel {

  /* private use helper functions */
  let createChildrenFrom: (children: Array<Object>) =>  DVModelChildren = (children: Array<Object>): DVModelChildren => {
    let result = new DVModelChildren();
    if (Array.isArray(children)) {
      (children as Array<Object>).forEach(child => {
        const childView = createDVModelFromJson(child);
        if (childView != undefined) {
          result.push(childView);
        }
      });
    }
    return result;
  }

  let setParams: (result: DVModelParameters | DVModelEvents, key: Any, element: Object ) => void = (result: DVModelParameters, key: Any, element: Any): void => {
    let newResult = result as Record<string, Any>;
    newResult[key] = element[key];
  }

  let createAttributesFrom: (attributes: Object) =>  DVModelParameters = (attributes: Object): DVModelParameters => {
    let result = new DVModelParameters();
    if ((typeof attributes == "object") && (!Array.isArray(attributes))) {
      Object.keys(attributes).forEach(k => {setParams(result, k, attributes)});
    }
    return result;
  }

  let createEventsFrom: (events: Object) =>  DVModelEvents = (events: Object): DVModelEvents => {
    let result = new DVModelEvents();
    if ((typeof events == "object") && (!Array.isArray(events))) {
      Object.keys(events).forEach(k => {setParams(result, k, events)});
    }
    return result;
  }

  if (typeof json !== 'object') {
    Log.e(TAG, "createDVModelFromJson: input is not JSON");
    return new DVModel("", "", "", createChildrenFrom([]));
  }

  let jsonObject = json as Record<string, string | Any>;
  return new DVModel(
    jsonObject["compType"],
    createAttributesFrom(jsonObject["attributes"]),
    createEventsFrom(jsonObject["events"]),
    createChildrenFrom(jsonObject["children"]),
    jsonObject["build"]
  );
}
