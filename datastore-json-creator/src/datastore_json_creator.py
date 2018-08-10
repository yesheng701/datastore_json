#!usr/bin/env python
# -*- coding: UTF-8 -*-

from store_common import *
from collections import OrderedDict
import os, json, fnmatch, argparse, sqlite3, datetime, re

global g_last_ver
g_last_ver = None

# 由于是 OrderedDict 字典，遍历顺序首先访问先插入元素，如
# 原顺序，需要先逆序 5_full:170,2:50,3:60,4:30,5:30
# 5:30,4:30,3:60,2:50,5_full:170
# 3-5_full 增量包总大小为 60 + 30 + 30
def make_version_and_size(dict_, dat_size=0):
    dict = OrderedDict()
    if len(dict_) <= 1:
        dict = dict_
    else:
        while len(dict_) >= 1:
            key, value= dict_.popitem()
            dict[key] = value
    version_and_size = None
    increamentalUpdateSize = 0
    for (key, value) in dict.items():
        if key.endswith("_full"):
             if version_and_size == None:
                 version_and_size = key + ":" + str(value)
             else:
                 version_and_size = version_and_size + "," + key + ":" + str(value)
        else:
            if version_and_size == None:
                increamentalUpdateSize += (value+dat_size)
                version_and_size = key + ":" + str(increamentalUpdateSize) 
            else:
                increamentalUpdateSize += value
                version_and_size = version_and_size + "," +key + ":" + str(increamentalUpdateSize)
    return version_and_size

def find_full_data_for_update(versions, version_num, file):
    file_name = None
    full_version_num = version_num - 1
    for version in versions:
        if version.get_version_no() == full_version_num and version.is_full():
            file_name = version.get_version_path() + "/" + file
    return file_name

def find_download_file(versions, base, data_id, start_version_no, end_version_no):
    rtn = []
    repeat = {}
    files = None
    versions_and_size_dict = collections.OrderedDict()
    file_filter = data_id + "*"
    for v in versions:
        dat_size = 0
        ver_num = v.get_version_no()
        if start_version_no <= ver_num <= end_version_no:
            if v.is_diff():
                continue

            path = v.get_version_path()
            dat_size = 0
            for f in v.get_files():
                fpath, fname = os.path.split(f)
                ftype = fname[0:fname.find(".") + 1]
                if not fnmatch.fnmatch(f, file_filter):
                    continue
                # 针对 cn 补丁
                if data_id == 'cn' and f.find('/') != f.rfind('/'):
                    continue
                # 获取 guid 或者 MD5，保存至 datastore.json 中
                file_path = v.get_version_path() + '/' + f
                guid = ''
                file_path = file_path.replace('\\', '/')
                if f.endswith('.nkvd') or f.endswith('.nkvds'):
                    if StoreGuid.guid_dict.has_key(file_path):
                        guid = StoreGuid.guid_dict[file_path]
                    if files == None:
                        files = fname + ":" + guid
                    else:                        
                        files = files + "," + fname + ":" + guid
                else:
                    md5 =  StoreUtil.get_file_md5(file_path)
                    if files == None:                       
                        files = fname + ":" + md5
                    else:                       
                        files = files + "," + fname + ":" + md5

                local_dir = f
                url = base + "/" + v.get_version_name() + "/" + local_dir              
                sz = os.path.getsize(os.path.join(path, f))
                if f.endswith('dat'):
                    dat_size = dat_size + sz
                entry = StoreFile(url,
                                  StoreUtil.get_file_md5(os.path.join(path, f)),
                                  sz,
                                  local_dir,
                                  guid).to_dict()
                entrys = []                    
                entrys.append(entry)
                if versions_and_size_dict.has_key(v.get_version_name()):
                    versions_and_size_dict[v.get_version_name()] = versions_and_size_dict[v.get_version_name()] + sz
                else:
                    versions_and_size_dict[v.get_version_name()] = sz
                # 补充增量更新数据
                for version in versions:    
                    if version.get_version_no() <= ver_num and version.is_diff():
                        patch_file_name = fname + '.' +str(version.get_version_no()) +".diff"
                        patch_dir = version.get_version_path()+ "/" + fpath + "/" + patch_file_name
                        if os.path.isfile(patch_dir):
                            sz = os.path.getsize(patch_dir)
                        url = base + "/" + str(version.get_version_no()) + "/" + fpath + "/" + patch_file_name
                        full_data_dir = find_full_data_for_update(versions, version.get_version_no(), f)
                        if full_data_dir != None and os.path.isfile(patch_dir) and os.path.isfile(full_data_dir):
                            # todo 补充 applyOnGuid
                            applyOnGuid = ''
                            full_data_dir = full_data_dir.replace('\\', '/')
                            if StoreGuid.guid_dict.has_key(full_data_dir):
                                applyOnGuid = StoreGuid.guid_dict[full_data_dir]                          
                            entry = StoreFile(url,
                                StoreUtil.get_file_md5(patch_dir),
                                sz,
                                local_dir,
                                '',
                                applyOnGuid).to_dict()

                            if versions_and_size_dict.has_key(version.get_version_name()):
                                versions_and_size_dict[version.get_version_name()] = versions_and_size_dict[version.get_version_name()]  + sz
                            else:
                                versions_and_size_dict[version.get_version_name()] = sz
                            entrys.append(entry)

                filename = url[url.rfind("/")+1:]
                idx = len(rtn)
                if filename not in repeat.keys():
                    rtn.append(entrys)
                    repeat[filename] = idx
                else:
                    rtn[repeat[filename]] = entrys
    return rtn, make_version_and_size(versions_and_size_dict, dat_size), files

