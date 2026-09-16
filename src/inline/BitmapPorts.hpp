#pragma once
#include "units/BitmapPorts.hpp"

namespace BitmapPorts {
    inline void MaskPixels8(void* Image, void* Mask, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t ImageSkip, std::int32_t MaskSkip, std::int32_t DestSkip, void* Table) {
        pas::mask_pixels8(Image, Mask, Dest, Columns, Rows, ImageSkip, MaskSkip, DestSkip, Table);
    }

    inline void BlendPalette16(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip, void* Palette, void* Table, std::uint8_t RGB565) {
        pas::blend_palette16(Source, Dest, Columns, Rows, SourceSkip, DestSkip, Palette, Table, RGB565);
    }

    inline void CopyPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip) {
        pas::copy_pixels32(Source, Dest, Columns, Rows, SourceSkip, DestSkip);
    }

    inline void BlendPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip, void* Table) {
        pas::blend_pixels32(Source, Dest, Columns, Rows, SourceSkip, DestSkip, Table);
    }

    inline void ShadowPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip) {
        pas::shadow_pixels32(Pixels, Columns, Rows, RowSkip);
    }

    inline void SwapRedBlue32(void* Pixels, std::int32_t Count) {
        pas::swap_red_blue32(Pixels, Count);
    }

    inline void FillLine16(void* Pixels, std::int32_t Count, std::int32_t Step, std::uint32_t Color) {
        pas::fill_line16(Pixels, Count, Step, Color);
    }

    inline void FillPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, std::uint32_t Color) {
        pas::fill_pixels32(Pixels, Columns, Rows, RowSkip, Color);
    }

    inline void ScaleAlpha32(void* Alpha, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, void* Table) {
        pas::scale_alpha32(Alpha, Columns, Rows, RowSkip, Table);
    }

} // namespace BitmapPorts
