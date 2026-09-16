#pragma once
#include "types/EC_File.hpp"

namespace EC_File {
    static_assert(sizeof(void*) != 4 || sizeof(EC_File::TFileEC) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_File::TFileEC, Handle) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_File::TFileEC, OpenDepth) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_File::TFileEC, FileName) == 12);

} // namespace EC_File
