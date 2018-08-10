#include "stdafx.h"
#include "navi_core_env.h"

NaviCoreEnv::NaviCoreEnv()
{
	cq_logSetLevel(MapbarLogLevel_error);
	Stdlib_init();
	App_init();
}

NaviCoreEnv::~NaviCoreEnv()
{
	App_cleanup();
	Stdlib_cleanup();
}