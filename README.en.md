Flutter Engine
==============

Original warehouse source: https://github.com/flutter/engine

## Warehouse description:
This warehouse is based on the extension of Flutter's official engine warehouse and can build a Flutter engine program that supports running on OpenHarmony devices.

## Build instructions:

* Build environment:
1. Supports building in Linux and Mac, mainly building gen_snapshot for Window environment;

2. Please ensure that the current build environment can access the allowed_hosts field configured in the DEPS file.

* Building steps:
1. Build a basic environment: please refer to the [official](https://github.com/flutter/flutter/wiki/Setting-up-the-Engine-development-environment) website;

   Basic libraries that need to be installed:

   ```
    sudo apt install python3
    sudo apt install pkg-config
    sudo apt install ninja-build
   ```

   Configure node: Download `node` and unzip it, and configure it into environment variables:

   ```
    # nodejs
    export NODE_HOME=/home/<user>/env/node-v14.19.1-linux-x64
    export PATH=$NODE_HOME/bin:$PATH
   ```

   for Window environment: 
   Refer to [official](https://github.com/flutter/flutter/wiki/Compiling-the-engine#compiling-for-windows)
   The "Compiling for Windows" section sets up the Windows build environment

2. Configuration file: Create an empty folder engine, create a new `.gclient` file in the engine, and edit the file:

   ```
    solutions = [
       {
         "managed": False,
         "name": "src/flutter",
         "url": "git@gitee.com:openharmony-sig/flutter_engine.git",
         "custom_deps": {},
         "deps_file": "DEPS",
         "safesync_url": "",
       },
    ]
   ```

3. Synchronize code: In the engine directory, execute `gclient sync`; here the engine source code, official packages repository will be synchronized, and the ohos_setup task will be executed;

4. Download sdk: From [OpenHarmony SDK]（https://developer.huawei.com/consumer/cn/develop）Download the matching development tool,Suites that are not downloaded through this channel are not supported

   ```sh
    # Environment variables that need to be set: HarmonyOS SDK, ohpm, hvigor, node
    export TOOL_HOME=/Applications/DevEco-Studio.app/Contents # For mac
    export DEVECO_SDK_HOME=$TOOL_HOME/sdk # command-line-tools/sdk
    export PATH=$TOOL_HOME/tools/ohpm/bin:$PATH # command-line-tools/ohpm/bin
    export PATH=$TOOL_HOME/tools/hvigor/bin:$PATH # command-line-tools/hvigor/bin
    export PATH=$TOOL_HOME/tools/node/bin:$PATH # command-line-tools/tool/node/bin
   ```

5. Start building: In the engine directory, execute `./ohos` to start building the flutter engine that supports ohos devices.
   
6. Update code: In the engine directory, execute `./ohos -b master`

## Engine Construction product

  [Construction product](https://docs.qq.com/sheet/DUnljRVBYUWZKZEtF?tab=BB08J2)

## FAQ:
1. When running the project, an error of Member notfound:'isOhos' is reported: Please ensure that all dart patches are applied in the src/third_party/dart directory (the patches are located in the src/flutter/attachment/repos directory, and you can use git apply to apply the patch). Recompile the engine after patching

2. Prompt Permission denied: Execute chmod +x <script file> to add execution permissions

3. Compile the engine in debug/release/profile mode separately: `./ohos -t debug|release|profile`

4. See help: `./ohos -h`

5. Due to the different ways Windows, macOS, and Linux handle line endings, applying Dart patches can result in different Dart VM snapshot hash values. You can obtain the current snapshot hash value using the following method:

   ```shell
    python xxx/src/third_party/dart/tools/make_version.py --format='{{SNAPSHOT_HASH}}'
   ```

   Here, xxx is the engine path you created yourself.

   If the obtained value is not “8af474944053df1f0a3be6e6165fa7cf”, then you need to check whether the xxx/src/third_party/dart/runtime/vm/dart.cc file and xxx/src/third_party/dart/runtime/vm/image_snapshot.cc file in the end of all lines is not with LF, windows can use notepad++ to view, other system specific methods please inquire



## Embedding layer code construction guide

1. Edit shell/platform/ohos/flutter_embedding/local.properties:

   ```
    sdk.dir=<OpenHarmony sdk directory>
    nodejs.dir=<nodejs sdk directory>
   ```

2. You need to copy files from the compiled `engine` directory to `shell/platform/ohos/flutter_embedding/flutter/libs/arm64-v8a/`
    1. debug/release，copy `libflutter.so`
    2. profile，copy `libflutter.so` and `libvmservice_snapshot.so`

3. In the shell/platform/ohos/flutter_embedding directory, execute

     ```
      # The optional values for buildMode are: debug release profile
      hvigorw --mode module -p module=flutter@default -p product=default -p buildMode=debug assembleHar --no-daemon
     ```



4. The har file output path is: shell/platform/ohos/flutter_embedding/flutter/build/default/outputs/default/flutter.har

5. After obtaining the har file, rename the file according to `flutter.har.BUILD_TYPE.API` format, where `BUILD_TYPE` refers to 'debug', 'release' and 'profile', and 'API' refers to the current SDK version, such as api11 is 11;  For example, if the debug version of api11 is currently built, rename it to `flutter.har.debug.11`.

6. Replace the corresponding file in the `flutter_flutter/packages/flutter_tools/templates/app_shared/ohos.tmpl/har/har_product.tmpl/` directory and run the project again to make the project take effect.

ps: If you are using the Beta version of DevEco Studio and encounter the error "must have required property 'compatibleSdkVersion', location: build-profile.json5:17:11" when compiling the project, Modify the shell/platform/ohos/flutter_embedding/hvigor/hvigor-config.json5 file by referring to section ‘6 Create the project and run Hello World’ [Configuration Plug-in] in《DevEco Studio Environment configuration guide.docx》.