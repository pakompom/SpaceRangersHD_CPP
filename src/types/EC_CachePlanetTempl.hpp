#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CachePlanetTempl {
    struct TCPlanetTemplControlEC;

    struct TCPlanetTemplEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCPlanetTemplControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCPlanetTemplControlEC, EC_Cache::TCacheControlEC, "TCPlanetTemplControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCPlanetTemplEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCPlanetTemplEC, EC_Cache::TCacheDataEC, "TCPlanetTemplEC", 40)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        void* TemplateData;
        std::int32_t ImageHeight;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CachePlanetTempl
