#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_File {
    struct TFileEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFileEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TFileEC, EC_Struct::TObjectEx, "TFileEC", 16)
        void p_destroy() override;
        void Reset();
        void SetFileName(pas::WideString NewFileName);
        void AcquireReadWriteHandle();
        void AcquireReadHandle(std::uint8_t FirstPackageOnly);
        std::uint8_t TryAcquireReadHandle(std::uint8_t FirstPackageOnly);
        void CreateNew();
        void ReleaseHandle();
        std::uint32_t GetSize();
        pas::WideString GetFileName();
        std::uint32_t SetPointer(std::uint32_t Offset, std::int32_t Origin);
        std::uint32_t GetPointer();
        void ReadBuffer(void* Dest, std::uint32_t ByteCount);
        void WriteBuffer(void* Source, std::uint32_t ByteCount);
        pas::WideString ReadWideString();
        std::int32_t Handle;
        std::int32_t OpenDepth;
        pas::WideString FileName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_File
