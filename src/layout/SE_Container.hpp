#pragma once
#include "types/SE_Container.hpp"

namespace SE_Container {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Container::TContainerSE) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Container::TContainerSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Container::TContainerSE, MinimapImagePath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Container::TContainerSE, Animation) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Container::TContainerSE, MinimapImage) == 88);

} // namespace SE_Container
