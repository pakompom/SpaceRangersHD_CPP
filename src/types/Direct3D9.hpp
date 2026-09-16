#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"

namespace Direct3D9 {
    struct IDirect3D9_Tag;

    struct IDirect3DBaseTexture9_Tag;

    struct IDirect3DDevice9_Tag;

    struct IDirect3DResource9_Tag;

    struct IDirect3DSurface9_Tag;

    struct IDirect3DTexture9_Tag;

    struct IDirect3DVertexShader9_Tag;

    struct TD3DAdapterIdentifier9;

    struct TD3DCaps9;

    struct TD3DGammaRamp;

    struct TD3DPresentParameters;

    struct TD3DLockedRect;

    struct TD3DSurfaceDesc;

    using IDirect3DTexture9 = pas::ComPtr<IDirect3DTexture9_Tag>;

    using IDirect3DBaseTexture9 = pas::ComPtr<IDirect3DBaseTexture9_Tag>;

    using IDirect3DResource9 = pas::ComPtr<IDirect3DResource9_Tag>;

    // D3DPRESENT_PARAMETERS, Win32 SDK layout. Native storage is
    // copied/cleared as 56 bytes and passed to CreateDevice.
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dpresent-parameters
    #pragma pack(push, 1)
    struct TD3DPresentParameters {
        std::uint32_t BackBufferWidth;
        std::uint32_t BackBufferHeight;
        std::uint32_t BackBufferFormat;
        std::uint32_t BackBufferCount;
        std::uint32_t MultiSampleType;
        std::uint32_t MultiSampleQuality;
        std::uint32_t SwapEffect;
        std::uint32_t DeviceWindow;
        std::int32_t Windowed;
        std::int32_t EnableAutoDepthStencil;
        std::uint32_t AutoDepthStencilFormat;
        std::uint32_t Flags;
        std::uint32_t FullScreenRefreshRateInHz;
        std::uint32_t PresentationInterval;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TD3DSurfaceDesc {
        std::uint32_t Format;
        std::uint32_t ResourceType;
        std::uint32_t Usage;
        std::uint32_t Pool;
        std::uint32_t MultiSampleType;
        std::uint32_t MultiSampleQuality;
        std::uint32_t Width;
        std::uint32_t Height;
    };
    #pragma pack(pop)

    using IDirect3DDevice9 = pas::ComPtr<IDirect3DDevice9_Tag>;

    #pragma pack(push, 1)
    struct TD3DLockedRect {
        std::int32_t Pitch;
        void* Bits;
    };
    #pragma pack(pop)

    using IDirect3DSurface9 = pas::ComPtr<IDirect3DSurface9_Tag>;

    #pragma pack(push, 1)
    struct TD3DAdapterIdentifier9 {
        pas::Array<std::uint8_t, 0, 511> Driver;
        pas::Array<std::uint8_t, 0, 511> Description;
        pas::Array<std::uint8_t, 0, 31> DeviceName;
        std::int64_t DriverVersion;
        std::uint32_t VendorId;
        std::uint32_t DeviceId;
        std::uint32_t SubSysId;
        std::uint32_t Revision;
        System::TGUID DeviceIdentifier;
        std::uint32_t WHQLLevel;
    };
    #pragma pack(pop)

    // D3DCAPS9 storage in GR_DXInit is $130 bytes. The texture-capability
    // fields around this caller's reads are modeled; other SDK fields stay opaque.
    #pragma pack(push, 1)
    struct TD3DCaps9 {
        pas::Array<std::uint8_t, 0, 87> CapabilitiesPrefix;
        std::uint32_t MaxTextureWidth;
        std::uint32_t MaxTextureHeight;
        std::uint32_t MaxVolumeExtent;
        std::uint32_t MaxTextureRepeat;
        std::uint32_t MaxTextureAspectRatio;
        std::uint32_t MaxAnisotropy;
        pas::Array<std::uint8_t, 0, 191> CapabilitiesTail;
    };
    #pragma pack(pop)

    // SDK order and ABI checked against native vtable calls and Wine include/d3d9.h:
    // https://github.com/wine-mirror/wine/blob/master/include/d3d9.h
    using IDirect3D9 = pas::ComPtr<IDirect3D9_Tag>;

    using IDirect3DVertexShader9 = pas::ComPtr<IDirect3DVertexShader9_Tag>;

    #pragma pack(push, 1)
    struct TD3DGammaRamp {
        pas::Array<std::uint16_t, 0, 255> Red;
        pas::Array<std::uint16_t, 0, 255> Green;
        pas::Array<std::uint16_t, 0, 255> Blue;
    };
    #pragma pack(pop)

