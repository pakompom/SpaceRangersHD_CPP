#include "layout/GI_StatusBar.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_StatusBar.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"

namespace GI_StatusBar {
    void TStatusBarGI_Create(TStatusBarGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->LeftImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->CenterImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->RightImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->LeftImage->SetDepth(1.0);
        Self->LeftImage->SetImageKindX(GI_Main::ikxLeft);
        Self->LeftImage->SetImageKindY(GI_Main::ikyCenter);
        Self->CenterImage->SetDepth(1.0);
        Self->CenterImage->SetImageKindX(GI_Main::ikxLeftFill);
        Self->CenterImage->SetImageKindY(GI_Main::ikyCenter);
        Self->RightImage->SetDepth(1.0);
        Self->RightImage->SetImageKindX(GI_Main::ikxLeft);
        Self->RightImage->SetImageKindY(GI_Main::ikyCenter);
        Self->Minimum = 0.0;
        Self->Maximum = 1.0E+2;
    }

    void TStatusBarGI_Destroy(TStatusBarGI* Self) {
        pas::free(Self->LeftImage);
        pas::free(Self->CenterImage);
        pas::free(Self->RightImage);
        GI_Panel::TPanelGI_Destroy(Self);
    }

    // Preserves Value; resets the range to 0..100.
    void TStatusBarGI::Clear() {
        Minimum = 0.0;
        Maximum = 1.0E+2;
        GI_Panel::TPanelGI::Clear();
    }

    // If MinValue exceeds MaxValue, lowers MinValue to MaxValue. Does not clamp the stored Value.
    void TStatusBarGI::SetRange(double MinValue, double MaxValue) {
        if (MinValue > MaxValue) {
            MinValue = MaxValue;
        }
        if (MinValue != Minimum || MaxValue != Maximum) {
            Minimum = MinValue;
            Maximum = MaxValue;
            UpdateImageLayout();
            Invalidate();
        }
    }

    void TStatusBarGI::SetValue(double NewValue) {
        if (NewValue < Minimum) {
            NewValue = Minimum;
        }
        if (NewValue > Maximum) {
            NewValue = Maximum;
        }
        if (NewValue != Value) {
            Value = NewValue;
            UpdateImageLayout();
            Invalidate();
        }
    }

    void TStatusBarGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        UpdateImageLayout();
        Invalidate();
    }

    void TStatusBarGI::UpdateImageLayout() {
        std::int32_t Width{};
        if (static_cast<long double>(Maximum) - Minimum == 0.0L) {
            Width = 0;
        } else {
            Width = System::Round(pas::real_divide(static_cast<long double>(Value) - Minimum, static_cast<long double>(Maximum) - Minimum) * ClientSize.X);
        }
        if (Minimum == Value) {
            LeftImage->SetSize(ClassesImports::Point(0, ClientSize.Y));
            CenterImage->SetSize(ClassesImports::Point(0, ClientSize.Y));
            RightImage->SetSize(ClassesImports::Point(0, ClientSize.Y));
        } else {
            std::int32_t cpp_left = LeftImage->GetContentSize().X;
            if (cpp_left + RightImage->GetContentSize().X >= Width) {
                {
                    std::int32_t x = LeftImage->GetContentSize().X;
                    std::int32_t y = ClientSize.Y;
                    Types::TPoint point = ClassesImports::Point(x, y);
                    GI_Image::TImageGI* leftImage = LeftImage;
                    leftImage->SetSize(point);
                }
                CenterImage->SetSize(ClassesImports::Point(0, ClientSize.Y));
                {
                    std::int32_t x_2 = RightImage->GetContentSize().X;
                    std::int32_t y_2 = ClientSize.Y;
                    Types::TPoint point_2 = ClassesImports::Point(x_2, y_2);
                    GI_Image::TImageGI* rightImage = RightImage;
                    rightImage->SetSize(point_2);
                }
                LeftImage->SetPosition(ClassesImports::Point(0, 0));
                CenterImage->SetPosition(ClassesImports::Point(LeftImage->ClientSize.X, 0));
                RightImage->SetPosition(ClassesImports::Point(LeftImage->ClientSize.X, 0));
            } else {
                {
                    std::int32_t x_3 = LeftImage->GetContentSize().X;
                    std::int32_t y_3 = ClientSize.Y;
                    Types::TPoint point_3 = ClassesImports::Point(x_3, y_3);
                    GI_Image::TImageGI* leftImage_2 = LeftImage;
                    leftImage_2->SetSize(point_3);
                }
                CenterImage->SetSize(ClassesImports::Point(Width - LeftImage->ClientSize.X - RightImage->ClientSize.X, ClientSize.Y));
                {
                    std::int32_t x_4 = RightImage->GetContentSize().X;
                    std::int32_t y_4 = ClientSize.Y;
                    Types::TPoint point_4 = ClassesImports::Point(x_4, y_4);
                    GI_Image::TImageGI* rightImage_2 = RightImage;
                    rightImage_2->SetSize(point_4);
                }
                LeftImage->SetPosition(ClassesImports::Point(0, 0));
                CenterImage->SetPosition(ClassesImports::Point(LeftImage->ClientSize.X, 0));
                RightImage->SetPosition(ClassesImports::Point(LeftImage->ClientSize.X + CenterImage->ClientSize.X, 0));
                CenterImage->SetImageKindX(GI_Main::ikxLeftFill);
            }
        }
    }

    void TStatusBarGI_LoadFromConfigPath(TStatusBarGI* Self, const pas::WideString& Path) {
        GI_Panel::TPanelGI_LoadFromConfigPath(Self, Path);
        Self->LoadStatusProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TStatusBarGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        LoadStatusProperties(Block);
    }

    void TStatusBarGI::LoadStatusProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"ImageLeft"_wref.get()) > 0) {
            LeftImage->SetImagePath(Block->GetParam(u"ImageLeft"sv));
        }
        if (Block->CountParams(u"ImageMiddle"_wref.get()) > 0) {
            CenterImage->SetImagePath(Block->GetParam(u"ImageMiddle"sv));
        }
        if (Block->CountParams(u"ImageRight"_wref.get()) > 0) {
            RightImage->SetImagePath(Block->GetParam(u"ImageRight"sv));
        }
        if (Block->CountParams(u"Min"_wref.get()) > 0 && Block->CountParams(u"Max"_wref.get()) > 0) {
            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Min"sv));
            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Max"sv));
            SetRange(extractDecimalToSingleW, extractDecimalToSingleW_2);
        }
        if (Block->CountParams(u"Cur"_wref.get()) > 0) {
            SetValue(EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Cur"sv)));
        }
        UpdateImageLayout();
    }

    void TStatusBarGI::p_destroy() {
        GI_StatusBar::TStatusBarGI_Destroy(this);
    }

    void TStatusBarGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_StatusBar::TStatusBarGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_StatusBar
