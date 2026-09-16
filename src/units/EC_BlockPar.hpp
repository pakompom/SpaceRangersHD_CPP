#pragma once
#include "types/EC_BlockPar.hpp"

namespace EC_BlockPar {
    extern const std::uint32_t BlockDatSeedKey;

    extern const std::uint32_t BlockDatCrcKey1;

    extern const std::uint32_t BlockDatCrcKey2;

    void TBlockParElEC_Create(TBlockParElEC* Self);

    void TBlockParElEC_Destroy(TBlockParElEC* Self);

    void TBlockParEC_Create(TBlockParEC* Self);

    void TBlockParEC_Destroy(TBlockParEC* Self);

} // namespace EC_BlockPar
