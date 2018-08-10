#include "stdafx.h"
#include "datastore_util.h"

TEST(DatastoreUtil, basic)
{
	EXPECT_STREQ(DatastoreUtil::currentDirectoryName(_S("cn"))->cstr(), L"cn");
	EXPECT_STREQ(DatastoreUtil::currentDirectoryName(_S("cn/cmr"))->cstr(), L"cmr");
	EXPECT_STREQ(DatastoreUtil::currentDirectoryName(_S("cn/hebei/0sjz"))->cstr(), L"0sjz");

	EXPECT_STREQ(DatastoreUtil::currentFileName(_S("cn/base.dat"))->cstr(), L"base.dat");
	EXPECT_STREQ(DatastoreUtil::currentFileName(_S(".\\cn/0beijing/0beijing_jv.nkvds"))->cstr(), L"0beijing_jv.nkvds");

	EXPECT_STREQ(DatastoreUtil::parentDirectoryName(_S("cn/hebei/0sjz"))->cstr(), L"cn/hebei");
	EXPECT_STREQ(DatastoreUtil::parentDirectoryName(_S("cn/0beijing"))->cstr(), L"cn");
	EXPECT_STREQ(DatastoreUtil::parentDirectoryName(_S("cn"))->cstr(), L"");

	EXPECT_EQ(DatastoreUtil::versionNumberFromString(_S("5_full")), 5);
	EXPECT_EQ(DatastoreUtil::versionNumberFromString(_S("full")), 0);
	EXPECT_EQ(DatastoreUtil::versionNumberFromString(_S("12ve//ds\\5_full")), 5);
}
