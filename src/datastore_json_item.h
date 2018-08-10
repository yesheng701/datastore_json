#pragma once

#include "jansson/jansson.h"
#include "datastore_util.h"

class DatastoreJsonItem : public NcObjectCpp
{
public:
	static DatastoreJsonItem* alloc() { return NcNew(DatastoreJsonItem); }
	static DatastoreJsonItem* instance() { return autorelease(alloc()); }

	NC_PROPERTY(name);
	void setName(NcString* name);
	NcString* name() { return m_name; }

	NC_PROPERTY(engName);
	void setEngName(NcString* engmane);
	NcString* engName() { return m_engName; }

	NC_PROPERTY(pinyin);
	void setPinyin(NcString* pinyin);
	NcString* pinyin() { return m_pinyin; }

	NC_PROPERTY(directoryName);
	void setDirectoryName(NcString* directoryName);
	NcString* directoryName() const { return m_directoryName; }

	NC_PROPERTY(versionsAndSize);
	void setVersionsAndSize(NcString* versionsAndSize);
	NcString* versionsAndSize() { return m_versionsAndSize; }

	NC_PROPERTY(files);
	void setFiles(NcString* files);
	NcString* files() { return m_files; }

	NC_PROPERTY(subItems);
	NcArray<DatastoreJsonItem>* subItems() { return m_subItems; }
	void addSubItem(DatastoreJsonItem* datastoreJsonItem);

	json_t* toJson(DatastoreJsonLanguageType type);

	void sortSubItems();

protected:
	DatastoreJsonItem();
	~DatastoreJsonItem();

private:
	NcString* m_name;
	NcString* m_directoryName;
	NcString* m_versionsAndSize;
	NcString* m_files;
	NcString* m_engName;
	NcString* m_pinyin;
	NcArray<DatastoreJsonItem>* m_subItems;
};