#pragma once
#include "types/GR_GraphBuf.hpp"

namespace GR_GraphBuf {
    static_assert(sizeof(void*) != 4 || sizeof(GR_GraphBuf::TGraphBufGR) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, Width) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, Height) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, PitchBytes) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, Pixels) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, StorageKind) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, BitsPerPixel) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, BytesPerPixel) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, UseTexture) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, UsesTextureStorage) == 33);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, TextureFlag22) == 34);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, Texture) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, TextureLocked) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TGraphBufGR, TextureLockedReadOnly) == 41);
    static_assert(sizeof(void*) != 4 || sizeof(GR_GraphBuf::TColorRGBA) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorRGBA, R) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorRGBA, G) == 1);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorRGBA, B) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorRGBA, A) == 3);
    static_assert(sizeof(void*) != 4 || sizeof(GR_GraphBuf::TPixelFormatGR) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, RedMask) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, GreenMask) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, BlueMask) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, AlphaMask) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, RedShift) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, GreenShift) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, BlueShift) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, AlphaShift) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, RedLevels) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, GreenLevels) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, BlueLevels) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, AlphaLevels) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, RedBits) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, GreenBits) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, BlueBits) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, AlphaBits) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, BytesPerPixel) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TPixelFormatGR, TotalChannelBits) == 72);
    static_assert(sizeof(void*) != 4 || sizeof(GR_GraphBuf::TColorBGRA) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorBGRA, B) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorBGRA, G) == 1);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorBGRA, R) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(GR_GraphBuf::TColorBGRA, A) == 3);

} // namespace GR_GraphBuf
