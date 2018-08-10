#pragma once

#include "cpputil/cli_command_center.h"
#include "diff_creator.h"

class DiffCommand : public CliCommand
{
public:
	static DiffCommand* alloc() { return NcNew(DiffCommand); }
	static DiffCommand* instance() { return autorelease(alloc()); }

	virtual const cqWCHAR* name() { return L"diff"; }
	virtual const cqWCHAR* shortDescription() { return L"run diff"; }
	virtual void printHelp();
	virtual bool parseArgs(ArgParser* parser);
	virtual int exec();

protected:
	DiffCommand();
	~DiffCommand();

private:
	DiffCreator* m_diffCreator;
};