#pragma once

#include "data_node.h"

class DatastorePatch : public NcObjectCpp
{
public:
	static DatastorePatch* alloc() { return NcNew(DatastorePatch); }
	static DatastorePatch* instance() { return autorelease(alloc()); }

	bool load(const cqWCHAR* patchPath);

	NcArray<DataNode>* dataNodes() { return m_dataNodes; }

protected:
	DatastorePatch();
	~DatastorePatch();

private:
	NcArray<DataNode>* m_dataNodes;
};