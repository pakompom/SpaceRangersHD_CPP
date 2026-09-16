#pragma once
#include "types/GI_Window.hpp"

namespace GI_Window {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Window::TWindowGI) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, LeftImage) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, RightImage) == 324);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, TopImage) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, BottomImage) == 332);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, TopLeftImage) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, TopRightImage) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, BottomLeftImage) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, BottomRightImage) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, TextureImage) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, WorkSubRect) == 356);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Window::TWindowGI, MinimumSize) == 372);

} // namespace GI_Window
