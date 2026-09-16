#pragma once
#include "types/Direct3D9.hpp"
#include "types/GR_DX.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GR_DX {
    // DisableTextureManager setting.
    extern std::uint8_t TextureManagerDisabled;

    extern WindowsSdk::TPoint MaxTextureSize;

    extern GR_DX::TScreenVerticesGR DrawVertices;

    extern pas::DynArray<GR_DX::TScreenVertexGR> PendingPoints;

    extern pas::List* TextureCaches;

    extern std::uint32_t AvailableTextureBytes;

    extern GR_DX::TCircleTableGR CircleCos;

    extern GR_DX::TCircleTableGR CircleSin;

    extern GR_DX::TLineAlphaTableGR LineAlphaTable;

    extern std::uint32_t ReservedTextureBytes;

    extern std::int32_t TextureIdleSeconds;

    extern std::uint32_t LastTextureEvictionTick;

    extern std::int32_t PendingPointCount;

    extern std::int32_t PendingPointCapacity;

    extern std::uint32_t ResidentTextureBytes;

    void EvictTextureCaches(std::uint8_t Force);

    void SubtractResidentTextureBytes(std::uint32_t ByteCount);

    TTextureGR* CreateTextureCache();

    // Clamps each dimension to at least 16. Returns nil without a device; retries allocation after evicting textures.
    void GR_CreateTexture(std::int32_t Width, std::int32_t Height, std::uint32_t Format, std::uint32_t Pool, Direct3D9::IDirect3DTexture9& Result);

    // Accepts nil.
    void FreeTextureCache(TTextureGR* Cache);

    void ClearTexturePixels(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg);

    void ReleaseAllTextureSurfaces();

    // Counts level zero only. Native format test repeats A8R8G8B8; X8R8G8B8 is not recognized.
    std::uint32_t GetTextureByteSize(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg);

    void AddResidentTextureBytes(std::uint32_t ByteCount);

    std::uint32_t Color565ToArgb(std::uint32_t Color);

    std::uint32_t ColorWithAlpha(std::uint32_t Color, std::uint32_t Alpha);

    void CreateTextureFromPixels(std::int32_t Width, std::int32_t Height, std::uint32_t Format, void* Pixels, std::int32_t PitchBytes, std::uint32_t Pool, Direct3D9::IDirect3DTexture9& Result);

    void QueueDrawPoint(std::int32_t X, std::int32_t Y, std::uint32_t Color, std::int32_t Alpha);

    void FlushDrawPoints(WindowsSdk::PRect ClipRect);

    void DrawAlphaLine(std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect ClipRect);

    void DrawGradientLine(std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, WindowsSdk::PRect ClipRect);

    void DrawColoredTriangle(std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, std::int32_t X3, std::int32_t Y3, std::uint32_t Color3, std::uint8_t Filled, WindowsSdk::PRect ClipRect);

    void DrawColoredRect(std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::uint32_t Color, std::int32_t Alpha, std::uint8_t Filled, WindowsSdk::PRect ClipRect);

    void DrawAntialiasedCircle(std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect ClipRect);

    void DrawCircle(std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint32_t Color, std::int32_t Alpha, std::int32_t Mode, WindowsSdk::PRect ClipRect);

    void DrawTexture(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg, std::int32_t X, std::int32_t Y, std::int32_t Alpha, std::uint32_t Color, WindowsSdk::PRect ClipRect, std::uint8_t UsePreparedVertices, std::uint8_t MirrorHorizontal);

    void DrawTextureSized(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> cpp_value_arg, std::int32_t X, std::int32_t Y, std::int32_t Width, std::int32_t Height, std::int32_t Alpha, std::uint32_t Color, WindowsSdk::PRect ClipRect, std::uint8_t UsePreparedVertices, std::uint8_t MirrorHorizontal);

    // Native routine ignores the supplied clip rectangle.
    void DrawAntialiasedLineDX(std::int32_t StartX, std::int32_t StartY, std::int32_t FinishX, std::int32_t FinishY, std::uint32_t Color, std::int32_t Alpha, WindowsSdk::PRect UnusedClipRect);

    // Uses LineAlphaTable and advances phase by 20 per column. Ignores the supplied clip rectangle.
    void DrawAnimatedLineDX(std::int32_t StartX, std::int32_t StartY, std::int32_t FinishX, std::int32_t FinishY, std::uint32_t Color, std::int32_t Phase, WindowsSdk::PRect UnusedClipRect);

    void TTextureGR_Create(TTextureGR* Self);

    void TTextureGR_Destroy(TTextureGR* Self);

} // namespace GR_DX
