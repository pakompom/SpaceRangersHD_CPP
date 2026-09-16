#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"

namespace EC_File {
    struct TFileEC;

} // namespace EC_File

namespace EC_FileStream {
    struct TFileStreamEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFileStreamEC : EC_Thread::TThreadEC {
        PAS_CLASS_META(TFileStreamEC, EC_Thread::TThreadEC, "TFileStreamEC", 88)
        void p_destroy() override;
        void SwapBuffers();
        void virtual_TThreadEC_Execute() override;
        std::int32_t Read(void* Destination, std::int32_t ByteCount);
        std::int32_t BlockSize;
        std::int32_t BufferCapacity;
        void* ReadBuffer;
        std::int32_t ReadAvailable;
        std::int32_t ReadPosition;
        void* FillBuffer;
        std::int32_t FillAvailable;
        EC_File::TFileEC* SourceFile;
        std::int32_t FileSize;
        std::uint8_t EndOfFile;
        std::uint8_t cpp_padding[3];
        pas::CriticalSection* BufferLock;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_FileStream