    using TDirect3DCreate9 = pas::StdcallProc<void*(std::uint32_t)>;

    // SDK constants used by device initialization and the screen vertex layout:
    // https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/shared/d3d9.h
    // https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/shared/d3d9types.h
    // https://github.com/microsoft/win32metadata/blob/main/generation/WinSDK/RecompiledIdlHeaders/shared/d3d9caps.h
    inline constexpr std::int32_t D3DADAPTER_DEFAULT = 0;

    inline constexpr std::int32_t D3DDEVTYPE_HAL = 1;

    inline constexpr std::int32_t D3DCREATE_MULTITHREADED = 0x00000004;

    inline constexpr std::int32_t D3DCREATE_SOFTWARE_VERTEXPROCESSING = 0x00000020;

    inline constexpr std::int32_t D3DCREATE_HARDWARE_VERTEXPROCESSING = 0x00000040;

    inline constexpr std::int32_t D3DPRESENT_INTERVAL_DEFAULT = 0;

    inline constexpr std::int32_t D3DPRESENT_INTERVAL_ONE = 1;

    inline constexpr std::uint32_t D3DPRESENT_INTERVAL_IMMEDIATE = 0x80000000u;

    inline constexpr std::int32_t D3DSWAPEFFECT_DISCARD = 1;

    inline constexpr std::int32_t D3DSWAPEFFECT_FLIP = 2;

    inline constexpr std::int32_t D3DBACKBUFFER_TYPE_MONO = 0;

    inline constexpr std::int32_t D3DMULTISAMPLE_NONE = 0;

    inline constexpr std::int32_t D3DCLEAR_TARGET = 0x00000001;

    inline constexpr std::int32_t D3DFVF_XYZRHW = 0x00000004;

    inline constexpr std::int32_t D3DFVF_DIFFUSE = 0x00000040;

    inline constexpr std::int32_t D3DFVF_TEX1 = 0x00000100;

    inline constexpr std::int32_t D3DFMT_R8G8B8 = 20;

    inline constexpr std::int32_t D3DFMT_A8R8G8B8 = 21;

    inline constexpr std::int32_t D3DFMT_X8R8G8B8 = 22;

    inline constexpr std::int32_t D3DFMT_R5G6B5 = 23;

    inline constexpr std::int32_t D3DFMT_A8 = 28;

    inline constexpr std::int32_t D3DPOOL_DEFAULT = 0;

    inline constexpr std::int32_t D3DPOOL_MANAGED = 1;

    inline constexpr std::int32_t D3DPOOL_SYSTEMMEM = 2;

    inline constexpr std::int32_t D3DLOCK_READONLY = 0x00000010;

    inline constexpr std::int32_t D3DPT_POINTLIST = 1;

    inline constexpr std::int32_t D3DPT_LINESTRIP = 3;

    inline constexpr std::int32_t D3DPT_TRIANGLELIST = 4;

    inline constexpr std::int32_t D3DPT_TRIANGLEFAN = 6;

    // Direct3D 9 SDK values: https://github.com/wine-mirror/wine/blob/master/include/d3d9types.h
    inline constexpr std::int32_t D3DRS_FILLMODE = 8;

    inline constexpr std::int32_t D3DFILL_WIREFRAME = 2;

    inline constexpr std::int32_t D3DFILL_SOLID = 3;

    inline constexpr std::int32_t D3DRS_SRCBLEND = 19;

    inline constexpr std::int32_t D3DRS_DESTBLEND = 20;

    inline constexpr std::int32_t D3DRS_CULLMODE = 22;

    inline constexpr std::int32_t D3DRS_ALPHABLENDENABLE = 27;

    inline constexpr std::int32_t D3DRS_SCISSORTESTENABLE = 174;

    inline constexpr std::int32_t D3DBLEND_SRCALPHA = 5;

    inline constexpr std::int32_t D3DBLEND_INVSRCALPHA = 6;

    inline constexpr std::int32_t D3DCULL_NONE = 1;

    inline constexpr std::int32_t D3DTSS_ALPHAOP = 4;

    inline constexpr std::int32_t D3DTOP_MODULATE = 4;

    inline constexpr std::int32_t D3DSAMP_MAGFILTER = 5;

    inline constexpr std::int32_t D3DSAMP_MINFILTER = 6;

    inline constexpr std::int32_t D3DSAMP_MIPFILTER = 7;

    inline constexpr std::int32_t D3DTEXF_LINEAR = 2;

    inline constexpr std::int32_t D3DERR_DEVICENOTRESET = -2005530519;

} // namespace Direct3D9
