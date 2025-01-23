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

/** The mode of the background of a Flutter {@code Activity}, either opaque or transparent. */
enum BackgroundMode {
  /** Indicates a FlutterActivity with an opaque background. This is the default. */
  opaque,
  /** Indicates a FlutterActivity with a transparent background. */
  transparent
}

export default class FlutterAbilityLaunchConfigs {

  static DART_ENTRYPOINT_META_DATA_KEY = "io.flutter.Entrypoint";
  static DART_ENTRYPOINT_URI_META_DATA_KEY = "io.flutter.EntrypointUri";
  static INITIAL_ROUTE_META_DATA_KEY = "io.flutter.InitialRoute";
  static SPLASH_SCREEN_META_DATA_KEY = "io.flutter.embedding.android.SplashScreenDrawable";
  static NORMAL_THEME_META_DATA_KEY = "io.flutter.embedding.android.NormalTheme";
  static HANDLE_DEEPLINKING_META_DATA_KEY = "flutter_deeplinking_enabled";
  // Intent extra arguments.
  static EXTRA_DART_ENTRYPOINT = "dart_entrypoint";
  static EXTRA_INITIAL_ROUTE = "route";
  static EXTRA_BACKGROUND_MODE = "background_mode";
  static EXTRA_CACHED_ENGINE_ID = "cached_engine_id";
  static EXTRA_DART_ENTRYPOINT_ARGS = "dart_entrypoint_args";
  static EXTRA_CACHED_ENGINE_GROUP_ID = "cached_engine_group_id";
  static EXTRA_DESTROY_ENGINE_WITH_ACTIVITY = "destroy_engine_with_activity";
  static EXTRA_ENABLE_STATE_RESTORATION = "enable_state_restoration";

  // Default configuration.
  static DEFAULT_DART_ENTRYPOINT = "main";
  static DEFAULT_INITIAL_ROUTE = "/";
  static DEFAULT_BACKGROUND_MODE = BackgroundMode.opaque
}