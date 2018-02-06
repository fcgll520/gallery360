// Plugin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gallery/interface.h"
#include "Core/guard.h"
namespace filesystem = std::experimental::filesystem;

const auto solution_path = filesystem::current_path().parent_path();



int main(int argc, char* argv[])
{
    
    ipc::message msg{ vr::Compositor_CumulativeStats{} };
    auto sssz = msg.valid_size();
    auto aa = std::thread{ []() mutable{
        auto time_mark = std::chrono::high_resolution_clock::now();
        ipc::channel send_ch{ false };
        auto count = -1;
        while (++count != 100) {
            fmt::print("!sending {} message!\n", count);
            auto duration = std::chrono::high_resolution_clock::now() - time_mark;
            send_ch.async_send(vr::Compositor_CumulativeStats{}, duration);
        }
        fmt::print("!!sending finish!!\n");
        std::this_thread::sleep_for(1h);
        fmt::print("!!sending finish future!!\n");
    } };
    aa.join();
}

