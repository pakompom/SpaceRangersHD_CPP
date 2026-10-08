#pragma once
#include "units/EC_Struct.hpp"
#include "units/aGalaxy.hpp"

namespace aGalaxy {
    inline void TCustomSystemInfo_Create(TCustomSystemInfo* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TInterfaceStateOverride_Create(TInterfaceStateOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TInterfaceTextOverride_Create(TInterfaceTextOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TInterfaceImageOverride_Create(TInterfaceImageOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TInterfacePosOverride_Create(TInterfacePosOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    inline void TInterfaceSizeOverride_Create(TInterfaceSizeOverride* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

} // namespace aGalaxy
