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

/**
 * {@link io.flutter.embedding.engine.plugins.FlutterPlugin} that is interested in {@link
 * ohos.app.ability.UIAbility} lifecycle events related to a {@link
 * io.flutter.embedding.engine.FlutterEngine} running within the given {@link ohos.app.ability.UIAbility}.
 */
import { AbilityPluginBinding } from './AbilityPluginBinding';

export default interface AbilityAware {
  /**
   * This {@code AbilityAware} {@link io.flutter.embedding.engine.plugins.FlutterPlugin} is now
   * associated with an {@link ohos.app.ability.UIAbility}.
   *
   * <p>This method can be invoked in 1 of 2 situations:
   *
   * <ul>
   *   <li>This {@code AbilityAware} {@link io.flutter.embedding.engine.plugins.FlutterPlugin} was
   *       just added to a {@link io.flutter.embedding.engine.FlutterEngine} that was already
   *       connected to a running {@link ohos.app.ability.UIAbility}.
   *   <li>This {@code AbilityAware} {@link io.flutter.embedding.engine.plugins.FlutterPlugin} was
   *       already added to a {@link io.flutter.embedding.engine.FlutterEngine} and that {@link
   *       io.flutter.embedding.engine.FlutterEngine} was just connected to an {@link
   *       ohos.app.ability.UIAbility}.
   * </ul>
   *
   * The given {@link AbilityPluginBinding} contains {@link ohos.app.ability.UIAbility}-related
   * references that an {@code AbilityAware} {@link
   * io.flutter.embedding.engine.plugins.FlutterPlugin} may require, such as a reference to the
   * actual {@link ohos.app.ability.UIAbility} in question. The {@link AbilityPluginBinding} may be
   * referenced until either {@link #onDetachedFromAbilityForConfigChanges()} or {@link
   * #onDetachedFromAbility()} is invoked. At the conclusion of either of those methods, the
   * binding is no longer valid. Clear any references to the binding or its resources, and do not
   * invoke any further methods on the binding or its resources.
   */
  onAttachedToAbility(binding: AbilityPluginBinding): void ;

  /**
   * This plugin has been detached from an {@link ohos.app.ability.UIAbility}.
   *
   * <p>Detachment can occur for a number of reasons.
   *
   * <ul>
   *   <li>The app is no longer visible and the {@link ohos.app.ability.UIAbility} instance has been
   *       destroyed.
   *   <li>The {@link io.flutter.embedding.engine.FlutterEngine} that this plugin is connected to
   *       has been detached from its {@link io.flutter.embedding.android.FlutterView}.
   *   <li>This {@code AbilityAware} plugin has been removed from its {@link
   *       io.flutter.embedding.engine.FlutterEngine}.
   * </ul>
   *
   * By the end of this method, the {@link ohos.app.ability.UIAbility} that was made available in {@link
   * #onAttachedToAbility(AbilityPluginBinding)} is no longer valid. Any references to the
   * associated {@link ohos.app.ability.UIAbility} or {@link AbilityPluginBinding} should be cleared.
   *
   * <p>Any {@code Lifecycle} listeners that were registered in {@link
   * #onAttachedToAbility(AbilityPluginBinding)} or {@link
   * #onReattachedToAbilityForConfigChanges(AbilityPluginBinding)} should be deregistered here to
   * avoid a possible memory leak and other side effects.
   */
  onDetachedFromAbility(): void;
}
