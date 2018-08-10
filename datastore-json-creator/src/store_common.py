#!usr/bin/env python
# -*- coding: UTF-8 -*-

import  hashlib, codecs, fnmatch, subprocess
from collections import OrderedDict
from store_config import *

class StoreUtil(object):
    @staticmethod
    def list_files(path, filters):
        """
        list files recursively
        :param path: init path
        :param filters: filter suffix, such as ['.nkvd'] etc, only files whose suffix match args will be kept
        :return: file list relative to path
        """
        path_list = []
        for root, dirs, files in os.walk(path):
            for f in files:
                if filters:
                    for tag in filters:
                        if f.endswith(tag):
                            path_list.append(os.path.join(root, f))
                else:
                    path_list.append(os.path.join(root, f))
        return [os.path.relpath(file_path, path).replace("\\", "/") for file_path in path_list]

    @staticmethod
    def get_file_md5(file_path):
        if os.path.isfile(file_path):
            md5 = hashlib.md5()
            file_size = os.path.getsize(file_path)
            f = file(file_path, 'rb')
            if file_size <= 200:
                md5.update(f.read())
            else:
                md5.update(f.read(100))
                f.seek(-100, os.SEEK_END)
                md5.update(f.read(100))
            f.close()
            return md5.hexdigest()
        else:
            print '[StoreUtil.get_file_md5] path is not a file : ', file_path
            return ''

    @staticmethod
    def parse_version_name(name):
        version_no = -1
        if name.endswith(StoreConfig.FULL_SUFFIX):
            name = name[0:(len(name) - len(StoreConfig.FULL_SUFFIX))]
            if name.isdigit():
                version_no = int(name)
        elif name.isdigit():
            version_no = int(name)
        return version_no


class StoreFile(object):
    def __init__(self, url='', md5='', size=0, local_dir='', guid = '', applyOnGuid = ''):
        self.url = url
        self.md5 = md5
        self.size = size
        self.localPath = local_dir
        self.guid = guid
        self.applyOnGuid = applyOnGuid

    def to_dict(self):
        obj_dict = OrderedDict([('url', self.url), ('md5', self.md5), ('size', self.size),('localPath', self.localPath),('guid', self.guid),('applyOnGuid', self.applyOnGuid)])
        if self.applyOnGuid == '':
            del obj_dict['applyOnGuid']
        if self.guid == '':
            del obj_dict['guid']

        return obj_dict


class StoreVersion(object):
    """
    A diff or full version
    """
    def __init__(self, path, name):
        """
        初始化
        :param path: 数据商店路径
        :param name: 版本名称，比如：000002，000002_full
        """
        self.store_path = path
        self.version_name = name
        self.file_names = StoreUtil.list_files(os.path.join(self.store_path, name), StoreConfig.FILE_SUFFIX)

    def get_files(self):
        """
        获取当前版本下所有的文件
        :return: 文件名列表，相对于version的路径，比如cn/base.dat, cn/0beijing_map.2.db
        """
        return self.file_names

    def get_file_paths(self):
        """
        获取当前版本下面的所有文件的完整路径
        :return: 文件完整路径列表，比如 E:/datastore/cn/000002_full/cn/base.dat
        """
        return [os.path.join(self.store_path, self.version_name, file_name) for file_name in self.file_names]

    def is_full(self):
        """
        判断当前版本是否是全量版本
        :return: 全量版本时返回true
        """
        return self.version_name is not None and self.version_name.endswith(StoreConfig.FULL_SUFFIX)

    def is_diff(self):
        return not self.is_full()

    def get_version_name(self):
        return self.version_name

    def get_version_no(self):
        """
        获取当前版本号，比如：版本000002_full和00002都返回2
        :return: 整型版本号，版本格式错误时返回－1
        """
        if self.version_name is not None and (self.version_name.isdigit() or self.version_name.endswith(StoreConfig.FULL_SUFFIX)):
            return StoreUtil.parse_version_name(self.version_name)
        else:
            return -1

    def get_version_path(self):
        """
        获取version的完整路径，示例：E:/datastore/cn/00002_full
        :return: path
        """
        return os.path.join(self.store_path, self.version_name)

    def get_store_path(self):
        """
        获取version所在商店都路径，示例：E:/datastore/cn
        :return: path
        """
        return self.store_path


