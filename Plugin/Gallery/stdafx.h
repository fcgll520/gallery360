// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN 
//#undef UNICODE
// Windows Header Files:
//#include <WinSock2.h>
#include <windows.h>

// TODO: reference additional headers your program requires here
#include "Core/pch.h"
#include "FFmpeg/pch.h"
#include "FFmpeg/base.h"
#include "FFmpeg/context.h"
#include "Core/base.h"
#include "Core/verify.hpp"
#include <d3d11.h>
#include "Unity/IUnityGraphicsD3D11.h"
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityInterface.h"
#include "Gallery/pch.h"
#include "Gallery/graphic.h"
#include "Gallery/interprocess.h"
namespace filesystem = std::experimental::filesystem;

#pragma comment(lib,"Core")
#pragma comment(lib,"FFmpeg")
