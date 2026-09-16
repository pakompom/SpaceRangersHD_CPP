#pragma once
#include "types/GI_Cursor.hpp"

namespace GI_Cursor {
    static_assert(sizeof(void*) != 4 || sizeof(GI_Cursor::TCursorGI) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, ImageControl) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, ImagePath) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, CursorHandles) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, FrameIndices) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, FrameDelays) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, FrameIndex) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_Cursor::TCursorGI, AnimationTimer) == 312);

} // namespace GI_Cursor
