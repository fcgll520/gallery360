// Gallery.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "interface.h"

BOOL GlobalCreate()
{
#ifdef NDEBUG
    try
    {
#endif
        dll::media_create();
#ifdef NDEBUG
    }
    catch (...) { return false; }
#endif
    return true;
}
void GlobalRelease()
{
#ifdef NDEBUG
    try
    {
#endif
        dll::media_release();
#ifdef NDEBUG
    }
    catch (...)
    {
        dll::media_clear();
    }
#endif
}

