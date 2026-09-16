#pragma once
#include "types/Direct3D9.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"

namespace Direct3D9 {
    // Native DLL export returns an unowned COM interface pointer in EAX.
    extern Direct3D9::TDirect3DCreate9 Direct3DCreate9;

    // Adopts the DLL's reference through an interface assignment followed by Release.
    void PAS_STDCALL CreateDirect3D9(std::uint32_t SDKVersion, IDirect3D9& Result);

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_GetLevelDesc(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, TD3DSurfaceDesc& Desc);

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_GetSurfaceLevel(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, IDirect3DSurface9& Surface);

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_LockRect(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, TD3DLockedRect& LockedRect, Types::PRect Rect, std::uint32_t Flags);

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_UnlockRect(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_TestCooperativeLevel(pas::ComView<IDirect3DDevice9_Tag> Self);

    inline std::uint32_t PAS_STDCALL IDirect3DDevice9_GetAvailableTextureMem(pas::ComView<IDirect3DDevice9_Tag> Self);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Reset(pas::ComView<IDirect3DDevice9_Tag> Self, void* Parameters);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Present(pas::ComView<IDirect3DDevice9_Tag> Self, Types::PRect SourceRect, Types::PRect DestRect, std::uint32_t DestWindow, void* DirtyRegion);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetBackBuffer(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t SwapChain, std::uint32_t BackBuffer, std::uint32_t BackBufferType, IDirect3DSurface9& Surface);

    inline void PAS_STDCALL IDirect3DDevice9_SetGammaRamp(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t SwapChain, std::uint32_t Flags, const void* Ramp);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateTexture(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Levels, std::uint32_t Usage, std::uint32_t Format, std::uint32_t Pool, IDirect3DTexture9& Texture, System::PCardinal SharedHandle);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Format, std::uint32_t MultiSample, std::uint32_t MultiSampleQuality, std::int32_t Lockable, IDirect3DSurface9& Surface, System::PCardinal SharedHandle);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_UpdateTexture(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DBaseTexture9_Tag> Source, pas::ComView<IDirect3DBaseTexture9_Tag> Dest);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetRenderTargetData(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Source, pas::ComView<IDirect3DSurface9_Tag> Dest);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_StretchRect(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Source, Types::PRect SourceRect, pas::ComView<IDirect3DSurface9_Tag> Dest, Types::PRect DestRect, std::uint32_t Filter);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_ColorFill(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Surface, Types::PRect Rect, std::uint32_t Color);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateOffscreenPlainSurface(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Format, std::uint32_t Pool, IDirect3DSurface9& Surface, System::PCardinal SharedHandle);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Index, pas::ComView<IDirect3DSurface9_Tag> Surface);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Index, IDirect3DSurface9& Surface);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_BeginScene(pas::ComView<IDirect3DDevice9_Tag> Self);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_EndScene(pas::ComView<IDirect3DDevice9_Tag> Self);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Clear(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t RectCount, Types::PRect Rects, std::uint32_t Flags, std::uint32_t Color, float Z, std::uint32_t Stencil);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetRenderState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t State, std::uint32_t Value);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetTexture(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Stage, pas::ComView<IDirect3DBaseTexture9_Tag> Texture);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetTextureStageState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Stage, std::uint32_t State, std::uint32_t Value);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetSamplerState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Sampler, std::uint32_t State, std::uint32_t Value);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetScissorRect(pas::ComView<IDirect3DDevice9_Tag> Self, Types::PRect Rect);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetScissorRect(pas::ComView<IDirect3DDevice9_Tag> Self, Types::TRect& Rect);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_DrawPrimitiveUP(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t PrimitiveType, std::uint32_t PrimitiveCount, void* Data, std::uint32_t Stride);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetFVF(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t FVF);

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetVertexShader(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DVertexShader9_Tag> Shader);

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_GetDesc(pas::ComView<IDirect3DSurface9_Tag> Self, TD3DSurfaceDesc& Desc);

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_LockRect(pas::ComView<IDirect3DSurface9_Tag> Self, TD3DLockedRect& LockedRect, Types::PRect Rect, std::uint32_t Flags);

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_UnlockRect(pas::ComView<IDirect3DSurface9_Tag> Self);

    inline std::int32_t PAS_STDCALL IDirect3D9_GetAdapterIdentifier(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Flags, TD3DAdapterIdentifier9& Identifier);

    inline std::uint32_t PAS_STDCALL IDirect3D9_GetAdapterModeCount(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Format);

    inline std::int32_t PAS_STDCALL IDirect3D9_EnumAdapterModes(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Format, std::uint32_t Mode, void* DisplayMode);

    inline std::int32_t PAS_STDCALL IDirect3D9_GetAdapterDisplayMode(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, void* DisplayMode);

    inline std::int32_t PAS_STDCALL IDirect3D9_CheckDeviceMultiSampleType(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, std::uint32_t Format, std::int32_t Windowed, std::uint32_t MultiSample, System::PCardinal QualityLevels);

    inline std::int32_t PAS_STDCALL IDirect3D9_GetDeviceCaps(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, TD3DCaps9& Caps);

    inline std::int32_t PAS_STDCALL IDirect3D9_CreateDevice(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, std::uint32_t FocusWindow, std::uint32_t BehaviorFlags, TD3DPresentParameters& Parameters, IDirect3DDevice9& Device);

} // namespace Direct3D9

#include "inline/Direct3D9.hpp"