def data_exist(v, data_id):
    if len(data_id.split("/")) == 2:
        country, province = data_id.split("/")
        file_filter = province + "*"
    elif len(data_id.split("/")) == 3:
        country, province, city = data_id.split("/")
        file_filter = city + "*"
    else:
        file_filter = data_id + "*"

    for f in v.get_files():
        fpath, fname = os.path.split(f)
        if fnmatch.fnmatch(fname, file_filter):
            return True
    return False

def build_json(src, data_id, base, versions, datastore_data, max_version=99999):
    last_full_version = None
    last_version = None
    file_list = []
    file = OrderedDict([("localPath",""),("sources",[])])
    for v in versions:
        if v.is_full() and (last_full_version is None or last_full_version.get_version_no() < v.get_version_no() <= max_version):
            last_full_version = v
        if last_version is None or (last_version.get_version_no() < v.get_version_no() <= max_version and data_exist(v, data_id)):
            last_version = v

    last_ver = last_version.get_version_no()

    readme = ReadmeTxt(os.path.join(last_version.get_version_path(), StoreConfig.README_FILE_NAME))
    release_note = readme.match_release_note(data_id)
    versionDescription = readme.get_data_version()
  
    download_file, version_and_size, files = find_download_file(versions, base, data_id, last_full_version.get_version_no(), last_ver)
    if len(download_file) != 0:
        for single_file in download_file:
            file = OrderedDict([
                ("localPath", ""),
                ("version", str(last_ver)),
                ("sources", [])
                ])
            file["localPath"] = single_file[0]["localPath"]
            for source in single_file:
                del source["localPath"]
                file["sources"].append(source)
            file_list.append(file)
    else:
        return

    sortKey = data_id
    if data_id.count('/') == 2:
        country, province, city = data_id.split("/")
        if province != "shan3xi":
            province = re.sub(r'([\d]+)','', province)
        city = city[1:len(city)]
        sortKey = country + "/" + province + "/" + city

    tree = OrderedDict([
                    ("name", StoreConfig.get_store_config().DATA_IDS[data_id]),
                    ("directoryName", data_id),
                    ("sortKey", sortKey),
                    ("versionDescription", versionDescription),
                    ("versionsAndSize", version_and_size),
                    ("files", files)
                    ])
    city = None
    if len(data_id.split("/")) == 2:
        country, province = data_id.split("/")
        datastore_data.append(tree)
    elif len(data_id.split("/")) == 3:
        country, province, city = data_id.split("/")
        province_tree = []
        for province_node in datastore_data:
            if province_node["name"] == StoreConfig.get_store_config().PROVINCE_IDS[country + "/" + province]:
                province_tree = province_node
                province_tree["subnodes"].append(tree)
        if len(province_tree) == 0:
            if sortKey.count('/') == 2:
                sortKey = sortKey[0:sortKey.rfind('/')]        
            province_tree = OrderedDict([
                        ("name", StoreConfig.get_store_config().PROVINCE_IDS[country + "/" + province]),
                        ("sortKey", sortKey),
                        ("directoryName", country + '/' + province),
                        ("versionDescription", versionDescription),
                        ("subnodes", [])                      
                        ])
            datastore_data.append(province_tree)
            province_tree["subnodes"].append(tree)
    else:
        province = data_id
        datastore_data.append(tree)

    if len(file_list) != 0:
        global g_last_ver
        current_version = last_full_version.get_version_no()
        dir_name = src + "/" + base + "/" +str(current_version) + "_json/" + data_id
        if not os.path.isdir(dir_name):
            os.makedirs(dir_name)
        if city == None:
            name = dir_name + "/" + province + ".json" 
        else:
             name = dir_name + "/" + city + ".json"
        output = open(name, "w")
        output.write(json.dumps(file_list, indent=3, ensure_ascii = False))


