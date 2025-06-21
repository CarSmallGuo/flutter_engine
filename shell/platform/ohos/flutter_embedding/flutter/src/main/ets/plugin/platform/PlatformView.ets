/*
* Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd. All rights reserved.
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE_KHZG file.
*
* Based on PlatformView.java originally written by
* Copyright (C) 2013 The Flutter Authors.
*
*/

import { DVModel, DynamicView } from '../../view/DynamicView/dynamicView'

export declare class Params {
  direction: Direction
  platformView: PlatformView
}

export declare class PlatformViewVisibleAreaEventOptions {
  enable: boolean // 开启可变区域监听
  ratios: Array<number> // 阈值数组
  expectedUpdateInterval: number // 预期更新间隔，单位为ms
  onInactiveThreshold: number // 纹理持续生产的暂停操作的可见区域阈值
  onActiveThreshold: number // 纹理持续生产的激活操作的可见区域阈值
}

// unit: ms
const defaultExpectedUpdateInterval: number = 1000;

/** A handle to an DynamicView to be embedded in the Flutter hierarchy. */
export default abstract class PlatformView {  

  getType(): string {
    return 'default';
  }

  /** Returns the DynamicView to be embedded in the Flutter hierarchy. */
  abstract getView(): WrappedBuilder<[Params]>;

  /**
   * Called by the {@link io.flutter.embedding.engine.FlutterEngine} that owns this {@code
   * PlatformView} when the DynamicView responsible for rendering a Flutter UI is
   * associated with the {@link io.flutter.embedding.engine.FlutterEngine}.
   *
   * <p>This means that our associated {@link io.flutter.embedding.engine.FlutterEngine} can now
   * render a UI and interact with the user.
   *
   * <p>Some platform views may have unusual dependencies on the {@link View} that renders Flutter
   * UIs, such as unique keyboard interactions. That {@link View} is provided here for those
   * purposes. Use of this {@link View} should be avoided if it is not absolutely necessary, because
   * depending on this {@link View} will tend to make platform view code more brittle to future
   * changes.
   */
  onFlutterViewAttached(dvModel: DVModel): void {
  }

  /**
   * Called by the {@link io.flutter.embedding.engine.FlutterEngine} that owns this {@code
   * PlatformView} when the DynamicView responsible for rendering a Flutter UI is detached
   * and disassociated from the {@link io.flutter.embedding.engine.FlutterEngine}.
   *
   * <p>This means that our associated {@link io.flutter.embedding.engine.FlutterEngine} no longer
   * has a rendering surface, or a user interaction surface of any kind.
   *
   * <p>This platform view must release any references related to the DynamicView that was
   * provided in {@link #onFlutterViewAttached(View)}.
   */
  onFlutterViewDetached(): void {
  }

  /**
   * Dispose this platform view.
   *
   * <p>The {@link PlatformView} object is unusable after this method is called.
   *
   * <p>Plugins implementing {@link PlatformView} must clear all references to the View object and
   * the PlatformView after this method is called. Failing to do so will result in a memory leak.
   *
   * <p>References related to the DynamicView attached in {@link
   * #onFlutterViewAttached(View)} must be released in {@code dispose()} to avoid memory leaks.
   */
  abstract dispose(): void;

  /**
   * Callback fired when the platform's input connection is locked, or should be used.
   *
   * <p>This hook only exists for rare cases where the plugin relies on the state of the input
   * connection. This probably doesn't need to be implemented.
   */
  onInputConnectionLocked(): void {
  }

  /**
   * Callback fired when the platform input connection has been unlocked.
   *
   * <p>This hook only exists for rare cases where the plugin relies on the state of the input
   * connection. This probably doesn't need to be implemented.
   */
  onInputConnectionUnlocked(): void {
  }

  // 获取关于外接纹理可见区域变化相关的参数
  getPlatformViewVisibleAreaEventOptions(): PlatformViewVisibleAreaEventOptions {
    return {
      enable: false,
      ratios: [0.0, 1.0],
      expectedUpdateInterval: defaultExpectedUpdateInterval,
      onInactiveThreshold : 0.0,
      onActiveThreshold : 1.0
    } as PlatformViewVisibleAreaEventOptions;
  }

  // 使外接纹理暂停持续生产的操作，包括动画、视频
  onInactive(): void {
  }

  // 使外接纹理激活持续生产的操作，包括动画、视频
  onActive(): void {
  }
}