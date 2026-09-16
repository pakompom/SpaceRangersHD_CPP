#include "layout/GI_ShrLight.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_ShrLight.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_ShrLight {
    void TShrLightGI_Create(TShrLightGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Kind = slkAll;
        Self->LightShift = 1;
    }

    void TShrLightGI_Destroy(TShrLightGI* Self) {
        if (Self->LightBuffer != nullptr) {
            pas::free(Self->LightBuffer);
            Self->LightBuffer = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TShrLightGI::Clear() {
        if (LightBuffer != nullptr) {
            pas::free(LightBuffer);
            LightBuffer = nullptr;
        }
        Kind = slkAll;
        LightShift = 1;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TShrLightGI::SetKind(TShrLightKindGI Value) {
        if (Kind != Value) {
            Kind = Value;
            if (Kind == slkBuffer) {
                LightBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                LightBuffer->AllocateGrayscale(ClientSize.X, ClientSize.Y);
                LightBuffer->FillPixels(0);
            } else if (LightBuffer != nullptr) {
                pas::free(LightBuffer);
                LightBuffer = nullptr;
            }
            Invalidate();
        }
    }

    void TShrLightGI::SetLightShift(std::int32_t Value) {
        if (LightShift != Value) {
            LightShift = Value;
            Invalidate();
        }
    }

    void TShrLightGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        if (Kind == slkBuffer) {
            LightBuffer->AllocateGrayscale(Size.X, Size.Y);
            LightBuffer->FillPixels(0);
        }
    }

    void TShrLightGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        LoadLightProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TShrLightGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadLightProperties(Block);
    }

    void TShrLightGI::LoadLightProperties(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        if (Block->CountParams(u"ShrLight"_wref.get()) > 0) {
            SetLightShift(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"ShrLight"_wref.get()))));
        }
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            Text = Block->GetParam(u"Kind"_wref.get());
            if (Text == u"All") {
                SetKind(slkAll);
            } else if (Text == u"Buf") {
                SetKind(slkBuffer);
            }
        }
    }

    void TShrLightGI::Draw(Types::TRect ClipRect) {
        std::int32_t Alpha{};
        if (GlobalsV::HardwareRenderingEnabled) {
            Alpha = 255;
            if (LightShift == 2) {
                Alpha = 128;
            } else if (LightShift == 0) {
                return;
            } else if (LightShift == -2) {
                Alpha = 64;
            } else {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"FShrLight=", SysUtils::IntToStr(LightShift)}));
            }
            if (Kind == slkBuffer) {
                GR_DX::DrawColoredRect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, 0u, Alpha, true, &ClipRect);
            } else if (Kind == slkAll) {
                GR_DX::DrawColoredRect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, 0u, Alpha, true, &ClipRect);
            }
        } else if (Kind == slkBuffer) {
            std::int32_t pitchBytes = LightBuffer->PitchBytes;
            std::int32_t cpp_arg_3 = ClipRect.Right - ClipRect.Left;
            std::int32_t cpp_arg_4 = ClipRect.Bottom - ClipRect.Top;
            std::int32_t cpp_arg_2 = (ClipRect.Top - HitTestBounds.Top) * LightBuffer->PitchBytes + (ClipRect.Left - HitTestBounds.Left);
            void* pixels_2 = LightBuffer->GetPixels();
            void* addPointerOffset = EC_Mem::AddPointerOffset(pixels_2, cpp_arg_2);
            std::int32_t cpp_arg = GR_Main::ScreenRenderBuffer->PitchBytes * ClipRect.Top + ClipRect.Left * 2;
            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
            void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels, cpp_arg);
            std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
            GR_Main::Ex_OKGR_ShrLightMask_16(addPointerOffset_2, pitchBytes_2, addPointerOffset, pitchBytes, cpp_arg_3, cpp_arg_4);
        } else if (Kind == slkAll) {
            GR_Main::ScreenRenderBuffer->ShiftLight16(LightShift, ClipRect);
        }
    }

    void TShrLightGI::p_destroy() {
        GI_ShrLight::TShrLightGI_Destroy(this);
    }

} // namespace GI_ShrLight
