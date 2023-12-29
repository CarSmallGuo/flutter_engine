import image from '@ohos.multimedia.image';

export class SurfaceTextureWrapper {
  private receiver: image.ImageReceiver;
  private released: boolean = false;
  private attached: boolean = false;

  constructor(receiver: image.ImageReceiver) {
    this.receiver = receiver;
  }

  getImageReceiver(): image.ImageReceiver {
    return this.receiver;
  }
}