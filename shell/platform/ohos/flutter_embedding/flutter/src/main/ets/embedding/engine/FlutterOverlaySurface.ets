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
* Based on FlutterOverlaySurface.java originally written by
* Copyright (C) 2020  cg021 <40409839+cg021@users.noreply.github.com>
*
*/

export class FlutterOverlaySurface {

  private id: number;

  constructor(id: number) {
    this.id = id
  }

  getId(): number {
    return this.id;
  }
}