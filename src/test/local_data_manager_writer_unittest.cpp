#include "stdafx.h"
#include "datastore_item_json_writer.h"
#include "data_tree_loader.h"
#include "store_hashmap.h"

class datastoreItemJsonWriterTest : public ::testing::Test
{
public:
	static void SetUpTestCase()
	{
		StoreHashmap::globleInstance()->addFilesGuidFromDir(_S("."));
	}

	static void TearDownTestCase()
	{
		StoreHashmap::globleInstance()->clear();
	}

	void SetUp()
	{
		m_pool = NcAutoreleasePool_alloc();
		m_o = DatastoreItemJsonWriter::alloc();
		m_o->setDataDir(_S("."));
	}

	void TearDown()
	{
		release(m_o);
		NcAutoreleasePool_release(m_pool);
	}

protected:
	DatastoreItemJsonWriter* m_o;

private:
	NcAutoreleasePool* m_pool;
};

TEST_F(datastoreItemJsonWriterTest, initLocalDataManager)
{
	DataTreeLoader* loader = DataTreeLoader::instanceWithDataDir(NULL);
	EXPECT_TRUE(loader->_loadWorldManagerByName(_S("data/wmr3.json")));
	EXPECT_TRUE(loader->addAdditionalNodesFromFile(L"store_patch.txt"));

	m_o->setVersion(3);
	m_o->initLocalDataManager(loader->root());

	DatastoreItem* cnData = m_o->items()->firstObject();
	EXPECT_EQ(cnData->localDatas()->count(), 5);
	EXPECT_STREQ(cnData->id()->cstr(), L"cn");

	LocalData* jvData = cnData->localDatas()->objectAtIndex(1);
	EXPECT_STREQ(jvData->localPath()->cstr(), L"cn/cn_jv.nkvds");
	EXPECT_EQ(jvData->sources()->count(), 3);

	SourceFile* sourceFile = jvData->sources()->firstObject();
	EXPECT_STREQ(L"data/3_full/cn/cn_jv.nkvds", sourceFile->url()->cstr());
	EXPECT_STREQ(L"2388ae2e-5128-1247-8b68-5c8a0a8cfb32", sourceFile->guid()->cstr());
}