class DataStore(object):
    """
    A datastore contains diff and full versions, specified by a path, list dir is 1_full, 2, etc
    """

    def __init__(self, base_path):
        assert base_path is not None
        self.versions = []
        self.base_path = base_path
        file_list = os.listdir(self.base_path)
        # delete path name with "json"
        i = 0
        while i < len(file_list):
            if file_list[i].count("json") != 0 or not os.path.isdir(base_path + "/" + file_list[i]):
                file_list.pop(i)
                i -= 1
            i += 1

        # sort by version number
        file_list.sort(key=lambda ver: StoreUtil.parse_version_name(ver))
        self.versions = [StoreVersion(self.base_path, version_name) for version_name in file_list]

    def get_versions(self):
        """
        get all diff and full versions
        :return: version list sorted by version number
        """
        return self.versions

    def get_version_num(self):
        return len(self.versions)

    def get_version_by_index(self, index):
        """
        get version by index
        :param index: index of version
        :return: version, if index is out of bounds, None is returned
        """
        if 0 <= index < len(self.versions):
            return self.versions[index]
        else:
            return None

    def get_full_versions(self):
        """
        get all full versions
        :return: full version list
        """
        return [version for version in self.versions if version.is_full()]

    def get_diff_versions(self):
        """
        get all diff versions
        :return: diff version list
        """
        return [version for version in self.versions if version.is_diff()]

    def get_first_version(self):
        if len(self.versions) > 0:
            return self.versions[0]
        return None

    def get_last_version(self):
        if len(self.versions) > 0:
            return self.versions[-1]
        return None


class HeaderTxt(object):
    """
    读取和修改 header.txt 的类
    """
    def __init__(self, file_name):
        self.d = OrderedDict()
        f = codecs.open(file_name, 'rb', 'utf_16')
        for line in f:
            equal_pos = line.find(u'=')
            if equal_pos == -1:
                continue
            self.d[line[:equal_pos].strip()] = line[equal_pos + 1:].strip()
        f.close()

    def set(self, key, value):
        self.d[key] = value

    def remove_key(self, key):
        if self.d.has_key(key):
            del self.d[key]

    def write_to_file(self, file_name):
        f = codecs.open(file_name, 'wb', 'utf_16')
        for key, value in self.d.iteritems():
            f.write(u'%s=%s\r\n' % (key, value))
        f.close()


class ReadmeTxt(object):
    """
    解析readme文件，文件格式示例：
    2015春季版             #数据版本
    2015/06/15          #更新日期
    cn.*=更新四维数据到2015春季版             #数据的releaseNote，匹配最后一个
    cn.cmr=更新了2000个电子眼, vip版本更新了4000个
    """
    def __init__(self, path):
        readme = codecs.open(path, 'rb', 'utf_16').read().encode("utf-8").split("\r\n")
        
        self.data_version, self.release_date, self.release_notes = None, None, OrderedDict()
        num = 0

        for each_line in readme:
            if num == 0:
                self.data_version = each_line.rstrip()
            elif num == 1:
                self.release_date = each_line.rstrip()
            else:
                k, v = each_line.split("=")
                note = k.replace(".", "/")
                self.release_notes[note] = v.rstrip()
            num += 1

    def get_data_version(self):
        return self.data_version

    def get_release_date(self):
        return self.release_date

    def match_release_note(self, data_id):
        release_note = None
        for k in self.release_notes.keys():
            if fnmatch.fnmatch(data_id, k):
                release_note = self.release_notes[k]
        return release_note.rstrip() if release_note is not None else None

global g_store_guid
g_store_guid = None

class StoreGuid(object):
    """
    nkvd 文件名和 guid 对应列表，由 nkvd ls 指令一次性获取
    """

    def __init__(self, path):

        subprocess_command = 'nkvd ls ' + path + ' -r'
        p = subprocess.Popen(subprocess_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        nkvd_rtn = p.communicate('')
        p.wait()

        rtn_lines = nkvd_rtn[0]
        lines = rtn_lines.splitlines()
        for line in lines:
            if line.startswith("---") or line.startswith("error"):
                continue
            file_name = line[0:line.find(' ')]
            file_name = file_name.replace('\\', '/')
            guid = line[line.find('guid:{') + 6:line.rfind('}')]
            if len(guid) == 0:
                continue
            version = line[line.rfind(':') + 1:len(line)]
            self.guid_dict[file_name] = guid
            self.version_dict[file_name] = int(version)

    @staticmethod
    def get_store_guid(path=None):
        global g_store_guid
        if g_store_guid == None:
            g_store_guid = StoreGuid(path)
        return g_store_guid

    guid_dict = collections.OrderedDict([])
    version_dict = collections.OrderedDict([])

