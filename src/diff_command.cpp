#include "stdafx.h"
#include "diff_command.h"
#include "diff_creator.h"

void DiffCommand::printHelp()
{
	printf(R"(Generate differential files between two set of data files.

Syntax:
	dsadmin diff FROM_DIR TO_DIR[OPTIONS]

Options :
	--output - o OUT_DIR Specify output folder.If not given, OUT_DIR
	is deduced from TO_DIR.When TO_DIR is "4_full",
	the output dir is "4".
	--force - f         Delete existing output directory before generating new files.
	--thread - t N       Use N threads to perform to action.)");
}

bool DiffCommand::parseArgs(ArgParser* parser)
{
	size_t count = parser->getPositionalArgNumber();
	if (count < 2)
		return false;

	m_diffCreator->setFromDir(NcString::stringWithCString(parser->getPositionalArgByIndex(0)));
	m_diffCreator->setToDir(NcString::stringWithCString(parser->getPositionalArgByIndex(1)));

	if (parser->hasArg(L"f", L"force"))
	{
		m_diffCreator->setIsForce(true);
	}

	if (parser->hasArg(L"t", L"thread"))
	{
		const cqWCHAR* threadValue = parser->getArg(L"t", L"thread");
		if (threadValue == NULL)
			return false;
		m_diffCreator->setThreadNumber(cq_wtoi(threadValue));
	}

	if (parser->hasArg(L"o", L"output"))
	{
		const cqWCHAR* outputValue = parser->getArg(L"o", L"output");
		if (outputValue == NULL)
			return false;
		m_diffCreator->setOutputDir(NcString::stringWithCString(outputValue));
	}

	return true;
}

int DiffCommand::exec()
{
	TimeTick startTime = Mapbar_getTickCount();

	bool flag = m_diffCreator->generateDifferentialFiles();

	TimeTick endTime = Mapbar_getTickCount();
	printf("Done %.2fs\n", (endTime - startTime) / 1000.0);

	return flag ? 0 : -1;
}

DiffCommand::DiffCommand()
{
	m_diffCreator = DiffCreator::alloc();
}

DiffCommand::~DiffCommand()
{
	release(m_diffCreator);
}
