// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
namespace process=boost::process;
namespace this_process=boost::this_process;
namespace filesystem=std::experimental::filesystem;

int main()
{
    auto arr2 = core::range<-5,7>();
    std::string message;
    fmt::MemoryWriter mem;
    std::stringstream ss;
    boost::archive::binary_oarchive oar{ ss };
    oar << "123" << 456;
    //boost::archive::binary_oarchive oar2{ ss };
    oar << "123" << 456;
    boost::archive::binary_iarchive iar{ ss };
    std::string ret;
    int ret2;
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

