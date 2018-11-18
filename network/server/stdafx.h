// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include "core/pch.h"
#include "network/pch.h"
#include "network/server.hpp"
#include "network/acceptor.hpp"

#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"Shlwapi")

#include <spdlog/sinks/stdout_color_sinks.h>

