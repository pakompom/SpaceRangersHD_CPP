#pragma once
#include "types/fChameleon.hpp"

namespace fChameleon {
    static_assert(sizeof(void*) != 4 || sizeof(fChameleon::TfChameleon) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fChameleon::TfChameleon, ChameleonActive) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fChameleon::TfChameleon, VisualType) == 209);
    static_assert(sizeof(void*) != 4 || offsetof(fChameleon::TfChameleon, Charges) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fChameleon::TfChameleon, Choice) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fChameleon::TfChameleon, ChoiceImages) == 228);

} // namespace fChameleon
