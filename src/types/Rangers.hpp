#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"

namespace Rangers {
    struct TAD;

    struct TSteamCallbacksThread;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TAD : pas::Object {
        PAS_CLASS_META(TAD, pas::Object, "TAD", 4)
        static void ApplicationActivated(pas::Object* Sender);
        static void ApplicationDeactivated(pas::Object* Sender);
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TWineGetHostVersion = pas::StdcallProc<void(std::uint8_t*&, std::uint8_t*&)>;

    using TWineGetBuildId = pas::StdcallProc<std::uint8_t*()>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSteamCallbacksThread : EC_Thread::TThreadEC {
        PAS_CLASS_META(TSteamCallbacksThread, EC_Thread::TThreadEC, "TSteamCallbacksThread", 44)
        void virtual_TThreadEC_Execute() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace Rangers
