#pragma once

#include "source_file.h"

// dataItem
class LocalData : public NcObjectCpp
{
public:
	static LocalData* alloc() { return NcNew(LocalData); }
	static LocalData* allocWithLocalPathAndVersion(NcString* localPath, int version);
	static LocalData* instance() { return autorelease(alloc()); }
	static LocalData* instanceWithLocalPathAndVersion(NcString* localPath, int version) { return autorelease(allocWithLocalPathAndVersion(localPath, version)); }

	NC_PROPERTY(localPath);
	void setLocalPath(NcString* localPath);
	NcString* localPath() { return m_localPath; }

	NC_PROPERTY(version);
	void setVersion(int version) { m_version = version; }
	int version() { return m_version; }

	NC_PROPERTY(dataDir);
	void setDataDir(NcString* dir);

	json_t* toJson();

	NC_PROPERTY(sources);
	void addSourceFile(SourceFile* sourceFile);
	NcArray<SourceFile>* sources() { return m_sources; }

	void initSourceFiles(NcString* filePath);

protected:
	LocalData();
	~LocalData();

private:
	NcString* m_localPath;
	int m_version;
	NcArray<SourceFile>* m_sources;
	NcString* m_dataDir;
};