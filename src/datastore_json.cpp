#include "stdafx.h"
#include "datastore_json.h"
#include "cqstl/cq_algorithm.h"

// 特殊ID 包括基础数据 电子眼 直辖市
const cqWCHAR* Municipality[] = {
	L"cn",
	L"cn/cmr",
	L"cn/0beijing",
	L"cn/0shanghai",
	L"cn/1tianjin",
	L"cn/1chongqing"
};

void DatastoreJson::addItem(DatastoreJsonItem* item)
{
	m_items->addObject(item);
}

DatastoreJsonItem* DatastoreJson::datastoreJsonItemById(NcString* id)
{
	int index = datasoreJsonItemIndexById(id);
	return index != -1 ? m_items->objectAtIndex(index) : NULL;
}

int DatastoreJson::datasoreJsonItemIndexById(NcString* id)
{
	NcArray<DatastoreJsonItem>* datastoreJsonItems = m_items;
	for (int i = 0; i < datastoreJsonItems->count(); i++)
	{
		DatastoreJsonItem* item = datastoreJsonItems->objectAtIndex(i);
		if (item->directoryName()->equalsSafe(id))
		{
			return i;
		}
	}
	return -1;
}

json_t* DatastoreJson::toJson(DatastoreJsonLanguageType type)
{
	json_t* json = json_object();
	if (m_latestVersion != 0)
		json_object_set_new(json, "latestVersion", json_integer(m_latestVersion));
	json_object_set_new(json, "currentVersion", json_integer(m_currentVersion));

	if (m_items->count() != 0)
	{
		json_t* itemsJson = json_array();
		for (int i = 0; i < m_items->count(); i++)
		{
			DatastoreJsonItem* item = m_items->objectAtIndex(i);
			json_array_append_new(itemsJson, item->toJson(type));
		}
		json_object_set(json, "items", itemsJson);
		json_decref(itemsJson);
	}

	return json;
}

static int _datastoreItemCompare(DatastoreJsonItem* l, DatastoreJsonItem* r, void* /* context */)
{
	const cqWCHAR* lId = l->directoryName()->cstr();
	const cqWCHAR* rId = r->directoryName()->cstr();

	while (*lId != L'\0'
		|| *rId != L'\0')
	{
		if (*lId != *rId)
		{
			if (cq_iswdigit(*lId))
				lId++;
			else if (cq_iswdigit(*rId))
				rId++;
			else
				break;
		}
		else
		{
			lId++;
			rId++;
		}
	}

	return *lId - *rId;
}

void DatastoreJson::sort()
{
	// 0. 首先对省内城市进行排序
	int count = m_items->count();
	for (int i = 0; i < count; i++)
	{
		m_items->objectAtIndex(i)->sortSubItems();
	}

	// 1. 其他省份A-Z排序
	m_items->sortUsingFunction(_datastoreItemCompare, NULL);

	// 2. 对特殊datastoreItem排序 [基础数据, cn, cn/cmr, 0beijing, 0shanghai....]
	int specialItemCount = element_of(Municipality); // 应该考虑的特殊DatastoreItem个数
	int miracle = 0; // 没有收录的Municipality个数
	for (int i = 0; i < specialItemCount; i++)
	{
		int index = datasoreJsonItemIndexById(NcString::stringWithCString(Municipality[i]));
		if (index != -1)
		{
			if (index != i - miracle)
			{
				DatastoreJsonItem* item = m_items->objectAtIndex(index);
				m_items->removeObjectAtIndex(index);
				m_items->insertObjectAtIndex(i - miracle, item);
			}
		}
		else
		{
			miracle += 1;
		}
	}

	// 3. 特别行政区
	for (int i = specialItemCount - miracle; i < count; i++)
	{
		DatastoreJsonItem* item = m_items->objectAtIndex(i);
		if (cq_wcsstr(item->directoryName()->cstr(), L"xianggang") != NULL)
		{
			m_items->removeObjectAtIndex(i);
			m_items->insertObjectAtIndex(count - 2, item);
		}
		else if (cq_wcsstr(item->directoryName()->cstr(), L"aomen") != NULL)
		{
			m_items->removeObjectAtIndex(i);
			m_items->insertObjectAtIndex(count - 1, item);
		}
	}
}

DatastoreJson::DatastoreJson()
{
	m_items = NcArray<DatastoreJsonItem>::alloc();
}

DatastoreJson::~DatastoreJson()
{
	release(m_items);
}

void DatastoreJson::setDatastoreJsonLanguageType(DatastoreJsonLanguageType type)
{
	m_type = type;
}
