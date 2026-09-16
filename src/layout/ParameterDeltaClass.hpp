#pragma once
#include "types/ParameterDeltaClass.hpp"

namespace ParameterDeltaClass {
    static_assert(sizeof(void*) != 4 || sizeof(ParameterDeltaClass::TParameterDelta) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, ParameterIndex) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, ValueConstraint) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, MultipleConstraint) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, MinValue) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, MaxValue) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, ChangeValue) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, ChangeByPercent) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, SetValue) == 29);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, UseExpression) == 30);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, ExpressionText) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, CriticalEvent) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, VisibilityChange) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterDeltaClass::TParameterDelta, LegacyFlag) == 44);

} // namespace ParameterDeltaClass
