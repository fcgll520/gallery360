#include "stdafx.h"
#include "interface.h"

namespace
{
    float unity_time = 0;
    std::unique_ptr<graphic> dll_graphic = nullptr;
    UnityGfxRenderer unity_device = kUnityGfxRendererNull;
    IUnityInterfaces* unity_interface = nullptr;
    IUnityGraphics* unity_graphics = nullptr;
}
void StoreTime(FLOAT t)
{
    unity_time = t;
}
void StoreAlphaTexture(HANDLE texY, HANDLE texU, HANDLE texV)
{
    core::verify(texY != nullptr, texU != nullptr, texV != nullptr);
    dll_graphic->store_textures(texY, texU, texV);
}
static void __stdcall OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        core::verify(unity_graphics->GetRenderer() == kUnityGfxRendererD3D11);
        unity_time = 0;
        dll_graphic = std::make_unique<graphic>();
        unity_device = kUnityGfxRendererD3D11;
    }
    if (dll_graphic != nullptr)
    {
        dll_graphic->process_event(eventType, unity_interface);
    }
    if (eventType == kUnityGfxDeviceEventShutdown)
    {
        unity_device = kUnityGfxRendererNull;
        dll_graphic.reset();
        //dll_graphic = nullptr;
    }
}
EXTERN void UNITYAPI UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    unity_interface = unityInterfaces;
    unity_graphics = unity_interface->Get<IUnityGraphics>();
    unity_graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
    //GlobalCreate();
}
EXTERN void UNITYAPI UnityPluginUnload()
{
    //GlobalRelease();
    unity_graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}
static void __stdcall OnRenderEvent(int eventID)
{
#ifndef NDEBUG
    core::verify(unity_graphics != nullptr);
#endif
    if (auto frame = dll::media_extract_frame(); frame.has_value())
        dll_graphic->update_textures(frame.value());
}
EXTERN UnityRenderingEvent UNITYAPI GetRenderEventFunc()
{
    return OnRenderEvent;
}
