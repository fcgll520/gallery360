// Gallery.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "interface.h"

BOOL unity::GlobalCreate()
{
#ifdef NDEBUG                       
    try
    {
#endif
        dll::media_prepare();
        dll::interprocess_create();
        dll::media_create();
        dll::interprocess_async_send(ipc::message{}.emplace(ipc::info_launch{}));
#ifdef NDEBUG
    }
    catch (...) { return false; }
#endif
    return true;
}
void unity::GlobalRelease()
{
    dll::interprocess_send(ipc::message{}.emplace(ipc::info_exit{}));
    std::this_thread::yield();
    dll::media_release();
    dll::interprocess_release();
    dll::graphics_release();
}
