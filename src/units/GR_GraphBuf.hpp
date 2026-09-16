#pragma once
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GR_GraphBuf {
    void TGraphBufGR_Create(TGraphBufGR* Self, std::uint8_t AUseTexture);

    void TGraphBufGR_Destroy(TGraphBufGR* Self);

    void TGraphBufGR_DrawHorizontalLine16(TGraphBufGR* Self, std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color);

    void TGraphBufGR_DrawVerticalLine16(TGraphBufGR* Self, std::int32_t X, std::int32_t Y, std::int32_t Count, std::uint32_t Color);

    void TGraphBufGR_FillRect32(TGraphBufGR* Self, WindowsSdk::TRect Rect, std::uint32_t Color);

    void TGraphBufGR_ScaleAlpha(TGraphBufGR* Self, WindowsSdk::TRect Rect, std::uint8_t Alpha);

    void TGraphBufGR_CopyRect32(TGraphBufGR* Self, WindowsSdk::TPoint Dest, TGraphBufGR* Source, WindowsSdk::TRect Rect);

    void TGraphBufGR_BlendRect32(TGraphBufGR* Self, WindowsSdk::TPoint Dest, TGraphBufGR* Source, WindowsSdk::TRect Rect);

    void TGraphBufGR_MakeShadow(TGraphBufGR* Self);

} // namespace GR_GraphBuf
