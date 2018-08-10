#pragma once

class StoreHashmap : public NcObjectCpp
{
public:
	static StoreHashmap* globleInstance();

	void addFilesGuidFromDir(NcString* path);

	NcString* guidByFilePath(NcString* filePath);

	void clear();

protected:
	StoreHashmap();
	~StoreHashmap();

private:
	NcHashmap<NcString, NcString>* m_guidMap;
};
