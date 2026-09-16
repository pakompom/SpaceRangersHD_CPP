#pragma once
#include "types/EC_OKGF.hpp"

namespace EC_OKGF {
    std::int32_t PAS_STDCALL OKGF_ZLib_UnCompress2(void* Dest, std::int32_t DestCapacity, void* Source, std::int32_t SourceSize);

} // namespace EC_OKGF
