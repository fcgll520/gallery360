// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
namespace process=boost::process;
namespace this_process=boost::this_process;
namespace filesystem=std::experimental::filesystem;


int main()
{

    auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::debug);
	console->debug("Monitor launched");
    fmt::print("laji");
    fmt::print("\bxxxxxx");
    std::string message;
    try
    {

    }
    catch(std::exception& e)
    {
        core::inspect_exception(e);
        return boost::exit_exception_failure;
    }
    catch(...)
    {
        fmt::print(std::cerr,"unstandard exception caught\n");
        return boost::exit_failure;
    }
    return boost::exit_success;
}

