// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <WinSock2.h>

#include "core/pch.h"
#include "multimedia/pch.h"
#include "network/pch.h"

#pragma comment(lib, "core")
#pragma comment(lib, "multimedia")
#pragma comment(lib, "network")

