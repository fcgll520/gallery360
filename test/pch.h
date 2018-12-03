#pragma once
#define _WIN32_WINNT 0x0A00             // Windows 10  
#pragma warning(disable:4217 4049 4819 4244 4172)

// TODO: add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

#include <SDKDDKVer.h>

#include "core/pch.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <fstream>
#include <folly/logging/xlog.h>


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
using testing::StrNe;
using testing::StrCaseEq;
using testing::StrCaseNe;
using testing::Not;
using testing::NotNull;
using testing::MatchesRegex;
using testing::IsNotSubstring;
using testing::IsEmpty;
using testing::IsNull;
using testing::IsSubsetOf;
using testing::IsSupersetOf;
using testing::HasSubstr;
