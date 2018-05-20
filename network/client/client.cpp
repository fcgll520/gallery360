// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

namespace
{
    std::ofstream ofs{ "C:/Media/Recv/recv.h264",std::ios::binary | std::ios::trunc | std::ios::out };
    std::ofstream ofs_yuv{ "C:/Media/Recv/recv.yuv",std::ios::binary | std::ios::trunc | std::ios::out };

}

int main(int argc, char* argv[])
{
    try
    {
        size_t index = 63;
        std::string str{ "(delim)" };
        auto str_copy = str;
        str_copy.append(reinterpret_cast<char*>(&++index), sizeof size_t);
        const auto io_context = std::make_shared<boost::asio::io_context>();
        std::vector<std::thread> thread_pool(std::thread::hardware_concurrency());
        auto client = std::make_shared<net::v1::client>(io_context);
        auto session_future = client->establish_session("localhost", "6666");
        {
            auto work_guard = make_work_guard(*io_context);
            std::generate_n(thread_pool.begin(), thread_pool.size(),
                [io_context] { return std::thread{ [io_context] { io_context->run(); } }; });
            fmt::print("start receiving\n");
            const std::string_view delim{ "(delim)" };
            auto session = session_future.get();
            boost::asio::streambuf recv_streambuf;
            // auto& recvbuf_smutex = session_ptr->external_recvbuf(recv_streambuf);
            session->recv_delim_suffix(delim);
            /*const av::io_context io{ { [session_ptr,&recv_streambuf](uint8_t* buffer, int size) mutable -> int
            { 
                static std::pair<uint16_t, uint64_t> last_streambuf_info{ 0,0 };
                assert(size > 0 && last_streambuf_info.second >= 0);
                int read_size;
                if (last_streambuf_info.second != 0)
                {
                    const auto available_size = static_cast<int>(std::minus<uint64_t>{}(last_streambuf_info.second, last_streambuf_info.first));
                    read_size = std::min<int>(available_size, size);
                    const auto streambuf_lock = session_ptr->external_recvbuf_lock();
                    std::copy_n(buffers_begin(recv_streambuf.data()), read_size, buffer);
                    if (available_size != read_size)
                    {
                        recv_streambuf.consume(read_size);
                        last_streambuf_info.second -= read_size;
                    }
                    else
                    {
                        recv_streambuf.consume(last_streambuf_info.second);
                        last_streambuf_info.second = 0;
                    }
                }
                else
                {
                    /*if (!session_ptr) return 0;
                    const auto[delim_size, transfer_size, streambuf_lock] = session_ptr->receive_externally();
                    fmt::print("delim size: {}, transfer size: {}\n", delim_size, transfer_size);
                    if (transfer_size < 32)
                    {
                        session_ptr->close_socket();
                        session_ptr = nullptr;
                        return 0;
                    }
                    const auto available_size = static_cast<int>(std::minus<uint64_t>{}(transfer_size, delim_size));
                    read_size = std::min<int>(available_size, size);
                    std::copy_n(buffers_begin(recv_streambuf.data()), read_size, buffer);
                    if (available_size != read_size)
                    {
                        recv_streambuf.consume(read_size);
                        last_streambuf_info.first = delim_size;
                        last_streambuf_info.second = transfer_size - read_size;
                    }
                    else recv_streambuf.consume(transfer_size);#1#
                }
                return read_size;
            },nullptr,. } };*/
            /*
            av::format_context format{ io,av::source::format{ "h264"} };
            auto[cdc, srm] = format.demux_with_codec(av::media::video{});
            av::codec_context codec{ cdc,srm };
            av::packet packet;
            uint64_t count = 0;
            while (!(packet = format.read(std::nullopt)).empty())
            {
                auto frame = codec.decode(packet);
                fmt::print("decoded count {}\n", ++count);
            }
            */

            while (session)
            {
                // auto[delim_size, transfer_size, streambuf_lock] = session_ptr->receive_externally();
                // fmt::print("delim size: {}, transfer size: {}\n", delim_size, transfer_size);
                // recv_streambuf.consume(transfer_size);
                // streambuf_lock.unlock();
                // if (transfer_size - delim_size > 32) continue;
                auto recvbuf = session->receive(delim);
                fmt::print("\t recv buffer size: {}\n", recvbuf.size());
                // std::copy_n(recv_buffer.cbegin(), recv_buffer.size(), std::ostreambuf_iterator<char>{ofs});
                if (recvbuf.size() > 32) continue;
                session->close_socket(core::defer_execute);
                session = nullptr;
            }
        }
        for (auto& thread : thread_pool)
            if (thread.joinable()) thread.join();
    }
    catch (const std::exception& e)
    {
        fmt::print(std::cerr, "exception detected: {}\n", e.what());
    }
    return 0;
}