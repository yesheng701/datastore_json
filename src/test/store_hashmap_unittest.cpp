#include "stdafx.h"
#include "store_hashmap.h"
#include "gtest/gtest.h"

TEST(StoreHashmap, basic)
{
	autoreleasepool
	{
		StoreHashmap* instance = StoreHashmap::globleInstance();
		instance->addFilesGuidFromDir(_S("."));
		NcString* guid = instance->guidByFilePath(_S("./data/1_full/cn/cn_jv.nkvds"));
		EXPECT_STREQ(guid->cstr(), L"cbafb23c-acf6-8644-a161-23b2eb7bf72f");
		StoreHashmap::globleInstance()->clear();
	}
}