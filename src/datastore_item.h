#pragma once

#include "local_data.h"
#include "jansson/jansson.h"

class DatastoreItem : public NcObjectCpp
{
public:
	static DatastoreItem* alloc() { return NcNew(DatastoreItem); }
	static DatastoreItem* instance() { return autorelease(alloc()); }

	NC_PROPERTY(localData);
	void addLocalData(LocalData* localData);
	NcArray<LocalData>* localDatas() { return m_localDatas; }
	
	NC_PROPERTY(id);
	void setId(NcString* id);
	NcString* id() { return m_id; }

	json_t* toJson();

	NcString* versionsAndSize();
	NcString* files();

protected:
	DatastoreItem();
	~DatastoreItem();

private:
	NcArray<LocalData>* m_localDatas;
	NcString* m_id;
};