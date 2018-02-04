// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include "Core/pch.h"
#include "FFmpeg/pch.h"
#include "Gallery/pch.h"
#include "Monitor/pch.h"
#include "Core/base.hpp"
#include "Core/verify.hpp"
#include "Gallery/openvr.h"
#include "Gallery/interprocess.h"
#pragma comment(lib,"Core")
#pragma comment(lib,"FFmpeg")
#pragma comment(lib,"Gallery")
