#include "layout/GI_Frame.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"

namespace GI_Frame {
    void TFrameGI_Create(TFrameGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Kind = fkHide;
        Self->Fill = false;
        Self->FillAlpha = 255;
    }

    void TFrameGI_Destroy(TFrameGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves fill and color fields.
    void TFrameGI::Clear() {
        Kind = fkHide;
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TFrameGI::SetKind(TFrameKindGI Value) {
        if (Kind != Value) {
            Kind = Value;
            Invalidate();
        }
    }

    void TFrameGI::SetColor(std::uint32_t Value) {
        if (Color != Value) {
            Color = Value;
            Invalidate();
        }
    }

    void TFrameGI::SetFillColor(std::uint32_t Value) {
        if (FillColor != Value) {
            FillColor = Value;
            Invalidate();
        }
    }

    void TFrameGI::SetFill(std::uint8_t Value) {
        if (Fill != Value) {
            Fill = Value;
            Invalidate();
        }
    }

    void TFrameGI_LoadFromConfigPath(TFrameGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadFrameProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TFrameGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadFrameProperties(Block);
    }

    void TFrameGI::LoadFrameProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            if (Block->GetParam(u"Kind"sv) == u"Hide") {
                Kind = fkHide;
            } else if (Block->GetParam(u"Kind"sv) == u"Rect") {
                Kind = fkRect;
            }
        }
        if (Block->CountParams(u"Color"_wref.get()) > 0) {
            SetColor(GI_Main::GetColorGI(pas::view(Block->GetParam(u"Color"sv))));
        }
        if (Block->CountParams(u"ColorFill"_wref.get()) > 0) {
            SetFillColor(GI_Main::GetColorGI(pas::view(Block->GetParam(u"ColorFill"sv))));
        }
        if (Block->CountParams(u"Fill"_wref.get()) > 0) {
            SetFill(GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"Fill"sv))));
        }
    }

    // Fill is independent of Kind. FillAlpha values other than 255 all produce alpha 64.
    void TFrameGI::Draw(Types::TRect ClipRect) {
        if (Fill) {
            if (GlobalsV::HardwareRenderingEnabled) {
                if (FillAlpha == 255) {
                    GR_DX::DrawColoredRect(ClipRect.Left, ClipRect.Top, ClipRect.Right - ClipRect.Left, ClipRect.Bottom - ClipRect.Top, GR_DX::Color565ToArgb(FillColor), 255, true, &ClipRect);
                } else {
                    GR_DX::DrawColoredRect(ClipRect.Left, ClipRect.Top, ClipRect.Right - ClipRect.Left, ClipRect.Bottom - ClipRect.Top, GR_DX::Color565ToArgb(FillColor), 64, true, &ClipRect);
                }
            } else if (FillAlpha == 255) {
                std::int32_t cpp_arg_2 = ClipRect.Bottom - ClipRect.Top;
                std::uint16_t fillColor = FillColor;
                std::int32_t cpp_arg = GR_Main::ScreenRenderBuffer->PitchBytes * ClipRect.Top + ClipRect.Left * 2;
                void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                void* addPointerOffset = EC_Mem::AddPointerOffset(pixels, cpp_arg);
                std::int32_t cpp_arg_3 = ClipRect.Right - ClipRect.Left;
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset, pitchBytes, cpp_arg_3, cpp_arg_2, fillColor);
            } else {
                GR_Main::ScreenRenderBuffer->DrawAlphaTrapezium16(HitTestBounds.Left, HitTestBounds.Right, HitTestBounds.Top, HitTestBounds.Left, HitTestBounds.Right, HitTestBounds.Bottom, FillColor, 64, ClipRect);
            }
        }
        if (Kind == fkRect) {
            if (GlobalsV::HardwareRenderingEnabled) {
                GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Top, Color, 255, &ClipRect);
                GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Bottom - 1, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, Color, 255, &ClipRect);
                GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Left, HitTestBounds.Bottom - 1, Color, 255, &ClipRect);
                GR_DX::DrawAlphaLine(HitTestBounds.Right - 1, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, Color, 255, &ClipRect);
            } else {
                GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(HitTestBounds.Left, HitTestBounds.Top), ClassesImports::Point(HitTestBounds.Right - 1, HitTestBounds.Top), Color, ClipRect);
                GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(HitTestBounds.Left, HitTestBounds.Bottom - 1), ClassesImports::Point(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1), Color, ClipRect);
                GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(HitTestBounds.Left, HitTestBounds.Top), ClassesImports::Point(HitTestBounds.Left, HitTestBounds.Bottom - 1), Color, ClipRect);
                GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(HitTestBounds.Right - 1, HitTestBounds.Top), ClassesImports::Point(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1), Color, ClipRect);
            }
        }
    }

    void TFrameGI::p_destroy() {
        GI_Frame::TFrameGI_Destroy(this);
    }

    void TFrameGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Frame::TFrameGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Frame
