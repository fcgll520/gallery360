// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* argv[]) {
    const auto logger = spdlog::stdout_color_mt("app");
    try {
        const auto port = net::config_entry<uint16_t>("net.server.port");
        const auto directory = net::config_entry<std::string>("net.server.directories.root");
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
        auto worker_executor = core::make_threaded_executor("ServerWorker");
        worker_executor->add(
            [directory, &asio_pool, &socket_queue, &session_map, &logger] {
                auto socket = folly::SemiFuture<boost::asio::ip::tcp::socket>::makeEmpty();
                do {
                    socket_queue.dequeue(socket);
                    assert(socket.hasValue());
                    auto endpoint = socket.value().remote_endpoint();
                    auto session = net::server::session<net::protocal::http>::create(
                        std::move(socket).get(), *asio_pool, directory,
                        [&session_map, &logger, endpoint] {
                            const auto erase_count = session_map.erase(endpoint);
                            logger->warn("session with endpoint {} erased {}", endpoint, erase_count);
                        });
                    assert(endpoint == session->remote_endpoint());
                    auto [session_iter, success] = session_map.emplace(endpoint, std::move(session));
                    if (!success) {
                        logger->error("endpoint duplicate");
                        throw std::runtime_error{ "ConcurrentHashMap emplace fail" };
                    }
                    assert(endpoint == session_iter->first);
                    session_iter->second->wait_request();
                }
                while (true);
            });
        while (true) {
            logger->info("port {} listening", port);
            socket_queue.enqueue(acceptor.accept_socket()
                                         .wait());
            logger->info("socket accepted");
        }
    } catch (...) {
        logger->error("catch exception \n{}", boost::current_exception_diagnostic_information());
    }
    logger->info("application quit");
    return 0;
}
