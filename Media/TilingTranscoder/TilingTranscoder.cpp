// TilingTranscoder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
const filesystem::path root_dir{ "D:/Media" };
const filesystem::path tile_dir = root_dir / "tile";
//const std::vector<std::pair<int, int>> mesh_size{ {8,4},{16,8} };   //{width,height}
const std::vector<int> qp_table{ 10,20,30,40 };
const auto min_resolution{ 500 * 500 };
const std::string output_format = "mp4";
std::string decoder;
auto count_entry(const filesystem::path& directory)
{
    assert(is_directory(directory));
    const filesystem::directory_iterator iterator{ directory };
    return std::distance(begin(iterator), end(iterator));
}
struct info
{
    filesystem::path source;
    filesystem::path sink;
    int column;
    int row;
    int qp;
    int width;
    int height;
};
std::vector<info> info_table;
void clear_directory(filesystem::path file, std::vector<std::pair<int,int>> meshs,int width, int height)
{
    const auto file_dir = tile_dir / file.stem();
    for(const auto& pair: meshs)
    {
        fmt::print("\ttile {}*{}\n", pair.first, pair.second);
        const auto file_tile_path = file_dir / fmt::format("{}x{}", pair.first, pair.second);
        if (!is_directory(file_tile_path))
        {
            remove_all(file_tile_path);
            create_directories(file_tile_path);
        }
        for(const auto qp :qp_table)
        {
            const auto qp_path = file_tile_path / fmt::format("qp{}", qp);
            //output_dirs.push_back(qp_path);
            info_table.emplace_back(
                info{file,qp_path,pair.first,pair.second,qp,width,height}
            );
            if(!exists(qp_path))
            {
                create_directories(qp_path);
                continue;
            }
            if(count_entry(qp_path)!=pair.first*pair.second)
            {
                remove_all(qp_path);
                create_directories(qp_path);
            }
        }
    }
}
void launch_command()
{
    for(const auto& info :info_table)
    {
        const auto entry_num = count_entry(info.sink);
        if (entry_num == info.row*info.column && 
            file_size(*begin(filesystem::directory_iterator{ info.sink }))>256)
            continue;
        if (info.column == 32)
            continue;
        if(entry_num!=0)
        {
            remove_all(info.sink);
            create_directories(info.sink);
        }
        auto extension = info.source.extension();
        decoder = extension.compare(std::string{ ".mp4" }) == 0 ?
            "h264_cuvid" : "vp9_cuvid";
        auto cmd = fmt::format("ffmpeg -c:v {} -i {} ", decoder, info.source.generic_string());
        std::string filter = "-filter_complex ";
        std::string map;
        const auto tile_width = info.width / info.column;
        const auto tile_height = info.height / info.row;
        for(auto i=0;i!=info.row;++i)
        {
            for(auto j=0;j!=info.column;++j)
            {
                //const auto x_pos = j == 0 ? "0" : fmt::format("{}in_w/{}", j, info.column);
                //const auto y_pos = i == 0 ? "0" : fmt::format("{}in_h/{}", i, info.row);
                const auto last = i == info.row - 1 && j == info.column - 1;
                filter = fmt::format("{}[0:v]crop={}:{}:{}:{}[{}_{}]{}",
                    filter, tile_width, tile_height,
                    j*tile_width, i*tile_height, j, i,
                    last?"":";"
                );
                map = fmt::format("{} -map \"[{}_{}]\" -qp {} {}.r{}c{}.{}",
                    map, j, i, info.qp,
                    (info.sink/"tile").generic_string(),
                    i, j, output_format
                );
            }
        }
        cmd = fmt::format("{} {} {}", cmd, filter, map);
        process::system(cmd);  
    }
}
void launch_command32()
{
    for(const auto& info :info_table)
    {
        const auto entry_num = count_entry(info.sink);
        if (entry_num == info.row*info.column && 
            file_size(*begin(filesystem::directory_iterator{ info.sink }))>256)
            continue;
        if (info.column != 32)
            continue;
        if(entry_num!=0)
        {
            remove_all(info.sink);
            create_directories(info.sink);
        }
        auto extension = info.source.extension();
        decoder = extension.compare(std::string{ ".mp4" }) == 0 ?
            "h264_cuvid" : "vp9_cuvid";
        auto cmd = fmt::format("ffmpeg -c:v {} -i {} ", decoder, info.source.generic_string());
    	std::string filter = "-filter_complex ";
        std::string map;
		std::vector<decltype(cmd)> cmd_buffer{4,cmd};
		std::vector<decltype(filter)> filter_buffer{4,filter};
		std::vector<decltype(map)> map_buffer{4,map};
        const auto tile_width = info.width / info.column;
        const auto tile_height = info.height / info.row;
        for(auto i=0;i!=info.row;++i)
        {
			const auto idx=i/4;
            for(auto j=0;j!=info.column;++j)
            {
                //const auto x_pos = j == 0 ? "0" : fmt::format("{}in_w/{}", j, info.column);
                //const auto y_pos = i == 0 ? "0" : fmt::format("{}in_h/{}", i, info.row);
                const auto last = i == info.row - 1 && j == info.column - 1;
                filter_buffer[idx] = fmt::format("{}[0:v]crop={}:{}:{}:{}[{}_{}]{}",
                    filter_buffer[idx], tile_width, tile_height,
                    j*tile_width, i*tile_height, j, i,
                    last?"":";"
                );
                map_buffer[idx] = fmt::format("{} -map \"[{}_{}]\" -qp {} {}.r{}c{}.{}",
                    map_buffer[idx], j, i, info.qp,
                    (info.sink/"tile").generic_string(),
                    i, j, output_format
                );
            }
			if((i+1)%4==0)
			{
				if(filter_buffer[idx].back()==';')
				{
					filter_buffer[idx].back()=' ';
				}
			}
        }
		for(auto i=0;i!=cmd_buffer.size();++i)
		{
			const auto command=fmt::format("{} {} {}", 
				cmd_buffer[i],filter_buffer[i],map_buffer[i]);
			process::system(command);  
		} 
    }
}

