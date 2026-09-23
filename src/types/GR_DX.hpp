#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"

namespace GR_DX {
    struct TScreenVertexGR;

    struct TTextureGR;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextureGR : pas::Object {
        PAS_CLASS_META(TTextureGR, pas::Object, "TTextureGR", 20)
        void p_destroy() override;
        void Clear();
        // Retains the array and SurfaceCount.
        void ReleaseSurfaces();
        // Returns nil when out of range; successful access refreshes LastUseTick.
        void GetSurface(std::int32_t Index, Direct3D9::IDirect3DTexture9& Result);
        // A negative index appends; indexes beyond the end create nil holes.
        void SetSurface(pas::ComView<Direct3D9::IDirect3DTexture9_Tag> Value, std::int32_t Index);
        std::uint32_t LastUseTick;
        std::int32_t SurfaceCount;
        // Delphi dynamic array of reference-counted surface interfaces.
        pas::DynArray<Direct3D9::IDirect3DTexture9> Surfaces;
        std::uint32_t ResidentBytes;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TScreenVertexGR {
        float X;
        float Y;
        float Z;
        float RHW;
        std::uint32_t Color;
        float U;
        float V;
    };
    #pragma pack(pop)

    using TScreenVerticesGR = pas::Array<TScreenVertexGR, 0, 15>;

    using TCircleTableGR = pas::Array<float, 0, 360>;

    using TLineAlphaTableGR = pas::Array<std::uint8_t, 0, 359>;

    // White texture tint; alpha is supplied separately.
    inline constexpr std::int32_t RgbWhite = 0x00ffffff;

} // namespace GR_DX
