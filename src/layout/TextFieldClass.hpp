#pragma once
#include "types/TextFieldClass.hpp"

namespace TextFieldClass {
    static_assert(sizeof(void*) != 4 || sizeof(TextFieldClass::TTextField) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(TextFieldClass::TTextField, Text) == 4);

} // namespace TextFieldClass
