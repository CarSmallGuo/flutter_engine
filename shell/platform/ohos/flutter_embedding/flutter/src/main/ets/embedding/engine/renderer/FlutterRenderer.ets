import image from '@ohos.multimedia.image';
import { BusinessError } from '@ohos.base';
import { SurfaceTextureEntry, TextureRegistry } from '../../../view/TextureRegistry';
import { FlutterAbility } from '../../ohos/FlutterAbility';
import FlutterNapi from '../FlutterNapi';
import { SurfaceTextureWrapper } from './SurfaceTextureWrapper';

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

  registerSurfaceTexture(receiver: image.ImageReceiver): SurfaceTextureEntry {
    this.nextTextureId = this.nextTextureId + 1;
    let surfaceTextureRegistryEntry = new SurfaceTextureRegistryEntry(this.nextTextureId, receiver);
    this.registerTexture(surfaceTextureRegistryEntry.id(), surfaceTextureRegistryEntry.textureWrapper());
    return surfaceTextureRegistryEntry;
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
  private registerTexture(id: number, surfaceTextureWrapper: SurfaceTextureWrapper): void {
    let receiver = surfaceTextureWrapper.getImageReceiver();
    receiver.on('imageArrival', () => {
      receiver.readNextImage((err: BusinessError, nextImage: image.Image) => {
        if (err || nextImage === undefined) {
          return;
        }
        this.flutterNapi.initNativeImage(id, nextImage);
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
  private textureId: number;
  private surfaceTextureWrapper: SurfaceTextureWrapper;
  private released: boolean = false;

  constructor(id: number, receiver: image.ImageReceiver) {
    this.textureId = id;
    this.surfaceTextureWrapper = new SurfaceTextureWrapper(receiver);
  }

  getImageReceiver(): image.ImageReceiver {
    return this.surfaceTextureWrapper.getImageReceiver();
  }

  id(): number {
    return this.textureId;
  }

  textureWrapper(): SurfaceTextureWrapper {
    return this.surfaceTextureWrapper;
  }
  release() {
    throw new Error('Method not implemented.');
  }
}