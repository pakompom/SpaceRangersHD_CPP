#pragma once
#include "types/EC_OKGF.hpp"

namespace EC_OKGF {
    // Accepts a ZL02 header followed by a zlib stream. Returns the decompressed byte count or zero on invalid input/decompression failure. Native DLL export uses four stack arguments and RET 16.
    std::int32_t PAS_STDCALL OKGF_ZLib_UnCompress2(void* Dest, std::int32_t DestCapacity, void* Source, std::int32_t SourceSize);

} // namespace EC_OKGF
