#pragma once
#include "types/aModsInfo.hpp"

namespace aModsInfo {
    static_assert(sizeof(void*) != 4 || sizeof(aModsInfo::TModInfo) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Folder) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, SwitchImage) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, IndexText) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Name) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Section) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, SmallDescription) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, FullDescription) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Author) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, DependencyNames) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, DependencyCount) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Dependencies) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, ConflictNames) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, ConflictCount) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Conflicts) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Priority) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, UnsupportedLanguage) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, MissingFolder) == 65);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, MissingDependency) == 66);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Misplaced) == 67);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, DuplicateName) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, ReferencedAsConflict) == 69);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, ReferencedAsDependency) == 70);
    static_assert(sizeof(void*) != 4 || offsetof(aModsInfo::TModInfo, Selected) == 71);

} // namespace aModsInfo
