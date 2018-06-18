// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


namespace http = boost::beast::http;


int main(int argc, char* argv[])
{
    try
    {
        auto const port = net::config().get<uint16_t>("net.server.port");
        auto const directory = net::config().get<std::string>("net.server.directories.root");
        auto const io_context = std::make_shared<boost::asio::io_context>();
        auto const io_context_guard = boost::asio::make_work_guard(*io_context);
        boost::thread_group thread_group;
        auto const thread_group_guard = core::make_guard([&] { thread_group.join_all(); });
        std::vector<boost::thread*> threads(boost::thread::hardware_concurrency());
        std::generate_n(threads.begin(), threads.size(),
                        [&] { return thread_group.create_thread([io_context]() { net::run_io_context(*io_context); }); });
        boost::asio::ip::tcp::endpoint const endpoint{ boost::asio::ip::tcp::v4(),port };
        auto const acceptor = std::make_shared<net::server::acceptor<boost::asio::ip::tcp>>(endpoint, io_context);
        while (true)
        {
            fmt::print("server session waiting\n");
            auto const server_session = acceptor->listen_session<net::protocal::http>(directory, net::use_chunk);
            server_session->async_run();
            fmt::print("server session monitored\n");
        }
    } catch (...)
    {
        fmt::print(std::cerr, "main thread exception:\n{}\n", boost::current_exception_diagnostic_information());
    }
    fmt::print("application quit\n");
    return 0;
}