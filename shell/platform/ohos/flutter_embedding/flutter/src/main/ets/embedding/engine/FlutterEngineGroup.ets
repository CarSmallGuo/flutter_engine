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

import FlutterEngine, { EngineLifecycleListener } from "./FlutterEngine"
import common from '@ohos.app.ability.common'
import FlutterLoader from './loader/FlutterLoader'
import FlutterInjector from '../../FlutterInjector'
import { DartEntrypoint } from './dart/DartExecutor'
import PlatformViewsController from '../../plugin/platform/PlatformViewsController'
import ArrayList from '@ohos.util.ArrayList'
import Log from '../../util/Log';

const TAG = "FlutterEngineGroup"

export default class FlutterEngineGroup {
  private activeEngines: ArrayList<FlutterEngine> = new ArrayList<FlutterEngine>();

  constructor() {

  }

  async checkLoader(context: common.Context, args: Array<string>) {
    let loader: FlutterLoader = FlutterInjector.getInstance().getFlutterLoader();
    if (!loader.initialized) {
      await loader.startInitialization(context);
      loader.ensureInitializationComplete(args);
    }
  }

  async createAndRunEngineByOptions(options: Options) {
    let engine: FlutterEngine | null = null;
    let context: common.Context = options.getContext();
    let dartEntrypoint: DartEntrypoint | null = options.getDartEntrypoint();
    let initialRoute: string = options.getInitialRoute();
    let dartEntrypointArgs: Array<string> = options.getDartEntrypointArgs();
    let platformViewsController: PlatformViewsController | null = options.getPlatformViewsController();
    let waitForRestorationData: boolean = options.getWaitForRestorationData();

    if (dartEntrypoint == null) {
      dartEntrypoint = DartEntrypoint.createDefault();
    }

    if (platformViewsController == null) {
      platformViewsController = new PlatformViewsController();
    }

    Log.i(TAG, "shellHolder, this.activeEngines.length=" + this.activeEngines.length)
    if (this.activeEngines.length == 0) {
      engine = this.createEngine(context, platformViewsController);
      await engine.init(context, null, // String[]. The Dart VM has already started, this arguments will have no effect.
        waitForRestorationData)
      if (initialRoute != null) {
        engine.getNavigationChannel()?.setInitialRoute(initialRoute);
      }
      engine.getDartExecutor().executeDartEntrypoint(dartEntrypoint, dartEntrypointArgs)
    } else {
      engine = await this.activeEngines[0]
        .spawn(
          context,
          dartEntrypoint,
          initialRoute,
          dartEntrypointArgs,
          platformViewsController,
          waitForRestorationData);
    }
    this.activeEngines.add(engine);

    const engineToCleanUpOnDestroy = engine;
    let listener: EngineLifecycleListener = new EngineLifecycleListenerImpl(
      platformViewsController,
      this.activeEngines,
      engineToCleanUpOnDestroy);
    engine?.addEngineLifecycleListener(listener);
    return engine;
  }

  createEngine(context: common.Context, platformViewsController: PlatformViewsController): FlutterEngine {
    return new FlutterEngine(context, null, null, platformViewsController);
  }
}

class EngineLifecycleListenerImpl implements EngineLifecycleListener {
  private platformViewsController: PlatformViewsController;
  private activeEngines: ArrayList<FlutterEngine> = new ArrayList();
  private engine: FlutterEngine | null;

  constructor(
    platformViewsController: PlatformViewsController,
    activeEngines: ArrayList<FlutterEngine>,
    engine: FlutterEngine | null) {
    this.platformViewsController = platformViewsController;
    this.activeEngines = activeEngines;
    this.engine = engine;
  }
  onPreEngineRestart(): void {
    this.platformViewsController.onPreEngineRestart();
  }
  onEngineWillDestroy(): void {
    this.activeEngines.remove(this.engine);
  }
}

export class Options {
  private context: common.Context;
  private dartEntrypoint: DartEntrypoint | null = null;
  private initialRoute: string = '';
  private dartEntrypointArgs: Array<string> = [];
  private platformViewsController: PlatformViewsController | null = null;
  private waitForRestorationData: boolean = false;

  constructor(context: common.Context) {
    this.context = context;
  }

  getContext(): common.Context {
    return this.context;
  }

  getDartEntrypoint(): DartEntrypoint | null {
    return this.dartEntrypoint;
  }

  getInitialRoute(): string {
    return this.initialRoute;
  }

  getDartEntrypointArgs(): Array<string> {
    return this.dartEntrypointArgs;
  }

  getWaitForRestorationData(): boolean {
    return this.waitForRestorationData;
  }

  getPlatformViewsController(): PlatformViewsController | null {
    return this.platformViewsController;
  }

  setDartEntrypoint(dartEntrypoint: DartEntrypoint): Options {
    this.dartEntrypoint = dartEntrypoint;
    return this;
  }

  setInitialRoute(initialRoute: string): Options {
    this.initialRoute = initialRoute;
    return this;
  }

  setDartEntrypointArgs(dartEntrypointArgs: Array<string>): Options {
    this.dartEntrypointArgs = dartEntrypointArgs;
    return this;
  }

  setWaitForRestorationData(waitForRestorationData: boolean): Options {
    this.waitForRestorationData = waitForRestorationData;
    return this;
  }

  setPlatformViewsController(platformViewsController: PlatformViewsController): Options {
    this.platformViewsController = platformViewsController;
    return this;
  }
}