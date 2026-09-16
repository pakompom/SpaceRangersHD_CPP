#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace SE_SoundRnd {
    struct TSoundRndUnitSE;

    struct TSoundRndSE;

    struct TSoundRndUnitSE {
        std::int32_t Weight;
        std::int32_t Group;
        std::int32_t NextTimeMin;
        std::int32_t NextTimeMax;
        pas::DynArray<pas::WideString> SoundNames;
        pas::DynArray<std::int32_t> SoundWeights;
        std::int32_t TotalSoundWeight;
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSoundRndSE : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSoundRndSE, EC_Struct::TObjectEx, "TSoundRndSE", 24)
        void p_destroy() override;
        void Clear();
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        pas::WideString SelectSound(std::int32_t GroupIndex);
        TSoundRndSE* Prev;
        TSoundRndSE* Next;
        pas::WideString Name;
        pas::DynArray<TSoundRndUnitSE> Groups;
        std::int32_t TotalGroupWeight;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_SoundRnd
