#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace fCustom {
    struct TfCustomLoop;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfCustomLoop : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfCustomLoop, GI_MessageLoop::TMessageLoopGI, "TfCustomLoop", 216)
        void InitializeLayout() override;
        void ProcessCallbackTimers() override;
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        pas::WideString ReservedBeforeText;
        pas::WideString ReservedAfterText;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fCustom
