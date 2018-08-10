#include "stdafx.h"
#include "index_command.h"
#include "base/base.h"
#include "datastore_util.h"

void IndexCommand::printHelp()
{
	printf(R"(Create datastore index files (datastore.json, data/1_json/cn/0beijing/0beijing.json).

Syntax:
   dsadmin create_index DATA_DIR [OPTIONS]

   DATA_DIR  The root directory which contains "data/1", "data/1_full" ...

OPTIONS:
   --full     create index file for all versions of data. 
             If not specified, only create index file for the latest data.
)");
}

bool IndexCommand::parseArgs(ArgParser* parser)
{
	size_t count = parser->getPositionalArgNumber();
	if (count < 1)
	{
		return false;
	}
	NcString* dir = NcString::allocWithCString(parser->getPositionalArgByIndex(0));
	JsonCreator::globalInstance()->setDataDir(dir);
	release(dir);
	if (parser->hasArg(L"full", L"f"))
	{
		JsonCreator::globalInstance()->setIsFull(true);
	}
	return true;
}

int IndexCommand::exec()
{
	TimeTick startTime = Mapbar_getTickCount();
	
	bool flag = JsonCreator::globalInstance()->createJson();

	TimeTick endTime = Mapbar_getTickCount();
	printf("Done %.2fs\n", (endTime - startTime) / 1000.0);

	return flag ? 0 : -1;
}

IndexCommand::IndexCommand()
{
}

IndexCommand::~IndexCommand()
{
}
