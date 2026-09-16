#pragma once
#include "types/EC_Data.hpp"

namespace EC_Data {
    static_assert(sizeof(void*) != 4 || sizeof(EC_Data::TDataEC) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, FileLock) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, SharesInternedFileList) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, InternedFileListHeadRef) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, InternedFileListTailRef) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, OwnedInternedFileListHead) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, OwnedInternedFileListTail) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, FirstEntry) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, LastEntry) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, IndexedEntries) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataEC, IndexedEntryCount) == 40);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Data::TDataFileEC) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataFileEC, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataFileEC, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataFileEC, FileRef) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Data::TDataElEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, Name) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, Kind) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, ChildData) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, SharedFileRef) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, FileOffset) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::TDataElEC, ByteCount) == 32);
    static_assert(sizeof(void*) != 4 || sizeof(EC_Data::tclist) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::tclist, NameCrc) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::tclist, FileCrc) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_Data::tclist, EncodedName) == 8);

} // namespace EC_Data
