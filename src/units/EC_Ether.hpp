#pragma once
#include "types/EC_Ether.hpp"

namespace EC_Ether {
    void TEther_Create(TEther* Self);

    // Native destructor leaves the critical section; it does not free the lock or clear entries.
    void TEther_Destroy(TEther* Self);

} // namespace EC_Ether
