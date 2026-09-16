#pragma once
#include "types/aTransport.hpp"

namespace aTransport {
    static_assert(sizeof(void*) != 4 || sizeof(aTransport::TTransport) == 1300);
    static_assert(sizeof(void*) != 4 || offsetof(aTransport::TTransport, TransportType) == 1296);

} // namespace aTransport
