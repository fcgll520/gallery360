// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#define _WIN32_WINNT 0x0A00             // Windows 10  

// TODO: add headers that you want to pre-compile here
#include "core/pch.h"
#include "gtest/gtest.h"

#endif //PCH_H

#pragma warning(disable:4217 4049)
#ifdef _DEBUG
#pragma comment(lib,"gtestd")
#pragma comment(lib,"gtest_maind")
#else   // _DEBUG 
#pragma comment(lib,"gtest")
#pragma comment(lib,"gtest_main")
#endif  // _DEBUG

#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"Shlwapi")
