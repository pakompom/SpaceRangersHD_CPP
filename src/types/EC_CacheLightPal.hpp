#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheLightPal {
    struct TCLightPalControlEC;

    struct TCLightPalEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCLightPalControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCLightPalControlEC, EC_Cache::TCacheControlEC, "TCLightPalControlEC", 24)
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
    struct TCLightPalEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCLightPalEC, EC_Cache::TCacheDataEC, "TCLightPalEC", 36)
        void p_destroy() override;
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        WindowsSdk::PWORD PaletteData;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_CacheLightPal
