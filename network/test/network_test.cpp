#include "stdafx.h"
#include "CppUnitTest.h"
#include <boost/asio/buffer.hpp>
#include "../component.h"
#pragma warning(disable:251)
#pragma comment(lib, "core")
#pragma comment(lib, "network")
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
    TEST_CLASS(NetworkTest) {
public:
    TEST_METHOD(DashManagerParseMpd) {
        auto manager = net::component::dash_manager::async_create_parsed("http://localhost:8900/dash/tos_srd_4K.mpd").get();
    }
    };
}