#include "stdafx.h"
#include "cq_stdlib.h"
#include "navi_core_env.h"
#include "test_command.h"
<<<<<<< HEAD
#include "index_command.h"
#include "diff_command.h"
=======
#include "json_creator_command.h"
>>>>>>> master

int printHelp()
{
	printf(R"(Create datastore json from navigation data.
%s version: %s
Compiled at %s %s

Usage:
datastore_json_creator [--version] [--help] [--changelog]
             <command> [<args>]

These are all arguments:
)", EXE_NAME, ControlUtil_versionFromChangelog(g_changelog), __DATE__, __TIME__);

	return 0;
}

int32 wmain(int32 argc, cqWCHAR** argvw)
{
	cq_logSetLevel(MapbarLogLevel_error);

	NaviCoreEnv env;

	ArgParser parser;
	parser.parse(argc, argvw);

	CliCommandCenter* commandCenter = CliCommandCenter::instance();
	commandCenter->addCommand(TestCommand::instance());
<<<<<<< HEAD
	commandCenter->addCommand(IndexCommand::instance());
	commandCenter->addCommand(DiffCommand::instance());
=======
	commandCenter->addCommand(JsonCreatorCommand::instance());
>>>>>>> master

	commandCenter->parseCommand(&parser);

	CliCommand* command = commandCenter->choosenCommand();
	if (command == NULL)
	{
		if (commandCenter->shouldPrintHelp())
		{
			printHelp();
			commandCenter->printCommandList();
		}
		else if (commandCenter->shouldPrintVersion())
		{
			printf(ControlUtil_versionFromChangelog(g_changelog));
			printf("\n");
		}
		else if (commandCenter->shouldPrintChangelog())
			printf(g_changelog);
		else
			return -1;

		return 0;
	}

	if (command == NULL)
	{
		return -1;
	}

	return command->exec();
}