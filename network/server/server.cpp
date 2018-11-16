// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <spdlog/sinks/stdout_color_sinks.h>

int main(int argc, char* argv[]) {
    const auto port = net::config_entry<uint16_t>({ "net", "server", "port" });
    const auto directory = net::config_entry<std::string>({ "net", "server", "directories", "root" });
    const auto logger = spdlog::stdout_color_mt("app");
    try {
        folly::ConcurrentHashMap<
            boost::asio::ip::tcp::endpoint,
            net::server::session_ptr<net::protocal::http>
        > session_map;
        auto asio_pool = net::make_asio_pool(std::thread::hardware_concurrency());
        net::server::acceptor<boost::asio::ip::tcp> acceptor{ port, *asio_pool };
        boost::asio::signal_set signals{ *asio_pool, SIGINT, SIGTERM };
        signals.async_wait([&](boost::system::error_code error,
                               int signal_count) {
            std::destroy_at(&asio_pool);
        });
        folly::USPSCQueue<
            folly::SemiFuture<boost::asio::ip::tcp::socket>, true
        > socket_queue;
        std::thread{
            [directory,&asio_pool,&socket_queue,&session_map,&logger] {
                auto socket = folly::SemiFuture<boost::asio::ip::tcp::socket>::makeEmpty();
                do {
                    socket_queue.dequeue(socket);
                    assert(socket.hasValue());
                    auto endpoint = socket.value().remote_endpoint();
                    auto session = net::server::session<net::protocal::http>::create(
                        std::move(socket).get(), *asio_pool, directory,
                        [&session_map,&logger,endpoint] {
                            const auto erase_count = session_map.erase(endpoint);
                            logger->warn("session with endpoint {} erased {}", endpoint, erase_count);
                        });
                    assert(endpoint == session->remote_endpoint());
                    auto [session_iter, success] = session_map.emplace(endpoint, std::move(session));
                    if (!success) {
                        logger->error("endpoint duplicate");
                        throw std::runtime_error{ "ConcurrentHashMap emplace fail" };
                    }
                    assert(session_iter->first == endpoint);
                    session_iter->second->wait_request();
                }
                while (true);
            }
        }.detach();
        while (true) {
            logger->info("server session socket waiting");
            socket_queue.enqueue(acceptor.accept_socket()
                                         .wait());
            logger->info("server session socket monitored");
        }
    } catch (...) {
        logger->error("catch exception");
        logger->error(boost::current_exception_diagnostic_information());
    }
    logger->info("application quit\n");
    return 0;
}
