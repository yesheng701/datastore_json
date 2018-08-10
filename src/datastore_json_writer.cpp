#include "stdafx.h"
#include "datastore_json_writer.h"
#include "source_file.h"
#include "datastore_util.h"

bool _datastoreItemFinder(DatastoreItem* item, void* context)
{
	return item->id()->startsWith((NcString*)context);
}

void DatastoreJsonWriter::_createJsonByItemAndRoot(NcArray<DatastoreItem>* items, DataNode* node, DatastoreJson* json)
{
	for (int i = 0; i < node->subNodes()->count(); i++)
	{
		DataNode* currentNode = node->subNodes()->objectAtIndex(i);
		int findIdx = items->indexOfItemWithFinder(_datastoreItemFinder, currentNode->id());
		if (findIdx != -1)
		{
			DatastoreItem* item = items->objectAtIndex(findIdx);
			DatastoreJsonItem* jsonItem = DatastoreJsonItem::instance();
			jsonItem->setName(currentNode->name());
			jsonItem->setEngName(currentNode->engName());
			jsonItem->setPinyin(currentNode->pinyin());
			jsonItem->setDirectoryName(currentNode->id());

			if (currentNode->subNodes()->count() == 0)
			{
				// only for city
				jsonItem->setVersionsAndSize(item->versionsAndSize());
				jsonItem->setFiles(item->files());
				DatastoreJsonItem* parent = json->datastoreJsonItemById(DatastoreUtil::parentDirectoryName(currentNode->id()));
				if (currentNode->id()->countOfCharacter('/') < 2 || parent == NULL)
				{
					json->addItem(jsonItem);
				}
				else
				{
					parent->addSubItem(jsonItem);
				}
			}
			else
			{
				json->addItem(jsonItem);				
			}			
			
			_createJsonByItemAndRoot(items, currentNode, json);
		}
	}
}

void DatastoreJsonWriter::createJsonByItemAndRoot(NcArray<DatastoreItem>* items, DataNode* root)
{
	DatastoreJson* datastoreJson = DatastoreJson::instance();
	datastoreJson->setCurrentVersion(m_version);
	datastoreJson->setLatestVersion(m_latestVersion);

	_createJsonByItemAndRoot(items, root, datastoreJson);

	datastoreJson->sort();
	setDatastoreJson(datastoreJson);
}

static const cqWCHAR* _suffixForLanguage(DatastoreJsonLanguageType lang)
{
	if (lang == DatastoreJsonLanguageType_english)
	{
		return L"_eng";
	}
	else
	{
		return L"";
	}
}

void DatastoreJsonWriter::writeJson()
{
	DatastoreJsonLanguageType languageTypes[] = { DatastoreJsonLanguageType_chinese, DatastoreJsonLanguageType_english };

	for (int i = 0; i < element_of(languageTypes); i++)
	{
		NcString* jsonFile = m_version == -1 ? NcString::stringWithFormat(L"datastore%s.json", _suffixForLanguage(languageTypes[i]))
			: NcString::stringWithFormat(L"datastore%d%s.json", m_datastoreJson->currentVersion(), _suffixForLanguage(languageTypes[i]));

		NcString* jsonFilePath = NcString::stringWithFormat(L"%@/%@", m_dataDir, jsonFile);

		if (m_version == -1)
			m_datastoreJson->setLatestVersion(m_latestVersion);

		json_t* json = m_datastoreJson->toJson(languageTypes[i]);
		cqCHAR* buffer = json_dumps(json, JSON_INDENT(3) | JSON_PRESERVE_ORDER);
		size_t size = cq_strlen(buffer);
		Util_writeBufferIntoFile(buffer, size, jsonFilePath->cstr());
		json_dump_free(buffer);
		json_decref(json);

		printf("making %ls Done\n", jsonFile->cstr());
	}
	
}

void DatastoreJsonWriter::setDatastoreJson(DatastoreJson* datastoreJson)
{
	if (m_datastoreJson != datastoreJson)
	{
		release(m_datastoreJson);
		m_datastoreJson = retain(datastoreJson);
	}
}

DatastoreJsonWriter::~DatastoreJsonWriter()
{
	release(m_datastoreJson);
	release(m_dataDir);
}

void DatastoreJsonWriter::setDataDir(NcString* dir)
{
	if (!m_dataDir->equalsSafe(dir))
	{
		release(m_dataDir);
		m_dataDir = retain(dir);
	}
}
