#pragma once
#include "types/GI_Label.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_CacheFont {
    struct TCFontEC;

} // namespace EC_CacheFont

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_Label {
    // Includes the native right/bottom padding.
    WindowsSdk::TRect MeasureLabelTextBounds(const pas::WideString& Text, const pas::WideString& FontName);

    WindowsSdk::TRect MeasureWrappedLabelBounds(std::int32_t Width, EC_Str::TStringsEC* TextLines, EC_CacheFont::TCFontEC* Font);

    void DrawWrappedLabelLines(GR_GraphBuf::TGraphBufGR* Buffer, std::int32_t Width, std::int32_t X, std::int32_t Y, EC_Str::TStringsEC* TextLines, EC_CacheFont::TCFontEC* Font);

    void RenderLabelTextToBuffer(GR_GraphBuf::TGraphBufGR* Buffer, std::int32_t Width, std::int32_t BorderWidth, std::int32_t ShadowOffset, const pas::WideString& Text, const pas::WideString& FontName, std::uint32_t TextColor, std::uint32_t BorderColor, std::uint32_t ShadowColor);

    void TLabelGI_Create(TLabelGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TLabelGI_Destroy(TLabelGI* Self);

} // namespace GI_Label
