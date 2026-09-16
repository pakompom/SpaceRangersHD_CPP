#pragma once
#include "types/ParViewStringClass.hpp"

namespace ParViewStringClass {
    void TParViewString_Create(TParViewString* Self, pas::WideString Value);

    void TParViewString_Destroy(TParViewString* Self);

} // namespace ParViewStringClass
