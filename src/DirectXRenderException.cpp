#include "layout/DirectXRenderException.hpp"
#include "units/DirectXRenderException.hpp"
#include "units/GR_Main.hpp"

namespace DirectXRenderException {
    void EDirectXRender_Create(EDirectXRender* Self, pas::AnsiString Message) {
        pas::exception_create(Self, Message);
    }

    void EDirectXRender_CreateCode(EDirectXRender* Self, pas::AnsiString Message, std::int32_t Code) {
        pas::exception_create(Self, pas::concat_ansi({Message, " = ", GR_Main::Direct3DErrorText(Code)}));
        Self->ErrorCode = Code;
    }

} // namespace DirectXRenderException
