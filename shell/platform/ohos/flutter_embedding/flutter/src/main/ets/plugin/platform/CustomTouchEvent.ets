/*
Copyright (c) 2024 Hunan OpenValley Digital Industry Development Co., Ltd.

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

export class CustomTouchEvent implements TouchEvent {
  type: TouchType = 0;
  touches: CustomTouchObject[];
  changedTouches: CustomTouchObject[];
  stopPropagation: () => void = () => {};

  timestamp: number;
  source: SourceType;
  pressure: number;
  tiltX: number;
  tiltY: number;
  sourceTool: SourceTool;

  constructor(type: TouchType, touches: CustomTouchObject[], changedTouches: CustomTouchObject[], timestamp: number, source: SourceType, pressure: number, tiltX: number, tiltY: number, sourceTool: SourceTool) {
    this.type = type;
    this.touches = touches;
    this.changedTouches = changedTouches;
    this.timestamp = timestamp;
    this.source = source;
    this.pressure = pressure;
    this.tiltX = tiltX;
    this.tiltY = tiltY;
    this.sourceTool = sourceTool;
  }

  preventDefault: () => void = () => {};

  getModifierKeyState(keys: string[]): boolean {
    throw new Error('Method not implemented.');
  }

  target: EventTarget = new CustomEventTarget(new CustomArea(0, 0, {x: 0, y: 0}, {x: 0, y: 0}));

  getHistoricalPoints(): HistoricalPoint[] {
    throw new Error('Method not implemented.');
  }
}

class CustomEventTarget implements EventTarget {
  area: Area = new CustomArea(0, 0, {x: 0, y: 0}, {x: 0, y: 0});

  constructor(area: Area) {
    this.area = area;
  }
}

class CustomArea implements Area {
  width: Length = 0;
  height: Length = 0;
  position: Position = {x: 0, y: 0};
  globalPosition: Position = {x: 0, y: 0};

  constructor(width: Length, height: Length, position: Position, globalPosition: Position ) {
    this.width = width;
    this.height = height;
    this.position = position;
    this.globalPosition = globalPosition;
  }
}


export class CustomTouchObject implements TouchObject {
  type: TouchType;
  id: number;
  displayX: number;
  displayY: number;
  windowX: number;
  windowY: number;
  screenX: number;
  screenY: number;
  x: number;
  y: number;

  constructor(type: TouchType, id: number, displayX: number, displayY: number, windowX: number, windowY: number, screenX: number, screenY: number, x: number, y: number) {
    this.type = type;
    this.id = id;
    this.displayX = displayX;
    this.displayY = displayY;
    this.windowX = windowX;
    this.windowY = windowY;
    this.screenX = screenX;
    this.screenY = screenY;
    this.x = x;
    this.y = y;
  }
}