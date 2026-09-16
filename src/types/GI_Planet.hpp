#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheBitmap {
    struct TCBitmapControlEC;

} // namespace EC_CacheBitmap

namespace EC_CacheLightPal {
    struct TCLightPalControlEC;

} // namespace EC_CacheLightPal

namespace EC_CachePalBitmap {
    struct TCPalBitmapControlEC;

} // namespace EC_CachePalBitmap

namespace EC_CachePlanetTempl {
    struct TCPlanetTemplControlEC;

} // namespace EC_CachePlanetTempl

namespace EC_CacheRotateBuf {
    struct TCRotateBufControlEC;

} // namespace EC_CacheRotateBuf

namespace GR_DX {
    struct TTextureGR;

} // namespace GR_DX

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GR_GraphBufPal {
    struct TGraphBufPalGR;

} // namespace GR_GraphBufPal

namespace GI_Planet {
    struct TPlanetGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanetGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TPlanetGI, GI_MessageLoop::TObjectGI, "TPlanetGI", 428)
        void p_destroy() override;
        // Preserves image caches, atmosphere storage and texture cache.
        void Clear() override;
        // Image width must be a power of two from 16 through 2048; height must not exceed half the width. The light map must cover that height on both axes.
        void SetImage(const pas::WideString& MaskPath, const pas::WideString& ImagePath, const pas::WideString& LightMapPath);
        // Requires the same dimensions as the surface map.
        void SetCloud1Image(const pas::WideString& Path);
        // Requires the same dimensions as the surface map.
        void SetCloud2Image(const pas::WideString& Path);
        // Requires the same dimensions as the surface map.
        void SetCloud3Image(const pas::WideString& Path);
        // Uses a diameter of 2*Radius+1. Applies the surface-map dimension checks but omits the light-map size check.
        void SetImageWithRadius(const pas::WideString& MaskPath, const pas::WideString& ImagePath, const pas::WideString& LightMapPath, std::int32_t Radius);
        // Uses a diameter of 2*Radius. Reuses an existing surface image and light buffers; ImagePath is used only when the surface cache key is empty.
        void SetImageFromTemplate(const pas::WideString& TemplateKey, const pas::WideString& ImagePath, std::int32_t Radius);
        // Appends ?Gray to both paths; Color is 0x00BBGGRR. Zero suppresses atmosphere drawing.
        void SetAtmosphere(pas::WideString ImagePath, pas::WideString MaskPath, std::uint32_t Color);
        // Requires space for 256 colors. RGB channels and the alpha ramp are truncated to multiples of eight.
        void BuildAtmospherePalette();
        void SetSurfaceMapOffset(std::int32_t Value);
        void SetCloud1MapOffset(std::int32_t Value);
        void SetCloud2MapOffset(std::int32_t Value);
        void SetCloud3MapOffset(std::int32_t Value);
        // A full turn has 256 steps; requires initialized light buffers when the angle changes.
        void SetLightAngle(std::uint8_t Value);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // The native routine does not release its third cloud layer's cache acquisitions.
        void Draw(Types::TRect ClipRect) override;
        // Resizes and clears Buffer; excludes clouds and atmosphere.
        void RenderSurfaceToBuffer(GR_GraphBuf::TGraphBufGR* Buffer);
        // Queues the template, surface, surface palette and light rotation only.
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CachePlanetTempl::TCPlanetTemplControlEC* TemplateCache;
        EC_CachePalBitmap::TCPalBitmapControlEC* SurfaceImageCache;
        EC_CacheLightPal::TCLightPalControlEC* SurfacePaletteCache;
        EC_CachePalBitmap::TCPalBitmapControlEC* Cloud1ImageCache;
        EC_CacheLightPal::TCLightPalControlEC* Cloud1PaletteCache;
        EC_CachePalBitmap::TCPalBitmapControlEC* Cloud2ImageCache;
        EC_CacheLightPal::TCLightPalControlEC* Cloud2PaletteCache;
        EC_CachePalBitmap::TCPalBitmapControlEC* Cloud3ImageCache;
        EC_CacheLightPal::TCLightPalControlEC* Cloud3PaletteCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* LightRotationCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* AtmosphereRotationCache;
        GR_GraphBufPal::TGraphBufPalGR* AtmosphereBuffer;
        EC_CacheBitmap::TCBitmapControlEC* AtmosphereImageCache;
        EC_CacheBitmap::TCBitmapControlEC* AtmosphereMaskCache;
        std::uint32_t AtmosphereColor;
        std::uint8_t AtmosphereDirty;
        std::uint8_t AtmospherePaletteDirty;
        std::uint8_t cpp_padding[2];
        std::int32_t MapWidthMask;
        std::int32_t SurfaceMapOffset;
        std::int32_t Cloud1MapOffset;
        std::int32_t Cloud2MapOffset;
        std::int32_t Cloud3MapOffset;
        pas::Array<std::int32_t, 0, 3> RenderedMapOffsets;
        // Opaque OKGR light buffers, released by Clear.
        void* SourceLightBuffer;
        void* RotatedLightBuffer;
        std::uint8_t LightAngle;
        std::uint8_t cpp_padding_2[3];
        std::int32_t MapWidth;
        std::int32_t MapHeight;
        GR_DX::TTextureGR* TextureCache;
        Types::TPoint TextureSize;
        Types::TPoint AtmosphereTextureSize;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Planet
