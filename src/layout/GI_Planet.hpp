#pragma once
#include "types/GI_Planet.hpp"

namespace GI_Planet {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Planet::TPlanetGI) == 428);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, TemplateCache) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, SurfaceImageCache) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, SurfacePaletteCache) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud1ImageCache) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud1PaletteCache) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud2ImageCache) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud2PaletteCache) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud3ImageCache) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud3PaletteCache) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, LightRotationCache) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereRotationCache) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereBuffer) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereImageCache) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereMaskCache) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereColor) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereDirty) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmospherePaletteDirty) == 349);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, MapWidthMask) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, SurfaceMapOffset) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud1MapOffset) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud2MapOffset) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, Cloud3MapOffset) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, RenderedMapOffsets) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, SourceLightBuffer) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, RotatedLightBuffer) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, LightAngle) == 396);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, MapWidth) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, MapHeight) == 404);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, TextureCache) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, TextureSize) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Planet::TPlanetGI, AtmosphereTextureSize) == 420);

} // namespace GI_Planet
