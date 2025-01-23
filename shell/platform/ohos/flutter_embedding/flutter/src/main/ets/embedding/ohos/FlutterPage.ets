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

import Log from '../../util/Log';
import { FlutterView } from '../../view/FlutterView';
import FlutterManager from './FlutterManager';
import { DVModel, DVModelChildren, DynamicView } from '../../view/DynamicView/dynamicView';
import flutter from 'libflutter.so';
const TAG = "FlutterPage";
export const OHOS_FLUTTER_PAGE_UPDATE = "ohos_flutter_page_update";

/**
 * 基础page组件，承载XComponent组件
 */
@Component
export struct FlutterPage {
  @Prop viewId: string = ""
  @Prop xComponentType: XComponentType = XComponentType.SURFACE

  defaultFocusOnTouch = false;
  @Builder doNothingBuilder() {}
  @BuilderParam splashScreenView: () => void = this.doNothingBuilder;
  @Builder defaultPage() {
    Stack() {
      ForEach(this.rootDvModel!!, (child: ESObject) => {
        DynamicView({
          model: child as DVModel,
          params: child.params,
          events: child.events,
          children: child.children,
          customBuilder: child.builder
        })
      }, (child: ESObject) => `${child.id_}`)


      Text('')
        .id('emptyFocusText' + this.viewId)
        .size({ width: 0, height: 0 })
        .opacity(0)
        .focusable(true)

      XComponent({ id: this.viewId, type: this.xComponentType, libraryname: 'flutter' })
        .id(this.viewId)
        .focusable(true)
        .focusOnTouch(this.defaultFocusOnTouch)
        .onLoad((context) => {
          this.flutterView?.onSurfaceCreated()
          Log.d(TAG, "XComponent onLoad ");
          // 当xcomponent窗口部分显示或完全隐藏时触发回调
          this.getUIContext()?.getAttachedFrameNodeById(this.viewId)?.commonEvent.setOnVisibleAreaApproximateChange(
            { ratios: [0.0, 1.0], expectedUpdateInterval: 0 },
            (ratioInc: boolean, ratio: number) => {
              if (ratioInc) {
                Log.i(TAG, "setOnVisibleAreaApproximateChange -> xcomponentId: " + this.viewId +
                           " ratioInc: " + ratioInc + " ratio: " + ratio);
                flutter.nativeGetXComponentId(this.viewId);
                // 保证获取xcomponentid之后再使用无障碍
                this.flutterView?.onAccessibilityIsOpen();
              }
            }
          )
        })
        .onDestroy(() => {
          Log.d(TAG, "XComponent onDestroy ");
          this.flutterView?.onSurfaceDestroyed()
        })
        .renderFit(RenderFit.TOP_LEFT)
        .backgroundColor(Color.Transparent)

      if (this.showSplashScreen) {
        this.splashScreenView();
      }
    }
    .defaultFocus(true)
    .onAreaChange((oldValue: Area, newValue: Area) => {
      if (this.isNeedUpdate || !this.lastArea || oldValue.width != newValue.width
        || oldValue.height != newValue.height) {
        Log.d(TAG, "onAreaChange, old=" + JSON.stringify(oldValue));
        Log.d(TAG, "onAreaChange, new=" + JSON.stringify(newValue));
        this.lastArea = newValue;
        this.flutterView?.onAreaChange(newValue)
        this.isNeedUpdate = false;
      }
    })
    .onKeyPreIme((event: KeyEvent) => {
      return this.flutterView?.onKeyPreIme(event) ?? false;
    })
    .onKeyEvent((event: KeyEvent) => {
      return this.flutterView?.onKeyEvent(event) ?? false;
    })
    .onDragEnter((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragEnterCbs().forEach(dragEnterCb => {
        dragEnterCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragEnter");
    })
    .onDragMove((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragMoveCbs().forEach(dragMoveCb => {
        dragMoveCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragMove");
    })
    .onDragLeave((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragLeaveCbs().forEach(dragLeaveCb => {
        dragLeaveCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragLeave");
    })
    .onDrop((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDropCbs().forEach(dropCb => {
        dropCb.do(event, extraParams);
      });
      Log.d(TAG, "onDrop");
    })
  }
  @Builder mouseWheelPage() {
    Stack() {
      ForEach(this.rootDvModel!!, (child: ESObject) => {
        DynamicView({
          model: child as DVModel,
          params: child.params,
          events: child.events,
          children: child.children,
          customBuilder: child.builder
        })
      }, (child: ESObject) => `${child.id_}`)


      Text('')
        .id('emptyFocusText' + this.viewId)
        .size({ width: 0, height: 0 })
        .opacity(0)
        .focusable(true)

      XComponent({ id: this.viewId, type: this.xComponentType, libraryname: 'flutter' })
        .id(this.viewId)
        .focusable(true)
        .focusOnTouch(this.defaultFocusOnTouch)
        .onLoad((context) => {
          this.flutterView?.onSurfaceCreated()
          Log.d(TAG, "XComponent onLoad ");
          // 当xcomponent窗口部分显示或完全隐藏时触发回调
          this.getUIContext()?.getAttachedFrameNodeById(this.viewId)?.commonEvent.setOnVisibleAreaApproximateChange(
            { ratios: [0.0, 1.0], expectedUpdateInterval: 0 },
            (ratioInc: boolean, ratio: number) => {
              if (ratioInc) {
                Log.i(TAG, "setOnVisibleAreaApproximateChange -> xcomponentId: " + this.viewId +
                            " ratioInc: " + ratioInc + " ratio: " + ratio);
                flutter.nativeGetXComponentId(this.viewId);
                // 保证获取xcomponentid之后再使用无障碍
                this.flutterView?.onAccessibilityIsOpen();
              }
            }
          )
        })
        .onDestroy(() => {
          Log.d(TAG, "XComponent onDestroy ");
          this.flutterView?.onSurfaceDestroyed()
        })
        .renderFit(RenderFit.TOP_LEFT)
        .backgroundColor(Color.Transparent)

      if (this.showSplashScreen) {
        this.splashScreenView();
      }
    }
    .defaultFocus(true)
    .onAreaChange((oldValue: Area, newValue: Area) => {
      if (this.isNeedUpdate || !this.lastArea || oldValue.width != newValue.width
        || oldValue.height != newValue.height) {
        Log.d(TAG, "onAreaChange, old=" + JSON.stringify(oldValue));
        Log.d(TAG, "onAreaChange, new=" + JSON.stringify(newValue));
        this.lastArea = newValue;
        this.flutterView?.onAreaChange(newValue)
        this.isNeedUpdate = false;
      }
    })
    .onKeyPreIme((event: KeyEvent) => {
      return this.flutterView?.onKeyPreIme(event) ?? false;
    })
    .onKeyEvent((event: KeyEvent) => {
      return this.flutterView?.onKeyEvent(event) ?? false;
    })
    .onDragEnter((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragEnterCbs().forEach(dragEnterCb => {
        dragEnterCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragEnter");
    })
    .onDragMove((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragMoveCbs().forEach(dragMoveCb => {
        dragMoveCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragMove");
    })
    .onDragLeave((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDragLeaveCbs().forEach(dragLeaveCb => {
        dragLeaveCb.do(event, extraParams);
      });
      Log.d(TAG, "onDragLeave");
    })
    .onDrop((event: DragEvent, extraParams: string) => {
      FlutterManager.getInstance().getDropCbs().forEach(dropCb => {
        dropCb.do(event, extraParams);
      });
      Log.d(TAG, "onDrop");
    })
    .gesture(
      PanGesture(this.panOption)
        .onActionStart((event: GestureEvent) => {
          this.flutterView?.onMouseWheel("actionStart", event);
        })
        .onActionUpdate((event: GestureEvent) => {
          this.flutterView?.onMouseWheel("actionUpdate", event);
        })
        .onActionEnd((event: GestureEvent) => {
          this.flutterView?.onMouseWheel("actionEnd", event);
        })
    )
  }
  @State showSplashScreen: boolean = true;

  @State checkFullScreen: boolean = true;
  @State checkKeyboard: boolean = true;
  @State checkGesture: boolean = true;
  @State checkMouseWheel: boolean = true;

  @StorageLink('nodeWidth') storageLinkWidth: number = 0;
  @StorageLink('nodeHeight') storageLinkHeight: number = 0;

  @State rootDvModel: DVModelChildren | undefined = undefined

  @State isNeedUpdate: boolean = false;

  private flutterView?: FlutterView | null
  private lastArea?: Area;
  private panOption: PanGestureOptions = new PanGestureOptions({ direction: PanDirection.Up | PanDirection.Down });

  aboutToAppear() {
    this.flutterView = FlutterManager.getInstance().getFlutterView(this.viewId);
    this.flutterView?.addFirstFrameListener(this)

    this.flutterView?.setCheckFullScreen(this.checkFullScreen)
    this.flutterView?.setCheckKeyboard(this.checkKeyboard)
    this.flutterView?.setCheckGesture(this.checkGesture)

    this.rootDvModel = this.flutterView!!.getDVModel().children
    getContext().eventHub.on(OHOS_FLUTTER_PAGE_UPDATE, () => {
      this.isNeedUpdate = true;
    })
  }

  aboutToDisappear() {
    this.flutterView?.removeFirstFrameListener(this);
    getContext()?.eventHub.off(OHOS_FLUTTER_PAGE_UPDATE)
  }

  onFirstFrame() {
    this.showSplashScreen = false;
  }

  build() {
    if (this.checkMouseWheel) {
      this.mouseWheelPage();
    } else {
      this.defaultPage();
    }
  }
}
