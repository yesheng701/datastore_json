#pragma once

#include "jansson/jansson.h"

class SourceFile : public NcObjectCpp
{
public:
	static SourceFile* alloc() { return NcNew(SourceFile); }
	static SourceFile* allocWithUrlAndDataDir(NcString* url, NcString* dataDir);
	static SourceFile* instance() { return autorelease(alloc()); }
	static SourceFile* instanceWithUrlAndDataDir(NcString* url, NcString* dataDir) { return autorelease(allocWithUrlAndDataDir(url, dataDir)); }

	NC_PROPERTY(version);
	void setVersion(int version) { m_version = version; }
	int version() { return m_version; }

	NC_PROPERTY(fileName);
	void setFileName(NcString* fileName);
	NcString* fileName() { return m_fileName; }

	NC_PROPERTY(url);
	void setUrl(NcString* url);
	NcString* url() { return m_url; }

	NC_PROPERTY(md5);
	void setMd5(NcString* md5);
	NcString* md5() { return m_md5; }

	NC_PROPERTY(size);
	void setSize(uint32 size) { m_size = size; }
	uint32 size() { return m_size; }

	NC_PROPERTY(guid);
	void setGuid(NcString* guid);
	NcString* guid() { return m_guid; }

	NC_PROPERTY(isGuid);
	void setIsGuid(bool isGuid) { m_isGuid = isGuid; }
	bool isGuid() { return m_isGuid; }

	json_t* toJson();

protected:
	SourceFile();
	~SourceFile();
	void initWithUrlAndDataDir(NcString* url, NcString* dataDir);

private:
	NcString* m_fileName;
	int m_version;
	NcString* m_url; // url
	NcString* m_md5;
	uint32 m_size;
	NcString* m_guid;
	bool m_isGuid;
};