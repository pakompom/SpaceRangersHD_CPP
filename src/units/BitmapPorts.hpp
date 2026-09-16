#pragma once
#include "runtime_support.hpp"

namespace BitmapPorts {
    inline void MaskPixels8(void* Image, void* Mask, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t ImageSkip, std::int32_t MaskSkip, std::int32_t DestSkip, void* Table);

    inline void BlendPalette16(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip, void* Palette, void* Table, std::uint8_t RGB565);

    inline void CopyPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip);

    inline void BlendPixels32(void* Source, void* Dest, std::int32_t Columns, std::int32_t Rows, std::int32_t SourceSkip, std::int32_t DestSkip, void* Table);

    inline void ShadowPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip);

    inline void SwapRedBlue32(void* Pixels, std::int32_t Count);

    inline void FillLine16(void* Pixels, std::int32_t Count, std::int32_t Step, std::uint32_t Color);

    inline void FillPixels32(void* Pixels, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, std::uint32_t Color);

    inline void ScaleAlpha32(void* Alpha, std::int32_t Columns, std::int32_t Rows, std::int32_t RowSkip, void* Table);

} // namespace BitmapPorts

#include "inline/BitmapPorts.hpp"
