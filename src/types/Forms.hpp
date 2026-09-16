#pragma once
#include "runtime_support.hpp"
#include "types/Classes.hpp"

namespace Forms {
    struct TApplication;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TApplication : pas::Object {
        PAS_CLASS_META(TApplication, pas::Object, "TApplication", 24)
        static void Initialize();
        std::uint8_t GetActive();
        std::uint32_t Handle;
        Classes::TNotifyEvent OnActivate;
        Classes::TNotifyEvent OnDeactivate;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace Forms
