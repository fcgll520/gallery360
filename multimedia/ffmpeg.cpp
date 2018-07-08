#include "stdafx.h"
#include "ffmpeg.h"

#ifdef MULTIMEDIA_USE_MSGPACK
#include <msgpack.hpp>
#include <msgpack/adaptor/define_decl.hpp>
#endif // MULTIMEDIA_USE_MSGPACK

media::frame::frame()
    : handle_(av_frame_alloc(), [](pointer p) { av_frame_free(&p); })
{}

media::frame::frame(std::nullptr_t)
{}

void media::register_all()
{
    // static std::once_flag once;
    // std::call_once(once, [] { av_register_all(); });
}

bool media::frame::empty() const
{
    return handle_ == nullptr || handle_->data == nullptr || handle_->data[0] == nullptr;
}

media::frame::pointer media::frame::operator->() const
{
    return handle_.get();
}

void media::frame::unref() const
{
    av_frame_unref(handle_.get());
}

media::packet::packet(std::nullptr_t)
{}

media::packet::packet(std::basic_string_view<uint8_t> sv)
    : packet()
{
    uint8_t* required_avbuffer = nullptr;
    core::verify(required_avbuffer = static_cast<uint8_t*>(av_malloc(sv.size() + AV_INPUT_BUFFER_PADDING_SIZE)));
    std::copy_n(sv.data(), sv.size(), required_avbuffer);
    core::verify(0 == av_packet_from_data(handle_.get(), required_avbuffer, static_cast<int>(sv.size())));
}

media::packet::packet(std::string_view csv)
    : packet(std::basic_string_view<uint8_t>{reinterpret_cast<uint8_t const*>(csv.data()), csv.size()})
{}

media::packet::packet()
    : handle_(av_packet_alloc(), [](pointer p) { av_packet_free(&p); })
{}

bool media::packet::empty() const
{
    return handle_ == nullptr || handle_->data == nullptr || handle_->size <= 0;
}

size_t media::packet::size() const
{
    return handle_ ? handle_->size : 0;
}

std::basic_string_view<uint8_t> media::packet::bufview() const
{
    return { handle_->data, boost::numeric_cast<size_t>(handle_->size) };
}

#ifdef MULTIMEDIA_USE_MSGPACK
std::string media::packet::serialize() const
{
    msgpack::sbuffer sbuf(handle_->size + sizeof(chunk));
    msgpack::pack(sbuf, chunk{ *this });
    return std::string{ sbuf.data(),sbuf.size() };
}
#endif // MULTIMEDIA_USE_MSGPACK

media::packet::pointer media::packet::operator->() const
{
    return handle_.get();
}

media::packet::operator bool() const
{
    return !empty();
}

void media::packet::unref() const
{
    av_packet_unref(handle_.get());
}

media::stream::stream(reference ref)
    : reference_wrapper(ref)
{}

media::stream::stream(const pointer ptr)
    : reference_wrapper(*ptr)
{}

media::codec::codec(reference ref)
    : reference_wrapper(ref)
{}

media::codec::codec(const pointer ptr)
    : reference_wrapper(*ptr)
{}

media::codec::pointer media::codec::operator->() const
{
    return std::addressof(get());
}

media::codec::codec()
    : reference_wrapper(core::make_null_reference_wrapper<type>())
{}

media::codec::parameter media::stream::params() const
{
    return std::cref(*get().codecpar);
}

media::media::type media::stream::media() const
{
    return get().codecpar->codec_type;
}

std::pair<int, int> media::stream::scale() const
{
    return std::make_pair(get().codecpar->width, get().codecpar->height);
}

media::stream::pointer media::stream::operator->() const
{
    return std::addressof(get());
}

media::stream::stream()
    : reference_wrapper(core::make_null_reference_wrapper<type>())
{}

int media::stream::index() const
{
    return get().index;
}
