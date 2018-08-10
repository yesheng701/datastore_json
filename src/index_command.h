#pragma once

#include "cpputil/cli_command_center.h"
#include "json_creator.h"

class IndexCommand : public CliCommand
{
public:
	static IndexCommand* alloc() { return NcNew(IndexCommand); }
	static IndexCommand* instance() { return autorelease(alloc()); }

	virtual const cqWCHAR* name() { return L"index"; }
	virtual const cqWCHAR* shortDescription() { return L"run index"; }
	virtual void printHelp();
	virtual bool parseArgs(ArgParser* parser);
	virtual int exec();

protected:
	IndexCommand();
	~IndexCommand();
};