def sortDatastoreJsonByPinyin(datastore_data):

    if(len(datastore_data)) < 10:
       for province_data in datastore_data:
          if province_data.has_key('subnodes'):
              city_array = province_data['subnodes']
              for city in city_array:
                 city.pop('sortKey')
          province_data.pop('sortKey')
       return datastore_data

    front_data = datastore_data[0 : 6]
    back_data = datastore_data[len(datastore_data) - 2 : len(datastore_data)]
    datastore_data = datastore_data[6 : len(datastore_data) - 2]
    datastore_data.sort(key=lambda x: x['sortKey'])
    datastore_data = front_data + datastore_data + back_data
    for province_data in datastore_data:
       if province_data.has_key('subnodes'):
           city_array = province_data['subnodes']
           city_array.sort(key=lambda x: x['sortKey'])
           for city in city_array:
               city.pop('sortKey')
       province_data.pop('sortKey')

    return datastore_data

def create_datastore_json(src, debug = True):

    base_folder = src
    map_data = 'data'
    if base_folder != '.':
        StoreGuid.get_store_guid(base_folder + '\\' + map_data)

    map_data_store = DataStore(os.path.join(base_folder, map_data))
    map_versions = map_data_store.get_versions()

    global g_last_ver
    g_last_ver = map_data_store.get_last_version().get_version_no()
    start_ver = map_data_store.get_first_version().get_version_no()

    if debug:
        for v in range(start_ver, g_last_ver + 1):
            datastore_data = []
            wmr_path = "wmr" + str(v) + ".json"
            store_config = StoreConfig.get_store_config(wmr_path)
            for data_id in store_config.DATA_IDS.keys():
                build_json(base_folder, data_id, map_data, map_versions, datastore_data, v)            
        
            name = "datastore" + str(v) + ".json" if v != g_last_ver else "datastore.json"
            output = open(os.path.join(base_folder, name), "w")
            datastore_data = sortDatastoreJsonByPinyin(datastore_data)
            if v == g_last_ver:
                save_datastore_data = OrderedDict([("latestVersion", g_last_ver),
                            ("currentVersion", v),
                            ("items", datastore_data)])
            else:
                save_datastore_data = OrderedDict([("currentVersion", v),
                            ("items", datastore_data)])
            output.write(json.dumps(save_datastore_data, indent=5, ensure_ascii = False))

            output.close()
    else:
        v = g_last_ver
        wmr_path = "wmr" + str(g_last_ver) + ".json"
        datastore_data = []
        store_config = StoreConfig.get_store_config(wmr_path)
        for data_id in store_config.DATA_IDS.keys():
            build_json(base_folder, data_id, map_data, map_versions, datastore_data, g_last_ver)        
                
        name = "datastore" + str(v) + ".json" if v != g_last_ver else "datastore.json"
        output = open(os.path.join(base_folder, name), "w")
        datastore_data = sortDatastoreJsonByPinyin(datastore_data)
        save_datastore_data = OrderedDict([("latestVersion", g_last_ver),
              ("currentVersion", v),
              ("items", datastore_data)])
        output.write(json.dumps(save_datastore_data, indent=5, ensure_ascii = False))
        output.close()


def main():
    begin = datetime.datetime.now()

    parser = argparse.ArgumentParser()
    parser.add_argument("--src", default=".", help='store root path.')
    args = parser.parse_args()

    create_datastore_json(args.src)

    end = datetime.datetime.now()
    print "costs " + str(end - begin)

if __name__ == '__main__':
    main()
