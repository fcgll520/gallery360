#include "stdafx.h"
#include "guard.h"

core::time_guard::time_guard() :
    time_mark_{ std::chrono::steady_clock::now() } 
{}

core::time_guard::~time_guard()
{
    //std::cout.setf(std::ios::hex);
    std::cout
        << "thread@" << std::this_thread::get_id() << ' '
        << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::steady_clock::now() - time_mark_).count()
        << " ms\n";
    //std::cout.unsetf(std::ios::hex);
}

core::scope_guard::scope_guard(std::function<void()> release, bool ctor_invoke)
    : release_(ctor_invoke ? release : std::move(release))
{
    if (ctor_invoke)
        release();
}

core::scope_guard::~scope_guard()
{
    if (release_)
        release_();
}