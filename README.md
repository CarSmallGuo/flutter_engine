Flutter Engine
==============

原始仓来源：https://github.com/flutter/engine

## 仓库说明：
本仓库是基于flutter官方engine仓库拓展，可构建支持在OpenHarmony设备上运行的flutter engine程序。

## 构建说明：

### 构建环境：
1. 目前仅支持linux、mac下构建；

2. 请确保当前构建环境，可以访问DEPS中配置的allowed_hosts列表。

### 构建步骤：
1. 参照https://github.com/flutter/flutter/wiki/Setting-up-the-Engine-development-environment 说明，配置好linux/mac下构建环境；

2. 获取源码，创建空文件夹engine，engine内新建.gclient文件，编辑文件：
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

3. 在engine目录，执行 `gclient sync`，这里会同步engine源码、官方packages仓，还有执行ohos_setup任务，如果中途失败，可多次尝试`gclient sync`；

4. 从 http://ci.openharmony.cn/workbench/cicd/dailybuild/dailylist 每日构建中：
* Linux：下载ohos-sdk-full，在engine根目录下，新建文件夹 `ndk/linux/4.0` ，解压ohos-sdk-full中的native文件夹到`ndk/linux/4.0`文件夹中；
* Mac：根据Mac芯片下载mac-sdk-m1-full或mac-sdk-full，新建文件夹`/ndk/mac/4.0`，解压mac-sdk-m1-full或mac-sdk-full中的native-darwin-arm64-4.xx.xx.zip文件到`/ndk/mac/4.0`中，确保`user/xxx/flutter_engine/engine/ndk/mac/4.0/native`中存在`build-tools`、`llvm`等文件(夹)；

5. engine目录，执行`make`，即可以开始构建支持ohos设备的flutter engine。
* 常见问题：
    * Mac系统弹窗提示“无法打开xxx”：到系统设置-隐私与安全性-安全性，点击“仍然打开”，重新执行`make`(可能会遇到多次)


## embedding层代码构建指导

1. 编辑shell/platform/ohos/flutter_embedding/local.properties，
    sdk.dir=\<OpenHarmony的sdk目录\>
    nodejs.dir=\<nodejs的sdk目录\>
  
2. 在shell/platform/ohos/flutter_embedding目录下，执行 
```
./hvigorw --mode module -p module=flutter@default -p product=default assembleHar
```


3. har文件输出路径为：shell/platform/ohos/flutter_embedding/flutter/build