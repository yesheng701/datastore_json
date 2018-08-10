#pragma once

#include "data_node.h"
#include "datastore_json.h"
#include "data_tree_loader.h"
#include "datastore_item.h"
#include "datastore_item_json_writer.h"
#include "jansson/jansson.h"

class DatastoreJsonWriter : public NcObjectCpp
{
public:
	static DatastoreJsonWriter* alloc() { return NcNew(DatastoreJsonWriter); }
	static DatastoreJsonWriter* instance() { return autorelease(alloc()); }

	void createJsonByItemAndRoot(NcArray<DatastoreItem>* items, DataNode* root);
	void writeJson();

	NC_PROPERTY(dataDir);
	void setDataDir(NcString* dir);

	NC_PROPERTY(version);
	void setVersion(int version) { m_version = version; }
	int version() { return m_version; }

	NC_PROPERTY(latestVersion);
	void setlLatestVersion(int latestVersion) { m_latestVersion = latestVersion; }
	int latestVersion() { return m_latestVersion; }

	NC_PROPERTY(datastoreJson);
	void setDatastoreJson(DatastoreJson* datastoreJson);
	DatastoreJson* datastoreJson() { return m_datastoreJson; }

protected:
	void _createJsonByItemAndRoot(NcArray<DatastoreItem>* items, DataNode* node, DatastoreJson* json);

	DatastoreJsonWriter() {}
	~DatastoreJsonWriter();

private:
	int m_version;				// json文件的版本 比如datastore1, datastore2...
	int m_latestVersion;		// 最高版本 设置就表示当前json文件为最高版本，不设置就不是。
	DatastoreJson* m_datastoreJson;
	NcString* m_dataDir;
};