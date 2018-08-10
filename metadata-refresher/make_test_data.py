#coding=utf-8

import  hashlib, codecs, fnmatch, shutil
import os, sys, json, argparse
from collections import OrderedDict

try:
    sys.path.append(os.path.abspath("../datastore-json-creator/src"))
    from store_config import *
except:
    pass

g_last_province_name = ""
g_special_admins = ("0beijing", "0shanghai", "1chongqing", "1tianjin", "xianggang", "aomen")
MAGIC_WORDS = "wyrlluyylfea1yyawyrlluyylfea1yyawyrlluyylfea1yya"
PATCH_MAGIC_WORDS = "ilyylvmbwy"

def make_detail_dbs(path, city, isfull, version):
    dbs = ("jv","poi","rp","map")
    if(os.path.isdir(path) != True):
        os.makedirs(path)
    if not path[0:path.rfind("/")].endswith("tts"):
       for db in dbs:
           if isfull:
               file_name = path + "\\" + city + "_" + db + ".nkvd"
           else:
               file_name =  path + "\\" + city + "_" + db +"." + str(version) + ".diff"             
           if(os.path.isdir(file_name) != True):
               f = file(file_name,"w")
               if isfull:
                   f.write(MAGIC_WORDS)
               else:
                   f.write(PATCH_MAGIC_WORDS)
               f.close()
    else:
        file_name = path + "\\" + city + ".dat"
        if(os.path.isdir(file_name) != True):
               f = file(file_name,"w")
               f.write(MAGIC_WORDS)
               f.close()


def make_test_directory_by_json(wmrfile, output, version, isfull = True):
    currentPath =  os.path.join(output, "cn", str(version))
    if isfull:
        currentPath +=  "_full"
    dict = StoreConfig.get_store_config(wmrfile).DATA_IDS

    for key in dict.keys():

        if key.find("/") == key.rfind("/"):
            district_name = key[key.find("/") + 1: len(key)]
        else:
            district_name = key[key.find("/") + 1: key.rfind("/")]

        key = key.replace("cn/","")
        path = currentPath + "\\" + key      

        if district_name in g_special_admins:
            make_detail_dbs(path, district_name, isfull, version)
        else:
            city = key[key.rfind("/") + 1: len(key)]
            make_detail_dbs(path, city, isfull, version)

    tts_path = os.path.join(output,"tts",str(version) + "_full")
    if not isfull:
        tts_path = tts_path.replace("_full","")
    shutil.move(currentPath + "/tts", tts_path)

def make_test_directory(wmrfile, output):
    dict = StoreConfig.get_store_config(wmrfile).DATA_IDS
    for key in dict.keys():

        path = output + "\\" + key
        path = path.replace("china", "cn")
        if(os.path.isdir(path) != True):
            os.makedirs(path)
 
def make_config_file(wmrfile):
    currentPath =  sys.path[0]
    try:
        out_file = file("config.txt", "w")
        f = codecs.open(wmrfile, "rb", "utf_16")
        for line in f:
            dir = currentPath + "\\cn"
            items = line.encode("utf-8").split()
            if len(items) < 5 or items[1] == "x":
                continue
            # admincode, xx, 城市名称，拼音，dataId
            district_name = items[4]
            if items[1] == "xx":
                dir = dir + "\\" + district_name
                g_last_province_name = district_name
                if(district_name in g_special_admins):
                   out_file.write(district_name + "\n")
            elif items[1] == "xxx":
                full = "        (\"cn." + g_last_province_name + "." + district_name + "\", \"" + items[2] + "\"),\n"
                out_file.write(full)
        f.close
        out_file.close
    except Exception, e:
        print "load wmr file failed:", wmrfile
        print e

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--wmrfile", default="wmr.json", help='WorldManger json file.')
    parser.add_argument("--output", help='output dir.')
    parser.add_argument("--version", default="1", help='data version.')

    args = parser.parse_args()
    make_test_directory_by_json(args.wmrfile, args.output, args.version)

if __name__ == '__main__':
    main()
  