#pragma once

namespace media::component
{
    namespace detail
    {
        using boost::asio::const_buffer;
    }

    class frame_segmentor
    {
        struct impl;
        std::shared_ptr<impl> impl_;

        struct buffer_drained_exception;

    public:
        frame_segmentor() = default;
        frame_segmentor(const frame_segmentor&) = default;
        frame_segmentor(frame_segmentor&&) noexcept = default;
        frame_segmentor& operator=(const frame_segmentor&) = default;
        frame_segmentor& operator=(frame_segmentor&&) noexcept = default;
        ~frame_segmentor() = default;

        explicit frame_segmentor(std::list<detail::const_buffer> buffer_list);

        void parse_context(std::list<detail::const_buffer> buffer_list);
        bool context_valid() const noexcept;
        bool buffer_available() const;
        void reset_buffer_list(std::list<detail::const_buffer> buffer_list);
        bool try_read();
        int try_consume();
        bool try_consume_once();
    };

}