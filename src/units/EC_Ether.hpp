#pragma once
#include "types/EC_Ether.hpp"

namespace EC_Ether {
    void TEther_Create(TEther* Self);

    // Native destructor leaves the critical section; it does not free the lock or clear entries.
    void TEther_Destroy(TEther* Self);

    // Native assembly restores EAX after loading the entry, returning Self instead of the indexed value.
    TEtherUnit* TEther_GetIndexedEntry(TEther* Self, std::int32_t Index);

} // namespace EC_Ether
