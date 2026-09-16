#pragma once
#include "types/ParameterClass.hpp"

namespace ParameterClass {
    static_assert(sizeof(void*) != 4 || sizeof(ParameterClass::TParameter) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, MinValue) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, MaxValue) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, Value) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, NameText) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, CriticalEvent) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, CriticalEventOverride) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, CriticalOutcome) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, Hidden) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, ShowWhenZero) == 33);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, CriticalAtMinimum) == 34);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, Enabled) == 35);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, IsMoney) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, ValueText) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, ViewStrings) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, ViewStringCount) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, ViewStringCapacity) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(ParameterClass::TParameter, InitialRange) == 56);

} // namespace ParameterClass
