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

export interface TextureRegistry {
  /*
   * create a OhosTextureWrapper, OhosTextureWrapper is mapping to OHOSExternalTextureGL in flutter engine
   * if OhosTextureWrapper end of used, call release() to release engine resource
   */
  registerOhosTexture(): OhosTexture;
}

export interface OhosTexture {
  /*
   * get TextureWrapper id ,not real Texture id
   */
  getTextureId(): number;

  /*
   * get OHOS NativeImage Surface id which create in flutter engine, same as NDK function "OH_NativeImage_GetSurfaceId"
   * this Surface id could be used to consume video frame date to external texture, e.g. camera preview output
   * if camera preview frame arrived, it will callback to NativeImage in engine, then update frame to real texture
   */
  getSurfaceId(): number;

  /*
   * update a new PixelMap to engine external texture
   */
  updatePixelMap(pixelMap: PixelMap): number;

  /*
   * mark a texture frame available after updatePixelMap
   */
  markTextureFrameAvailable(textureId: number): void;
  /*
   * release engine texture resource, e.g. OHOSExternalTextureGL
   */
  release(): void;
}

export interface OnFrameConsumedListener {
  onFrameConsumed(): void;
}

export interface OnTrimMemoryListener {
  onTrimMemory(level: number) : void;
}