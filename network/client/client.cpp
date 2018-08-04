// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <folly/system/MemoryMapping.h>

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
        std::vector<boost::thread*> threads(boost::thread::hardware_concurrency() / 2);
        std::multimap<
            boost::asio::ip::tcp::endpoint,
            std::unique_ptr<net::client::session<net::protocal::http>>
        > sessions;
        boost::thread_group thread_group;
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::endpoint const endpoint{ boost::asio::ip::tcp::v4(), port };
        net::connector<boost::asio::ip::tcp> connector{ io_context };
        net::executor_guard executor_guard{ thread_group, io_context };
        std::generate_n(threads.begin(), threads.size(),
                        [&] { return thread_group.create_thread([&] { io_context.run(); }); });
        while (true)
        {
            fmt::print("app: client session waiting\n");
            std::string_view const host{ "www.techslides.com" };
            std::string_view const target{ "/demos/sample-videos/small.mp4" };
            if (auto session_ptr = connector.establish_session<net::protocal::http>(host, "80"); session_ptr != nullptr)
            {
                auto& session = *sessions.emplace(endpoint, std::move(session_ptr))->second;
                auto response = std::make_shared<boost::beast::http::response<
                    boost::beast::http::dynamic_body>>(session.async_send_request(host, target).get());
                media::io_context io_source{ response->body() };
                media::format_context format{ io_source };
                media::codec_context codec{ format,media::category::video{} };
                media::packet packet{ nullptr };
                size_t readCount = 0;
                size_t decodeCount = 0;
                while ((packet = format.read(media::category::video{})))
                {
                    ++readCount;
                    auto frames = codec.decode(packet);
                    decodeCount += frames.size();
                }
                break;
            }
            fmt::print("app: client session monitored\n\n");
        }
    }
    catch (std::exception const& exp)
    {
        core::inspect_exception(exp);
    }
    catch (boost::exception const& exp)
    {
        core::inspect_exception(exp);
    }
    fmt::print("app: application quit\n");
    return 0;
}
