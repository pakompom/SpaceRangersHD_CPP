#pragma once
#include "types/EC_Struct.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Main {
    void BreakUiMessage();

    GI_MessageLoop::TObjectGI* CreateControlByName(pas::WideString Name, GI_MessageLoop::TObjectGI* Owner);

    TImageKindXGI ParseImageKindXName(pas::WideString Name);

    TImageKindYGI ParseImageKindYName(pas::WideString Name);

    TTextAlignXGI ParseTextAlignXName(pas::WideString Name);

    TTextAlignYGI ParseTextAlignYName(pas::WideString Name);

    std::uint8_t ParseEnabledNameGI(pas::WideString Name);

    std::uint32_t GetColorGI(pas::WideString ColorText);

    Types::TPoint GetPointGI(pas::WideString PointText);

    std::int32_t ParseAutoGeometryFlagsGI(pas::WideString Values);

    EC_Struct::TPointF GetFloatPointGI(pas::WideString PointText);

    Types::TRect GetRectGI(pas::WideString RectText);

} // namespace GI_Main
