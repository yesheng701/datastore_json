#pragma once

#include "data_node.h"
#include "jansson/jansson.h"

class DataTreeLoader : public NcObjectCpp
{
public:
	static DataTreeLoader* allocWithDataDir(NcString* dir) { return NcNew(DataTreeLoader, dir); }
	static DataTreeLoader* instanceWithDataDir(NcString* dir) { return autorelease(allocWithDataDir(dir)); }
	
	bool addAdditionalNodesFromFile(const cqWCHAR* path); // call DatastorePatch
	bool addAdditionalNodesFromFile(NcString* path); 

	NC_PROPERTY(root);
	DataNode* root() { return m_dataNodeRoot; }

	// for test
	bool _loadWorldManagerByName(NcString* wmrFile);

protected:
	bool loadWorldManager();

	DataTreeLoader(NcString* dataDir);
	~DataTreeLoader();

private:
	DataNode* m_dataNodeRoot;
	NcString* m_dataDir;
};