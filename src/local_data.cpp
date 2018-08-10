#include "stdafx.h"
#include "local_data.h"
#include "datastore_util.h"
#include "json_creator.h"
#include "store_hashmap.h"

LocalData* LocalData::allocWithLocalPathAndVersion(NcString* localPath, int version)
{
	LocalData* o = NcNew(LocalData);
	o->setLocalPath(localPath);
	o->setVersion(version);
	return o;
}

void LocalData::setLocalPath(NcString* currentDirectory)
{
	if (m_localPath != currentDirectory)
	{
		release(m_localPath);
		m_localPath = retain(currentDirectory);
	}
}

json_t* LocalData::toJson()
{
	json_t* json = json_object();
	json_object_set_new(json, "localPath", json_string_S(m_localPath->cstr()));
	json_object_set_new(json, "version", json_string_S(NcString::stringWithFormat(L"%d", m_version)->cstr()));
	json_t* sourcesJson = json_array();
	for (int i = 0; i < m_sources->count(); i++)
	{
		SourceFile* source = m_sources->objectAtIndex(i);
		json_array_append_new(sourcesJson, source->toJson());
	}
	
	json_object_set(json, "sources", sourcesJson);
	json_decref(sourcesJson);

	return json;
}

void LocalData::addSourceFile(SourceFile* sourceFile)
{
	m_sources->addObject(sourceFile);
}

void LocalData::initSourceFiles(NcString* filePath)
{
	autoreleasepool
	{
		SourceFile* fullFile = SourceFile::instanceWithUrlAndDataDir(filePath, m_dataDir);
		fullFile->setVersion(m_version);
		fullFile->setIsGuid(true);
		fullFile->setGuid(StoreHashmap::globleInstance()->guidByFilePath(filePath));
		addSourceFile(fullFile);

		for (int i = 2; i <= m_version; i++)
		{
			NcString* url = NcString::stringWithFormat(L"data/%d/%@.%d.diff", i, m_localPath, i);
			NcString* nonFullFilePath = NcString::stringWithFormat(L"%@/%@", m_dataDir, url);
			NcString* prevNkvdPath = NcString::stringWithFormat(L"%@/data/%d_full/%@", m_dataDir, i - 1, m_localPath);

			if (!FileSys_pathFileExists(prevNkvdPath->cstr()) 
				|| !FileSys_pathFileExists(nonFullFilePath->cstr()))
				continue;

			SourceFile* incrementFile = JsonCreator::globalInstance()->getSourceFile(url);
			if (incrementFile == NULL)
			{
				incrementFile = SourceFile::instanceWithUrlAndDataDir(nonFullFilePath, m_dataDir);
				incrementFile->setVersion(i);
				incrementFile->setIsGuid(false);
				incrementFile->setGuid(StoreHashmap::globleInstance()->guidByFilePath(prevNkvdPath));
				JsonCreator::globalInstance()->addSourceFileCache(incrementFile->url(), incrementFile);
			}
			addSourceFile(incrementFile);
		}
	}
}

LocalData::LocalData()
{
	m_sources = NcArray<SourceFile>::alloc();
}

LocalData::~LocalData()
{
	release(m_sources);
	release(m_dataDir);
	release(m_localPath);
}

void LocalData::setDataDir(NcString* dir)
{
	if (!m_dataDir->equalsSafe(dir))
	{
		release(m_dataDir);
		m_dataDir = retain(dir);
	}
}
