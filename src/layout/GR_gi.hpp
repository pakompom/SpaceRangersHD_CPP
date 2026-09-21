#pragma once
#include "types/GR_gi.hpp"

namespace GR_gi {
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiHeader) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, Magic) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, Version) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, Bounds) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, FrameCount) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, Flags) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, SequenceTableOffset) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiHeader, SequenceTableSize) == 36);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TgiGR) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiGR, Data) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiGR, DataSize) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiGR, UsesExternalData) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiGR, Header) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TgiHeaderGR) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, Magic) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, Version) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, Bounds) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, RedMask) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, GreenMask) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, BlueMask) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, AlphaMask) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, Format) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, PlaneCount) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, ClipRectCount) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiHeaderGR, ClipRectTableOffset) == 52);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiSequenceTableHeader) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiSequenceTableHeader, SequenceCount) == 0);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TgiClipRectDiskGR) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiClipRectDiskGR, Left) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiClipRectDiskGR, Top) == 2);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiClipRectDiskGR, Bottom) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiClipRectDiskGR, Right) == 6);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiFrameEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiFrameEntry, DataOffset) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiFrameEntry, DataSize) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TgiPlaneGR) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiPlaneGR, DataOffset) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiPlaneGR, DataSize) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TgiPlaneGR, Bounds) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiSequenceDataBlock) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiSequenceDataBlock, FrameCount) == 0);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiSequenceFrameEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiSequenceFrameEntry, SourceFrameIndex) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiSequenceFrameEntry, FrameDelay) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(GR_gi::TGaiSequenceDirectoryEntry) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GR_gi::TGaiSequenceDirectoryEntry, SequenceDataOffset) == 0);

} // namespace GR_gi
