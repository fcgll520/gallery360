// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gallery/openvr.h"
namespace process = boost::process;
namespace this_process = boost::this_process;
namespace filesystem = std::experimental::filesystem;

int main()
{

    try
    {
        using test = core::atomic_value<std::atomic<int>>;
        using test_future = core::future_value<std::future<void>>;
        auto xx2 = core::is_future<std::future<void>&>::value;
        auto xx = core::is_atomic<const std::atomic<const int>&&>::value;
    }
    catch (std::exception& e)
    {
        core::inspect_exception(e);
        return boost::exit_exception_failure;
    }
    catch (...)
    {
        fmt::print(std::cerr, "unstandard exception caught\n");
        return boost::exit_failure;
    }
    return boost::exit_success;
}

