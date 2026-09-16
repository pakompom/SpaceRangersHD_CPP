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
        // Closes even when OpenDepth is nonzero.
        void Reset();
        // Closes the current entry regardless of OpenDepth.
        void SetFileName(pas::WideString NewFileName);
        // Nested acquisitions reuse the existing handle and access mode.
        // Opening converts FileName to ANSI for the package collection.
        void AcquireReadWriteHandle();
        void AcquireReadHandle(std::uint8_t FirstPackageOnly);
        // False when the package lookup cannot open the file. A successful call acquires one handle reference, including when already open.
        std::uint8_t TryAcquireReadHandle(std::uint8_t FirstPackageOnly);
        // Truncates an existing file; ignores prior OpenDepth and leaves it at one.
        void CreateNew();
        // FileName is retained after closing.
        void ReleaseHandle();
        // If closed, opens for read/write and releases that acquisition on success.
        std::uint32_t GetSize();
        pas::WideString GetFileName();
        // Requires an open entry. Origin 0 is absolute, 1 adds Offset to the current position, 2 subtracts Offset from the size; returns the new position.
        std::uint32_t SetPointer(std::uint32_t Offset, std::int32_t Origin);
        // Returns 0xFFFFFFFF when no entry is open.
        std::uint32_t GetPointer();
        // Requires an open entry; raises on backend read failure, including short uncompressed reads.
        void ReadBuffer(void* Dest, std::uint32_t ByteCount);
        // Requires an open writable uncompressed entry; raises on backend failure or a short write. Zero count does nothing.
        void WriteBuffer(void* Source, std::uint32_t ByteCount);
        // Consumes UTF-16 code units through the terminating NUL; requires an open entry.
        pas::WideString ReadWideString();
        // Package index * 16 + open-entry slot; -1 while closed.
        std::int32_t Handle;
        std::int32_t OpenDepth;
        pas::WideString FileName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_File
