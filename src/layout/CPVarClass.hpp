#pragma once
#include "types/CPVarClass.hpp"

namespace CPVarClass {
    static_assert(sizeof(void*) != 4 || sizeof(CPVarClass::TCPVariant) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(CPVarClass::TCPVariant, Range) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(CPVarClass::TCPVariant, FloatValue) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(CPVarClass::TCPVariant, IntValue) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(CPVarClass::TCPVariant, ValueKind) == 24);

} // namespace CPVarClass
