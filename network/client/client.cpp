// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };
std::ofstream ofs{ "C:/Media/Recv/recv.h264",std::ios::binary | std::ios::trunc | std::ios::out };

int main(int argc, char* argv[])
{
    const auto io_context = std::make_shared<boost::asio::io_context>();
    net::client::session_pool session_pool{ io_context };
    try
    {
        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve("localhost", argv[1]));
        
//        std::cout << "Enter message: ";
//        char request[max_length];
//        std::cin.getline(request, max_length);
//        size_t request_length = std::strlen(request);
//        boost::asio::write(s, boost::asio::buffer(request, request_length));
        fmt::print("start receiving\n");
        //[[maybe_unused]] char reply[max_length];
        while(true)
        {
            boost::asio::streambuf streambuf;
            
            size_t reply_length = read_until(s, streambuf, "\r\n123");
            fmt::print("Reply size {}", reply_length);
            //std::copy(buffers_begin(streambuf.data()), buffers_end(streambuf.data()), std::ostreambuf_iterator<char>{ofs});  
            break;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}