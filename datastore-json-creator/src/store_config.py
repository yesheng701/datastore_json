#!usr/bin/env python
# -*- coding: UTF-8 -*-

import os, platform, collections, sys
import json
from store_patch import *
reload(sys)
sys.setdefaultencoding('utf-8')

g_special_json_admins = ("china/0beijing", "china/0shanghai", "china/1chongqing", "china/1tianjin", "china/xianggang", "china/aomen")
global g_store_config
g_store_config = None

class StoreConfig(object):
    """
    权限和文件对应列表，如果更新数据有列表有变化，应该更新次列表
    """

    def __init__(self, path):
        """
        初始化，从 base.dat 解析到的 wmr.json 得到数据Id -- 名称字典
        :param path: wmr.json 路径
        """

        f = file("cfg.txt","w")
        dict = json.load(open(path))
        self.DATA_IDS.clear()
        self.DATA_IDS["cn"] = "基础数据"
        self.DATA_IDS["cn/cmr"] = "电子眼"
        for i in range(0, len(dict["subnodes"])):
            province = dict["subnodes"][i]
            if province["directoryName"] in g_special_json_admins:
                data =  province["directoryName"] + "  " + province["chsName"] + "\n"
                directoryName = province["directoryName"].replace("china", "cn")
                self.DATA_IDS[directoryName] = province["chsName"]
                f.write(data)
                continue
            provinceName = province["directoryName"].replace("china", "cn")
            self.PROVINCE_IDS[provinceName] = province["chsName"]
            for j in range(0, len(province["subnodes"])):
                 city = province["subnodes"][j]
                 data =  city["directoryName"] + "  " + city["chsName"] + "\n"
                 directoryName = city["directoryName"].replace("china", "cn")
                 self.DATA_IDS[directoryName] = city["chsName"]
                 f.write(data)
        self.add_from_store_patch(path)
        f.close()

    @staticmethod
    def get_store_config(wmrfile=None):
        global g_store_config
        if wmrfile != None:
            g_store_config = StoreConfig(wmrfile)
        return g_store_config

    def add_from_store_patch(self, path):
        path = "store_patch.txt"
        if os.path.isfile(path):
            patch = StorePatch(path)
            for key,value in patch.DATA_IDS.items():
                self.DATA_IDS[key] = value
   
    # 每个版本下readme文件名称
    README_FILE_NAME = 'readme.txt'
    #完整版本的后缀
    FULL_SUFFIX = '_full'
    #文件后缀，遍历时只扫描相应后缀的文件
    FILE_SUFFIX = ['.nkvd','.nkvds', '.dat','.db']

    # 数据Id <--> 数据名称
    DATA_IDS = collections.OrderedDict([])

    PROVINCE_IDS = collections.OrderedDict([])



       