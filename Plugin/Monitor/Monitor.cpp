// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/process.hpp>
#include "Gallery/openvr.h"
namespace process = boost::process;
namespace this_process = boost::this_process;
namespace filesystem = std::experimental::filesystem;
template<typename ...Types>
struct reverse_tuple
{
    using tuple = std::tuple<Types...>;
};

int main()
{
    enum task { init, parse, decode, last };
    auto xx = sizeof(vr::Compositor_CumulativeStats);
    try
    {
        auto xxxx = 60Ui64;
        task x = last;
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

