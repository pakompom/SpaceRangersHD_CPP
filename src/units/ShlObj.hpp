#pragma once
#include "types/ShlObj.hpp"

namespace ShlObj {
    std::int32_t PAS_STDCALL SHGetSpecialFolderLocation(std::uint32_t Window, std::int32_t Folder, PItemIDList& ItemIdList);

    std::int32_t PAS_STDCALL SHGetPathFromIDListA(PItemIDList ItemIdList, std::uint8_t* Path);

} // namespace ShlObj
