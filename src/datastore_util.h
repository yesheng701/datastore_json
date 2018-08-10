#pragma once

enum DatastoreJsonLanguageType
{
	DatastoreJsonLanguageType_none = 0,
	DatastoreJsonLanguageType_chinese = 1,
	DatastoreJsonLanguageType_english = 2
};

class DatastoreUtil
{
public:
	// data/3_full/cn ==> cn
	static NcString* currentDirectoryName(NcString* currentPath);

	// data/3_full/cn/base.dat ==> base.dat
	// data/3/cn/cn_jv.nkvds ==> cn_jv.nkvds
	static NcString* currentFileName(NcString* fileNamePath);

	// cn/cmr ==> cn, cn ==> "", cn/hebei/0sjz ==> cn/hebei
	static NcString* parentDirectoryName(NcString* directoryName);

	// data/3json ==> 3,  data/5version\\12json==> 12
	static int versionNumberFromString(NcString* str);
};