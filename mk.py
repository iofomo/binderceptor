#!/usr/bin/env python
# -*- coding:utf-8 -*-
# @Brief:  mk
# @Date:   2023.11.28 16:22:33

import sys, os

g_env_path = os.getcwd()
g_this_file = os.path.realpath(sys.argv[0])
g_this_path = os.path.dirname(g_this_file)
# --------------------------------------------------------------------------------------------------------------------------
sys.path.append(g_this_path)

from pyutils.utils_cmn import CmnUtils
from pyutils.utils_file import FileUtils
from pyutils.utils_logger import LoggerUtils
from pyutils.arguments import BasicArgumentsValue
# --------------------------------------------------------------------------------------------------------------------------


def getModules():
    if False:
        # 你也可以在这里定制要编译模块和顺序
        return [
            "jni",
            "cmpt-mts-binderceptor",
            # add more module here ...
            "app",
        ]

    # 默认编译所有项，且jni模块第一个编译，app模块最后一个编译
    modules = []
    hasJni, hasApp = False, False
    for item in os.listdir(g_this_path):
        if not os.path.isdir(g_this_path + os.sep + item): continue
        if not os.path.isfile(g_this_path + os.sep + item + '/mk.sh'): continue
        if 'jni' == item: hasJni = True
        elif 'app' == item: hasApp = True
        else: modules.append(item)
    if hasJni: modules.insert(0, 'jni')
    if hasApp: modules.append('app')
    return modules

def buildModules(OUT_PATH, BUILD_TYPE, modules):
    OUT_PATH += os.sep + BUILD_TYPE
    FileUtils.remove(OUT_PATH)
    FileUtils.ensureDir(OUT_PATH)
    for module in modules: buildModule(OUT_PATH, BUILD_TYPE, module)

def buildModule(OUT_PATH, BUILD_TYPE, module):
        modulePath = g_this_path + os.sep + module
        if not os.path.exists(modulePath): return
        LoggerUtils.println('Build ' + module)
        if 'jni' != module:# aar module build
            ret = CmnUtils.doCmdCall('cd %s && ./mk.sh %s %s %s' % (modulePath, OUT_PATH, BUILD_TYPE, module))
        else:# native module build
            OUT_JNI_PATH = g_this_path + '/out/jni/' + BUILD_TYPE
            FileUtils.ensureDir(OUT_JNI_PATH + '/armeabi-v7a/')
            FileUtils.ensureDir(OUT_JNI_PATH + '/arm64-v8a/')
            FileUtils.ensureDir(OUT_JNI_PATH + '/x86/')
            FileUtils.ensureDir(OUT_JNI_PATH + '/x86_64/')
            ret = CmnUtils.doCmdCall('cd %s && ./mk.sh %s %s %s %s' % (modulePath, OUT_PATH, OUT_JNI_PATH, BUILD_TYPE, module))
        assert ret, 'Error: build fail ' + module

def ensureEnv():
    local = g_this_path + '/local.properties'
    if os.path.isfile(local): return
    header = [
        '## This file must *NOT* be checked into Version Control Systems,\n'
        '# as it contains information specific to your local configuration.\n',
        '#\n',
        '# Location of the SDK. This is only used by Gradle.\n',
        '# For customization when using a Version Control System, please read the\n',
        '# header note.\n',
        '#Wed Oct 28 18:13:44 CST 2023\n'
    ]
    with open(local, 'w') as f: f.writelines(header)

def run():
    zarg = BasicArgumentsValue()
    if 2 <= zarg.count():
        OUT_PATH, BUILD_TYPE, MODULE = zarg.get(0), zarg.get(1), zarg.get(2)
    else: # build from local
        BUILD_TYPE = 'debug'
        OUT_PATH = g_this_path + '/out'
        MODULE = None
    try:
        ensureEnv()
        if os.path.exists(g_this_path + '/gradlew'):
            ret = CmnUtils.doCmdCall('cd %s && ./gradlew clean' % (g_this_path))
            assert ret, 'Error: gradlew clean'

        modules = getModules()
        assert CmnUtils.isEmpty(MODULE) or MODULE in modules, 'Not found: ' + MODULE

        if BUILD_TYPE == 'all':
            if CmnUtils.isEmpty(MODULE):
                buildModules(OUT_PATH, 'debug', modules)
                buildModules(OUT_PATH, 'release', modules)
            else:
                buildModule(OUT_PATH, 'debug', MODULE)
                buildModule(OUT_PATH, 'release', MODULE)
        elif CmnUtils.isEmpty(MODULE):
            buildModules(OUT_PATH, BUILD_TYPE, modules)
        else:
            buildModule(OUT_PATH, BUILD_TYPE, MODULE)
    except Exception as e:
        LoggerUtils.println(e)
        raise SyntaxError(e)

if __name__ == "__main__":
    run()
