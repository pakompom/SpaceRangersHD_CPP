#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GR_gi {
    struct TGaiFrameEntry;

    struct TGaiHeader;

    struct TGaiSequenceFrameEntry;

    struct TGaiSequenceTableHeader;

    struct TgiClipRectDiskGR;

    struct TgiHeaderGR;

    struct TgiPlaneGR;

    struct TGaiSequenceDirectoryEntry;

    struct TGaiSequenceDataBlock;

    struct TgiGR;

    using PGaiHeader = TGaiHeader*;

    using PGaiSequenceTableHeader = TGaiSequenceTableHeader*;

    // Shared GI/GAI disk structures; ownership inferred from both readers and unit order.
    #pragma pack(push, 1)
    struct TGaiHeader {
        // 'gai' followed by zero.
        pas::Array<std::uint8_t, 0, 3> Magic;
        // One in the game resources.
        std::int32_t Version;
        WindowsSdk::TRect Bounds;
        std::int32_t FrameCount;
        std::uint32_t Flags;
        std::int32_t SequenceTableOffset;
        // Bytes in the sequence table; zero when absent.
        std::int32_t SequenceTableSize;
        std::uint8_t cpp_padding[8];
    };
    #pragma pack(pop)

    using PgiHeaderGR = TgiHeaderGR*;

    using PgiPlaneGR = TgiPlaneGR*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TgiGR : EC_Struct::TObjectEx {
        PAS_CLASS_META(TgiGR, EC_Struct::TObjectEx, "TgiGR", 20)
        void p_destroy() override;
        // Borrowed data is detached without freeing it.
        void ClearData();
        std::uint8_t IsEmpty();
        // BufferPtr is borrowed; its header and length are not validated.
        void LoadRawGiBytes(void* BufferPtr, std::int32_t ByteCount);
        // Owns its copy; ignores SourceBuffer.Position.
        void LoadRawGiFromBuffer(EC_Buf::TBufEC* SourceBuffer);
        // Owns decompressed storage; empty on decompression failure.
        void LoadCompressedGiBytes(void* BufferPtr, std::int32_t ByteCount);
        WindowsSdk::TRect GetBoundsRect();
        WindowsSdk::TPoint GetContentSize();
        WindowsSdk::TPoint GetTopLeft();
        std::int32_t GetFormat();
        // Does not validate PlaneIndex.
        PgiPlaneGR GetPlane(std::int32_t PlaneIndex);
        std::int32_t GetClipRectCount();
        // Does not validate RectIndex.
        WindowsSdk::TRect GetClipRect(std::int32_t RectIndex);
        // Modifies Data even when borrowed.
        void BuildPalettedFormat4ColorCache();
        void DrawToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t X, std::int32_t Y, WindowsSdk::TRect DrawRect, std::uint8_t BlendMode, std::uint8_t Alpha);
        // Formats 0..4 create a 32-bit destination; formats 5/6 decode into an existing sufficiently large buffer. Keep16BitPixels affects format 0 without an alpha mask.
        void DecodeToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::uint8_t Keep16BitPixels);
        // Only format 0 is supported; other formats report an error.
        void DecodeRawRegion(void* Destination, std::int32_t PitchBytes, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels);
        void DecodeToPixels(void* Destination, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels);
        // Mode 1 uses RGB565; mode 0 uses ARGB masks. Native allocation reserves two bytes per pixel except for mode 2, whose payload is left uninitialized.
        void CreateFromGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t StorageMode);
        // Uses RGB565 masks.
        void CreateFormat2FromGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, WindowsSdk::TPoint TopLeft);
        void* Data;
        std::int32_t DataSize;
        std::uint8_t UsesExternalData;
        std::uint8_t cpp_padding[3];
        PgiHeaderGR Header;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TgiHeaderGR {
        pas::Array<std::uint8_t, 0, 3> Magic;
        std::int32_t Version;
        WindowsSdk::TRect Bounds;
        std::uint32_t RedMask;
        std::uint32_t GreenMask;
        std::uint32_t BlueMask;
        std::uint32_t AlphaMask;
        std::int32_t Format;
        std::int32_t PlaneCount;
        std::int32_t ClipRectCount;
        std::int32_t ClipRectTableOffset;
        std::uint8_t cpp_padding[8];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGaiSequenceTableHeader {
        std::int32_t SequenceCount;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TgiClipRectDiskGR {
        std::uint16_t Left;
        std::uint16_t Top;
        std::uint16_t Bottom;
        std::uint16_t Right;
    };
    #pragma pack(pop)

    using PgiClipRectDiskGR = TgiClipRectDiskGR*;

    #pragma pack(push, 1)
    struct TGaiFrameEntry {
        std::int32_t DataOffset;
        std::int32_t DataSize;
    };
    #pragma pack(pop)

    using PGaiFrameEntry = TGaiFrameEntry*;

    #pragma pack(push, 1)
    struct TgiPlaneGR {
        std::int32_t DataOffset;
        std::int32_t DataSize;
        WindowsSdk::TRect Bounds;
        std::uint8_t cpp_padding[8];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGaiSequenceDataBlock {
        std::int32_t FrameCount;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGaiSequenceFrameEntry {
        std::int32_t SourceFrameIndex;
        std::int32_t FrameDelay;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGaiSequenceDirectoryEntry {
        std::int32_t SequenceDataOffset;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    using PGaiSequenceFrameEntry = TGaiSequenceFrameEntry*;

} // namespace GR_gi
