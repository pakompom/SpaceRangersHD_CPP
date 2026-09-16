#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GR_GraphBuf.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_GraphBufPal {
    struct TGraphBufPalGR;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGraphBufPalGR : EC_Struct::TObjectEx {
        PAS_CLASS_META(TGraphBufPalGR, EC_Struct::TObjectEx, "TGraphBufPalGR", 32)
        void p_destroy() override;
        void Clear();
        void AllocateTight(std::int32_t AWidth, std::int32_t AHeight, std::int32_t APaletteCount);
        void AllocateBuffer(std::int32_t AWidth, std::int32_t AHeight, std::int32_t APaletteCount, std::int32_t APitchBytes);
        std::uint32_t GetPaletteColor(std::int32_t Index);
        void SetPalette(GR_GraphBuf::PColorRGBA Source, std::int32_t Count);
        std::uint8_t GetPixelIndex(std::int32_t X, std::int32_t Y);
        void LoadImage(EC_Buf::TBufEC* Buffer);
        void ClearPixels();
        void FillPixels(std::uint8_t Value);
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t PitchBytes;
        std::int32_t BytesPerPixel;
        void* Pixels;
        std::int32_t PaletteCount;
        GR_GraphBuf::PColorRGBA Palette;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GR_GraphBufPal
