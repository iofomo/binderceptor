# Binderceptor

<p align="left">
    <img src="doc/README.assets/license-mit.svg" alt="license:MIT" />
    <img src="doc/README.assets/python-2x3x.svg" alt="python:2x3x" />
</p>

### 说明

`Binderceptor`是`Android`平台下底层`Binder`拦截器，目的是解决现有方案拦截不全，开发效率低的问题，具有以下特点：

-   支持Android 6.x~14.x系统版本
-   支持所有`Binder`通信拦截
-   支持`Binder`通信日志打印
-   支持`Binder`通信特征数据过滤
-   支持`Binder`通信协议拦截/修改/伪装

详细的文档可参考：

[【Android】深入底层Binder拦截](https://blog.csdn.net/chendianbo/article/details/134719327)

### 快速开始

本项目在以下开发环境中编译通过：

-   Android Studio：4.2
-   Gradle：6.9.2
-   NDK：21.4.7075529

```shell
$ ./mk.py
```

编译结果在`out`目录：

```shell
out
├── debug
│   ├── app.apk
│   ├── cmpt-mts-binderceptor.aar
│   └── libs
│       ├── arm64-v8a
│       │   └── libifmabinderceptor-jni.so
│       └── armeabi-v7a
│           └── libifmabinderceptor-jni.so
├── jni
│   ├── debug
│   │   ├── arm64-v8a
│   │   │   └── libifmabinderceptor-native.a
│   │   └── armeabi-v7a
│   │       └── libifmabinderceptor-native.a
│   └── relaese
│       ├── arm64-v8a
│       │   └── libifmabinderceptor-native.a
│       └── armeabi-v7a
│           └── libifmabinderceptor-native.a
└── release
│    ├── app.apk
│    ├── cmpt-mts-binderceptor.aar
│    └── libs
│        ├── arm64-v8a
│        │   └── libifmabinderceptor-jni.so
│        └── armeabi-v7a
│            └── libifmabinderceptor-jni.so
```

### 许可协议

本项目基于`MIT`许可协议，详情查看 [许可协议](doc/LICENSE) 文档。

>   本项目和所有的工具都是MIT许可证下的开源工具，这意味着你可以完全访问源代码，并可以根据自己的需求进行修改。
