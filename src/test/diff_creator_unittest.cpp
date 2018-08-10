#include "stdafx.h"
#include "diff_creator.h"
#include "cpputil/concurrent_task_runner.h"

class DiffCreatorTest : public ::testing::Test
{
public:
	static void SetUpTestCase() {}
	static void TearDownTestCase() {}

	void SetUp()
	{
		m_pool = NcAutoreleasePool_alloc();
		m_o = DiffCreator::alloc();
	}

	void TearDown()
	{
		release(m_o);
		NcAutoreleasePool_release(m_pool);
	}

protected:
	DiffCreator* m_o;

private:
	NcAutoreleasePool* m_pool;
};

TEST_F(DiffCreatorTest, generateDifferentialFiles)
{
	m_o->setFromDir(_S("data/2_full"));
	m_o->setToDir(_S("data/3_full"));
	m_o->setIsForce(true);
	m_o->setThreadNumber(3);
	EXPECT_TRUE(m_o->generateDifferentialFiles());
}