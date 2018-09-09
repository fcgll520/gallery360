#include "stdafx.h"
#include "cursor.h"

namespace media
{
    //-- cursor
    cursor::cursor(const multi_buffer& buffer)
        : buffer_begin(boost::asio::buffer_sequence_begin(buffer.data()))
        , buffer_end(boost::asio::buffer_sequence_end(buffer.data()))
        , buffer_iter(buffer_begin) {
        auto& buffer_sizes = core::as_mutable(this->buffer_sizes);
        std::transform(buffer_begin, buffer_end, std::back_inserter(buffer_sizes),
                       [](const_buffer_iterator::reference buffer) { return boost::asio::buffer_size(buffer); });
        //std::partial_sum(buffer_sizes.begin(), buffer_sizes.end(), buffer_sizes.begin());
    }

    int64_t cursor::seek_sequence(int64_t seek_offset) {
        auto const size_iter = std::upper_bound(buffer_sizes.crbegin(), buffer_sizes.crend(), seek_offset, std::greater<>{});
        buffer_iter = std::prev(buffer_end, std::distance(buffer_sizes.crbegin(), size_iter));
        auto const partial_sequence_size = size_iter != buffer_sizes.crend() ? *size_iter : 0;
        buffer_offset = std::min(seek_offset - partial_sequence_size, buffer_size());
        sequence_offset = partial_sequence_size + buffer_offset;
        return sequence_offset;
    }

    int64_t cursor::buffer_size() const {
        //auto const size = boost::asio::buffer_size(*buffer_iter);
        auto const size = buffer_sizes.at(std::distance(buffer_begin, buffer_iter));
        return boost::numeric_cast<int64_t>(size);
    }

    int64_t cursor::sequence_size() const {
        return std::accumulate(buffer_sizes.begin(), buffer_sizes.end(), 0i64);
        //fmt::print("sequence_size {}\n", size);
    }

    //-- generic_cursor
    generic_cursor::generic_cursor(read_context&& rfunc, write_context&& wfunc, seek_context&& sfunc)
        : reader(std::move(rfunc))
        , writer(std::move(wfunc))
        , seeker(std::move(sfunc)) {}

    int generic_cursor::read(uint8_t* buffer, const int size) {
        return readable() ? reader(buffer, size) : std::numeric_limits<int>::min();
    }

    int generic_cursor::write(uint8_t* buffer, const int size) {
        return writable() ? writer(buffer, size) : std::numeric_limits<int>::min();
    }

    int64_t generic_cursor::seek(const int64_t offset, const int whence) {
        return seekable() ? seeker(offset, whence) : std::numeric_limits<int64_t>::min();
    }

    bool generic_cursor::readable() {
        return reader != nullptr;
    }

    bool generic_cursor::writable() {
        return writer != nullptr;
    }

    bool generic_cursor::seekable() {
        return seeker != nullptr;
    }

    std::shared_ptr<generic_cursor> generic_cursor::create(read_context&& rfunc, write_context&& wfunc, seek_context&& sfunc) {
        return std::make_shared<generic_cursor>(std::move(rfunc), std::move(wfunc), std::move(sfunc));
    }

    //-- random_access_curser
    random_access_cursor::random_access_cursor(const multi_buffer& buffer)
        : cursor(buffer) {}

    int random_access_cursor::read(uint8_t* buffer, int expect_size) {
        if (buffer_iter == buffer_end)
            return AVERROR_EOF;
        auto total_read_size = 0;
        fmt::print("cursor reading, expect_size {}, sequence{}/{}\n", expect_size, sequence_offset, sequence_size());
        while (buffer_iter != buffer_end && total_read_size < expect_size) {
            auto const read_ptr = static_cast<char const*>((*buffer_iter).data());
            auto const read_size = std::min<int64_t>(expect_size - total_read_size, buffer_size() - buffer_offset);
            assert(read_size > 0);
            std::copy_n(read_ptr + buffer_offset, read_size, buffer + total_read_size);
            buffer_offset += read_size;
            sequence_offset += read_size;
            if (buffer_offset == buffer_size()) {
                buffer_iter.operator++();
                buffer_offset = 0;
            }
            total_read_size += boost::numeric_cast<int>(read_size);
        }
        fmt::print("read_size {}, expect_size {}, sequence{}/{}\n", total_read_size, expect_size, sequence_offset, sequence_size());
        return boost::numeric_cast<int>(total_read_size);
    }

    int random_access_cursor::write(uint8_t* buffer, int size) {
        throw core::not_implemented_error{ __FUNCSIG__ };
    }

    int64_t random_access_cursor::seek(int64_t seek_offset, int whence) {
        switch (whence) {
        case SEEK_SET: fmt::print("SEEK_SET OFFSET {}\n", seek_offset);
            break;
        case SEEK_END: fmt::print("SEEK_END OFFSET {}\n", seek_offset);
            seek_offset += sequence_size();
            break;
        case SEEK_CUR: fmt::print("SEEK_CUR OFFSET {}\n", seek_offset);
            seek_offset += sequence_offset;
            break;
        case AVSEEK_SIZE: fmt::print("AVSEEK_SIZE OFFSET {}\n", seek_offset);
            return sequence_size();
        default:
            throw core::unreachable_execution_branch{ __FUNCSIG__ };
        }
        return seek_sequence(seek_offset);
    }

    bool random_access_cursor::readable() {
        return true;
    }

    bool random_access_cursor::writable() {
        return false;
    }

    bool random_access_cursor::seekable() {
        return true;
    }

    std::shared_ptr<random_access_cursor> random_access_cursor::create(const multi_buffer& buffer) {
        return std::make_shared<random_access_cursor>(buffer);
    }

    forward_stream_cursor::forward_stream_cursor(buffer_supplier&& supplier)
        : on_future_buffer(std::move(supplier))
        , future_buffer(on_future_buffer()) {}

    bool forward_stream_cursor::shift_next_buffer() {
        try {
            current_buffer = future_buffer.get(); // exception point
            io_base = random_access_cursor::create(current_buffer.value());
            future_buffer = on_future_buffer();
        } catch (...) {
            return false;
        }
        return true;
    }

    int forward_stream_cursor::read(uint8_t* buffer, int expect_size) {
        if (eof || !current_buffer.has_value() && !shift_next_buffer()) {
            fmt::print("----- eof\n");
            return AVERROR_EOF;
        }
        auto total_read_size = 0;
        auto increment_read_size = 0;
        fmt::print("----- start expect {}\n", expect_size);
        while (total_read_size < expect_size) {
            increment_read_size = io_base->read(buffer + total_read_size, expect_size - total_read_size);
            if (increment_read_size == AVERROR_EOF) {
                fmt::print("rebuild\n");
                if (!shift_next_buffer()) {
                    eof = true;
                    break;
                }
                increment_read_size = io_base->read(buffer + total_read_size, expect_size - total_read_size);
            }
            assert(increment_read_size > 0);
            total_read_size += increment_read_size;
        }
        fmt::print("----- end total read {}\n", total_read_size);
        return total_read_size;
    }

    std::shared_ptr<forward_stream_cursor> forward_stream_cursor::create(buffer_supplier&& supplier) {
        return std::make_shared<forward_stream_cursor>(std::move(supplier));
    }
}