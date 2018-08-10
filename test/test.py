#!usr/bin/env python
# -*- coding: utf-8 -*-
import os, sys, subprocess, shutil, json
reload(sys)
sys.setdefaultencoding('utf-8')
import unittest

try:
    sys.path.append(os.path.abspath("../datastore-json-creator/src"))
    sys.path.append(os.path.abspath("../metadata-refresher"))
    from store_common import *
    from make_test_data import *
    from datastore_json_creator import *
except:
    pass

def create_datastore(path, version, isfull = True):
    make_test_directory_by_json(path + "/wmr.json", path, version, isfull)
    cn_readme_path = path + "/cn_readme.txt"
    tts_readme_path = path + "/tts_readme.txt"

    dst_cn_readme_path = path + "/cn/" + str(version) +"_full/readme.txt"
    if not isfull:
        dst_cn_readme_path = dst_cn_readme_path.replace("_full","")
    shutil.copyfile(cn_readme_path, dst_cn_readme_path)

    dst_tts_readme_path = path + "/tts/" + str(version) +"_full/readme.txt"
    if not isfull:
        dst_tts_readme_path = dst_tts_readme_path.replace("_full","")
    shutil.copyfile(tts_readme_path, dst_tts_readme_path)

class DataStoreTest(unittest.TestCase):
    def setUp(self):
        return

    def test_store_config(self):
        dataIds = StoreConfig.get_store_config().DATA_IDS

        self.assertTrue("cn/0beijing" in dataIds.keys())
        self.assertTrue("cn/hebei/0sjz" in dataIds.keys())
        self.assertFalse("beijing" in dataIds.keys())
        self.assertFalse("cn/hebei/bds" in dataIds.keys())

        self.assertTrue(dataIds["cn/0shanghai"] == "上海市")
        self.assertTrue(dataIds["cn/shandong/0jns"] == "济南市")
        self.assertFalse(dataIds["cn/xianggang"] == "香港市")
        self.assertFalse(dataIds["cn/hubei/0whs"] == "武汉")

    def test_make_dir(self):
        dir = sys.path[0]
        dir = dir.replace("test", "test_data")
        if os.path.isdir(dir + "/cn"):
           shutil.rmtree(dir + "/cn")
        if os.path.isdir(dir + "/tts"):
           shutil.rmtree(dir + "/tts")

        make_test_directory_by_json(dir + "/wmr.json", dir, 1)    
            
        self.assertTrue(os.path.isdir(dir + "/cn/1_full/cn/0beijing"))
        self.assertTrue(os.path.isfile(dir + "/cn/1_full/cn/0beijing/0beijing_jv.nkvd"))
        self.assertTrue(os.path.isfile(dir + "/cn/1_full/cn/henan/0zzs/0zzs_jv.nkvd"))
        self.assertFalse(os.path.isfile(dir + "/cn/1_full/cn/hebei/0sjz_rp.nkvd"))
        self.assertFalse(os.path.isdir(dir + "/cn/1_full/cn/tianjin"))

        shutil.rmtree(dir + "/cn")
        shutil.rmtree(dir + "/tts")

    def test_make_json(self):
        dir = sys.path[0]
        dir = dir.replace("test", "test_data")
        if os.path.isdir(dir + "/cn"):
           shutil.rmtree(dir + "/cn")
        if os.path.isdir(dir + "/tts"):
           shutil.rmtree(dir + "/tts")

        create_datastore(dir, 1)
        create_datastore(dir, 1, False)
        create_datastore(dir, 2)
        create_datastore(dir, 2, False)
        create_datastore(dir, 3)
        create_datastore(dir, 3, False)
        create_datastore_json(dir)
        
        datastore = json.load(open(dir + "/datastore.json"))["datastore"]
        tree = json.load(open(dir + "/tree.json"))["china"]

        # 市级行政区数量，7 为 tts 语音项数目
        self.assertEqual(len(datastore), 368 + 7)
        ## 北京节点
        for data in datastore:
            if data["directoryName"] == "cn/beijing":
                self.assertEqual(data["name"], "北京市")
                self.assertEqual(data["version"], 3)
                file_lists = data["fileLists"]
                self.assertEqual(len(file_list), 4)
                download_files = file_lists[0]
                self.assertEqual(download_files["localPath"], "cn/0beijing/0beijing_jv.nkvd")
                self.assertEqual(len(download_files["sources"]), 4)
                self.assertEqual(download_files["sources"][2]["url"].endswith("2.patch"))
                break

        # 省级行政区数量 方言占7
        self.assertEqual(len(tree), 33 + 7)
        # 河北节点
        for tree_data in tree:
            if(tree_data["directoryName"] == "cn/hebei"):
                subnodes = tree_data["subnodes"]
                self.assertEqual(len(subnodes), 11)
                self.assertEqual(subnodes[0]["directoryName"], "cn/hebei/0sjz")
                self.assertEqual(subnodes[2]["directoryName"], "cn/hebei/2qhd")
                break

        shutil.rmtree(dir + "/cn")
        shutil.rmtree(dir + "/tts")
        os.remove(dir + "/datastore.json")
        os.remove(dir + "/tree.json")

def main():
    suite = unittest.TestLoader().loadTestsFromTestCase(DataStoreTest)
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    return len(result.failures) + len(result.errors)

if __name__ == "__main__":
    sys.exit(int(main() or 0))