#pragma once
#include "types/CPDiapClass.hpp"

namespace CPDiapClass {
    static_assert(sizeof(void*) != 4 || sizeof(CPDiapClass::TCPDiapazone) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(CPDiapClass::TCPDiapazone, RangeStarts) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(CPDiapClass::TCPDiapazone, RangeEnds) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(CPDiapClass::TCPDiapazone, RangeCount) == 12);

} // namespace CPDiapClass
