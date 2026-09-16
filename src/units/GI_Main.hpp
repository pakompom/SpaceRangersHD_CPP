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
    GI_MessageLoop::TObjectGI* CreateControlByName(pas::WideString Name, GI_MessageLoop::TObjectGI* Owner);

    // Exact spelling required; unknown names raise.
    TImageKindXGI ParseImageKindXName(pas::WideString Name);

    // Exact spelling required; unknown names raise.
    TImageKindYGI ParseImageKindYName(pas::WideString Name);

    TTextAlignXGI ParseTextAlignXName(pas::WideString Name);

    TTextAlignYGI ParseTextAlignYName(pas::WideString Name);

    // True only for Yes, yes, True, true, TRUE or 1.
    std::uint8_t ParseEnabledNameGI(pas::WideString Name);

    // At least three comma-separated components are required; only their low bytes are used.
    std::uint32_t GetColorGI(pas::WideString ColorText);

    Types::TPoint GetPointGI(pas::WideString PointText);

    // Comma-separated pos and size names, trimmed and case-insensitive; unknown names are ignored. Placement in GI_Main is inferred.
    std::int32_t ParseAutoGeometryFlagsGI(pas::WideString Values);

    EC_Struct::TPointF GetFloatPointGI(pas::WideString PointText);

    Types::TRect GetRectGI(pas::WideString RectText);

} // namespace GI_Main
