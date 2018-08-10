#include "stdafx.h"
#include "datastore_item_json_writer.h"
#include "datastore_util.h"

void DatastoreItemJsonWriter::addItem(DatastoreItem* item)
{
	m_items->addObject(item);
}

DatastoreItem* DatastoreItemJsonWriter::datastoreItemById(NcString* id)
{
	for (int i = 0; i < m_items->count(); i++)
	{
		DatastoreItem* localDataManager = m_items->objectAtIndex(i);
		if (localDataManager->id()->equalsSafe(id))
		{
			return localDataManager;
		}
	}
	return NULL;
}

void DatastoreItemJsonWriter::initLocalDataManager(DataNode* root)
{
	autoreleasepool
	{
		NcString* fullDirName = NcString::stringWithFormat(L"%d_full", m_version);
		size_t length = cq_wcslen(fullDirName->cstr());

		NcString* path = NcString::stringWithFormat(L"%@/data/%@", m_dataDir, fullDirName);
		FileSysWalker* walker = FileSysWalker::instanceWithPath(path);
		if (walker->next())
		{
			NcArray<NcString>* directories = walker->directories(); // cn, tts
			for (int i = 0; i < directories->count(); i++)
			{
				NcString* parentDirectoriePath = directories->objectAtIndex(i);
				FileSysWalker* subDirectorie = FileSysWalker::instanceWithPath(NcString::stringWithFormat(L"%@/%@", path, parentDirectoriePath));
				while (subDirectorie->next())
				{
					NcString* currentPath = subDirectorie->currentDirectory();
					cqWCHAR* temp = cq_wcsstr(currentPath->cstr(), fullDirName->cstr());
					NcString* id = NcString::stringWithCString(temp + length + 1);
					if (root->nodeById(id) != NULL)
					{
						NcArray<NcString>* files = subDirectorie->files();
						if (files->count() > 0)
						{
							DatastoreItem* localDataManager = DatastoreItem::instance();
							for (int i = 0; i < files->count(); i++)
							{
								NcString* filePath = NcString::stringWithFormat(L"%@/%@", currentPath, files->objectAtIndex(i));
								if (filePath->endsWith(L"nkvds") || filePath->endsWith(L"dat"))
								{
									NcString* localPath = NcString::stringWithFormat(L"%@/%@", id, files->objectAtIndex(i));
									LocalData* localData = LocalData::instanceWithLocalPathAndVersion(localPath, m_version);
									localData->setDataDir(m_dataDir);
									localData->initSourceFiles(filePath);

									localDataManager->setId(id);
									localDataManager->addLocalData(localData);
								}
							}
							addItem(localDataManager);
						}
					}
				}
			}
		}
	}
}

void DatastoreItemJsonWriter::writeJson()
{
	autoreleasepool
	{
		int count = m_items->count();
		for (int i = 0; i < count; i++)
		{
			DatastoreItem* item = m_items->objectAtIndex(i);

			NcString* currentDirectoryName = DatastoreUtil::currentDirectoryName(item->id());

			NcString* jsonFile = NcString::stringWithFormat(L"%@/data/%d_json/%@", m_dataDir, m_version, item->id());
			FileSys_createDeepDir(jsonFile->cstr());

			NcString* jsonFilePath = NcString::stringWithFormat(L"%@/%@.json", jsonFile, currentDirectoryName);

			json_t* json = item->toJson();
			cqCHAR* buffer = json_dumps(json, JSON_INDENT(3) | JSON_PRESERVE_ORDER);
			size_t size = cq_strlen(buffer);
			Util_writeBufferIntoFile(buffer, size, jsonFilePath->cstr());
			json_dump_free(buffer);
			json_decref(json);

			static int printLen = 0;
			for (int k = 0; k < printLen; ++k)
			{
				printf("\b ");
				printf("\b");
			}
			ConsoleProgress_print(i, count);
			NcString* str = NcString::stringWithFormat(L"%@/%@.json", DatastoreUtil::currentDirectoryName(item->id())
				, currentDirectoryName);
			wprintf(str->cstr());
			printLen = str->length();
		}
	}
}

DatastoreItemJsonWriter::DatastoreItemJsonWriter()
{
	m_items = NcArray<DatastoreItem>::alloc();
}

DatastoreItemJsonWriter::~DatastoreItemJsonWriter()
{
	release(m_items);
	release(m_dataDir);
}

void DatastoreItemJsonWriter::setDataDir(NcString* dir)
{
	if (!m_dataDir->equalsSafe(dir))
	{
		release(m_dataDir);
		m_dataDir = retain(dir);
	}
}
