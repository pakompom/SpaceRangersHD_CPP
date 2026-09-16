#pragma once
#include "types/SequenceClass.hpp"

namespace SequenceClass {
    void TSequence_Create(TSequence* Self);

    // Clears member Sequence links without freeing the members.
    void TSequence_Destroy(TSequence* Self);

} // namespace SequenceClass
