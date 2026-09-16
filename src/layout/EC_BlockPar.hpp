#pragma once
#include "types/EC_BlockPar.hpp"

namespace EC_BlockPar {
    static_assert(sizeof(void*) != 4 || sizeof(EC_BlockPar::TBlockParEC) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, FirstEntry) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, LastEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, EntryCount) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, StringParamCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, ChildBlockCount) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, UseSortedIndex) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, SortedEntries) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParEC, SortedEntryCount) == 32);
    static_assert(sizeof(void*) != 4 || sizeof(EC_BlockPar::TBlockParElEC) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, OwnerBlock) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, ItemType) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, Name) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, StringValue) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, Comment) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, ChildBlock) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, GroupIndex) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_BlockPar::TBlockParElEC, GroupCount) == 40);

} // namespace EC_BlockPar
