#include "stdafx.h"
#include "data_node.h"

DataNode* DataNode::nodeById(NcString* id)
{
	if (id == NULL)
		return NULL;
	if (m_id->equalsSafe(id))
		return this;

	queue<DataNode*> q;
	q.push_back(this);

	while (!q.empty())
	{
		DataNode* o = q.front();
		q.pop_front();

		for (int i = 0; i < o->subNodes()->count(); i++)
		{
			DataNode* item = o->subNodes()->objectAtIndex(i);
			if (item->id()->equalsSafe(id))
			{
				return item;
			}
			q.push_back(item);
		}
	}

	return NULL;
}

void DataNode::setId(NcString* id)
{
	if (m_id != id)
	{
		release(m_id);
		m_id = retain(id);
	}
}

void DataNode::setName(NcString* name)
{
	if (m_name != name)
	{
		release(m_name);
		m_name = retain(name);
	}
}

void DataNode::setEngName(NcString* engName)
{
	if (m_engName != engName)
	{
		release(m_engName);
		m_engName = retain(engName);
	}
}

void DataNode::setPinyin(NcString* pinyin)
{
	if (m_pinyin != pinyin)
	{
		release(m_pinyin);
		m_pinyin = retain(pinyin);
	}
}

DataNode::DataNode()
{
	m_subNodes = NcArray<DataNode>::alloc();
}

DataNode::~DataNode()
{
	release(m_id);
	release(m_name);
	release(m_subNodes);
	release(m_pinyin);
	release(m_engName);
}

bool DataNode::fromJson(json_t* json)
{
	json_t* chsName = json_object_get(json, "chsName");
	json_t* directoryName = json_object_get(json, "directoryName");
	const cqWCHAR* directoryNameValue = json_string_value_S(directoryName);
	const cqWCHAR* chsNameValue = json_string_value_S(chsName);

	if (directoryNameValue == NULL)
		return false;

	setName(NcString::stringWithCString(chsNameValue));
	setId(NcString::stringWithCString(directoryNameValue));
	setId(m_id->stringByReplacingOccurrencesOfString(_S("china"), _S("cn")));
	setEngName(NcString::stringWithCString(json_string_value_S(json_object_get(json, "engName"))));
	setPinyin(NcString::stringWithCString(json_string_value_S(json_object_get(json, "pinyin"))));

	json_t* subnodes = json_object_get(json, "subnodes");
	if (json_is_array(subnodes))
	{
		NcArray<DataNode>* subnodeArray = NcArray<DataNode>::instance();
		size_t count = json_array_size(subnodes);
		for (size_t i = 0; i < count; i++)
		{
			DataNode* node = DataNode::instance();
			if(node->fromJson(json_array_get(subnodes, i)))
				subnodeArray->addObject(node);
		}

		setSubnodes(subnodeArray);
	}

	return true;
}

void DataNode::setSubnodes(NcArray<DataNode>* subNodes)
{
	if (m_subNodes != subNodes)
	{
		release(m_subNodes);
		m_subNodes = retain(subNodes);
	}
}
