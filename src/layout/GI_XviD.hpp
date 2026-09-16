#pragma once
#include "types/GI_XviD.hpp"

namespace GI_XviD {
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TxvidGI) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, SourceFile) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, CompressedFrame) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, DecoderHandle) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, DecodedFrameCount) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, TargetFrame) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, VideoWidth) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, VideoHeight) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, PlaybackFinished) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, ColorSpace) == 336);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, FillViewport) == 340);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, AviFile) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, AviStream) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, FrameCount) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TxvidGI, FramesPerSecond) == 360);
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TXvidImage) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidImage, ColorSpace) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidImage, Planes) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidImage, Strides) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TXvidDecoderFrame) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, Version) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, General) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, Bitstream) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, Length) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, Output) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderFrame, Brightness) == 52);
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TXvidDecoderStats) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderStats, Version) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderStats, FrameType) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderStats, Data) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TXvidGlobalInit) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidGlobalInit, Version) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidGlobalInit, CpuFlags) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidGlobalInit, Debug) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(GI_XviD::TXvidDecoderCreate) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderCreate, Version) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderCreate, Width) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderCreate, Height) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_XviD::TXvidDecoderCreate, Handle) == 12);

} // namespace GI_XviD
