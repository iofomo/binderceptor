#!/usr/bin/env python
# -*- coding:utf-8 -*-
# @brief:  utils for logger print
# @date:   2023.08.10 14:40:50

import traceback, os, sys, ctypes, threading
import binascii
os.system("") # Unable to explain this, just for Windows cmd color print

from pyutils.utils_import import ImportUtils
ImportUtils.initEnv()


# -------------------------------------------------------------
class LoggerUtils:
    BLACK = '\033[0;30m'
    RED = '\033[0;31m' #'\033[91m'
    GREEN = '\033[0;32m'#'\033[92m'
    YELLOW = '\033[0;33m'#'\033[93m'
    BLUE = '\033[0;34m'#'\033[94m'
    RED_GRAY = '\033[0;35m'#'\033[95m' # 洋红色
    BLUE_GREEN = '\033[0;36m'#'\033[96m' # 青色, 蓝绿色
    BLUE_GRAY = '\033[96m' # 淡蓝色
    WHITE = '\033[0;37m'#'\033[97m'
    END = '\033[0m'

    STD_INPUT_HANDLE = -10
    STD_OUTPUT_HANDLE = -11
    STD_ERROR_HANDLE = -12

    g_os_win = -1
    g_log_file = None
    g_log_size = None

    @classmethod
    def isOsWindows(cls):
        if cls.g_os_win < 0:
            opt = sys.platform.lower()
            cls.g_os_win = 1 if opt.startswith('win') else 0
        return 1 == cls.g_os_win

    @staticmethod
    def alignLine(txt, num=48):
        txt = txt if txt != None else ''
        tail = num - len(txt)
        if 0 < tail:
            for i in range(tail): txt += ' '
        return txt

    @staticmethod
    def printColorTexts(text1, color1, text2=None, color2=BLACK, text3=None, color3=BLACK):
        _text1 = text1 if text1 is not None else ''
        LoggerUtils.__do_print_color_text__(_text1, color1, False)
        if text2 is not None:
            LoggerUtils.__do_print_color_text__(text2, color2, False)
        if text3 is not None:
            LoggerUtils.__do_print_color_text__(text3, color3, False)
        sys.stdout.write('\n')
        sys.stdout.flush()

    @staticmethod
    def info(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.BLUE)

    @staticmethod
    def i(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.BLUE)

    @staticmethod
    def light(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.GREEN)

    @staticmethod
    def warning(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.YELLOW)

    @staticmethod
    def warn(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.YELLOW)

    @staticmethod
    def w(msg):
        LoggerUtils.__do_print_color_text__(msg, LoggerUtils.YELLOW)

    @staticmethod
    def error(msg, stack=False):
        LoggerUtils.printRed(msg)
        if not stack: return
        traceback.print_stack()
        traceback.print_exc()

    @staticmethod
    def e(msg, stack=False):
        LoggerUtils.printRed(msg)
        if not stack: return
        traceback.print_stack()
        traceback.print_exc()

    @staticmethod
    def exception(e):
        LoggerUtils.println(e)
        traceback.print_stack()
        traceback.print_exc()

    @staticmethod
    def println(*p):
        if 1 == len(p):
            print(p[0])
        else:
            print(p)
        sys.stdout.flush()

    @staticmethod
    def printClear():
        os.system('cls' if LoggerUtils.isOsWindows() else 'clear')
        sys.stdout.flush()

    std_out_handle = None

    @staticmethod
    def getHandler():
        if None == LoggerUtils.std_out_handle:
            LoggerUtils.std_out_handle = ctypes.windll.kernel32.GetStdHandle(LoggerUtils.STD_OUTPUT_HANDLE)
        return LoggerUtils.std_out_handle

    @staticmethod
    def printRed(txt):
        LoggerUtils.__do_print_color_text__(txt, LoggerUtils.RED)

    @staticmethod
    def printGreen(txt):
        LoggerUtils.__do_print_color_text__(txt, LoggerUtils.GREEN)

    @staticmethod
    def printBlue(txt):
        LoggerUtils.__do_print_color_text__(txt, LoggerUtils.BLUE)

    @staticmethod
    def printYellow(txt):
        LoggerUtils.__do_print_color_text__(txt, LoggerUtils.YELLOW)

    @staticmethod
    def isPrintText(txt):
        if isinstance(txt, str): return True
        if sys.version_info.major < 3:
            try:
                if isinstance(txt, unicode): return True
            except Exception as e:
                pass
        return False

    @staticmethod
    def __do_print_color_text__(txt, color_code, flush=True):
        if LoggerUtils.isPrintText(txt):
            sys.stdout.write(color_code + txt + LoggerUtils.END)
            if flush and not txt.endswith('\n') and not txt.endswith('\r'):
                sys.stdout.write('\n')
        else:
            print(txt)
        if flush: sys.stdout.flush()

    @staticmethod
    def hexdump(s):
        if s is None: return
        hex_string = binascii.hexlify(s)
        hex_groups = [hex_string[i:i + 32] for i in range(0, len(hex_string), 32)]
        ascii_groups = [s[i:i + 16] for i in range(0, len(s), 16)]
        offset = 0
        for hex_group, ascii_group in zip(hex_groups, ascii_groups):
            hex_offset = '{0:08x}'.format(offset)
            hex_group_formatted = ' '.join(hex_group[i:i + 2] for i in range(0, len(hex_group), 2))
            ascii_group_formatted = ''.join(char if 32 <= ord(char) < 127 else '.' for char in ascii_group)
            print("{0}: {1:<47} {2}".format(hex_offset, hex_group_formatted, ascii_group_formatted))
            offset += 16
        sys.stdout.flush()

    @classmethod
    def __monitorLoggerFile__(cls, _fname):
        while cls.g_log_file is not None:
            try:
                if not os.path.isfile(cls.g_log_file): continue
                currSize = os.path.getsize(cls.g_log_file)
                if currSize < cls.g_log_size: cls.g_log_size = currSize
                with open(cls.g_log_file, 'r') as f:
                    f.seek(cls.g_log_size, 1)
                    while None != cls.g_log_file:
                        line = f.readline()
                        if line is None: break  # exception
                        l = len(line)
                        if l <= 0: break  # finished
                        cls.g_log_size += l
                        print(line)
            except Exception as e:
                pass

    @classmethod
    def monitorFile(cls, fname):
        if cls.g_log_file is None:  # create
            try:
                os.remove(fname)
            except Exception as e:
                pass
            cls.g_log_size = 0
            cls.g_log_file = fname
            t = threading.Thread(target=LoggerUtils.__monitorLoggerFile__, args=(fname,))
            t.start()
        elif cls.g_log_file != fname:
            cls.g_log_size = 0
            cls.g_log_file = fname

    @classmethod
    def monitorFileCancel(cls):
        cls.g_log_size = 0
        cls.g_log_file = None

    @staticmethod
    def doPrintTreeLine(line, maxLen):
        if maxLen <= 0:
            print(line)
        else:
            line = line.strip()
            ll = line.replace('─', ' ').replace('├', ' ').replace('│', ' ').replace('└', ' ')
            l = len(ll.decode())
            l += (len(ll) - l) / 2
            assert l < maxLen, '%d, %d' % (maxLen, l)
            print(line + ' ' + ('-' * (maxLen - l)))

    @staticmethod
    def getTreeMaxLen(dir_path, indent=''):
        maxLen = 0
        files = os.listdir(dir_path)
        for i, file in enumerate(files):
            full_path = os.path.join(dir_path, file)
            if os.path.isdir(full_path):
                l = LoggerUtils.getTreeMaxLen(full_path, indent + '    ')
            else:
                l = len(indent + '    ' + file)
            if maxLen < l: maxLen = l
        return maxLen

    @staticmethod
    def doPrintTree(dir_path, maxLen, indent=''):
        files = os.listdir(dir_path)
        files.sort()

        fcnt = len(files)
        for i, file in enumerate(files):
            full_path = os.path.join(dir_path, file)
            is_last = i == fcnt - 1
            if os.path.isdir(full_path):
                if is_last:
                    LoggerUtils.doPrintTreeLine(indent + '└── ' + file, maxLen)
                    if indent.startswith(' ') or len(indent) <= 0:
                        LoggerUtils.doPrintTree(full_path, maxLen, '│' + indent + '    ')
                    else:
                        LoggerUtils.doPrintTree(full_path, maxLen, indent + '    ')
                else:
                    LoggerUtils.doPrintTreeLine(indent + '├── ' + file, maxLen)
                    LoggerUtils.doPrintTree(full_path, maxLen, indent + '│   ')
            else:
                if is_last:
                    LoggerUtils.doPrintTreeLine(indent + '└── ' + file, maxLen)
                else:
                    LoggerUtils.doPrintTreeLine(indent + '├── ' + file, maxLen)

    @staticmethod
    def printTree(dir_path, printLine=False):
        print(os.path.basename(dir_path))
        maxLen = LoggerUtils.getTreeMaxLen(dir_path) + 4 if printLine else 0
        LoggerUtils.doPrintTree(dir_path, maxLen)


def run():
    LoggerUtils.println(None)
    LoggerUtils.println('a')
    LoggerUtils.println('a', 'b')
    LoggerUtils.println(['a'], 'b', {'c'})
    LoggerUtils.printBlue('blue\n')
    LoggerUtils.printRed('red\n')
    LoggerUtils.printGreen('green\n')
    LoggerUtils.printColorTexts("title", LoggerUtils.RED, 'content', LoggerUtils.GREEN, 'information\n', LoggerUtils.BLUE)
    LoggerUtils.println('done')


if __name__ == "__main__":
    run()
