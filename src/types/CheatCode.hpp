#pragma once
#include "runtime_support.hpp"

namespace CheatCode {
    struct TCheatEntry;

    struct TCheatList;

    using PCheatEntry = TCheatEntry*;

    using TCheatCallback = pas::Proc<void()>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCheatList : pas::List {
        PAS_CLASS_META(TCheatList, pas::List, "TCheatList", 16)
        void p_destroy() override;
        std::int32_t AddEntry(PCheatEntry Entry);
        PCheatEntry GetEntry(std::int32_t Index);
        std::int32_t AddCheat(pas::WideString Text, TCheatCallback Callback);
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    struct TCheatEntry {
        pas::WideString Text;
        TCheatCallback Callback;
    };

} // namespace CheatCode
