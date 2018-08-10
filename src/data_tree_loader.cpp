#include "stdafx.h"
#include "data_tree_loader.h"
#include "datastore_util.h"
#include "datastore_patch.h"

bool DataTreeLoader::_loadWorldManagerByName(NcString* wmrFile)
{
	size_t fileLen;
	char* fileContent;

	fileContent = Util_readTextFileIntoBufferA(wmrFile->cstr(), &fileLen);
	if (fileContent == NULL)
		return false;
	
	json_t* root = json_loadb(fileContent, fileLen, 0, NULL);
	Util_freeFileInBuffer(fileContent);

	m_dataNodeRoot->fromJson(root);
	m_dataNodeRoot->setId(_S(""));
	json_decref(root);

	return true;
}

bool DataTreeLoader::addAdditionalNodesFromFile(const cqWCHAR* patchPath)
{
	DatastorePatch* datastorePatch = DatastorePatch::instance();
	if (datastorePatch->load(patchPath))
	{
		NcArray<DataNode>* patchNodes = datastorePatch->dataNodes();
		m_dataNodeRoot->subNodes()->addObjectsFromArray(patchNodes);
		return true;
	}
	return false;
}

bool DataTreeLoader::addAdditionalNodesFromFile(NcString* path)
{
	return addAdditionalNodesFromFile(path->cstr());
}

DataTreeLoader::DataTreeLoader(NcString* dataDir)
{
	m_dataDir = retain(dataDir);
	m_dataNodeRoot = DataNode::alloc();
	loadWorldManager();
}

DataTreeLoader::~DataTreeLoader()
{
	release(m_dataNodeRoot);
	release(m_dataDir);
}

bool DataTreeLoader::loadWorldManager()
{
	bool succ = false;

	if (m_dataDir == NULL)
		return false;

	FileSys_deleteFileIfExists(L"wmr.json");

	autoreleasepool
	{
		NcString* wmrPath = NcString::stringWithFormat(L"%@/cn/worldmanager3.nkvds", m_dataDir);
		NcString* cmd = NcString::stringWithFormat(L"wmrc convert %@ wmr.json", wmrPath);
		_wsystem(cmd->cstr());
		if (FileSys_isFile(L"wmr.json"))
		{
			_loadWorldManagerByName(_S("wmr.json"));
			succ = true;
		}
		else
		{
			CQ_LOG_ERROR("[dsadmin] %@ worldManger can not convert to json!", wmrPath);
		}
	}

	FileSys_deleteFileIfExists(L"wmr.json");

	return succ;
}
