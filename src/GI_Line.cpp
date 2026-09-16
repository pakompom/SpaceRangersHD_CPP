#include "layout/GI_Line.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Line.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"

namespace GI_Line {
    void TLineGI_Create(TLineGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
    }

    void TLineGI_Destroy(TLineGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TLineGI::Clear() {
        Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TLineGI::SetColor(std::uint32_t Value) {
        if (Color != Value) {
            Color = Value;
            Invalidate();
        }
    }

    void TLineGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        LoadLineProperties(Block);
    }

    void TLineGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadLineProperties(Block);
    }

    void TLineGI::LoadLineProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Color"_wref.get()) > 0) {
            Color = GI_Main::GetColorGI(Block->GetParam(u"Color"_wref.get()));
        }
    }

    void TLineGI::Draw(Types::TRect ClipRect) {
        if (GlobalsV::HardwareRenderingEnabled) {
            GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, GR_DX::Color565ToArgb(Color), 255, &ClipRect);
        } else {
            GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(HitTestBounds.Left, HitTestBounds.Top), ClassesImports::Point(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1), Color, ClipRect);
        }
    }

    void TLineGI::p_destroy() {
        GI_Line::TLineGI_Destroy(this);
    }

} // namespace GI_Line
