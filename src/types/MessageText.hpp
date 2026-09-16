#pragma once
#include "runtime_support.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace MessageText {
    struct TQuestMessages;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TQuestMessages : pas::Object {
        PAS_CLASS_META(TQuestMessages, pas::Object, "TQuestMessages", 8)
        void p_destroy() override;
        pas::WideString GetText(pas::WideString Path);
        pas::WideString GetTextOrKey(pas::WideString Key);
        EC_BlockPar::TBlockParEC* Entries;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace MessageText
