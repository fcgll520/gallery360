// Plugin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gallery/interface.h"
namespace process=boost::process;
namespace this_process=boost::this_process;
namespace filesystem=std::experimental::filesystem;

const auto solution_path=filesystem::current_path().parent_path();



int main(int argc, char* argv[])
{
    const auto work_path=solution_path/"x64"
#ifndef NDEBUG
    /"Debug";
#else
    /"Release";
#endif  
    try
    {

        
        using namespace av;
        using namespace std::chrono;
        using test=std::int64_t;
        const std::string filepath="C:/Media/AngelFallsVenezuela7680x3840.mkv";

        fmt::print("plugin launched\n");
        auto flag=true;
        auto count=0;
        LaunchModules();
        flag&= ParseMedia(filepath.c_str());
        int w,h;
        INT64 num;
        InfoPlayParamsVideo(w,h);
        auto frame=dll::ExtractFrame();
        while (!frame.empty())
        {
            ++count;
            
            if(count==100)
            {
                Release();
                flag&= ParseMedia(filepath.c_str());
                frame=dll::ExtractFrame();   
                break;
            }
            
            frame.unref();
            frame=dll::ExtractFrame();
        }           
        fmt::print("count {}\n",count);
        Release();
    }        
    catch (const std::exception& e) {
        Release();
        return core::inspect_exception(e);
    }
    return boost::exit_success;

}

