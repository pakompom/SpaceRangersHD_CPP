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
    struct TGaiHeader;

    struct TGaiSequenceTableHeader;

    struct TgiClipRectDiskGR;

    struct TgiHeaderGR;

    struct TgiPlaneGR;

    struct TGaiFrameEntry;

    struct TGaiSequenceDirectoryEntry;

    struct TGaiSequenceFrameEntry;

    struct TGaiSequenceDataBlock;

    struct TgiGR;

    using PGaiHeader = TGaiHeader*;

    using PGaiSequenceTableHeader = TGaiSequenceTableHeader*;

    #pragma pack(push, 1)
    struct TGaiHeader {
        std::uint8_t cpp_padding[8];
        WindowsSdk::TRect Bounds;
        std::int32_t FrameCount;
        std::uint32_t Flags;
        std::int32_t SequenceTableOffset;
        std::uint8_t cpp_padding_2[12];
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
        void ClearData();
        std::uint8_t IsEmpty();
        void LoadRawGiBytes(void* BufferPtr, std::int32_t ByteCount);
        void LoadRawGiFromBuffer(EC_Buf::TBufEC* SourceBuffer);
        void LoadCompressedGiBytes(void* BufferPtr, std::int32_t ByteCount);
        WindowsSdk::TRect GetBoundsRect();
        WindowsSdk::TPoint GetContentSize();
        WindowsSdk::TPoint GetTopLeft();
        std::int32_t GetFormat();
        PgiPlaneGR GetPlane(std::int32_t PlaneIndex);
        std::int32_t GetClipRectCount();
        WindowsSdk::TRect GetClipRect(std::int32_t RectIndex);
        void BuildPalettedFormat4ColorCache();
        void DrawToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t X, std::int32_t Y, WindowsSdk::TRect DrawRect, std::uint8_t BlendMode, std::uint8_t Alpha);
        void DecodeToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::uint8_t Keep16BitPixels);
        void DecodeRawRegion(void* Destination, std::int32_t PitchBytes, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels);
        void DecodeToPixels(void* Destination, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels);
        void CreateFromGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t StorageMode);
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

    #pragma pack(push, 1)
    struct TgiPlaneGR {
        std::int32_t DataOffset;
        std::int32_t DataSize;
        WindowsSdk::TRect Bounds;
        std::uint8_t cpp_padding[8];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TGaiSequenceDirectoryEntry {
        std::int32_t SequenceDataOffset;
        std::uint8_t cpp_padding[4];
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

} // namespace GR_gi
