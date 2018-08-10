#pragma once

class DiffCreator : public NcObjectCpp
{
public:
	static DiffCreator* alloc() { return NcNew(DiffCreator); }
	static DiffCreator* instance() { return autorelease(alloc()); }

	void setIsForce(bool isForce) { m_isForce = isForce; }
	void setOutputDir(NcString* outputDir);
	void setFromDir(NcString* dataFullDir);
	void setToDir(NcString* nextDataFullDir);
	void setThreadNumber(int threadCount) { m_threadCount = threadCount; }

	bool generateDifferentialFiles();

protected:
	bool securityCheck();
	void nkvdDifferFilesWriter();

	DiffCreator() {}
	~DiffCreator();

private:
	bool m_isForce;
	NcString* m_outputDir;
	NcString* m_fromDir;
	NcString* m_toDir;
	int m_threadCount;
	int m_fromDirNum;
	int m_toDirNum;
};