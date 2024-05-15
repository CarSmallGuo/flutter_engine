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

import image from '@ohos.multimedia.image';
import { OhosTexture, OnTrimMemoryListener, TextureRegistry } from '../../../view/TextureRegistry';
import FlutterNapi from '../FlutterNapi';

const TAG = "FlutterRenderer"
export class FlutterRenderer implements TextureRegistry {
  private static nextTextureWrapperId: number = 0;
  private flutterNapi: FlutterNapi;

  constructor(flutterNapi: FlutterNapi) {
    this.flutterNapi = flutterNapi;
  }

  registerOhosTexture(): OhosTexture {
    FlutterRenderer.nextTextureWrapperId++;
    let ohosTexture = new OhosTextureWrapper(this.flutterNapi, FlutterRenderer.nextTextureWrapperId);
    return ohosTexture;
  }
}

export class OhosTextureWrapper implements OhosTexture {
  private textureId: number = 0;
  private surfaceId: number = 0;
  private flutterNapi: FlutterNapi;

  constructor(flutterNapi: FlutterNapi, id: number) {
    this.flutterNapi = flutterNapi;
    this.textureId = id;
    this.surfaceId = this.flutterNapi.registerTexture(this.textureId);
  }

  getTextureId(): number {
    return this.textureId;
  }

  getSurfaceId(): number {
    return this.surfaceId;
  }

  updatePixelMap(pixelMap: image.PixelMap): number {
    this.flutterNapi.updatePixelMap(this.textureId, pixelMap);
    return this.textureId;
  }

  markTextureFrameAvailable(textureId: number): void {
    this.flutterNapi.markTextureFrameAvailable(textureId);
  }

  release(): void {
    this.flutterNapi.unregisterTexture(this.textureId);
  }
}