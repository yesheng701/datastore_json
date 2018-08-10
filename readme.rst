数据商店使用文档
================

准备工作
--------

数据需要按照一定的目录结构组织，如下(--表示一级缩进)：

.. code-block:: c

    --datastore
    ----data
    ------1_full
    --------cn
    ----------0beijing
    ------------0beijing_jv.nkvds
    ------------0beijing_poi.nkvds
    ------------0beijing_rp.nkvds
    ------------0beijing_map.nkvds
    ----------anhui
    ------------0hfs
    --------------0hfs_jv.nkvds
    --------------0hfs_poi.nkvds
    --------------0hfs_rp.nkvds
    --------------0hfs_map.nkvds
    ------------......
    ----------......
    --------readme.txt
    ------1_json
    --------cn
    ----------0beijing
    ------------0beijing.json
    ----------......
    ------2_full
    ------2
    ------2_json
    ----datastore.json
    ----datastore1.json
    ----datastore2.json

说明：

#. 数据目录结构需要和上面对应
#. 1_full表示这是第一版数据，里面的数据的版本号应该都为 1
#. 数据以市为单位存储，行政区划分取自 WorldManager.wmr 生成的 json文件。 "jv" "poi" "rp" "map" 分别为放大图、poi、算路、底图数据

数据发布
--------

发布数据前先需要生成 json 文件，然后将 json 和数据一起部署在内网以及外网。
生成 datastore.json 使用 datastore_json_creator.py ，用法如下：

.. code-block:: c

    datastore_json_creator.py --src e:/datastore

说明：

#. --src 数据商店目录
#. 会在数据商店目录生成 datastore.json 与每个城市的数据描述文件 city.json，datastore.json 包含城市层级以及当前版本数据文件信息，city.json 包含当前版本城市数据数据文件及下载 url 等信息。
#. 如果想引入并不在 worldManager 里面的条目（如电子眼、东北话等），需要在当前目录包含 store_patch.txt。文件每一行的内容为 id=中文描述。比如，电子眼数据是 cn/cmr=电子眼。
#. 由于每版数据 worldManager 不同，当前目录下要放置对应版本的 worldManager，命名格式为 wmr[version].json。
   
内网数据置于 \\yq-wangyue.mapbar.com\datastore\006000000 下，更换或增加数据时只需要替换相应的数据文件和 json 即可。
如果之前版本数据没有变动，只需要增添新的 datastore[version].json 以及 [version]_json 目录下的 city.json 即可。

数据上传后，先测试 datastore.json 的内容是否正常，地址为
https://navicore.mapbar.com/nc/v1/datastore/005002000/datastore.json，浏览 datastore.json 的目录结构，观察城市层级关系是否正常。之后随机测试几个 city.json 的文件 url，如能正常下载，则部署正确。

