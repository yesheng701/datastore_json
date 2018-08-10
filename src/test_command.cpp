#include "stdafx.h"
#include "test_command.h"
<<<<<<< HEAD
#include "gtest/gtest.h"
=======
>>>>>>> master

void TestCommand::printHelp()
{
	printf(R"(Create datastore json from navigation data.
%s version: %s
Compiled at %s %s

Usage:
datastore_json_creator [--version] [--help] [--changelog]
             <command> [<args>]

These are all arguments:
)", EXE_NAME, ControlUtil_versionFromChangelog(g_changelog), __DATE__, __TIME__);
<<<<<<< HEAD
=======

>>>>>>> master
}

int TestCommand::exec()
{
<<<<<<< HEAD
	int result = 0;

	int commandCount = -1;
	LPWSTR commandLine = GetCommandLineW();
	LPWSTR* commandArray = CommandLineToArgvW(commandLine, &commandCount);

	// command --test [parameter...]
	if (commandCount < 2 || commandArray == NULL)
		return -1;
	
	// command [parameter...]
	memmove(commandArray + 1, commandArray + 2, sizeof(LPWSTR) * (commandCount - 2));
	commandCount--;

	if (!FileSys_isDir(L"test_data"))
	{
		printError("Do not have test data!");
		return -1;
	}

	_chdir("test_data");

	::testing::InitGoogleTest(&commandCount, commandArray);
	result = RUN_ALL_TESTS();

	LocalFree(commandArray);

	return result;
}
=======
	LPWSTR* argList;
	int argNum;
	int result;

	argList = CommandLineToArgvW(GetCommandLineW(), &argNum);
	if (argList == NULL)
	{
		printError("CommandLineToArgvW failed\n");
		return -1;
	}

	if (argNum < 2)
	{
		printError("argNum(%d) < 2\n", argNum);
		return -1;
	}

	memmove(argList + 1, argList + 2, (argNum - 2) * sizeof(LPWSTR));
	--argNum;

	::testing::InitGoogleTest(&argNum, argList);
	result = RUN_ALL_TESTS();

	LocalFree(argList);

	return result;
}
>>>>>>> master
