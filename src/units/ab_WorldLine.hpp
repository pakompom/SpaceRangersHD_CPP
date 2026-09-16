#pragma once
#include "types/EC_Struct.hpp"
#include "types/ab_WorldLine.hpp"

namespace ab_WorldLine {
    extern std::uint32_t WorldLineHeap;

    extern ab_WorldLine::PabWorldLine FirstWorldLine;

    extern ab_WorldLine::PabWorldLine LastWorldLine;

    void ab_WorldLine_Clear();

    PabWorldLine ab_WorldLine_Add();

    void ab_WorldLine_Delete(PabWorldLine Line);

    PabWorldLine ab_WorldLine_Create(EC_Struct::TVector3D First, EC_Struct::TVector3D Last, std::int32_t Kind, std::uint32_t FrontColor, std::uint32_t BackColor, std::uint8_t ShowBehindSphere);

    void ab_WorldLine_Set(PabWorldLine Line, EC_Struct::TVector3D First, EC_Struct::TVector3D Last, std::int32_t Kind, std::uint32_t FrontColor, std::uint32_t BackColor, std::uint8_t ShowBehindSphere);

    void ab_WorldLine_Update();

} // namespace ab_WorldLine
