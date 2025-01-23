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

import { FlutterPlugin } from './FlutterPlugin';

export default interface PluginRegistry {
  /**
   * Attaches the given {@code plugin} to the {@link io.flutter.embedding.engine.FlutterEngine}
   * associated with this {@code PluginRegistry}.
   */
  add(plugin: FlutterPlugin): void;

  /**
   * Attaches the given {@code plugins} to the {@link io.flutter.embedding.engine.FlutterEngine}
   * associated with this {@code PluginRegistry}.
   */
  addList(plugins: Set<FlutterPlugin>): void;

  /**
   * Returns true if a plugin of the given type is currently attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} associated with this {@code PluginRegistry}.
   */
  //Class<? extends FlutterPlugin>
  has(pluginClassName: string): boolean;

  /**
   * Returns the instance of a plugin that is currently attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} associated with this {@code PluginRegistry}, which
   * matches the given {@code pluginClass}.
   *
   * <p>If no matching plugin is found, {@code null} is returned.
   */
  //Class<? extends FlutterPlugin>
  get(pluginClassName: string): FlutterPlugin;

  /**
   * Detaches the plugin of the given type from the {@link
   * io.flutter.embedding.engine.FlutterEngine} associated with this {@code PluginRegistry}.
   *
   * <p>If no such plugin exists, this method does nothing.
   */
  //Class<? extends FlutterPlugin>
  remove(pluginClassName: string): void;

  /**
   * Detaches the plugins of the given types from the {@link
   * io.flutter.embedding.engine.FlutterEngine} associated with this {@code PluginRegistry}.
   *
   * <p>If no such plugins exist, this method does nothing.
   */
  //Class<? extends FlutterPlugin>
  removeList(pluginClassNames: Set<string>): void;

  /**
   * Detaches all plugins that are currently attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} associated with this {@code PluginRegistry}.
   *
   * <p>If no plugins are currently attached, this method does nothing.
   */
  removeAll(): void;
}