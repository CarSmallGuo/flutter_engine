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

import matrix4 from '@ohos.matrix4'
import List from '@ohos.util.List';

export enum FlutterMutatorType {
  CLIP_RECT,
  CLIP_PATH,
  TRANSFORM,
  OPACITY
}

class Rect {
  width: number;
  height: number;
  radius: string | number | Array<string | number>;

  constructor(width:number, height:number, radius?:string | number | Array<string | number>) {
    this.width = width;
    this.height = height;
    this.radius = radius ?? 0;
  }
}

class Path {
  width: number | string;
  height: number | string;
  commands: string;

  constructor(width:number | string, height:number | string, commands?:string) {
    this.width = width;
    this.height = height;
    this.commands = commands ?? '';
  }
}

export class FlutterMutator {
  private matrix: matrix4.Matrix4Transit | null = null;
  private rect: Rect = new Rect(0, 0);
  private path: Path = new Path(0, 0);

  constructor(args: matrix4.Matrix4Transit | Rect | Path) {
    if (args instanceof Rect) {
      this.rect = args;
    } else if (args instanceof Path) {
      this.path = args;
    } else {
      this.matrix = args;
    }
  }

  public getMatrix() : matrix4.Matrix4Transit | null {
    return this.matrix;
  }

  public getRect() {
    return this.rect;
  }

  public getPath() {
    return this.path;
  }
}

export class FlutterMutatorsStack {
  private mutators: List<FlutterMutator>;
  private finalClippingPaths: List<Path>;
  private finalClippingRects: List<Rect>;

  private finalMatrix: matrix4.Matrix4Transit;

  constructor() {
    this.mutators = new List();
    this.finalClippingPaths = new List();
    this.finalClippingRects = new List();
    this.finalMatrix = matrix4.identity();
  }

  public pushTransform(values: Array<number>): void {
    if (values.length != 16) {
      return;
    }
    let index = 0;
    let matrix = matrix4.init(
      [values[index++], values[index++], values[index++], values[index++],
      values[index++], values[index++], values[index++], values[index++],
      values[index++], values[index++], values[index++], values[index++],
      values[index++], values[index++], values[index++], values[index++]]);
    let mutator = new FlutterMutator(matrix);
    this.mutators.add(mutator);
    this.finalMatrix.combine(matrix);
  }

  public pushClipRect(width:number, height:number, radius?:number) {
    let rect = new Rect(width, height, radius);
    let mutator = new FlutterMutator(rect);
    this.mutators.add(mutator);
    this.finalClippingRects.add(rect);
  }

  public pushClipPath(width:number, height:number, command?:string) {
    let path = new Path(width, height, command);
    let mutator = new FlutterMutator(path);
    this.mutators.add(mutator);
    this.finalClippingPaths.add(path);
  }

  public getMutators() {
    return this.mutators;
  }

  public getFinalClippingPaths() {
    return this.finalClippingPaths;
  }

  public getFinalClippingRects() {
    return this.finalClippingRects;
  }

  public getFinalMatrix() {
    return this.finalMatrix;
  }
}