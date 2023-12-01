#!/usr/bin/env python
# -*- coding:utf-8 -*-
# @brief:  ...
# @date:   2023.05.10 14:40:50

import os, sys

g_this_file = os.path.realpath(sys.argv[0])
g_this_path = os.path.dirname(g_this_file)
sys.path.append(os.path.dirname(g_this_path))

from pyutils.utils_cmn import CmnUtils
from pyutils.utils_logger import LoggerUtils


# ----------------------------------------------------------------------------------------------------------------------
class BasicArgumentsValue:
    """
        待解析的参数(默认为sys.argv)有序的，通过序号索引值来获取
        the template tools for value arguments by order, eg:
         'template.py'
         'template.py help'
         'template.py /home/in.apk /home/out.apk'
    """

    def __init__(self, argvs=None, help=[]):
        self.mKeys = []
        self.mHelp = help
        if None == argvs:
            if 1 < len(sys.argv): self.mKeys = sys.argv[1:]  # skip the first
        else:
            self.mKeys = argvs

    def has(self, v):
        for vv in self.mKeys:
            if v == vv: return True
        return False

    def get(self, index, default=None):
        if 0 <= index < self.count(): return self.mKeys[index]
        return default

    def getLast(self, index):
        if 0 <= index < self.count(): return self.mKeys[index:]
        return []

    def isEmpty(self):
        return self.count() <= 0

    def count(self):
        return len(self.mKeys)

    def println(self):
        if self.isEmpty():
            LoggerUtils.println('Empty')
            return
        for i in range(self.count()): LoggerUtils.println('value[%d]: %s' % (i, self.mKeys[i]))

    def isHelp(self):
        if not self.isEmpty() and not self.has('help'): return False
        if CmnUtils.isEmpty(self.mHelp): return False
        LoggerUtils.println('Commands like this:')
        for i in range(len(self.mHelp)): LoggerUtils.println('\t [%d]: %s' % (i, self.mHelp[i]))
        return True


class BasicArgumentsKeyValue:
    """
    待解析的参数(默认为sys.argv)无序的，参数是key:value对应，通过Key来获取
    the template tools for key value arguments, eg:
         'template.py'
         'template.py -help'
         'template.py -m -c'
         'template.py -i /home/in.apk -o /home/out.apk'
    """

    def __init__(self, argvs=None, help=[]):
        self.mKeys = {}
        self.mHelp = help
        if None == argvs:
            i = 1  # skip the first
            argvs = sys.argv
        else:
            i = 0
        l = len(argvs)
        while i < l:
            k = argvs[i]
            if len(k) <= 1: continue
            if k.startswith('-'):
                if (i + 1) < l and not argvs[i + 1].startswith('-'):
                    self.mKeys[k[1:]] = argvs[i + 1]
                    i = i + 1
                else:
                    self.mKeys[k[1:]] = ''
            i = i + 1

    def has(self, k):
        return self.mKeys.has_key(k)

    def get(self, k):
        if self.mKeys.has_key(k): return self.mKeys[k]
        return None

    def isEmpty(self):
        return len(self.mKeys) <= 0

    def count(self):
        return len(self.mKeys)

    def println(self):
        if self.isEmpty():
            LoggerUtils.println('Empty')
            return
        for k, v in self.mKeys.items():
            if 0 < len(v):
                LoggerUtils.println('key: ' + k + ", value: " + v)
            else:
                LoggerUtils.println('key: ' + k)

    def isHelp(self):
        if not self.isEmpty() and not self.has('help'): return False
        if None == self.mHelp: return False
        LoggerUtils.println('Commands like this:')
        for i in range(len(self.mHelp)): LoggerUtils.println('\t [%d]: %s' % (i, self.mHelp[i]))
        return True


def run():
    pass


if __name__ == "__main__":
    run()
