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
    let textureId = this.nextTextureId + 1;
    this.flutterNapi.registerPixelMap(textureId, pixelMap);
    return textureId;
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
      console.info('[camera test] ImageReceiver is ok');
    } else {
      console.info('[camera test] ImageReceiver is not ok');
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
        console.log("[camera test] format: " + nextImage.format);
        nextImage.release((err : BusinessError) =>{
          if (err != undefined) {
            console.log('Failed to release the image source instance.');
          } else {
            console.log('Succeeded in releasing the image source instance.');
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