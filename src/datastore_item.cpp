#include "stdafx.h"
#include "datastore_item.h"
#include "datastore_util.h"

void DatastoreItem::addLocalData(LocalData* localData)
{
	m_localDatas->addObject(localData);
}

void DatastoreItem::setId(NcString* id)
{
	if (m_id != id)
	{
		release(m_id);
		m_id = retain(id);
	}
}

json_t* DatastoreItem::toJson()
{
	json_t* json = json_array();
	for (int i = 0; i < m_localDatas->count(); i++)
	{
		LocalData* localData = m_localDatas->objectAtIndex(i);
		json_array_append_new(json, localData->toJson());
	}
	return json;
}

static int _comparator(NcString* l, NcString* r, void* /*context*/)
{
	if (l->endsWith(L"full"))
		return 1;
	if (r->endsWith(L"full"))
		return -1;
	return r->compare(l);
}

NcString* DatastoreItem::versionsAndSize()
{
	NcHashmap<NcString, NcNumber>* versionSizeMap = NcHashmap<NcString, NcNumber>::instance();

	int baseDataSize = 0;

	for (int i = 0; i < m_localDatas->count(); i++)
	{
		LocalData* localData = m_localDatas->objectAtIndex(i);
		NcArray<SourceFile>* sources = localData->sources();

		for (int j = 0; j < sources->count(); j++)
		{
			SourceFile* source = sources->objectAtIndex(j);
			if (source->url()->endsWith(L"base.dat"))
			{
				baseDataSize = source->size();
			}
			NcString* version = NcString::stringWithFormat(source->isGuid() ? L"%d_full" : L"%d", source->version());
			NcNumber* size = versionSizeMap->objectWithKey(version);
			if (size != NULL)
			{
				versionSizeMap->removeObjectWithKey(version);
				versionSizeMap->addObjectForKey(version, NcNumber::instanceWithInt(size->intValue() + (int)source->size()));
			}
			else
			{
				versionSizeMap->addObjectForKey(version, NcNumber::instanceWithInt((int)source->size()));
			}
		}
	}
	NcArray<NcString>* keys = versionSizeMap->keys();

	keys->sortUsingFunction(_comparator, NULL);

	NcString* versionsAndSize = NULL;
	uint32 increamentalUpdateSize = baseDataSize;
	for (int i = 0; i < keys->count(); i++)
	{
		NcString* key = keys->objectAtIndex(i);
		int value = versionSizeMap->objectWithKey(key)->intValue();
		if (i == keys->count() - 1)
		{
			int value = versionSizeMap->objectWithKey(key)->intValue();
			versionsAndSize = versionsAndSize != NULL ? NcString::stringWithFormat(L"%@,%@:%d", versionsAndSize, key, value)
				: NcString::stringWithFormat(L"%@:%d", key, value);
			break;
		}
		increamentalUpdateSize += value;
		versionsAndSize = versionsAndSize != NULL ? NcString::stringWithFormat(L"%@,%@:%d", versionsAndSize, key, increamentalUpdateSize)
			: NcString::stringWithFormat(L"%@:%d", key, increamentalUpdateSize);
	}
	
	return versionsAndSize;
}

NcString* DatastoreItem::files()
{
	NcArray<NcString>* fileNames = NcArray<NcString>::instance();
	NcArray<NcString>* guids = NcArray<NcString>::instance();
	for (int i = 0; i < m_localDatas->count(); i++)
	{
		LocalData* localData = m_localDatas->objectAtIndex(i);
		SourceFile* sourceFile = localData->sources()->objectAtIndex(0);
		fileNames->addObject(sourceFile->fileName());
		guids->addObject(sourceFile->guid() != NULL ? sourceFile->guid() : sourceFile->md5());
	}

	NcString* files = NULL;
	for (int i = 0; i < fileNames->count(); i++)
	{
		files = files != NULL ? NcString::stringWithFormat(L"%@,%@:%@", files, fileNames->objectAtIndex(i), guids->objectAtIndex(i))
			: NcString::stringWithFormat(L"%@:%@", fileNames->objectAtIndex(i), guids->objectAtIndex(i));
	}
	return files;
}

DatastoreItem::DatastoreItem()
{
	m_localDatas = NcArray<LocalData>::alloc();
}

DatastoreItem::~DatastoreItem()
{
	release(m_id);
	release(m_localDatas);
}