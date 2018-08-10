#pragma once

#include "datastore_json_item.h"
#include "datastore_util.h"
#include "jansson/jansson.h"

class DatastoreJson : public NcObjectCpp
{
public:
	static DatastoreJson* alloc() { return NcNew(DatastoreJson); }
	static DatastoreJson* instance() { return autorelease(alloc()); }

	NC_PROPERTY(latestVersion);
	void setLatestVersion(int latestVersion) { m_latestVersion = latestVersion; }
	int latestVersion() { return m_latestVersion; }

	NC_PROPERTY(currentVersion);
	void setCurrentVersion(int currentVersion) { m_currentVersion = currentVersion; }
	int currentVersion() { return m_currentVersion; }

	NC_PROPERTY(itemArray);
	void addItem(DatastoreJsonItem* item);
	NcArray<DatastoreJsonItem>* items() { return m_items; }

	DatastoreJsonItem* datastoreJsonItemById(NcString* id);
	int datasoreJsonItemIndexById(NcString* id);

	json_t* toJson(DatastoreJsonLanguageType type);

	void setDatastoreJsonLanguageType(DatastoreJsonLanguageType type);

	/*
		排列顺序：
		1.搜索框
		2.基础数据
		3.电子眼
		4.直辖市：北京，上海，天津，重庆，
		5.其他省份，其他省份按照A-Z顺序排列，省内城市排列顺序：省会城市排第一位，其他城市按照A-Z顺序排列。
		6.特别行政区
	*/
	void sort();

protected:
	DatastoreJson();
	~DatastoreJson();

private:
	int m_latestVersion; // json对象中的latestVersion字段
	int m_currentVersion; // json对象中的currentVersion字段
	NcArray<DatastoreJsonItem>* m_items;
	DatastoreJsonLanguageType m_type;
};