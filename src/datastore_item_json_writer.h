#pragma once

#include "data_node.h"
#include "datastore_item.h"
#include "data_tree_loader.h"

class DatastoreItemJsonWriter : public NcObjectCpp
{
public:
	static DatastoreItemJsonWriter* alloc() { return NcNew(DatastoreItemJsonWriter); }
	static DatastoreItemJsonWriter* instance() { return autorelease(alloc()); }

	void initLocalDataManager(DataNode* root);
	void writeJson();

	NC_PROPERTY(version);
	void setVersion(int version) { m_version = version; }
	int version() { return m_version; }

	NC_PROPERTY(datastoreItems);
	void addItem(DatastoreItem* item);
	NcArray<DatastoreItem>* items() { return m_items; }

	NC_PROPERTY(dataDir);
	void setDataDir(NcString* dir);

	DatastoreItem* datastoreItemById(NcString* id);

protected:
	DatastoreItemJsonWriter();
	~DatastoreItemJsonWriter();

private:
	NcArray<DatastoreItem>* m_items;
	int m_version;
	NcString* m_dataDir;
};