#pragma once
#include "types/EC_Struct.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Main {
    // Sets the shared exception-log-copy suppression flag and raises EBreakMessageGI. Placement in GI_Main is inferred.
    void BreakUiMessage();

    // Exact type-name lookup; returns nil for unknown names. Placement in GI_Main is inferred from its configuration-helper region.
    GI_MessageLoop::TObjectGI* CreateControlByName(const std::u16string_view& Name, GI_MessageLoop::TObjectGI* Owner);

    // Exact spelling required; unknown names raise.
    TImageKindXGI ParseImageKindXName(const std::u16string_view& Name);

    // Exact spelling required; unknown names raise.
    TImageKindYGI ParseImageKindYName(const std::u16string_view& Name);

    TTextAlignXGI ParseTextAlignXName(const std::u16string_view& Name);

    TTextAlignYGI ParseTextAlignYName(const std::u16string_view& Name);

    // True only for Yes, yes, True, true, TRUE or 1.
    std::uint8_t ParseEnabledNameGI(const std::u16string_view& Name);

    // At least three comma-separated components are required; only their low bytes are used.
    std::uint32_t GetColorGI(const std::u16string_view& ColorText);

    Types::TPoint GetPointGI(const std::u16string_view& PointText);

    // Comma-separated pos and size names, trimmed and case-insensitive; unknown names are ignored. Placement in GI_Main is inferred.
    std::int32_t ParseAutoGeometryFlagsGI(pas::WideString Values);

    EC_Struct::TPointF GetFloatPointGI(const std::u16string_view& PointText);

    Types::TRect GetRectGI(const std::u16string_view& RectText);

} // namespace GI_Main
