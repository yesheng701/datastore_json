#include "stdafx.h"
#include "readme_txt.h"
#include "gtest/gtest.h"

TEST(ReadmeTxt, basic)
{
	autoreleasepool
	{
		ReadmeTxt* readmeTxtInstance = ReadmeTxt::globleInstance();
		readmeTxtInstance->readFile(_S("cn_readme.txt"));

		EXPECT_STREQ(readmeTxtInstance->dataVersionChs()->cstr(), L"2017´º¼¾ÔöÇ¿°æ");
		EXPECT_STREQ(readmeTxtInstance->dataVersionEng()->cstr(), L"2017 Spring");
	}
}