#pragma once

namespace net
{
    namespace protocal
    {
        struct http
        {
            static constexpr auto default_version = 11;
            static constexpr auto default_method = boost::beast::http::verb::get;

            struct protocal_base
            {
                template<typename Body>
                using response_type = boost::beast::http::response<Body, boost::beast::http::fields>;
                template<typename Body>
                using request_type = boost::beast::http::request<Body, boost::beast::http::fields>;
                template<typename Body>
                using response_parser_type = boost::beast::http::response_parser<Body>;
                template<typename Body>
                using request_parser_type = boost::beast::http::request_parser<Body>;
                template<typename Body>
                using response_ptr = std::unique_ptr<response_type<Body>>;
                template<typename Body>
                using request_ptr = std::unique_ptr<request_type<Body>>;
                template<typename Body>
                using response_promise = std::promise<response_type<Body>>;
                template<typename Body>
                using request_promise = std::promise<request_type<Body>>;
                using under_protocal_type = boost::asio::ip::tcp;
                using socket_type = boost::asio::ip::tcp::socket;
            };
        };

        struct tcp
        {
            struct protocal_base
            {
                using protocal_type = boost::asio::ip::tcp;
                using socket_type = boost::asio::ip::tcp::socket;
                using context_type = boost::asio::io_context;
            };
        };

        struct dash : http
        {
            int64_t last_tile_index = 1;

            struct represent
            {
                int16_t id = 0;
                int bandwidth = 0;
                std::string media;
                std::string initialization;
            };

            struct adaptation_set
            {
                std::string codecs;
                std::string mime_type;
                std::vector<represent> represents;
            };

            struct video_adaptation_set : adaptation_set
            {
                int16_t x = 0;
                int16_t y = 0;
                int16_t width = 0;
                int16_t height = 0;
            };

            struct audio_adaptation_set : adaptation_set
            {
                int sample_rate = 0;
            };

            class parser
            {
                struct impl;
                std::shared_ptr<impl> impl_;

            public:
                explicit parser(std::string_view xml_text);
                parser(const parser&) = default;
                parser(parser&&) = default;
                parser& operator=(const parser&) = default;
                parser& operator=(parser&&) = default;
                ~parser() = default;

                std::string_view title() const;
                std::pair<int16_t, int16_t> scale() const;
                video_adaptation_set& video_set(int column, int row) const;
                audio_adaptation_set& audio_set() const;

                static std::chrono::milliseconds parse_duration(std::string_view duration);
            };
        };
    }

    inline namespace tag
    {
        inline namespace encoding
        {
            inline struct use_chunk_t {} use_chunk;
        }
    }

    std::vector<boost::thread*> create_asio_threads(boost::asio::io_context& context,
                                                    boost::thread_group& thread_group,
                                                    uint32_t num = std::thread::hardware_concurrency());
    std::vector<std::thread> create_asio_named_threads(boost::asio::io_context& context,
                                                       uint32_t num = std::thread::hardware_concurrency());

    template<typename... Options>
    struct policy;

    inline constexpr size_t default_max_chunk_size{ 128_kbyte };
    inline constexpr size_t default_max_chunk_quantity{ 1024 };

    template<typename Body>
    boost::beast::http::request<Body> make_http_request(std::string_view host, std::string_view target) {
        static_assert(boost::beast::http::is_body<Body>::value);
        boost::beast::http::request<Body> request;
        request.version(protocal::http::default_version);
        request.method(protocal::http::default_method);
        request.target(target.data());
        request.keep_alive(true);
        request.set(boost::beast::http::field::host, host);
        //request.set(boost::beast::http::field::user_agent, "MetaPlus");
        return request;
    }

    std::filesystem::path config_path() noexcept;
    std::string config_entry(std::string_view entry_name);

    template<typename T>
    T config_entry(std::string_view entry_name) {
        auto entry = config_entry(entry_name);
        if constexpr (meta::is_within<T, std::string, std::string_view>::value) {
            return entry;
        } else {
            return boost::lexical_cast<T>(entry);
        }
    }

    class state_base
    {
        enum state_index { active, state_size };
        folly::AtomicBitSet<state_size> state_;

    protected:
        bool is_active() const {
            return state_.test(active, std::memory_order_acquire);
        }

        bool is_active(bool active) {
            return state_.set(state_index::active, active, std::memory_order_release);
        }
    };
}

template<typename Protocal>
struct std::less<boost::asio::basic_socket<Protocal>>
{
    bool operator()(boost::asio::basic_socket<Protocal> const& sock1,
                    boost::asio::basic_socket<Protocal> const& sock2) const {
        return sock1.remote_endpoint() < sock2.remote_endpoint()
            || !(sock2.remote_endpoint() < sock1.remote_endpoint())
            && sock1.local_endpoint() < sock2.local_endpoint();
    }
};

template<typename Protocal>
struct std::equal_to<boost::asio::basic_socket<Protocal>>
{
    bool operator()(boost::asio::basic_socket<Protocal> const& sock1,
                    boost::asio::basic_socket<Protocal> const& sock2) const {
        return sock1.remote_endpoint() == sock2.remote_endpoint()
            && sock1.local_endpoint() == sock2.local_endpoint();
    }
};
