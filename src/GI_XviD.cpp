#include "layout/GI_XviD.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_XviD.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/VFW.hpp"
#include "units/WindowsImports.hpp"

namespace GI_XviD {
    std::uint32_t XvidLibrary = 0u;

    GI_XviD::TXvidFunction XvidGlobal = nullptr;

    GI_XviD::TXvidFunction XvidDecore = nullptr;

    void TxvidGI_Create(TxvidGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

    void TxvidGI_Destroy(TxvidGI* Self) {
        Self->ImageClose();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TxvidGI::Clear() {
        ImageClose();
        GI_MessageLoop::TObjectGI::Clear();
    }

    std::uint8_t TxvidGI::ImageOpen(const pas::WideString& FileName, std::uint8_t FillViewport) {
        pas::AnsiString cpp_text{};
        std::int32_t ErrorCode{};
        std::int32_t FormatSize{};
        std::int32_t Status{};
        TXvidGlobalInit GlobalInit{};
        TXvidDecoderCreate DecoderCreate{};
        WindowsSdk::TBitmapInfoHeader Format{};
        VFW::TAVIStreamInfoA Info{};
        std::uint8_t Result = true;
        this->FillViewport = FillViewport;
        ImageClose();
        try {
            if (XvidLibrary == 0) {
                XvidLibrary = WindowsImports::LoadLibrary(pas::literal_pointer("xvidcore.dll"));
                if (XvidLibrary == 0) {
                    GR_Main::RaiseWideMessage(u"Error xvidcore.dll"_wref.get());
                }
                XvidGlobal = pas::callback_from_address<TXvidFunction>(WindowsImports::GetProcAddress(XvidLibrary, pas::literal_pointer("xvid_global")));
                if (!(XvidGlobal != nullptr)) {
                    GR_Main::RaiseWideMessage(u"Error xvid_global"_wref.get());
                }
                XvidDecore = pas::callback_from_address<TXvidFunction>(WindowsImports::GetProcAddress(XvidLibrary, pas::literal_pointer("xvid_decore")));
                if (!(XvidDecore != nullptr)) {
                    GR_Main::RaiseWideMessage(u"Error xvid_decore"_wref.get());
                }
            }
            pas::fill_memory(&GlobalInit, static_cast<std::int32_t>(sizeof(TXvidGlobalInit)), static_cast<std::uint8_t>(0));
            GlobalInit.Version = 0x00010100;
            GlobalInit.CpuFlags = 0u;
            GI_XviD::XvidGlobal(nullptr, 0, &GlobalInit, nullptr);
            SourceFile = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
            CompressedFrame = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            CompressedFrame->SetSize(0x00180000);
            VFW::AVIFileInit();
            Status = ([&] {
                std::uint8_t* cpp_arg = (cpp_text = static_cast<pas::AnsiString>(FileName), cpp_text.pchar());
                VFW::IAVIFile& aviFile = AviFile;
                return VFW::AVIFileOpenA(aviFile, cpp_arg, 0u, nullptr);
            }());
            if (Status != 0) {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"Error AVIFileOpenA = ", SysUtils::IntToStr(Status)})));
            }
            Status = VFW::AVIFileGetStream(AviFile, AviStream, 0x73646976u, 0);
            if (Status != 0) {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"Error AVIFileGetStream = ", SysUtils::IntToStr(Status)})));
            }
            VFW::AVIStreamInfoA(AviStream, Info, static_cast<std::int32_t>(sizeof(VFW::TAVIStreamInfoA)));
            FramesPerSecond = pas::real_divide(Info.Rate, Info.Scale);
            FormatSize = static_cast<std::int32_t>(sizeof(WindowsSdk::TBitmapInfoHeader));
            Status = VFW::AVIStreamReadFormat(AviStream, 0, &Format, FormatSize);
            if (Status != 0) {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"Error AVIStreamReadFormat ret = ", SysUtils::IntToStr(Status)})));
            }
            FrameCount = VFW::AVIStreamLength(AviStream);
            if (FrameCount < 0) {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"Error AVIStreamLength FAVILen = ", SysUtils::IntToStr(FrameCount)})));
            }
            pas::fill_memory(&DecoderCreate, static_cast<std::int32_t>(sizeof(TXvidDecoderCreate)), static_cast<std::uint8_t>(0));
            DecoderCreate.Version = 0x00010100;
            DecoderCreate.Width = Format.biWidth;
            DecoderCreate.Height = Format.biHeight;
            Status = GI_XviD::XvidDecore(nullptr, 0, &DecoderCreate, nullptr);
            if (Status != 0) {
                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"Error xvid_decore_func = ", SysUtils::IntToStr(Status)})));
            }
            DecoderHandle = DecoderCreate.Handle;
            DecodedFrameCount = 0;
            ColorSpace = 0x00000040;
            VideoWidth = Format.biWidth;
            VideoHeight = Format.biHeight;
            if (GR_Main::Direct3DDevice == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>("TxvidGI.ImageOpen(..)::GR_D3DDevice = nil"_a));
            }
            ErrorCode = ([&] {
                Direct3D9::IDirect3DTexture9& offscreenTexture = pas::out_interface(GR_Main::OffscreenTexture);
                std::uint32_t videoHeight = VideoHeight;
                std::uint32_t videoWidth = VideoWidth;
                pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice = GR_Main::Direct3DDevice;
                return Direct3D9::IDirect3DDevice9_CreateTexture(direct3DDevice, videoWidth, videoHeight, 1u, 0u, Direct3D9::D3DFMT_X8R8G8B8, Direct3D9::D3DPOOL_MANAGED, offscreenTexture, nullptr);
            }());
            if (ErrorCode != 0) {
                pas::raise(pas::make_exception<pas::Exception>(GR_Main::Direct3DErrorText(ErrorCode)));
            }
            SetFramePosition(1);
            GR_Main::OffscreenFillViewport = this->FillViewport;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                Result = false;
                ImageClose();
            } else {
                throw;
            }
        }
        return Result;
    }

    void TxvidGI::XvidClose() {
        std::int32_t Status{};
        if (DecoderHandle != nullptr) {
            Status = GI_XviD::XvidDecore(DecoderHandle, 1, nullptr, nullptr);
            DecoderHandle = nullptr;
            if (Status != 0) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Error XvidClose: xvid_decore_func - XVID_DEC_DESTROY = ", SysUtils::IntToStr(Status)}));
            }
        }
        if (AviStream != nullptr) {
            AviStream = nullptr;
        }
        if (AviFile != nullptr) {
            AviFile = nullptr;
        }
        VFW::AVIFileExit();
    }

    void TxvidGI::ImageClose() {
        XvidClose();
        if (SourceFile != nullptr) {
            pas::free(SourceFile);
            SourceFile = nullptr;
        }
        if (CompressedFrame != nullptr) {
            pas::free(CompressedFrame);
            CompressedFrame = nullptr;
        }
        if (GR_Main::OffscreenTexture != nullptr) {
            GR_Main::OffscreenTexture = nullptr;
        }
    }

    void TxvidGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        TxvidGI::ReadVideoConfig(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TxvidGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TxvidGI::ReadVideoConfig(Block);
    }

    void TxvidGI::ReadVideoConfig(EC_BlockPar::TBlockParEC* Block) {
    }

    std::uint8_t TxvidGI::DecodeNextFrame() {
        std::int32_t BytesUsed{};
        std::uint32_t BytesRead{};
        Direct3D9::TD3DLockedRect LockedRect{};
        TXvidDecoderFrame Frame{};
        TXvidDecoderStats Stats{};
        if (DecodedFrameCount >= FrameCount) {
            return false;
        }
        if (VFW::AVIStreamRead(AviStream, DecodedFrameCount, 1, CompressedFrame->Data, CompressedFrame->DataSize, &BytesRead, nullptr) != 0) {
            GR_Main::RaiseWideMessage(u"AVI stream read"_wref.get());
        }
        void* Data = CompressedFrame->Data;
        std::int32_t ErrorCode = Direct3D9::IDirect3DTexture9_LockRect(GR_Main::OffscreenTexture, 0u, LockedRect, nullptr, 0u);
        if (ErrorCode != 0) {
            pas::raise(pas::make_exception<pas::Exception>("GR_lpTexAVI.LockRect error"_a));
        }
        while (BytesRead > 1) {
            pas::fill_memory(&Stats, static_cast<std::int32_t>(sizeof(TXvidDecoderStats)), static_cast<std::uint8_t>(0));
            Stats.Version = 0x00010100;
            pas::fill_memory(&Frame, static_cast<std::int32_t>(sizeof(TXvidDecoderFrame)), static_cast<std::uint8_t>(0));
            Frame.Version = 0x00010100;
            Frame.General = 1;
            Frame.Bitstream = Data;
            Frame.Length = BytesRead;
            Frame.Output.ColorSpace = ColorSpace;
            Frame.Output.Planes[0] = LockedRect.Bits;
            Frame.Output.Strides[0] = LockedRect.Pitch;
            BytesUsed = GI_XviD::XvidDecore(DecoderHandle, 2, &Frame, &Stats);
            if (BytesUsed < 0) {
                GR_Main::RaiseWideMessage(u"AVI decode"_wref.get());
            }
            Data = static_cast<std::uint8_t*>(Data) + BytesUsed;
            BytesRead -= BytesUsed;
        }
        Direct3D9::IDirect3DTexture9_UnlockRect(GR_Main::OffscreenTexture, 0u);
        ++DecodedFrameCount;
        GR_Main::OffscreenFillViewport = FillViewport;
        GR_Main::OffscreenFrameUpdated = true;
        return DecodedFrameCount < FrameCount;
    }

    std::uint8_t TxvidGI::SetPlaybackTime(double TimeMs) {
        std::uint8_t Result = false;
        std::int32_t Frame = System::Round(0.001L * TimeMs * FramesPerSecond);
        if (Frame >= FrameCount) {
            Frame = FrameCount - 1;
            Result = true;
        }
        SetFramePosition(Frame);
        return Result;
    }

    void TxvidGI::SetFramePosition(std::int32_t Frame) {
        if (DecoderHandle != nullptr) {
            TargetFrame = Frame;
            while (TargetFrame > DecodedFrameCount) {
                if (!DecodeNextFrame()) {
                    XvidClose();
                    if (pas::assigned(PlaybackFinished)) {
                        PlaybackFinished(this);
                    }
                    break;
                }
            }
        }
    }

    void TxvidGI::p_destroy() {
        GI_XviD::TxvidGI_Destroy(this);
    }

} // namespace GI_XviD
