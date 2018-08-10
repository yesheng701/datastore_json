#include "stdafx.h"
#include "source_file.h"
#include "md5.h"
#include "base/nkvd/nkvd.h"
#include "datastore_util.h"
#include "store_hashmap.h"

SourceFile* SourceFile::allocWithUrlAndDataDir(NcString* url,NcString* dataDir)
{
	SourceFile* o = NcNew(SourceFile);
	o->initWithUrlAndDataDir(url, dataDir);
	return o;
}

void SourceFile::setFileName(NcString* fileName)
{
	if (m_fileName != fileName)
	{
		release(m_fileName);
		m_fileName = retain(fileName);
	}
}

void SourceFile::setUrl(NcString* url)
{
	if (m_url != url)
	{
		release(m_url);
		m_url = retain(url);
	}
}

void SourceFile::setMd5(NcString* md5)
{
	if (m_md5 != md5)
	{
		release(m_md5);
		m_md5 = retain(md5);
	}
}

void SourceFile::setGuid(NcString* guid)
{
	if (m_guid != guid)
	{
		release(m_guid);
		m_guid = retain(guid);
	}
}

json_t* SourceFile::toJson()
{
	json_t* json = json_object();
	json_object_set_new(json, "url", json_string_S(m_url->cstr()));	
	json_object_set_new(json, "size", json_integer(m_size));
	if (m_guid != NULL)
		json_object_set_new(json, m_isGuid ? "guid" : "applyOnGuid", json_string_S(m_guid->cstr()));
	else
		json_object_set_new(json, "md5", json_string_S(m_md5->cstr()));
	return json;
}

SourceFile::SourceFile()
{
	m_isGuid = true;
}

SourceFile::~SourceFile()
{
	release(m_url);
	release(m_md5);
	release(m_guid);
	release(m_fileName);
}

void SourceFile::initWithUrlAndDataDir(NcString* url, NcString* dataDir)
{
	autoreleasepool
	{
		int length = dataDir->length();
		setUrl(NcString::stringWithCString(url->cstr() + length + 1));
		setFileName(DatastoreUtil::currentFileName(url));
	}

	FileObject file;
	File_construct(&file);

	if (!File_openSimpleFile(&file, url->cstr(), File_OpenForRead))
	{
		File_destruct(&file);
		return;
	}

	FileSys_getFileLength(url->cstr(), &m_size);
	File_destruct(&file);

	if (url->endsWith(L"nkvd") || url->endsWith(L"nkvds"))
	{
		setGuid(StoreHashmap::globleInstance()->guidByFilePath(url));
	}
	else if (!url->endsWith(L"diff"))
	{
		setMd5(StoreHashmap::globleInstance()->guidByFilePath(url));
	}
}
