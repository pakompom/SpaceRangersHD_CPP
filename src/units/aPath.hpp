#pragma once
#include "types/aPath.hpp"

namespace aPath {
    extern std::uint32_t PathNodeHeap;

    extern aPath::PSPathNode PathPoolHead;

    extern aPath::PSPathNode PathPoolTail;

    extern pas::CriticalSection* PathPoolLock;

    extern std::int32_t PathGrowthBlockCount;

    extern void* PathInitialBlock;

    extern std::int32_t PathPoolFreeCount;

    extern pas::DynArray<void*> PathGrowthBlocks;

    void InitializePathNodePool();

    void EnsurePathGrowthBlockSlot();

    void FreePathGrowthBlocks();

    std::uint8_t ReservePathGrowthBlock();

    std::uint8_t GrowPathNodePool();

    void FinalizePathNodePool();

    void TSPath_Create(TSPath* Self);

    void TSPath_Destroy(TSPath* Self);

} // namespace aPath
