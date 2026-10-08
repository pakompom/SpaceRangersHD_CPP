#pragma once
#include "units/EC_Struct.hpp"
#include "units/aEObjInfo.hpp"

namespace aEObjInfo {
    inline void TEObjInfo_Create(TEObjInfo* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace aEObjInfo
