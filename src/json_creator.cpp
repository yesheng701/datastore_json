#include "stdafx.h"
#include "json_creator.h"
#include "datastore_util.h"
#include "local_data.h"
#include "readme_txt.h"
#include "store_hashmap.h"

JsonCreator* g_instance = NULL;

static void _cleanupJsonCreator(void*)
{
	release(g_instance);
}

JsonCreator* JsonCreator::globalInstance()
{
	static volatile long lock = 0;
	if (g_instance == NULL)
	{
		spinlock(&lock)
		{
			if (g_instance == NULL)
			{
				cq_registerCleanupFunction(_cleanupJsonCreator, NULL);
				g_instance = NcNew(JsonCreator);
			}
		}
	}

	return g_instance;
}

bool JsonCreator::_createJson()
{
	int startVersion = m_isFull ? m_oldestVersion : m_latestVersion;

	for (int version = startVersion; version <= m_latestVersion; version++)
	{
		NcString* dataFull = NcString::stringWithFormat(L"%@/data/%d_full", m_dataDir, version);
		if (!FileSys_pathFileExists(dataFull->cstr()))
			continue;
		if (!initIndexCreateJson(version))
			return false;
	}
	return true;
}

bool JsonCreator::createJson()
{
	printf("start scan...\n");
	initVersion();
	if (m_latestVersion == 0)
		return false;

	printf("calculation guid and md5..\n");
	TimeTick begin = Mapbar_getTickCount();

	StoreHashmap::globleInstance()->addFilesGuidFromDir(NcString::stringWithFormat(L"%@/data", m_dataDir));

	TimeTick end = Mapbar_getTickCount();
	printf("guid and md5 end takes %fs\n", (1.0 * end - begin) / 1000);

	return _createJson();
}

JsonCreator::JsonCreator()
{
	m_sourceFileCache = NcHashmap<NcString, SourceFile>::alloc();
	m_oldestVersion = INT_MAX;
	m_latestVersion = INT_MIN;
}

JsonCreator::~JsonCreator()
{
	release(m_sourceFileCache);
	release(m_dataDir);
}

void JsonCreator::initVersion()
{
	int version = 0;

	FileSysWalker* walker = FileSysWalker::instanceWithPath(NcString::stringWithFormat(L"%@/data", m_dataDir));
	if (walker->next())
	{
		NcArray<NcString>* directories = walker->directories();
		for (int i = 0; i < directories->count(); i++)
		{
			version = directories->objectAtIndex(i)->toInt();
			m_latestVersion = cq_max(m_latestVersion, version);
			m_oldestVersion = cq_min(m_oldestVersion, version);
		}
	}
}

bool JsonCreator::initIndexCreateJson(int version)
{
	autoreleasepool
	{
		ReadmeTxt* readme = ReadmeTxt::globleInstance();
		readme->readFile(NcString::stringWithFormat(L"%@/data/%d_full/readme.txt", m_dataDir, version));

		NcString* versionDescriptionChs = readme->dataVersionChs();
		NcString* versionDescriptionEng = readme->dataVersionEng();
		if (versionDescriptionChs == NULL || versionDescriptionEng == NULL)
		{
			printError("Please confirm. \"readme.txt\" is in the \"cn\" directory.");
			return false;
		}

		if (!FileSys_pathFileExists(NcString::stringWithFormat(L"%@/data/%d_full/cn", m_dataDir, version)->cstr())
			&& !FileSys_pathFileExists(NcString::stringWithFormat(L"%@/data/%d_full/tts", m_dataDir, version)->cstr()))
		{
			printError("the directory format is not correct.");
			return false;
		}

		NcString* wmrPath = NcString::stringWithFormat(L"%@/data/%d_full", m_dataDir, version);
		DataTreeLoader* loader = DataTreeLoader::instanceWithDataDir(wmrPath);

		loader->addAdditionalNodesFromFile(NcString::stringWithFormat(L"%@/store_patch.txt", m_dataDir));

		ConsoleProgress_begin("version %d", version);

		DatastoreItemJsonWriter* itemJsonWriter = DatastoreItemJsonWriter::instance();
		itemJsonWriter->setDataDir(m_dataDir);
		itemJsonWriter->setVersion(version);
		itemJsonWriter->initLocalDataManager(loader->root());
		itemJsonWriter->writeJson();

		ConsoleProgress_end(true);

		DatastoreJsonWriter* datastoreJsonWriter = DatastoreJsonWriter::instance();
		datastoreJsonWriter->setDataDir(m_dataDir);
		datastoreJsonWriter->setVersion(version);
		datastoreJsonWriter->createJsonByItemAndRoot(itemJsonWriter->items(), loader->root());
		datastoreJsonWriter->writeJson();

		if (version == m_latestVersion)
		{
			datastoreJsonWriter->setVersion(-1);
			datastoreJsonWriter->setlLatestVersion(m_latestVersion);
			datastoreJsonWriter->writeJson();
		}
	}
	return true;
}

void JsonCreator::setDataDir(NcString* dir)
{
	if (!m_dataDir->equalsSafe(dir))
	{
		release(m_dataDir);
		m_dataDir = retain(dir);
	}
}
