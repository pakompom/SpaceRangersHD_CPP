#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/VFW.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_File {
    struct TFileEC;

} // namespace EC_File

namespace GI_XviD {
    struct TXvidGlobalInit;

    struct TXvidDecoderCreate;

    struct TXvidImage;

    struct TXvidDecoderFrame;

    struct TXvidDecoderStats;

    struct TxvidGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TxvidGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TxvidGI, GI_MessageLoop::TObjectGI, "TxvidGI", 368)
        void p_destroy() override;
        void Clear() override;
        std::uint8_t ImageOpen(const pas::WideString& FileName, std::uint8_t FillViewport);
        void XvidClose();
        void ImageClose();
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        static void ReadVideoConfig(EC_BlockPar::TBlockParEC* Block);
        std::uint8_t DecodeNextFrame();
        std::uint8_t SetPlaybackTime(double TimeMs);
        void SetFramePosition(std::int32_t Frame);
        EC_File::TFileEC* SourceFile;
        EC_Buf::TBufEC* CompressedFrame;
        std::uint8_t cpp_padding[8];
        void* DecoderHandle;
        std::int32_t DecodedFrameCount;
        std::int32_t TargetFrame;
        std::int32_t VideoWidth;
        std::int32_t VideoHeight;
        std::uint8_t cpp_padding_2[4];
        GI_MessageLoop::TObjectNotifyEventGI PlaybackFinished;
        std::int32_t ColorSpace;
        std::uint8_t FillViewport;
        std::uint8_t cpp_padding_3[3];
        VFW::IAVIFile AviFile;
        VFW::IAVIStream AviStream;
        std::int32_t FrameCount;
        std::uint8_t cpp_padding_4[4];
        double FramesPerSecond;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TXvidFunction = pas::Proc<std::int32_t(void*, std::int32_t, void*, void*)>;

    #pragma pack(push, 1)
    struct TXvidImage {
        std::int32_t ColorSpace;
        pas::Array<void*, 0, 3> Planes;
        pas::Array<std::int32_t, 0, 3> Strides;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TXvidDecoderFrame {
        std::int32_t Version;
        std::int32_t General;
        void* Bitstream;
        std::int32_t Length;
        TXvidImage Output;
        std::int32_t Brightness;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TXvidDecoderStats {
        std::int32_t Version;
        std::int32_t FrameType;
        // Native code clears the whole structure but does not inspect the union.
        pas::Array<std::uint8_t, 0, 23> Data;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TXvidGlobalInit {
        std::int32_t Version;
        std::uint32_t CpuFlags;
        std::int32_t Debug;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TXvidDecoderCreate {
        std::int32_t Version;
        std::int32_t Width;
        std::int32_t Height;
        void* Handle;
    };
    #pragma pack(pop)

} // namespace GI_XviD
