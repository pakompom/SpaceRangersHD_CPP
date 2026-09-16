#pragma once
#include "types/DirectXRenderException.hpp"

namespace DirectXRenderException {
    static_assert(sizeof(void*) != 4 || sizeof(DirectXRenderException::EDirectXRender) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(DirectXRenderException::EDirectXRender, ErrorCode) == 12);

} // namespace DirectXRenderException
