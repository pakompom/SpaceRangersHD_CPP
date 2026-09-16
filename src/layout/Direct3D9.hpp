#pragma once
#include "types/Direct3D9.hpp"

namespace Direct3D9 {
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DPresentParameters) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, BackBufferWidth) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, BackBufferHeight) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, BackBufferFormat) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, BackBufferCount) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, MultiSampleType) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, MultiSampleQuality) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, SwapEffect) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, DeviceWindow) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, Windowed) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, EnableAutoDepthStencil) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, AutoDepthStencilFormat) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, Flags) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, FullScreenRefreshRateInHz) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DPresentParameters, PresentationInterval) == 52);
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DSurfaceDesc) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, Format) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, ResourceType) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, Usage) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, Pool) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, MultiSampleType) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, MultiSampleQuality) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, Width) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DSurfaceDesc, Height) == 28);
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DLockedRect) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DLockedRect, Pitch) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DLockedRect, Bits) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DAdapterIdentifier9) == 1100);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, Driver) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, Description) == 512);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, DeviceName) == 1024);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, DriverVersion) == 1056);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, VendorId) == 1064);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, DeviceId) == 1068);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, SubSysId) == 1072);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, Revision) == 1076);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, DeviceIdentifier) == 1080);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DAdapterIdentifier9, WHQLLevel) == 1096);
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DCaps9) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, CapabilitiesPrefix) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxTextureWidth) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxTextureHeight) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxVolumeExtent) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxTextureRepeat) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxTextureAspectRatio) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, MaxAnisotropy) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DCaps9, CapabilitiesTail) == 112);
    static_assert(sizeof(void*) != 4 || sizeof(Direct3D9::TD3DGammaRamp) == 1536);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DGammaRamp, Red) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DGammaRamp, Green) == 512);
    static_assert(sizeof(void*) != 4 || offsetof(Direct3D9::TD3DGammaRamp, Blue) == 1024);

} // namespace Direct3D9
