#pragma once
#include "types/LocationClass.hpp"

namespace LocationClass {
    static_assert(sizeof(void*) != 4 || sizeof(LocationClass::TLocation) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, ParameterChanges) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, EditorX) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, EditorY) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, Days) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, Id) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, EventCount) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, Events) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, UseEventExpression) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, NextEventIndex) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, EventExpression) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, IsDeath) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, IsEmpty) == 45);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, IsStart) == 46);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, IsSuccess) == 47);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, IsFailure) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, VisitLimit) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, VisitCount) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(LocationClass::TLocation, Sequence) == 60);

} // namespace LocationClass
