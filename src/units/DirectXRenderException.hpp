#pragma once
#include "types/DirectXRenderException.hpp"

namespace DirectXRenderException {
    void EDirectXRender_Create(EDirectXRender* Self, pas::AnsiString Message);

    void EDirectXRender_CreateCode(EDirectXRender* Self, pas::AnsiString Message, std::int32_t Code);

} // namespace DirectXRenderException
