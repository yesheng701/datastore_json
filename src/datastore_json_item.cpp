#include "stdafx.h"
#include "datastore_json_item.h"
#include "datastore_util.h"
#include "readme_txt.h"

void DatastoreJsonItem::setName(NcString* name)
{
	if (m_name != name)
	{
		release(m_name);
		m_name = retain(name);
	}
}

void DatastoreJsonItem::setDirectoryName(NcString* directoryName)
{
	if (m_directoryName != directoryName)
	{
		release(m_directoryName);
		m_directoryName = retain(directoryName);
	}
}

void DatastoreJsonItem::setVersionsAndSize(NcString* versionsAndSize)
{
	if (m_versionsAndSize != versionsAndSize)
	{
		release(m_versionsAndSize);
		m_versionsAndSize = retain(versionsAndSize);
	}
}

void DatastoreJsonItem::setFiles(NcString* files)
{
	if (m_files != files)
	{
		release(m_files);
		m_files = retain(files);
	}
}

void DatastoreJsonItem::addSubItem(DatastoreJsonItem* datastoreJsonItem)
{
	m_subItems->addObject(datastoreJsonItem);
}

json_t* DatastoreJsonItem::toJson(DatastoreJsonLanguageType type)
{
	json_t* json = json_object();	

	if (type == DatastoreJsonLanguageType_english)
	{
		json_object_set_new(json, "name", json_string_S(engName()->cstr()));
		json_object_set_new(json, "directoryName", json_string_S(m_directoryName->cstr()));
		json_object_set_new(json, "versionDescription", json_string_S(ReadmeTxt::globleInstance()->dataVersionEng()->cstr()));
	}
	else
	{
		json_object_set_new(json, "name", json_string_S(m_name->cstr()));
		json_object_set_new(json, "directoryName", json_string_S(m_directoryName->cstr()));
		json_object_set_new(json, "pinyin", json_string_S(m_pinyin->cstr()));
		json_object_set_new(json, "versionDescription", json_string_S(ReadmeTxt::globleInstance()->dataVersionChs()->cstr()));
	}	

	if (m_versionsAndSize != NULL)
		json_object_set_new(json, "versionsAndSize", json_string_S(m_versionsAndSize->cstr()));

	if (m_files != NULL)
		json_object_set_new(json, "files", json_string_S(m_files->cstr()));

	if (m_subItems->count() != 0)
	{
		json_t* subnodes = json_array();
		for (int i = 0; i < m_subItems->count(); i++)
		{
			DatastoreJsonItem* item = m_subItems->objectAtIndex(i);
			json_array_append_new(subnodes, item->toJson(type));
		}
		json_object_set(json, "subnodes", subnodes);
		json_decref(subnodes);
	}

	return json;
}

static int _datastoreSubItemCompare(DatastoreJsonItem* l, DatastoreJsonItem* r, void* /* context */)
{
	NcString* lId = DatastoreUtil::currentDirectoryName(l->directoryName())->substringFrom(1);
	NcString* rId = DatastoreUtil::currentDirectoryName(r->directoryName())->substringFrom(1);
	return lId->compare(rId);
}

void DatastoreJsonItem::sortSubItems()
{
	// 地级市按照A-Z排序
	if (m_subItems->count() <= 1)
		return;
	DatastoreJsonItem* capital = m_subItems->firstObject();
	m_subItems->removeObjectAtIndex(0);
	m_subItems->sortUsingFunction(_datastoreSubItemCompare, NULL);
	m_subItems->insertObjectAtIndex(0, capital);
}

DatastoreJsonItem::DatastoreJsonItem()
{
	m_subItems = NcArray<DatastoreJsonItem>::alloc();
}

DatastoreJsonItem::~DatastoreJsonItem()
{
	release(m_name);
	release(m_engName);
	release(m_directoryName);
	release(m_pinyin);
	release(m_versionsAndSize);
	release(m_files);
	release(m_subItems);
}

void DatastoreJsonItem::setEngName(NcString* engmane)
{
	if (m_engName != engmane)
	{
		release(m_engName);
		m_engName = retain(engmane);
	}
}

void DatastoreJsonItem::setPinyin(NcString* pinyin)
{
	if (m_pinyin != pinyin)
	{
		release(m_pinyin);
		m_pinyin = retain(pinyin);
	}
}
