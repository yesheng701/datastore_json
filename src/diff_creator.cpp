#include "stdafx.h"
#include "diff_creator.h"
#include "base/nkvd/nkvd.h"
#include "base/nkvd/nkvd_differ.h"
#include "datastore_util.h"
#include "cpputil/concurrent_task_runner.h"

static int g_count = 0;
static int g_totalDiffer = 0;
static volatile long g_spinlock = 0;

class GenerateDifferFileTask : public ConcurrentTask
{
	typedef ConcurrentTask super;

public:
	static GenerateDifferFileTask* instanceWithDiffer(NkvdDiffer* differ)
	{
		GenerateDifferFileTask* o = NcNew(GenerateDifferFileTask);
		o->m_differ = retain(differ);
		return autorelease(o);
	}

	virtual bool run()
	{
		if (!m_differ->diff())
			return false;
		spinlock(&g_spinlock)
		{
			++g_count;
		}
		static int printLen = 0;
		for (int k = 0; k < printLen; ++k)
		{
			printf("\b \b");
		}
		ConsoleProgress_print(g_count, g_totalDiffer);
		autoreleasepool
		{
			NcString* printInfo = DatastoreUtil::currentFileName(m_differ->fromFileName());
			wprintf(printInfo->cstr());
			printLen = printInfo->length();
		}

		return true;
	}

protected:
	GenerateDifferFileTask() {}
	~GenerateDifferFileTask()
	{
		release(m_differ);
	}

private:
	NkvdDiffer* m_differ;
};

void DiffCreator::setOutputDir(NcString* outputDir)
{
	if (m_outputDir != outputDir)
	{
		release(m_outputDir);
		m_outputDir = retain(outputDir);
	}
}

void DiffCreator::setFromDir(NcString* dataFullDir)
{
	if (m_fromDir != dataFullDir)
	{
		release(m_fromDir);
		m_fromDir = retain(dataFullDir);
	}
}

void DiffCreator::setToDir(NcString* nextDataFullDir)
{
	if (m_toDir != nextDataFullDir)
	{
		release(m_toDir);
		m_toDir = retain(nextDataFullDir);
	}
}

bool DiffCreator::generateDifferentialFiles()
{
	if (!securityCheck())
		return false;

	nkvdDifferFilesWriter();

	return true;
}

bool DiffCreator::securityCheck()
{
	autoreleasepool
	{
		if (m_fromDir == NULL || m_toDir == NULL)
			return false;

		DatastoreUtil::currentDirectoryName(m_fromDir);
		int fromDirNum = DatastoreUtil::versionNumberFromString(m_fromDir);
		int toDirNum = DatastoreUtil::versionNumberFromString(m_toDir);
		if (fromDirNum == 0 || toDirNum == 0 || toDirNum != fromDirNum + 1)
		{
			return false;
		}
		m_fromDirNum = fromDirNum;
		m_toDirNum = toDirNum;

		if (m_outputDir == NULL)
		{
			NcString* parentName = DatastoreUtil::parentDirectoryName(m_toDir);
			setOutputDir(NcString::stringWithFormat(L"%@/%d", parentName, toDirNum));
		}

		if (!m_isForce && FileSys_pathFileExists(m_outputDir->cstr()))
		{
			printError("output dir \"%d\" already exists. Use -f to overwrite it.", toDirNum);
			return false;
		}
		else if (FileSys_pathFileExists(m_outputDir->cstr()))
		{
			FileSys_removeDirTreeRestricted(m_outputDir->cstr(), FALSE);
		}
	}
	return true;
}

void DiffCreator::nkvdDifferFilesWriter()
{
	autoreleasepool
	{
		ConsoleProgress_begin("diff make");
		NcArray<ConcurrentTask>* tasks = NcArray<ConcurrentTask>::instance();
		FileSysWalker* walker = FileSysWalker::instanceWithPath(m_toDir);
		while (walker->next())
		{
			NcString* currDirectoryName = walker->currentDirectory();
			cqWCHAR* id = cq_wcsstr(currDirectoryName->cstr(), L"cn");

			if (id == NULL)
				continue;

			NcString* fromDir = NcString::stringWithFormat(L"%@/%s", m_fromDir, id);
			NcString* toDir = NcString::stringWithFormat(L"%@/%s", m_toDir, id);
			if (!FileSys_pathFileExists(fromDir->cstr()) || !FileSys_pathFileExists(toDir->cstr()))
				continue;

			NcArray<NcString>* files = walker->files();
			for (int i = 0; i < files->count(); i++)
			{
				NcString* fileName = files->objectAtIndex(i);
				NcString* fromFileName = NcString::stringWithFormat(L"%@/%@", fromDir, fileName);
				NcString* toFileName = NcString::stringWithFormat(L"%@/%@", toDir, fileName);
				NcString* outputFileName = NcString::stringWithFormat(L"%@/%s/%@.%d.diff", m_outputDir, id, fileName, m_toDirNum);

				if (!fileName->endsWith(L"nkvds"))
					continue;

				if (!FileSys_pathFileExists(fromFileName->cstr()) || !FileSys_pathFileExists(toFileName->cstr()))
					continue;

				NcString* createDeepDir = NcString::stringWithFormat(L"%@/%s", m_outputDir, id);
				FileSys_createDeepDir(createDeepDir->cstr());

				NkvdDiffer* differ = NkvdDiffer::instance();
				differ->setFromFileName(fromFileName);
				differ->setToFileName(toFileName);
				differ->setOutputFileName(outputFileName);
				tasks->addObject(GenerateDifferFileTask::instanceWithDiffer(differ));
			}
		}

		g_totalDiffer = tasks->count();
		m_threadCount = m_threadCount > 0 ? (m_threadCount > g_totalDiffer ? g_totalDiffer : m_threadCount) : 1;

		ConcurrentTaskRunner* runner = ConcurrentTaskRunner::instanceWithTasks(tasks);
		runner->setThreadNumber(m_threadCount);
		runner->runTasks();
		runner->waitUntilDone();
		ConsoleProgress_end("diff make done");
	}
}

DiffCreator::~DiffCreator()
{
	release(m_outputDir);
	release(m_fromDir);
	release(m_toDir);
}