#pragma once
#include "types/GR_Main.hpp"

namespace GR_Main {
    static_assert(sizeof(void*) != 4 || sizeof(GR_Main::TCCInterface) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCInterface, Buffer) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCInterface, SnapshotHead) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCInterface, Lock) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Main::TCCSnapshot) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, ResourceChecksumFailed) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, TamperDetected) == 9);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, Flag0A) == 10);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, ProtectedStateXorSeed) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, Value10) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, IntegrityStatus) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, IntegrityError) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, IntegrityChecksum) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, IntegrityChecksum1) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, IntegrityChecksum2) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, EncodedCheatPoints) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCCSnapshot, EditableStateApplied) == 44);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Main::TCursorUnit) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCursorUnit, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCursorUnit, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCursorUnit, Name) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCursorUnit, ImagePath) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TCursorUnit, HotSpot) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Main::TMemoryStatusEx) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, Length) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, MemoryLoad) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, TotalPhys) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, AvailPhys) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, TotalPageFile) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, AvailPageFile) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, TotalVirtual) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, AvailVirtual) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TMemoryStatusEx, AvailExtendedVirtual) == 56);
    static_assert(sizeof(void*) != 4 || sizeof(GR_Main::TDisplayModeGR) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TDisplayModeGR, Width) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TDisplayModeGR, Height) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TDisplayModeGR, RefreshRate) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_Main::TDisplayModeGR, Format) == 12);

} // namespace GR_Main
