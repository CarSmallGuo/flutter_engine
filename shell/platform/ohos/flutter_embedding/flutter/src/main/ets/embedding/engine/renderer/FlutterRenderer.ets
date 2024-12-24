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

import image from '@ohos.multimedia.image';
import { BusinessError } from '@ohos.base';
import { SurfaceTextureEntry, TextureRegistry } from '../../../view/TextureRegistry';
import { FlutterAbility } from '../../ohos/FlutterAbility';
import FlutterNapi from '../FlutterNapi';
import { SurfaceTextureWrapper } from './SurfaceTextureWrapper';
import Log from '../../../util/Log';

const TAG = "FlutterRenderer"
export class FlutterRenderer implements TextureRegistry {
  private nextTextureId: number = 0;
  private flutterNapi: FlutterNapi;

  constructor(flutterNapi: FlutterNapi) {
    this.flutterNapi = flutterNapi;
  }
  createSurfaceTexture(): SurfaceTextureEntry {
    let receiver: image.ImageReceiver = this.getImageReceiver();
    return this.registerSurfaceTexture(receiver);
  }

  getTextureId(): number {
    this.nextTextureId = this.nextTextureId + 1;
    Log.i(TAG, "getTextureId: ", this.nextTextureId )
    return this.nextTextureId;
  }

  registerTexture(textureId: number): SurfaceTextureEntry {
    let surfaceTextureRegistryEntry = new SurfaceTextureRegistryEntry(this.nextTextureId);
    let surfaceId = this.flutterNapi.registerTexture(textureId);
    Log.i(TAG, "registerTexture, surfaceId=" + surfaceId);
    surfaceTextureRegistryEntry.setSurfaceId(surfaceId);
    return surfaceTextureRegistryEntry;
  }

  registerSurfaceTexture(receiver: image.ImageReceiver): SurfaceTextureEntry {
    this.nextTextureId = this.nextTextureId + 1;
    let surfaceTextureRegistryEntry = new SurfaceTextureRegistryEntry(this.nextTextureId);
    surfaceTextureRegistryEntry.setImageReceiver(receiver);
    this.registerImage(surfaceTextureRegistryEntry.getTextureId(), surfaceTextureRegistryEntry.textureWrapper());
    return surfaceTextureRegistryEntry;
  }

  registerPixelMap(pixelMap: PixelMap): number {
    this.nextTextureId = this.nextTextureId + 1;
    this.flutterNapi.registerPixelMap(this.nextTextureId, pixelMap);
    return this.nextTextureId;
  }

  setTextureBackGroundPixelMap(textureId: number, pixelMap: PixelMap): void {
    this.flutterNapi.setTextureBackGroundPixelMap(textureId, pixelMap);
  }

  setTextureBufferSize(textureId: number, width: number, height: number): void {
    this.flutterNapi.setTextureBufferSize(textureId, width, height);
  }

  unregisterTexture(textureId: number): void {
    this.flutterNapi.unregisterTexture(textureId);
  }
  onTrimMemory(level: number) {
    throw new Error('Method not implemented.');
  }

  private getImageReceiver(): image.ImageReceiver {
    let receiver: image.ImageReceiver = image.createImageReceiver(640, 480, 4, 8);
    if (receiver !== undefined) {
      Log.i(TAG, '[camera test] ImageReceiver is ok');
    } else {
      Log.i(TAG, '[camera test] ImageReceiver is not ok');
    }
    return receiver;
  }
  private registerImage(textureId: number, surfaceTextureWrapper: SurfaceTextureWrapper): void {
    let receiver = surfaceTextureWrapper.getImageReceiver();
    receiver.on('imageArrival', () => {
      receiver.readNextImage((err: BusinessError, nextImage: image.Image) => {
        if (err || nextImage === undefined) {
          return;
        }
        this.flutterNapi.initNativeImage(textureId, nextImage);
        Log.i(TAG, "[camera test] format: " + nextImage.format);
        nextImage.release((err : BusinessError) =>{
          if (err != undefined) {
            Log.i(TAG, 'Failed to release the image source instance.');
          } else {
            Log.i(TAG, 'Succeeded in releasing the image source instance.');
          }
        });
      })
    })
  }

}

export class SurfaceTextureRegistryEntry implements SurfaceTextureEntry {
  private textureId: number = 0;
  private surfaceId: number = 0;
  private surfaceTextureWrapper: SurfaceTextureWrapper | null = null;
  private released: boolean = false;

  constructor(id: number) {
    this.textureId = id;
  }

  getImageReceiver(): image.ImageReceiver {
    return this.surfaceTextureWrapper!.getImageReceiver();
  }

  setImageReceiver(receiver: image.ImageReceiver): void {
    this.surfaceTextureWrapper = new SurfaceTextureWrapper(receiver);
  }

  getTextureId(): number {
    return this.textureId;
  }

  getSurfaceId(): number {
    return this.surfaceId;
  }

  setSurfaceId(surfaceId: number): void {
    this.surfaceId = surfaceId;
  }

  textureWrapper(): SurfaceTextureWrapper {
    return this.surfaceTextureWrapper!;
  }
  release() {
    throw new Error('Method not implemented.');
  }
}