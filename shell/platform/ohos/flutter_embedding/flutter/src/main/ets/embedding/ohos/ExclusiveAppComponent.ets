// Copyright (c) 2023 Hunan OpenValley Digital Industry Development Co., Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

export default interface ExclusiveAppComponent<T> {
  /**
   * Called when another App Component is about to become attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} this App Component is currently attached to.
   *
   * <p>This App Component's connections to the {@link io.flutter.embedding.engine.FlutterEngine}
   * are still valid at the moment of this call.
   */
  detachFromFlutterEngine(): void;

  /**
   * Retrieve the App Component behind this exclusive App Component.
   *
   * @return The app component.
   */
  getAppComponent(): T;
}