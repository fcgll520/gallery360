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
#pragma warning(disable:4217 4049 4819 4244)

// TODO: add headers that you want to pre-compile here
#include "core/pch.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#endif //PCH_H

#ifdef _DEBUG
#pragma comment(lib,"gtestd")
#pragma comment(lib,"gtest_maind")
#pragma comment(lib,"gmockd")
#pragma comment(lib,"gmock_maind")
#else
#pragma comment(lib,"gtest")
#pragma comment(lib,"gtest_main")
#pragma comment(lib,"gmock")
#pragma comment(lib,"gmock_main")
#endif  // _DEBUG

#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"Shlwapi")

using testing::StrEq;
using testing::Not;
using testing::NotNull;
using testing::MatchesRegex;