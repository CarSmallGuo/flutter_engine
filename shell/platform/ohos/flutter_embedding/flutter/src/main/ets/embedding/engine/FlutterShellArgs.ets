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

import Want from '@ohos.app.ability.Want';

/**
 * 封装flutter shell的参数
 */
export default class FlutterShellArgs {
  static ARG_KEY_TRACE_STARTUP = "trace-startup";
  static ARG_TRACE_STARTUP = "--trace-startup";
  static ARG_KEY_START_PAUSED = "start-paused";
  static ARG_START_PAUSED = "--start-paused";
  static ARG_KEY_DISABLE_SERVICE_AUTH_CODES = "disable-service-auth-codes";
  static ARG_DISABLE_SERVICE_AUTH_CODES = "--disable-service-auth-codes";
  static ARG_KEY_ENDLESS_TRACE_BUFFER = "endless-trace-buffer";
  static ARG_ENDLESS_TRACE_BUFFER = "--endless-trace-buffer";
  static ARG_KEY_USE_TEST_FONTS = "use-test-fonts";
  static ARG_USE_TEST_FONTS = "--use-test-fonts";
  static ARG_KEY_ENABLE_DART_PROFILING = "enable-dart-profiling";
  static ARG_ENABLE_DART_PROFILING = "--enable-dart-profiling";
  static ARG_KEY_ENABLE_SOFTWARE_RENDERING = "enable-software-rendering";
  static ARG_ENABLE_SOFTWARE_RENDERING = "--enable-software-rendering";
  static ARG_KEY_SKIA_DETERMINISTIC_RENDERING = "skia-deterministic-rendering";
  static ARG_SKIA_DETERMINISTIC_RENDERING = "--skia-deterministic-rendering";
  static ARG_KEY_TRACE_SKIA = "trace-skia";
  static ARG_TRACE_SKIA = "--trace-skia";
  static ARG_KEY_TRACE_SKIA_ALLOWLIST = "trace-skia-allowlist";
  static ARG_TRACE_SKIA_ALLOWLIST = "--trace-skia-allowlist=";
  static ARG_KEY_TRACE_SYSTRACE = "trace-systrace";
  static ARG_TRACE_SYSTRACE = "--trace-systrace";
  static ARG_KEY_ENABLE_IMPELLER = "enable-impeller";
  static ARG_ENABLE_IMPELLER = "--enable-impeller";
  static ARG_KEY_DUMP_SHADER_SKP_ON_SHADER_COMPILATION =
    "dump-skp-on-shader-compilation";
  static ARG_DUMP_SHADER_SKP_ON_SHADER_COMPILATION =
    "--dump-skp-on-shader-compilation";
  static ARG_KEY_CACHE_SKSL = "cache-sksl";
  static ARG_CACHE_SKSL = "--cache-sksl";
  static ARG_KEY_PURGE_PERSISTENT_CACHE = "purge-persistent-cache";
  static ARG_PURGE_PERSISTENT_CACHE = "--purge-persistent-cache";
  static ARG_KEY_VERBOSE_LOGGING = "verbose-logging";
  static ARG_VERBOSE_LOGGING = "--verbose-logging";
  static ARG_KEY_OBSERVATORY_PORT = "observatory-port";
  static ARG_OBSERVATORY_PORT = "--observatory-port=";
  static ARG_KEY_DART_FLAGS = "dart-flags";
  static ARG_DART_FLAGS = "--dart-flags=";
  static ARG_KEY_MSAA_SAMPLES = "msaa-samples";
  static ARG_MSAA_SAMPLES = "--msaa-samples=";

  /**
   * 从意图中解析参数，创建shellArgs
   * @returns
   */
  static fromWant(want: Want): FlutterShellArgs {
    let flutterShellArgs: FlutterShellArgs = new FlutterShellArgs();
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_TRACE_STARTUP, FlutterShellArgs.ARG_TRACE_STARTUP, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_START_PAUSED, FlutterShellArgs.ARG_START_PAUSED, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_DISABLE_SERVICE_AUTH_CODES, FlutterShellArgs.ARG_DISABLE_SERVICE_AUTH_CODES, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_ENDLESS_TRACE_BUFFER, FlutterShellArgs.ARG_ENDLESS_TRACE_BUFFER, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_USE_TEST_FONTS, FlutterShellArgs.ARG_USE_TEST_FONTS, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_ENABLE_DART_PROFILING, FlutterShellArgs.ARG_ENABLE_DART_PROFILING, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_ENABLE_SOFTWARE_RENDERING, FlutterShellArgs.ARG_ENABLE_SOFTWARE_RENDERING, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_SKIA_DETERMINISTIC_RENDERING, FlutterShellArgs.ARG_SKIA_DETERMINISTIC_RENDERING, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_TRACE_SKIA, FlutterShellArgs.ARG_TRACE_SKIA, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_TRACE_SYSTRACE, FlutterShellArgs.ARG_TRACE_SYSTRACE, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_ENABLE_IMPELLER, FlutterShellArgs.ARG_ENABLE_IMPELLER, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_DUMP_SHADER_SKP_ON_SHADER_COMPILATION, FlutterShellArgs.ARG_DUMP_SHADER_SKP_ON_SHADER_COMPILATION, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_CACHE_SKSL, FlutterShellArgs.ARG_CACHE_SKSL, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_PURGE_PERSISTENT_CACHE, FlutterShellArgs.ARG_PURGE_PERSISTENT_CACHE, want, flutterShellArgs);
    FlutterShellArgs.checkArg(FlutterShellArgs.ARG_KEY_VERBOSE_LOGGING, FlutterShellArgs.ARG_VERBOSE_LOGGING, want, flutterShellArgs);

    let skia_allow_list: Object = want.parameters![FlutterShellArgs.ARG_KEY_TRACE_SKIA_ALLOWLIST];
    if (skia_allow_list != undefined) {
      flutterShellArgs.add(FlutterShellArgs.ARG_TRACE_SKIA_ALLOWLIST + (skia_allow_list as string));
    }

    let observatory_port: Object = want.parameters![FlutterShellArgs.ARG_KEY_OBSERVATORY_PORT];
    if (observatory_port != undefined && (observatory_port as number > 0)) {
      flutterShellArgs.add(FlutterShellArgs.ARG_OBSERVATORY_PORT + (observatory_port as number));
    }

    let msaa: Object = want.parameters![FlutterShellArgs.ARG_KEY_MSAA_SAMPLES];
    if (msaa != undefined && (msaa as number > 1)) {
      flutterShellArgs.add(FlutterShellArgs.ARG_MSAA_SAMPLES + (msaa as number));
    }

    let dart_flags: Object = want.parameters![FlutterShellArgs.ARG_KEY_DART_FLAGS];
    if (dart_flags != undefined) {
      flutterShellArgs.add(FlutterShellArgs.ARG_DART_FLAGS + (msaa as string));
    }
    return flutterShellArgs;
  }

  static checkArg(argKey: string, argFlag: string, want: Want, flutterShellArgs: FlutterShellArgs) {
    if (want.parameters == undefined) {
      return;
    }
    let value: Object = want.parameters![argKey];
    if (value != undefined && value as Boolean) {
      flutterShellArgs.add(argFlag);
    }
  }

  //参数
  args: Set<string> = new Set();

  add(arg: string) {
    this.args.add(arg);
  }

  remove(arg: string) {
    this.args.delete(arg);
  }

  toArray(): Array<string> {
    return Array.from(this.args);
  }
}