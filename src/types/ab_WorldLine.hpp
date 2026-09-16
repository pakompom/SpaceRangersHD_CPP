#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_PolyLine.hpp"

namespace ab_WorldLine {
    struct TabWorldLine;

    using PabWorldLine = TabWorldLine*;

    #pragma pack(push, 1)
    struct TabWorldLine {
        PabWorldLine Prev;
        PabWorldLine Next;
        EC_Struct::TVector3D First;
        EC_Struct::TVector3D Last;
        GI_PolyLine::PPolyLineSegmentGI Segment;
        std::int32_t Kind;
        std::uint32_t FrontColor;
        std::uint32_t BackColor;
        std::uint32_t FrontEndColor;
        std::uint32_t BackEndColor;
        std::uint8_t ShowBehindSphere;
        std::uint8_t cpp_padding[7];
    };
    #pragma pack(pop)

} // namespace ab_WorldLine
