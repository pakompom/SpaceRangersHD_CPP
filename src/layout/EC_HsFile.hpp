#pragma once
#include "types/EC_HsFile.hpp"

namespace EC_HsFile {
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::TPackCollectionEC) == 532);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackCollectionEC, FirstPack) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackCollectionEC, LastPack) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackCollectionEC, NameToPackIndexHash) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackCollectionEC, UseFastNameIndex) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackCollectionEC, PackByIndex) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::TPackOpenSlotEC) == 30);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, FileHandle) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, IsAvailable) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, DataStartOffset) == 5);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, CurrentDataOffset) == 9);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, DataSize) == 13);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, CompressedBlockBuffer) == 17);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, DecompressedBlockBuffer) == 21);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, UsesChainedBlocks) == 25);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackOpenSlotEC, CurrentBlockIndex) == 26);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::TPackFileEC) == 528);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, NextPack) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, PrevPack) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, UseLooseFiles) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, PackageHandle) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, PackagePath) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, RootFolder) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, OpenSlots) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, RootSubtreeOffset) == 508);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackFileEC, CollectionIndex) == 524);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::THsFolderEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, UpperName) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, OriginalName) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, HeaderSize) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, EntryCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, EntryRecordSize) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, Parent) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, EntryBuffer) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, ChangedFlag) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THsFolderEC, InitializedEmptyFlag) == 33);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::TPackEntryEC) == 158);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, StoredSize) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, DataSize) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, UpperName) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, OriginalName) == 71);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, Kind) == 134);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, KindCopy) == 138);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, Flags) == 142);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, TargetOffset) == 150);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::TPackEntryEC, ChildFolder) == 154);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::THashSlotEC) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashSlotEC, FullHash) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashSlotEC, MappedValue) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashSlotEC, HitCount) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashSlotEC, Unknown0C) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashSlotEC, KeySuffix) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(EC_HsFile::THashEC) == 53276);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, OperationCount) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, HitCount) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, HitCountCopy) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, StaleValueCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, MissCount) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, ReservedText) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_HsFile::THashEC, Slots) == 28);

} // namespace EC_HsFile
