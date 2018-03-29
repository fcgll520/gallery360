// monitor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define CPPCONN_PUBLIC_FUNC 
#include <iostream>
#include "gallery/interface.h"

#pragma warning(disable: 5040)
#pragma comment(lib,"mysqlcppconn")



using namespace std;
namespace av
{
    struct command
    {
        static void thumbnail(const filesystem::path input, filesystem::path output_dir = {},
            const std::tuple<unsigned, unsigned, double> seek_time = { 0,0,10 })
        {
            std::stringstream ss;
            if (output_dir.empty())
                output_dir = input.parent_path() / "thumbnail";
            if (!is_directory(output_dir))
                create_directories(output_dir);
            ss << "ffmpeg -i " << input << " -ss " << std::get<0>(seek_time) << ":" << std::get<1>(seek_time) << ":" << std::get<2>(seek_time)
                << " -frames:v 1 " << (output_dir / input.filename()).replace_extension("png").generic_string() << " -y";
            std::system(ss.str().c_str());
        }
    };

}

namespace av
{

}
int main(void)
{
    //const auto session = std::make_shared<dll::media_session>("C:/Media/MercedesBenz4096x2048.mp4"s);

    size_t hash_id = 0;
    unity::_nativeMediaCreate();
    hash_id = unity::_nativeMediaSessionCreate("C:/Media/NewYork.mp4");
    auto frame_count = unity::debug::_nativeMediaSessionGetFrameCount(hash_id);
    uint64_t decode_count = 0;
    while (unity::_nativeMediaSessionHasNextFrame(hash_id))
    {
        const auto res = unity::debug::_nativeMediaSessionDropFrame(hash_id, 1);
        std::cout << "decode count" << (decode_count += res) << "\n";
        if (decode_count > 600)
        {
            unity::_nativeMediaSessionRelease(hash_id);
            hash_id = unity::_nativeMediaSessionCreate("C:/Media/MercedesBenz4096x2048.mp4");
            decode_count = 0;
            int dummy = 1;
        }
    }
    unity::_nativeMediaSessionRelease(hash_id);
    unity::_nativeMediaRelease();
    return EXIT_SUCCESS;
}