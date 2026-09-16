#pragma once
#include "types/aTranclucator.hpp"

namespace aTranclucator {
    static_assert(sizeof(void*) != 4 || sizeof(aTranclucator::TTranclucator) == 1260);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, ArtefactSize) == 1232);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, ArtefactSystemName) == 1236);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, OwnerShip) == 1240);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, FollowOwner) == 1244);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, SeekItems) == 1245);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, AutoArrange) == 1246);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, StoreOnLanding) == 1247);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, CollectionPermissions) == 1248);
    static_assert(sizeof(void*) != 4 || offsetof(aTranclucator::TTranclucator, StoragePermissions) == 1255);

} // namespace aTranclucator
