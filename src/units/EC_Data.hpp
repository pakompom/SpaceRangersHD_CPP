#pragma once
#include "types/EC_Data.hpp"

namespace EC_Data {
    extern const EC_Data::TResourceChecksumTable ResourceChecksums;

    extern const std::uint32_t ResourceDatSeedKey;

    extern const std::uint32_t ResourceDatCrcKey1;

    extern const std::uint32_t ResourceDatCrcKey2;

    void VerifyResourceFileChecksum(const pas::WideString& FileName);

    void TDataFileEC_Create(TDataFileEC* Self);

    void TDataFileEC_Destroy(TDataFileEC* Self);

    void TDataElEC_Create(TDataElEC* Self);

    void TDataElEC_Destroy(TDataElEC* Self);

    void TDataEC_Create(TDataEC* Self);

    void TDataEC_Destroy(TDataEC* Self);

} // namespace EC_Data
