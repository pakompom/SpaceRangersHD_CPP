#pragma once
#include "types/System.hpp"
#include "types/Types.hpp"
#include "units/Direct3D9.hpp"

namespace Direct3D9 {
    inline std::int32_t PAS_STDCALL IDirect3DTexture9_GetLevelDesc(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, TD3DSurfaceDesc& Desc) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, TD3DSurfaceDesc*);
        return pas::com_method<CppMethod>(Self.get(), 17)(Self.get(), Level, &Desc);
    }

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_GetSurfaceLevel(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, IDirect3DSurface9& Surface) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, void**);
        return pas::com_method<CppMethod>(Self.get(), 18)(Self.get(), Level, Surface.address());
    }

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_LockRect(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level, TD3DLockedRect& LockedRect, Types::PRect Rect, std::uint32_t Flags) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, TD3DLockedRect*, Types::PRect, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 19)(Self.get(), Level, &LockedRect, Rect, Flags);
    }

    inline std::int32_t PAS_STDCALL IDirect3DTexture9_UnlockRect(pas::ComView<IDirect3DTexture9_Tag> Self, std::uint32_t Level) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 20)(Self.get(), Level);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_TestCooperativeLevel(pas::ComView<IDirect3DDevice9_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 3)(Self.get());
    }

    inline std::uint32_t PAS_STDCALL IDirect3DDevice9_GetAvailableTextureMem(pas::ComView<IDirect3DDevice9_Tag> Self) {
        using CppMethod = std::uint32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 4)(Self.get());
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Reset(pas::ComView<IDirect3DDevice9_Tag> Self, void* Parameters) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*);
        return pas::com_method<CppMethod>(Self.get(), 16)(Self.get(), Parameters);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Present(pas::ComView<IDirect3DDevice9_Tag> Self, Types::PRect SourceRect, Types::PRect DestRect, std::uint32_t DestWindow, void* DirtyRegion) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, Types::PRect, Types::PRect, std::uint32_t, void*);
        return pas::com_method<CppMethod>(Self.get(), 17)(Self.get(), SourceRect, DestRect, DestWindow, DirtyRegion);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetBackBuffer(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t SwapChain, std::uint32_t BackBuffer, std::uint32_t BackBufferType, IDirect3DSurface9& Surface) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, void**);
        return pas::com_method<CppMethod>(Self.get(), 18)(Self.get(), SwapChain, BackBuffer, BackBufferType, Surface.address());
    }

    inline void PAS_STDCALL IDirect3DDevice9_SetGammaRamp(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t SwapChain, std::uint32_t Flags, const void* Ramp) {
        using CppMethod = void (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, const void*);
        return pas::com_method<CppMethod>(Self.get(), 21)(Self.get(), SwapChain, Flags, Ramp);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateTexture(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Levels, std::uint32_t Usage, std::uint32_t Format, std::uint32_t Pool, IDirect3DTexture9& Texture, System::PCardinal SharedHandle) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, void**, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 23)(Self.get(), Width, Height, Levels, Usage, Format, Pool, Texture.address(), SharedHandle);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Format, std::uint32_t MultiSample, std::uint32_t MultiSampleQuality, std::int32_t Lockable, IDirect3DSurface9& Surface, System::PCardinal SharedHandle) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t, void**, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 28)(Self.get(), Width, Height, Format, MultiSample, MultiSampleQuality, Lockable, Surface.address(), SharedHandle);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_UpdateTexture(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DBaseTexture9_Tag> Source, pas::ComView<IDirect3DBaseTexture9_Tag> Dest) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, void*);
        return pas::com_method<CppMethod>(Self.get(), 31)(Self.get(), pas::interface_pointer(Source), pas::interface_pointer(Dest));
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetRenderTargetData(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Source, pas::ComView<IDirect3DSurface9_Tag> Dest) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, void*);
        return pas::com_method<CppMethod>(Self.get(), 32)(Self.get(), pas::interface_pointer(Source), pas::interface_pointer(Dest));
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_StretchRect(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Source, Types::PRect SourceRect, pas::ComView<IDirect3DSurface9_Tag> Dest, Types::PRect DestRect, std::uint32_t Filter) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, Types::PRect, void*, Types::PRect, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 34)(Self.get(), pas::interface_pointer(Source), SourceRect, pas::interface_pointer(Dest), DestRect, Filter);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_ColorFill(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DSurface9_Tag> Surface, Types::PRect Rect, std::uint32_t Color) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, Types::PRect, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 35)(Self.get(), pas::interface_pointer(Surface), Rect, Color);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_CreateOffscreenPlainSurface(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Width, std::uint32_t Height, std::uint32_t Format, std::uint32_t Pool, IDirect3DSurface9& Surface, System::PCardinal SharedHandle) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, void**, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 36)(Self.get(), Width, Height, Format, Pool, Surface.address(), SharedHandle);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Index, pas::ComView<IDirect3DSurface9_Tag> Surface) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, void*);
        return pas::com_method<CppMethod>(Self.get(), 37)(Self.get(), Index, pas::interface_pointer(Surface));
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetRenderTarget(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Index, IDirect3DSurface9& Surface) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, void**);
        return pas::com_method<CppMethod>(Self.get(), 38)(Self.get(), Index, Surface.address());
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_BeginScene(pas::ComView<IDirect3DDevice9_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 41)(Self.get());
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_EndScene(pas::ComView<IDirect3DDevice9_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 42)(Self.get());
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_Clear(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t RectCount, Types::PRect Rects, std::uint32_t Flags, std::uint32_t Color, float Z, std::uint32_t Stencil) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, Types::PRect, std::uint32_t, std::uint32_t, float, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 43)(Self.get(), RectCount, Rects, Flags, Color, Z, Stencil);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetRenderState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t State, std::uint32_t Value) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 57)(Self.get(), State, Value);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetTexture(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Stage, pas::ComView<IDirect3DBaseTexture9_Tag> Texture) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, void*);
        return pas::com_method<CppMethod>(Self.get(), 65)(Self.get(), Stage, pas::interface_pointer(Texture));
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetTextureStageState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Stage, std::uint32_t State, std::uint32_t Value) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 67)(Self.get(), Stage, State, Value);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetSamplerState(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t Sampler, std::uint32_t State, std::uint32_t Value) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 69)(Self.get(), Sampler, State, Value);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetScissorRect(pas::ComView<IDirect3DDevice9_Tag> Self, Types::PRect Rect) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, Types::PRect);
        return pas::com_method<CppMethod>(Self.get(), 75)(Self.get(), Rect);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_GetScissorRect(pas::ComView<IDirect3DDevice9_Tag> Self, Types::TRect& Rect) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, Types::TRect*);
        return pas::com_method<CppMethod>(Self.get(), 76)(Self.get(), &Rect);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_DrawPrimitiveUP(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t PrimitiveType, std::uint32_t PrimitiveCount, void* Data, std::uint32_t Stride) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, void*, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 83)(Self.get(), PrimitiveType, PrimitiveCount, Data, Stride);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetFVF(pas::ComView<IDirect3DDevice9_Tag> Self, std::uint32_t FVF) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 89)(Self.get(), FVF);
    }

    inline std::int32_t PAS_STDCALL IDirect3DDevice9_SetVertexShader(pas::ComView<IDirect3DDevice9_Tag> Self, pas::ComView<IDirect3DVertexShader9_Tag> Shader) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*);
        return pas::com_method<CppMethod>(Self.get(), 92)(Self.get(), pas::interface_pointer(Shader));
    }

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_GetDesc(pas::ComView<IDirect3DSurface9_Tag> Self, TD3DSurfaceDesc& Desc) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, TD3DSurfaceDesc*);
        return pas::com_method<CppMethod>(Self.get(), 12)(Self.get(), &Desc);
    }

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_LockRect(pas::ComView<IDirect3DSurface9_Tag> Self, TD3DLockedRect& LockedRect, Types::PRect Rect, std::uint32_t Flags) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, TD3DLockedRect*, Types::PRect, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 13)(Self.get(), &LockedRect, Rect, Flags);
    }

    inline std::int32_t PAS_STDCALL IDirect3DSurface9_UnlockRect(pas::ComView<IDirect3DSurface9_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 14)(Self.get());
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_GetAdapterIdentifier(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Flags, TD3DAdapterIdentifier9& Identifier) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, TD3DAdapterIdentifier9*);
        return pas::com_method<CppMethod>(Self.get(), 5)(Self.get(), Adapter, Flags, &Identifier);
    }

    inline std::uint32_t PAS_STDCALL IDirect3D9_GetAdapterModeCount(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Format) {
        using CppMethod = std::uint32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 6)(Self.get(), Adapter, Format);
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_EnumAdapterModes(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t Format, std::uint32_t Mode, void* DisplayMode) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, void*);
        return pas::com_method<CppMethod>(Self.get(), 7)(Self.get(), Adapter, Format, Mode, DisplayMode);
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_GetAdapterDisplayMode(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, void* DisplayMode) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, void*);
        return pas::com_method<CppMethod>(Self.get(), 8)(Self.get(), Adapter, DisplayMode);
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_CheckDeviceMultiSampleType(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, std::uint32_t Format, std::int32_t Windowed, std::uint32_t MultiSample, System::PCardinal QualityLevels) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t, std::uint32_t, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 11)(Self.get(), Adapter, DeviceType, Format, Windowed, MultiSample, QualityLevels);
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_GetDeviceCaps(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, TD3DCaps9& Caps) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, TD3DCaps9*);
        return pas::com_method<CppMethod>(Self.get(), 14)(Self.get(), Adapter, DeviceType, &Caps);
    }

    inline std::int32_t PAS_STDCALL IDirect3D9_CreateDevice(pas::ComView<IDirect3D9_Tag> Self, std::uint32_t Adapter, std::uint32_t DeviceType, std::uint32_t FocusWindow, std::uint32_t BehaviorFlags, TD3DPresentParameters& Parameters, IDirect3DDevice9& Device) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, TD3DPresentParameters*, void**);
        return pas::com_method<CppMethod>(Self.get(), 16)(Self.get(), Adapter, DeviceType, FocusWindow, BehaviorFlags, &Parameters, Device.address());
    }

} // namespace Direct3D9
