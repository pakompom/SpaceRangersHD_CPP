#pragma once
#include "types/GI_Line.hpp"

namespace GI_Line {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Line::TLineGI) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Line::TLineGI, Color) == 288);

} // namespace GI_Line
