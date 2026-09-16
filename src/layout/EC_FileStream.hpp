#pragma once
#include "types/EC_FileStream.hpp"

namespace EC_FileStream {
    static_assert(sizeof(void*) != 4 || sizeof(EC_FileStream::TFileStreamEC) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, BlockSize) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, BufferCapacity) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, ReadBuffer) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, ReadAvailable) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, ReadPosition) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, FillBuffer) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, FillAvailable) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, SourceFile) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, FileSize) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, EndOfFile) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(EC_FileStream::TFileStreamEC, BufferLock) == 84);

} // namespace EC_FileStream
