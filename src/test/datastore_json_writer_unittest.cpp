#include "stdafx.h"
#include "datastore_json_writer.h"
#include "datastore_item_json_writer.h"
#include "data_tree_loader.h"
#include "datastore_util.h"
#include "store_hashmap.h"

class DatastoreJsonWriterTest : public ::testing::Test
{
public:
	static void SetUpTestCase()
	{
		StoreHashmap::globleInstance()->addFilesGuidFromDir(_S("."));
	}

	static void TearDownTestCase()
	{
		StoreHashmap::globleInstance()->clear();;
	}

	void SetUp()
	{
		m_pool = NcAutoreleasePool_alloc();
		m_loader = DataTreeLoader::allocWithDataDir(NULL);
		m_loader->_loadWorldManagerByName(_S("data/wmr3.json"));
		m_loader->addAdditionalNodesFromFile(L"store_patch.txt");

		m_writer = DatastoreItemJsonWriter::alloc();
		m_writer->setDataDir(_S("."));
		m_writer->setVersion(3);
		m_writer->initLocalDataManager(m_loader->root());

		m_o = DatastoreJsonWriter::alloc();
		m_o->setDataDir(_S("."));
		m_o->setVersion(3);
		m_o->createJsonByItemAndRoot(m_writer->items(), m_loader->root());
	}

	void TearDown()
	{
		release(m_o);
		release(m_writer);
		release(m_loader);
		NcAutoreleasePool_release(m_pool);
	}

protected:
	DataTreeLoader* m_loader;
	DatastoreItemJsonWriter* m_writer;
	DatastoreJsonWriter* m_o;

private:
	NcAutoreleasePool* m_pool;
};

TEST_F(DatastoreJsonWriterTest, initDatastoreJson)
{
	DatastoreJson* datastoreJson = m_o->datastoreJson();
	EXPECT_EQ(3, datastoreJson->currentVersion());
	
	DatastoreJsonItem* item = datastoreJson->items()->objectAtIndex(0);
	EXPECT_STREQ(L"基础数据", item->name()->cstr());
	EXPECT_STREQ(L"cn", item->directoryName()->cstr());
	EXPECT_STREQ(L"3:848,2:1694,3_full:9497", item->versionsAndSize()->cstr());
	EXPECT_STREQ(L"base.dat:f1290186a5d0b1ceab27f4e77c0c5d68,cn_jv.nkvds:2388ae2e-5128-1247-8b68-5c8a0a8cfb32,cn_map.nkvds:0cf2236a-360d-514c-8a55-92ff60db6ead,cn_poi.nkvds:8fef9b5c-78f8-5b42-b2df-2461bba5cfa8,cn_rp.nkvds:0abc8116-079e-8646-a291-0c6ba2cd003a", item->files()->cstr());
}

TEST_F(DatastoreJsonWriterTest, sortDatastoreJson)
{
	DatastoreJson* datastoreJson = m_o->datastoreJson();

	DatastoreJsonItem* item = datastoreJson->items()->objectAtIndex(1);
	EXPECT_STREQ(L"电子眼", item->name()->cstr());
	EXPECT_STREQ(L"cn/cmr", item->directoryName()->cstr());
	EXPECT_STREQ(L"3:128,2:256,3_full:2374", item->versionsAndSize()->cstr());
	EXPECT_STREQ(L"cmr.nkvds:567e5604-0fa0-164d-9786-93f0f7c66a19", item->files()->cstr());

	item = datastoreJson->items()->objectAtIndex(2);
	EXPECT_STREQ(L"北京市", item->name()->cstr());
	EXPECT_STREQ(L"cn/0beijing", item->directoryName()->cstr());
	EXPECT_STREQ(L"3:845,2:1687,3_full:9496", item->versionsAndSize()->cstr());
	EXPECT_STREQ(L"0beijing_jv.nkvds:2451a3fb-3374-424b-b941-aacc259c198f,0beijing_map.nkvds:f85b89da-6857-b64f-821c-5500173b5b71,0beijing_poi.nkvds:0ebeaa17-0258-3e45-90f9-3df0430f4f8e,0beijing_rp.nkvds:6e2522f4-9fbb-5f46-a9b9-08b5a8b6bfeb"
		, item->files()->cstr());
}