#pragma once

#include "jansson/jansson.h"

class DataNode : public NcObjectCpp
{
public:
	static DataNode* alloc() { return NcNew(DataNode); }
	static DataNode* instance() { return autorelease(alloc()); }

	DataNode* nodeById(NcString* id);

	NC_PROPERTY(id);
	void setId(NcString* id);
	NcString* id() { return m_id; }

	NC_PROPERTY(name);
	void setName(NcString* name);
	NcString* name() { return m_name; }

	NC_PROPERTY(engName);
	void setEngName(NcString* engName);
	NcString* engName() { return m_engName; }

	NC_PROPERTY(pinyin);
	void setPinyin(NcString* pinyin);
	NcString* pinyin() { return m_pinyin; }

	NC_PROPERTY(subNodes);
	NcArray<DataNode>* subNodes() { return m_subNodes; }
	void setSubnodes(NcArray<DataNode>* subNodes);

	bool fromJson(json_t* json);

protected:
	DataNode();
	~DataNode();

private:
	NcString* m_id;
	NcString* m_name;
	NcString* m_pinyin;
	NcString* m_engName;
	NcArray<DataNode>* m_subNodes;
};