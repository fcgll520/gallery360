// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_PARALLEL_ALGORITHMS_EXPERIMENTAL_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS           
#include <WinSock2.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include "Core/pch.h"
#include "FFmpeg/pch.h"
#include "Gallery/pch.h"
#include "Monitor/pch.h"
#include "Core/base.h"
#include "Core/verify.hpp"
#include "Gallery/openvr.h"
#include "Gallery/interprocess.h"
#include <termcolor/termcolor.hpp>

#pragma comment(lib,"Core")
#pragma comment(lib,"FFmpeg")
#pragma comment(lib,"Gallery")
