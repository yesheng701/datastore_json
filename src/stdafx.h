#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "objbase.h"

#include "gtest/gtest.h"

#include "cq_stdlib.h"
#include "zlib/zlib.h"
#include "dalr/dalr.h"
#include "sqlite3/sqlite3.h"
#include "util/mem_pools.h"
#include "util/mvlc.h"
#include "util/bsdiff.h"
#include "util/byte_stream_reader.h"
#include "util/util.h"
#include "util/sqlite_util.h"
#include "util/sqlite_mem_table.h"
#include "util/python_test_util.h"

#include "cqstl/vector.h"
#include "cqstl/queue.h"
#include "nc_cpp.h"
#include "nc_string.h"

#include "cpputil/arg_parser.h"
#include "cpputil/console_util.h"
#include "cpputil/wmr_table.h"
#include "cpputil/admin_painter.h"
#include "cpputil/cli_command_center.h"

using namespace cqstd;

<<<<<<< HEAD
static const char* g_changelog = 
R"(
* 1.0.0 Initial version
=======
static const char* g_changelog = R"(
* 1.0   Initial version
>>>>>>> master
)";

#define EXE_NAME "datastore_json_creator"

// 0.1.0
const VersionNumber g_versionNumber = 0x01000000;