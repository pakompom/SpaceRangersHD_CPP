#include "layout/EC_FileStream.hpp"
#include "units/EC_File.hpp"
#include "units/EC_FileStream.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Thread.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/Windows.hpp"

namespace EC_FileStream {
    void TFileStreamEC_Create(TFileStreamEC* Self, std::int32_t BufferBytes, const pas::WideString& FileName) {
        EC_Thread::TThreadEC_Create(Self);
        Self->BufferLock = pas::make_critical_section<pas::CriticalSection>();
        Self->SourceFile = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        Self->SourceFile->SetFileName(FileName);
        Self->SourceFile->AcquireReadHandle(false);
        Self->EndOfFile = false;
        Self->FileSize = Self->SourceFile->GetSize();
        Self->BlockSize = 4096;
        Self->BufferCapacity = pas::idiv(BufferBytes, Self->BlockSize) * Self->BlockSize + Self->BlockSize;
        Self->ReadBuffer = EC_Mem::AllocEC(Self->BufferCapacity);
        Self->FillBuffer = EC_Mem::AllocEC(Self->BufferCapacity);
        Self->SetPriority(EC_Thread::ThreadPriorityLowest);
        Self->Start();
    }

    void TFileStreamEC_Destroy(TFileStreamEC* Self) {
        Self->RequestStop();
        if (Self->IsRunning()) {
            Self->WaitForIdle(WindowsSdk::INFINITE);
        }
        if (Self->SourceFile != nullptr) {
            pas::free(Self->SourceFile);
            Self->SourceFile = nullptr;
        }
        if (Self->ReadBuffer != nullptr) {
            EC_Mem::FreeEC(Self->ReadBuffer);
            Self->ReadBuffer = nullptr;
        }
        if (Self->FillBuffer != nullptr) {
            EC_Mem::FreeEC(Self->FillBuffer);
            Self->FillBuffer = nullptr;
        }
        Self->BlockSize = 0;
        Self->BufferCapacity = 0;
        if (Self->BufferLock != nullptr) {
            pas::free(Self->BufferLock);
            Self->BufferLock = nullptr;
        }
        EC_Thread::TThreadEC_Destroy(Self);
    }

    void TFileStreamEC::SwapBuffers() {
        void* Buffer = ReadBuffer;
        ReadBuffer = FillBuffer;
        FillBuffer = Buffer;
        std::int32_t Available = ReadAvailable;
        ReadAvailable = FillAvailable;
        FillAvailable = Available;
        ReadPosition = 0;
    }

    void TFileStreamEC_Execute(TFileStreamEC* Self) {
        std::int32_t ByteCount{};
        while (static_cast<std::uint8_t>(Self->IsStopRequested() ^ 1) && static_cast<std::uint8_t>(Self->EndOfFile ^ 1)) {
            if (Self->FillAvailable >= Self->BufferCapacity) {
                pas::critical_enter(Self->BufferLock);
                if (Self->ReadAvailable > 0) {
                    pas::critical_leave(Self->BufferLock);
                    return;
                }
                Self->SwapBuffers();
                pas::critical_leave(Self->BufferLock);
            }
            ByteCount = Self->BlockSize;
            {
                std::int32_t cpp_right = Self->SourceFile->GetPointer();
                if (Self->FileSize - cpp_right < ByteCount) {
                    std::int32_t cpp_right_2 = Self->SourceFile->GetPointer();
                    ByteCount = Self->FileSize - cpp_right_2;
                }
            }
            if (ByteCount <= 0) {
                Self->EndOfFile = true;
                return;
            }
            Self->SourceFile->ReadBuffer(EC_Mem::AddPointerOffset(Self->FillBuffer, Self->FillAvailable), ByteCount);
            Self->FillAvailable += ByteCount;
            {
                std::int32_t cpp_left = Self->SourceFile->GetPointer();
                if (cpp_left > Self->FileSize) {
                    Self->EndOfFile = true;
                }
            }
            SysUtilsImports::Sleep(0u);
        }
    }

    std::int32_t TFileStreamEC::Read(void* Destination, std::int32_t ByteCount) {
        std::int32_t Total = 0;
        pas::critical_enter(BufferLock);
        std::int32_t Chunk = ByteCount;
        if (Chunk > ReadAvailable) {
            Chunk = ReadAvailable;
        }
        if (Chunk > 0) {
            Windows::CopyMemory(Destination, EC_Mem::AddPointerOffset(ReadBuffer, ReadPosition), Chunk);
            ReadPosition += Chunk;
            ReadAvailable -= Chunk;
            Destination = EC_Mem::AddPointerOffset(Destination, Chunk);
            ByteCount -= Chunk;
            Total += Chunk;
        }
        pas::critical_leave(BufferLock);
        if (ByteCount <= 0) {
            return Total;
        }
        if (IsRunning()) {
            RequestStop();
            WaitForIdle(WindowsSdk::INFINITE);
        }
        while (true) {
            Chunk = ByteCount;
            if (Chunk > ReadAvailable) {
                Chunk = ReadAvailable;
            }
            if (Chunk > 0) {
                Windows::CopyMemory(Destination, EC_Mem::AddPointerOffset(ReadBuffer, ReadPosition), Chunk);
                ReadPosition += Chunk;
                ReadAvailable -= Chunk;
                Destination = EC_Mem::AddPointerOffset(Destination, Chunk);
                ByteCount -= Chunk;
                Total += Chunk;
            }
            if (ByteCount <= 0) {
                break;
            }
            SwapBuffers();
            Chunk = ByteCount;
            if (Chunk > ReadAvailable) {
                Chunk = ReadAvailable;
            }
            if (Chunk > 0) {
                Windows::CopyMemory(Destination, EC_Mem::AddPointerOffset(ReadBuffer, ReadPosition), Chunk);
                ReadPosition += Chunk;
                ReadAvailable -= Chunk;
                Destination = EC_Mem::AddPointerOffset(Destination, Chunk);
                ByteCount -= Chunk;
                Total += Chunk;
            }
            if (ByteCount <= 0 || EndOfFile) {
                break;
            }
            Chunk = BufferCapacity;
            {
                std::int32_t cpp_right = SourceFile->GetPointer();
                if (FileSize - cpp_right < Chunk) {
                    std::int32_t cpp_right_2 = SourceFile->GetPointer();
                    Chunk = FileSize - cpp_right_2;
                }
            }
            if (Chunk <= 0) {
                EndOfFile = true;
                break;
            }
            SourceFile->ReadBuffer(EC_Mem::AddPointerOffset(FillBuffer, FillAvailable), Chunk);
            FillAvailable += Chunk;
            {
                std::int32_t cpp_left = SourceFile->GetPointer();
                if (cpp_left > FileSize) {
                    EndOfFile = true;
                }
            }
        }
        if (!EndOfFile) {
            Start();
        }
        return Total;
    }

    void TFileStreamEC::p_destroy() {
        EC_FileStream::TFileStreamEC_Destroy(this);
    }

    void TFileStreamEC::virtual_TThreadEC_Execute() {
        EC_FileStream::TFileStreamEC_Execute(this);
    }

} // namespace EC_FileStream
