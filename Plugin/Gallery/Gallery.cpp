// Gallery.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "interface.h"

void Release()
{
    try
    {
        dll::MediaRelease();
    }
    catch (...)
    {
        dll::MediaClear();
    }
}

