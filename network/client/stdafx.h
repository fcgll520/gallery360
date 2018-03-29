// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <WinSock2.h>

#define CORE_USE_FMTLIB
#include "core/pch.h"
#include "multimedia/pch.h"
#include "network/pch.h"
#include "network/session.h"

#pragma comment(lib, "core")
#pragma comment(lib, "multimedia")
#pragma comment(lib, "network")