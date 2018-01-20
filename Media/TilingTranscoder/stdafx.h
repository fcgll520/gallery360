// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
#include "Core/precompiled.h"
#include "LibAv/precompiled.h"
#include "Core/base.hpp"
#include "LibAv/context.h"
#include "LibAv/scale.h"
#include "LibAv/origin.h"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace process = boost::process;
namespace this_process = boost::this_process;

#pragma comment(lib,"Core")
#pragma comment(lib,"LibAv")