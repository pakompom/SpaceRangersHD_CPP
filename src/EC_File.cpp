#include "layout/EC_File.hpp"
#include "units/EC_File.hpp"
#include "units/EC_HsFile.hpp"
#include "units/EC_Struct.hpp"
#include "units/WindowsImports.hpp"

namespace EC_File {
    void TFileEC_Create(TFileEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Handle = -1;
    }

    void TFileEC_Destroy(TFileEC* Self) {
        Self->Reset();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TFileEC::Reset() {
        if (Handle != -1) {
            pas::critical_enter(EC_HsFile::PackageFileLock);
            EC_HsFile::PackageCollection->CloseEntryHandle(Handle);
            pas::critical_leave(EC_HsFile::PackageFileLock);
        }
        OpenDepth = 0;
        Handle = -1;
        FileName = pas::WideString();
    }

    void TFileEC::SetFileName(pas::WideString NewFileName) {
        Reset();
        FileName = std::move(NewFileName);
    }

    void TFileEC::AcquireReadWriteHandle() {
        if (OpenDepth == 0) {
            pas::critical_enter(EC_HsFile::PackageFileLock);
            Handle = EC_HsFile::PackageCollection->OpenEntryByPathAcrossPackages(static_cast<pas::AnsiString>(FileName), WindowsImports::GENERIC_READ | WindowsImports::GENERIC_WRITE, false);
            pas::critical_leave(EC_HsFile::PackageFileLock);
            if (Handle == -1) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.Open. FileName=", FileName}))));
            }
        }
        ++OpenDepth;
    }

    void TFileEC::AcquireReadHandle(std::uint8_t FirstPackageOnly) {
        if (OpenDepth == 0) {
            pas::critical_enter(EC_HsFile::PackageFileLock);
            Handle = EC_HsFile::PackageCollection->OpenEntryByPathAcrossPackages(static_cast<pas::AnsiString>(FileName), WindowsImports::GENERIC_READ, FirstPackageOnly);
            pas::critical_leave(EC_HsFile::PackageFileLock);
            if (Handle == -1) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.Open. FileName=", FileName}))));
            }
        }
        ++OpenDepth;
    }

    std::uint8_t TFileEC::TryAcquireReadHandle(std::uint8_t FirstPackageOnly) {
        if (OpenDepth == 0) {
            pas::critical_enter(EC_HsFile::PackageFileLock);
            Handle = EC_HsFile::PackageCollection->OpenEntryByPathAcrossPackages(static_cast<pas::AnsiString>(FileName), WindowsImports::GENERIC_READ, FirstPackageOnly);
            pas::critical_leave(EC_HsFile::PackageFileLock);
            if (Handle == -1) {
                return false;
            }
        }
        ++OpenDepth;
        return true;
    }

    void TFileEC::CreateNew() {
        OpenDepth = 1;
        ReleaseHandle();
        pas::critical_enter(EC_HsFile::PackageFileLock);
        Handle = EC_HsFile::PackageCollection->CreateLooseFile(FileName);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        if (Handle == -1) {
            Handle = -1;
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.CreateNew. FileName=", FileName}))));
        }
        OpenDepth = 1;
    }

    void TFileEC::ReleaseHandle() {
        --OpenDepth;
        if (OpenDepth <= 0) {
            if (Handle != -1) {
                pas::critical_enter(EC_HsFile::PackageFileLock);
                EC_HsFile::PackageCollection->CloseEntryHandle(Handle);
                pas::critical_leave(EC_HsFile::PackageFileLock);
            }
            Handle = -1;
            OpenDepth = 0;
        }
    }

    std::uint32_t TFileEC::GetSize() {
        AcquireReadWriteHandle();
        pas::critical_enter(EC_HsFile::PackageFileLock);
        std::uint32_t Size = EC_HsFile::PackageCollection->GetEntryHandleSize(Handle);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        if (Size == 0xffffffffu) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.GetSize. FileName=", FileName}))));
        }
        ReleaseHandle();
        return Size;
    }

    pas::WideString TFileEC::GetFileName() {
        return FileName;
    }

    std::uint32_t TFileEC::SetPointer(std::uint32_t Offset, std::int32_t Origin) {
        pas::critical_enter(EC_HsFile::PackageFileLock);
        std::uint8_t Success = EC_HsFile::PackageCollection->SeekEntryHandle(Handle, Offset, Origin);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        if (!Success) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.SetPointer. FileName=", FileName}))));
        }
        pas::critical_enter(EC_HsFile::PackageFileLock);
        std::uint32_t Result = EC_HsFile::PackageCollection->GetEntryHandlePosition(Handle);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        return Result;
    }

    std::uint32_t TFileEC::GetPointer() {
        pas::critical_enter(EC_HsFile::PackageFileLock);
        std::uint32_t Result = EC_HsFile::PackageCollection->GetEntryHandlePosition(Handle);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        return Result;
    }

    void TFileEC::ReadBuffer(void* Dest, std::uint32_t ByteCount) {
        pas::critical_enter(EC_HsFile::PackageFileLock);
        std::uint8_t Success = EC_HsFile::PackageCollection->ReadEntryHandle(Handle, Dest, ByteCount);
        pas::critical_leave(EC_HsFile::PackageFileLock);
        if (!Success) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.Read. FileName=", FileName, u" kolbyte=", pas::wide_int64_to_str(static_cast<std::int64_t>(ByteCount)), u" GetLastError=", pas::wide_int64_to_str(static_cast<std::int64_t>(WindowsImports::GetLastError()))}))));
        }
    }

    void TFileEC::WriteBuffer(void* Source, std::uint32_t ByteCount) {
        std::uint8_t Success{};
        if (ByteCount > 0) {
            pas::critical_enter(EC_HsFile::PackageFileLock);
            Success = EC_HsFile::PackageCollection->WriteEntryHandle(Handle, Source, ByteCount);
            pas::critical_leave(EC_HsFile::PackageFileLock);
            if (!Success) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TFileEC.Write. FileName=", FileName, u" kolbyte=", pas::wide_int64_to_str(static_cast<std::int64_t>(ByteCount))}))));
            }
        }
    }

    pas::WideString TFileEC::ReadWideString() {
        pas::WideString Result{};
        char16_t Ch{};
        while (true) {
            ReadBuffer(&Ch, static_cast<std::int32_t>(sizeof(char16_t)));
            if (Ch == u'\000') {
                break;
            }
            Result = pas::concat_wide({Result, Ch});
        }
        return Result;
    }

    void TFileEC::p_destroy() {
        EC_File::TFileEC_Destroy(this);
    }

} // namespace EC_File
