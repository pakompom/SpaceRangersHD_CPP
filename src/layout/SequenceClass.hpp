#pragma once
#include "types/SequenceClass.hpp"

namespace SequenceClass {
    static_assert(sizeof(void*) != 4 || sizeof(SequenceClass::TSequence) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SequenceClass::TSequence, UnknownFlag) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SequenceClass::TSequence, TraversalLimit) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SequenceClass::TSequence, Locations) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SequenceClass::TSequence, Paths) == 16);

} // namespace SequenceClass
