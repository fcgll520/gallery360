// Plugin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gallery/interface.h"
#include "Core/guard.h"
namespace process = boost::process;
namespace this_process = boost::this_process;
namespace filesystem = std::experimental::filesystem;

const auto solution_path = filesystem::current_path().parent_path();



int main(int argc, char* argv[])
{
    const auto work_path = solution_path / "x64"
#ifndef NDEBUG
        / "Debug";
#else
        / "Release";
#endif  

    try
    {
        using namespace av;
        using namespace std::chrono;
        /*
        const std::string filepath = "C:/Media/AngelFallsVenezuela7680x3840.mkv";
        fmt::print("plugin launched\n");
        auto flag = true;
        auto count = 0;
        LaunchModules();
        flag &= ParseMedia(filepath.c_str());
        int w, h;
        INT64 num;
        LoadParamsVideo(w, h);
        auto frame = dll::ExtractFrame();
        while (!frame.empty())
        {
            ++count;

            if (count == 100)
            {
                Release();
                flag &= ParseMedia(filepath.c_str());
                frame = dll::ExtractFrame();
                break;
            }
            frame.unref();
            frame = dll::ExtractFrame();
        }
        fmt::print("count {}\n", count);
        Release();
        */
        tbb::concurrent_queue<int64_t> test;
        std::thread{[&]{
            auto mark = steady_clock::now() + seconds{10};
            while (steady_clock::now() < mark)
            {
                test.push(std::numeric_limits<int64_t>::max());
                std::this_thread::sleep_for(milliseconds{ 2 });
            }
        } }.detach();


        std::this_thread::sleep_for(seconds{ 2 });
        fmt::print("queue size {}\n", test.unsafe_size());        
        std::this_thread::sleep_for(seconds{ 2 });
        fmt::print("queue size {}\n", test.unsafe_size());
        auto now = steady_clock::now();
        test.clear();
        auto elapses = duration_cast<milliseconds>(steady_clock::now() - now).count();
        fmt::print("elapsed {}\n", elapses);
        std::this_thread::sleep_for(seconds{ 2 });
        fmt::print("queue size {}\n", test.unsafe_size());
        std::this_thread::sleep_for(seconds{ 2 });
        fmt::print("queue size {}\n", test.unsafe_size());
    }
    catch (const std::exception& e) {
        Release();
        return core::inspect_exception(e);
    }
    return boost::exit_success;

}

