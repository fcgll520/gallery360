// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/process.hpp>
namespace process = boost::process;
namespace this_process = boost::this_process;
namespace filesystem = std::experimental::filesystem;

namespace core
{

}

int main()
{
    auto szz = core::max_size<int, std::string, vr::Compositor_FrameTiming, vr::Compositor_CumulativeStats>::value;
    auto szz2 = core::max_size<std::variant<int, std::string, vr::Compositor_FrameTiming, vr::Compositor_CumulativeStats>>::value;
    auto szz3 = core::max_size<std::pair<std::string, std::string_view>, std::variant<int, std::string, vr::Compositor_FrameTiming, vr::Compositor_CumulativeStats>>::value;
    auto msgsz = szz + 128 - szz2 % 128;
    //ipc::channel ch(ipc::role::servant
    std::optional<interprocess::message_queue> mq;
    auto szzzzz = ipc::message::aligned_size();
    try
    {
        ipc::channel ch{};
        //mq.emplace(interprocess::open_only,"dddd" );
    }
    catch (...)
    {
        auto xxxx = mq.has_value();
        int xx = 1;
    }
    auto x = 1;
    
}
