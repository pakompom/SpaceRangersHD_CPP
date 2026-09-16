#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Cache.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace GR_gi {
    struct TgiGR;

} // namespace GR_gi

namespace EC_CacheGI {
    struct TCGiControlEC;

    struct TCGiEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCGiControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCGiControlEC, EC_Cache::TCacheControlEC, "TCGiControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCGiEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCGiEC, EC_Cache::TCacheDataEC, "TCGiEC", 52)
        void p_destroy() override;
        WindowsSdk::TPoint GetTileOrigin(std::int32_t TileIndex);
        // Caches the last requested surface at index zero; non-square tile grids use an incorrect stride.
        void GetOrCreateSurface(std::int32_t SurfaceIndex, Direct3D9::IDirect3DTexture9& Result);
        // May modify SourceBuffer for resource-specific layout fixups. Ignores LoadOption.
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        // Modifies SourceBuffer in place.
        static void ApplyWideScreenLayoutFixups(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& ResourceKey);
        GR_gi::TgiGR* Image;
        GR_DX::TTextureGR* SurfaceCache;
        std::uint8_t UsesTiledSurfaces;
        std::uint8_t cpp_padding[3];
        pas::DynArray<WindowsSdk::TPoint> TileOrigins;
        std::int32_t TileCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheGI
