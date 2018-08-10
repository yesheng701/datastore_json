#pragma once

#include "data_tree_loader.h"
#include "datastore_json_writer.h"
#include "datastore_item_json_writer.h"

class JsonCreator : public NcObjectCpp
{
public:
	static JsonCreator* globalInstance();

	bool createJson();
	void setIsFull(bool isFull) { m_isFull = isFull; }

	NC_PROPERTY(dataDir);
	void setDataDir(NcString* dir);

	void addSourceFileCache(NcString* key, SourceFile* value) { m_sourceFileCache->addObjectForKey(key, value); }
	SourceFile* getSourceFile(NcString* key) { return m_sourceFileCache->objectWithKey(key); }

	NC_PROPERTY(latestVersion);
	int getLatestVersion() { return m_latestVersion; }

	NC_PROPERTY(oldestVersion);
	int getOldestVersion() { return m_oldestVersion; }

protected:
	JsonCreator();
	~JsonCreator();
	bool _createJson();

private:
	void initVersion();
	bool initIndexCreateJson(int index);

private:
	NcString* m_dataDir;
	int m_latestVersion;
	int m_oldestVersion;
	bool m_isFull;
	NcHashmap<NcString, SourceFile>* m_sourceFileCache;
};