int main(int argc, char* argv[])
{
    /*
    av::tile::transcode executor{ "C:/Media/NewYork.mp4",{2,1} };
    executor.run();
    */
    av_register_all();    
    auto executor = boost::process::search_path("ffmpeg");
    assert(is_regular_file(executor));
    assert(is_directory(root_dir));
    if (!is_directory(tile_dir))
        create_directory(tile_dir);
    for(const auto& entry : filesystem::directory_iterator{root_dir})
    {
        if (!is_regular_file(entry))
            continue;
        const auto file = entry.path().generic_string(); 
        const auto file_name = entry.path().stem();
        const auto file_dir = tile_dir / file_name;
        auto format = avformat_alloc_context();
        avformat_open_input(&format, file.c_str(), nullptr, nullptr);
        avformat_find_stream_info(format, nullptr);
        AVCodec* codec{ nullptr };
	    const auto video_index = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
        codec = avcodec_find_decoder_by_name(
            fmt::format("{}_cuvid", codec->name).c_str());
        decoder = codec->name;
	    const auto video_stream = format->streams[video_index];
        const auto width = video_stream->codecpar->width;
        const auto height = video_stream->codecpar->height;
        fmt::print("filename {}\n", file_name);
        fmt::print("\twidth{}, height{}\n", width, height);
        std::vector<std::pair<int, int>> meshs{ {8,4} };
        
        if (width*height>3000*1500)
        {
            meshs.push_back({ 16,8 });
            if (width*height > 6000 * 3000)
                meshs.push_back({ 32,16 });
        }
        clear_directory(entry.path(), meshs, width, height);

        //break;
    }
    launch_command32();
}
