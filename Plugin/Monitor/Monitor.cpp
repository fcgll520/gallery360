// Monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
namespace filesystem = std::experimental::filesystem;

int main()
{
    {
        // std::optional<core::scope_guard> laji;
         //laji.emplace([i = 0]() mutable{ std::cout << ++i << std::endl; }, true);
         //ipc::channel ch{ 1s, false };
        //ipc::message msg{ vr::Compositor_FrameTiming{} };
        ipc::message msg{ 1.2 };
        auto x = msg.index();
        auto y=msg.visit([](auto& laji) {
            return 1;
        });
        auto z=msg.visit_as<double>([](double& d) {
            return d;
        });
        core::scope_guard laji;
        //core::scope_guard laji{ [] {fmt::print("hehe\n"); },true };
        laji = core::scope_guard{ [] {fmt::print("hehe\n"); } };
        auto dummy = 1;
        dummy = 2;
    }

    auto dummy2 = 2;

    return 0;
    
}