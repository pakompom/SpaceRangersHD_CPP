#pragma once
#include "types/EC_Data.hpp"

namespace EC_Data {
    // Open addressing by CRC of the ASCII-folded UTF-16 filename. Each
    // stored filename character is shifted by three; zero hashes end probes.
    extern const EC_Data::TResourceChecksumTable ResourceChecksums;

    extern const std::uint32_t ResourceDatSeedKey;

    extern const std::uint32_t ResourceDatCrcKey1;

    extern const std::uint32_t ResourceDatCrcKey2;

    // Checks only names present in the built-in checksum table; folds ASCII uppercase for lookup.
    void VerifyResourceFileChecksum(const pas::WideString& FileName);

    void TDataFileEC_Create(TDataFileEC* Self);

    void TDataFileEC_Destroy(TDataFileEC* Self);

    void TDataElEC_Create(TDataElEC* Self);

    void TDataElEC_Destroy(TDataElEC* Self);

    void TDataEC_Create(TDataEC* Self);

    void TDataEC_Destroy(TDataEC* Self);

} // namespace EC_Data
