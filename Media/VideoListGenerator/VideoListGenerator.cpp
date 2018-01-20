// VideoListGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
namespace property_tree = boost::property_tree;
using namespace std::literals;
namespace filesystem=std::experimental::filesystem;
int main()
{
    filesystem::path root="C:/Media";
    filesystem::path sink=root/"list.xml";
    property_tree::ptree videos;
    try
    {

        core::verify(is_directory(root));
        remove_all(sink);
        for(auto& entry :filesystem::directory_iterator{root})
        {
            if(auto path=entry.path();!is_regular_file(path)||path.extension()!="xml")
            {
                fmt::print("entry {}\n",path.generic_string());
                videos.add("video",path.generic_string());
            }
        }
    }catch(const std::exception& e)
    {
        return core::inspect_exception(e);
    }
    write_xml(sink.generic_string(),videos);
    return EXIT_SUCCESS;
}

