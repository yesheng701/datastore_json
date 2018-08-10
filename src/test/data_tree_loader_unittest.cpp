#include "stdafx.h"
#include "data_tree_loader.h"

class DataTreeLoaderTest : public ::testing::Test
{
public:
	static void SetUpTestCase() {}
	static void TearDownTestCase() {}

	void SetUp()
	{
		m_pool = NcAutoreleasePool_alloc();
		m_o = DataTreeLoader::allocWithDataDir(NULL);
		m_o->_loadWorldManagerByName(_S("data/wmr1.json"));
	}

	void TearDown()
	{
		release(m_o);
		NcAutoreleasePool_release(m_pool);
	}

protected:
	DataTreeLoader* m_o;

private:
	NcAutoreleasePool* m_pool;
};

TEST_F(DataTreeLoaderTest, getDataNodeRoot)
{
	DataNode* root = m_o->root();

	EXPECT_STREQ(root->name()->cstr(), L"中国");
	EXPECT_STREQ(root->id()->cstr(), L"");
}

TEST_F(DataTreeLoaderTest, addAdditionalNodesFromFile)
{
	m_o->addAdditionalNodesFromFile(L"store_patch.txt");

	DataNode* root = m_o->root();
	DataNode* item = root->nodeById(_S("cn/cmr"));

	ASSERT_TRUE(item != NULL);
	EXPECT_STREQ(item->name()->cstr(), L"电子眼");
	EXPECT_STREQ(item->id()->cstr(), L"cn/cmr");
}

TEST_F(DataTreeLoaderTest, load)
{
	ASSERT_TRUE(m_o->_loadWorldManagerByName(_S("data/wmr3.json")));
	DataNode* root = m_o->root();
	DataNode* item = root->nodeById(_S("cn/hebei"));
	ASSERT_TRUE(item != NULL);
	EXPECT_STREQ(item->name()->cstr(), L"河北省");
	EXPECT_STREQ(item->id()->cstr(), L"cn/hebei");

	item = root->nodeById(_S("cn/hebei/9lfs"));
	ASSERT_TRUE(item != NULL);
	EXPECT_STREQ(item->name()->cstr(), L"廊坊市");
	EXPECT_STREQ(item->id()->cstr(), L"cn/hebei/9lfs");
}