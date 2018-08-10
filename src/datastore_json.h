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
		����˳��
		1.������
		2.��������
		3.������
		4.ֱϽ�У��������Ϻ���������죬
		5.����ʡ�ݣ�����ʡ�ݰ���A-Z˳�����У�ʡ�ڳ�������˳��ʡ������ŵ�һλ���������а���A-Z˳�����С�
		6.�ر�������
	*/
	void sort();

protected:
	DatastoreJson();
	~DatastoreJson();

private:
	int m_latestVersion; // json�����е�latestVersion�ֶ�
	int m_currentVersion; // json�����е�currentVersion�ֶ�
	NcArray<DatastoreJsonItem>* m_items;
	DatastoreJsonLanguageType m_type;
};