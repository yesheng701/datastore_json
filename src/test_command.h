#pragma once

#include "cpputil/cli_command_center.h"

class TestCommand : public CliCommand
{
public:
	static TestCommand* alloc() { return NcNew(TestCommand); }
	static TestCommand* instance() { return autorelease(alloc()); }

	virtual const cqWCHAR* name() { return L"test"; }
	virtual const cqWCHAR* shortDescription() { return L"run all test cases"; }
	virtual void printHelp();
	virtual bool parseArgs(ArgParser* parser) { UNUSED_VAR(parser); return true; }
	virtual int exec();

protected:
	TestCommand() {}
	~TestCommand() {}
};