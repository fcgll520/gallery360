// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

namespace
{
    auto const sample_url = "http://techslides.com/demos/sample-videos/small.mp4";
}

int main(int argc, char* argv[])
{
    try
    {
        auto const port = net::config().get<uint16_t>("net.client.port");
        auto const directory = net::config().get<std::string>("net.client.directories.root");
        std::vector<boost::thread*> threads(boost::thread::hardware_concurrency());
        std::multimap<
            boost::asio::ip::tcp::endpoint,
            std::unique_ptr<net::client::session<net::protocal::http>>
        > sessions;
        boost::thread_group thread_group;
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::endpoint const endpoint{ boost::asio::ip::tcp::v4(), port };

        net::executor_guard executor_guard{ thread_group, io_context };
        std::generate_n(threads.begin(), threads.size(), [&]
        {
            return thread_group.create_thread([&]
            {
                io_context.run();
            });
        });
        while (true)
        {
            fmt::print("app: client session waiting\n");

            fmt::print("app: client session monitored\n\n");
        }
    }
    catch (std::exception const& exp)
    {
        core::inspect_exception(exp);
    } catch (boost::exception const& exp)
    {
        core::inspect_exception(exp);
    }
    fmt::print("app: application quit\n");
    return 0;
}
