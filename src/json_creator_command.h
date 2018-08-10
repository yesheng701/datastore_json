#pragma once

#include "cpputil/cli_command_center.h"

class JsonCreatorCommand : public CliCommand
{
public:
	static JsonCreatorCommand* alloc() { return NcNew(JsonCreatorCommand); }
	static JsonCreatorCommand* instance() { return autorelease(alloc()); }

	virtual const cqWCHAR* name() { return L"json creator"; }
	virtual const cqWCHAR* shortDescription() { return L"run jsoncreator"; }
	virtual void printHelp();
	virtual bool parseArgs(ArgParser* parser) { UNUSED_VAR(parser); return true; }
	virtual int exec();

protected:
	JsonCreatorCommand() {}
	~JsonCreatorCommand() {}
};