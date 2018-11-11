#pragma once

namespace net::server
{
    using default_policy = policy<string_body>;

    template<typename Protocal>
    class session;

    template<typename Protocal>
    using session_ptr = std::unique_ptr<session<Protocal>>;
    using http_session = session<protocal::http>;
    using http_session_ptr = std::unique_ptr<session<protocal::http>>;

    template<>
    class session<protocal::http> : detail::session_base<boost::asio::ip::tcp::socket, flat_buffer>,
                                    protocal::http::protocal_base
    {
        const int64_t index_ = 0;
        const std::shared_ptr<spdlog::logger> logger_;
        const std::filesystem::path root_path_;
        mutable boost::asio::io_context::strand strand_;

    public:
        session(boost::asio::ip::tcp::socket&& socket,
                boost::asio::io_context& context,
                std::filesystem::path root,
                bool chunked = false);

        using session_base::local_endpoint;
        using session_base::remote_endpoint;

        void wait_request();

        static http_session_ptr create(socket_type&& socket,
                                       boost::asio::io_context& context,
                                       std::filesystem::path root,
                                       bool chunked = false);

    private:

        template<typename Body>
        folly::Function<void(boost::system::error_code, std::size_t)>
        on_recv_request(request_ptr<Body> request) {
            namespace http = boost::beast::http;
            return [this, request = std::move(request)](boost::system::error_code errc,
                                                        std::size_t transfer_size) {
                logger_->info("on_recv_request errc {} transfer {}", errc, transfer_size);
                logger_->debug("on_recv_request request head {}", request->base());
                if (errc || request->need_eof()) {
                    logger_->error("close_socket shutdown_receive");
                    return close_socket(boost::asio::socket_base::shutdown_receive);
                }
                auto target_path = concat_target_path(request->target());
                logger_->info("on_recv_request {} {}", target_path, exists(target_path) ? "valid" : "invalid");
                auto send_response = [this](auto&& response_ptr) {
                    auto& response_ref = *response_ptr;
                    logger_->info("on_recv_request response reason {}", response_ptr->reason());
                    http::async_write(socket_, response_ref, on_send_response(std::move(response_ptr)));
                };
                if (std::filesystem::exists(target_path)) {
                    boost::system::error_code file_errc;
                    file_body::value_type response_body;
                    response_body.open(target_path.generic_string().c_str(), boost::beast::file_mode::scan, file_errc);
                    assert(!file_errc);
                    auto response = std::make_unique<
                        http::response<file_body>>(http::status::ok,
                                                   request->version(),
                                                   std::move(response_body));
                    response->content_length(response_body.size());
                    response->set(http::field::server, "MetaPlus");
                    response->keep_alive(request->keep_alive());
                    send_response(std::move(response));
                } else {
                    send_response(std::make_unique<
                        http::response<empty_body>>(http::status::bad_request,
                                                    request->version()));
                }
            };
        }

        template<typename Body>
        folly::Function<void(boost::system::error_code, std::size_t)>
        on_send_response(response_ptr<Body> response) {
            return [this, response = std::move(response)](boost::system::error_code errc,
                                                          std::size_t transfer_size) mutable {
                logger_->info("on_send_response errc {} last {} transfer {}", errc, response->need_eof(), transfer_size);
                if (errc || response->need_eof()) {
                    logger_->error("on_send_response errmsg {}", errc.message());
                    logger_->error("close_socket shutdown_send");
                    return close_socket(boost::asio::socket_base::shutdown_send);
                }
                wait_request();
            };
        }

        std::filesystem::path concat_target_path(boost::beast::string_view request_target) const {
            return std::filesystem::path{ root_path_ }.concat(request_target.begin(),
                                                              request_target.end());
        }
    };
}
