#!usr/bin/env python
# -*- coding: UTF-8 -*-
import os, platform, collections, sys
import hashlib, codecs, fnmatch
from collections import OrderedDict

class StorePatch(object):
    """
    解析 StorePatch 文件，文件格式示例
    directoryName -- chsName
    增添 worldManager 中不存在的 directoryName，如电子眼、tts语音等
    """
    def __init__(self, path):
        store_patch = codecs.open(path, 'rb', 'utf_16').read().encode("utf-8").split("\r\n")
        
        for each_line in store_patch:
             k, v = each_line.split("=")
             self.DATA_IDS[k] = v

    DATA_IDS = collections.OrderedDict([])