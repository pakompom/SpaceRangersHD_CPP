#pragma once
#include "runtime_support.hpp"

namespace DirectXRenderException {
    struct EDirectXRender;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct EDirectXRender : pas::Exception {
        PAS_CLASS_META(EDirectXRender, pas::Exception, "EDirectXRender", 16)
        std::int32_t ErrorCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace DirectXRenderException
