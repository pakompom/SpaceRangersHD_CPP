#pragma once
#include "runtime_support.hpp"

namespace BitmapPorts {
    // Move keeps Word/Cardinal loads and stores valid for unaligned pixel rows.
    // Copy32 snapshots one complete pixel before writing it, then moves forward;
    // replacing the whole operation with Move would change overlapping-copy results.
    void MaskPixels8(void* Image, void* Mask, void* Dest, std::int32_t Columns, std::int32_t Rows,
        std::int32_t ImageSkip, std::int32_t MaskSkip, std::int32_t DestSkip, void* Table);

    void BlendPalette16(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip, void* Palette, void* Table, std::uint8_t RGB565);

    void CopyPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip);

    void BlendPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip,
        std::int32_t DestSkip, void* Table);

    void ShadowPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip);

    void SwapRedBlue32(void* Pixels, std::int32_t Count);

    void FillLine16(void* Pixels, std::int32_t Count, std::int32_t Step, std::uint32_t Color);

    void FillPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, std::uint32_t Color);

    void ScaleAlpha32(void* Alpha, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, void* Table);

} // namespace BitmapPorts
