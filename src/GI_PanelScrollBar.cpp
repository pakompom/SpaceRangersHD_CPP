#include "layout/GI_PanelScrollBar.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_PanelScrollBar.hpp"
#include "units/GI_ScrollBar.hpp"
#include "units/GR_Main.hpp"

namespace GI_PanelScrollBar {
    void TPanelScrollBarGI_Create(TPanelScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->HorizontalScrollBar = pas::construct_call<GI_ScrollBar::TScrollBarGI>(GI_ScrollBar::TScrollBarGI_Create, Self);
        Self->HorizontalScrollBar->UserValue = -1;
        Self->VerticalScrollBar = pas::construct_call<GI_ScrollBar::TScrollBarGI>(GI_ScrollBar::TScrollBarGI_Create, Self);
        Self->VerticalScrollBar->UserValue = -1;
        Self->HorizontalScrollBar->DestroyNotify = pas::bind_method<&TPanelScrollBarGI::ScrollbarDestroyed>(Self);
        Self->VerticalScrollBar->DestroyNotify = pas::bind_method<&TPanelScrollBarGI::ScrollbarDestroyed>(Self);
        Self->HorizontalScrollBar->SetActive(false);
        Self->HorizontalScrollBar->SetOrientation(1);
        Self->HorizontalScrollBar->SetKindCalcMode(1);
        Self->VerticalScrollBar->SetActive(false);
        Self->VerticalScrollBar->SetOrientation(2);
        Self->VerticalScrollBar->SetKindCalcMode(1);
        Self->HorizontalScrollBar->SetDepth(-1.0E+30);
        Self->VerticalScrollBar->SetDepth(-1.0E+30);
        Self->HorizontalScrollBar->PositionChangedCallback = pas::bind_method<&TPanelScrollBarGI::ScrollbarPositionChanged>(Self);
        Self->VerticalScrollBar->PositionChangedCallback = pas::bind_method<&TPanelScrollBarGI::ScrollbarPositionChanged>(Self);
        Self->AutoHorizontalPlacement = true;
        Self->AutoVerticalPlacement = true;
        Self->ScrollbarsOutside = false;
        Self->UnlimitedWorld = true;
        Self->ScrollChangedCallback = pas::bind_method<&TPanelScrollBarGI::PanelScrollChanged>(Self);
    }

    // Frees both scrollbars, including when parented outside this panel.
    void TPanelScrollBarGI_Destroy(TPanelScrollBarGI* Self) {
        if (Self->HorizontalScrollBar != nullptr) {
            pas::free(Self->HorizontalScrollBar);
            Self->HorizontalScrollBar = nullptr;
        }
        if (Self->VerticalScrollBar != nullptr) {
            pas::free(Self->VerticalScrollBar);
            Self->VerticalScrollBar = nullptr;
        }
        GI_Panel::TPanelGI_Destroy(Self);
    }

    void TPanelScrollBarGI::Clear() {
        GI_Panel::TPanelGI::Clear();
        if (HorizontalScrollBar != nullptr && VerticalScrollBar != nullptr) {
            SetScrollbarsOutside(false);
        }
        UnlimitedWorld = true;
    }

    void TPanelScrollBarGI::SetVerticalScrollBarConfigPath(pas::WideString Path) {
        VerticalScrollBar->SetConfigPath(Path);
    }

    void TPanelScrollBarGI::SetHorizontalScrollbarEnabled(std::uint8_t Value) {
        HorizontalScrollBar->SetActive(Value);
        if (Value == true) {
            HorizontalScrollBar->UpdateSizeForOrientation();
        }
    }

    std::uint8_t TPanelScrollBarGI::IsVerticalScrollbarEnabled() {
        return VerticalScrollBar->Active;
    }

    void TPanelScrollBarGI::SetVerticalScrollbarEnabled(std::uint8_t Value) {
        VerticalScrollBar->SetActive(Value);
        if (Value == true) {
            VerticalScrollBar->UpdateSizeForOrientation();
        }
    }

    // The panel retains ownership of scrollbars parented outside it.
    void TPanelScrollBarGI::SetScrollbarsOutside(std::uint8_t Value) {
        if (Value != ScrollbarsOutside) {
            ScrollbarsOutside = Value;
            if (!ScrollbarsOutside) {
                HorizontalScrollBar->Reparent(this);
                VerticalScrollBar->Reparent(this);
                HorizontalScrollBar->SetDepth(-1.0E+30);
                VerticalScrollBar->SetDepth(-1.0E+30);
            } else {
                HorizontalScrollBar->Reparent(Parent);
                VerticalScrollBar->Reparent(Parent);
                HorizontalScrollBar->SetDepth(Depth);
                VerticalScrollBar->SetDepth(Depth);
            }
            UpdateScrollbarPlacement();
            Invalidate();
        }
    }

    void TPanelScrollBarGI::SetUnlimitedWorldEnabled(std::uint8_t Value) {
        if (Value != UnlimitedWorld) {
            UnlimitedWorld = Value;
            Invalidate();
        }
    }

    void TPanelScrollBarGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        UpdateScrollbarPlacement();
    }

    void TPanelScrollBarGI::SetOrigin(Types::TPoint Origin) {
        GI_MessageLoop::TObjectGI::SetOrigin(Origin);
        UpdateScrollbarPlacement();
    }

    void TPanelScrollBarGI::SetScrollOffset(Types::TPoint Offset) {
        GI_Panel::TPanelGI::SetScrollOffset(Offset);
        if (HorizontalScrollBar != nullptr) {
            HorizontalScrollBar->SetPositionInternal(ScrollOffset.X);
        }
        if (VerticalScrollBar != nullptr) {
            VerticalScrollBar->SetPositionInternal(ScrollOffset.Y);
        }
    }

    void TPanelScrollBarGI::SetDepth(double NewDepth) {
        GI_MessageLoop::TObjectGI::SetDepth(NewDepth);
        if (ScrollbarsOutside) {
            HorizontalScrollBar->SetDepth(NewDepth);
            VerticalScrollBar->SetDepth(NewDepth);
        }
    }

    void TPanelScrollBarGI::UpdateScrollbarPlacement() {
        if (!ScrollbarsOutside) {
            if (AutoHorizontalPlacement) {
                HorizontalScrollBar->SetPosition(ClassesImports::Point(0, ClientSize.Y - HorizontalScrollBar->ClientSize.Y));
                HorizontalScrollBar->SetSize(ClassesImports::Point(ClientSize.X - VerticalScrollBar->ClientSize.X, HorizontalScrollBar->ClientSize.Y));
            } else {
                HorizontalScrollBar->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 0)));
                HorizontalScrollBar->SetSize(EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 0))));
            }
            if (AutoVerticalPlacement) {
                VerticalScrollBar->SetPosition(ClassesImports::Point(ClientSize.X - VerticalScrollBar->ClientSize.X, 0));
                VerticalScrollBar->SetSize(ClassesImports::Point(VerticalScrollBar->ClientSize.X, ClientSize.Y - HorizontalScrollBar->ClientSize.Y));
            } else {
                VerticalScrollBar->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 0)));
                VerticalScrollBar->SetSize(EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 0))));
            }
        } else {
            if (AutoHorizontalPlacement) {
                HorizontalScrollBar->SetPosition(ClassesImports::Point(LocalPosition.X, LocalPosition.Y + ClientSize.Y));
                HorizontalScrollBar->SetSize(ClassesImports::Point(ClientSize.X, HorizontalScrollBar->ClientSize.Y));
            } else {
                HorizontalScrollBar->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 0)));
                HorizontalScrollBar->SetSize(EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&HorizontalScrollBarRect, 0))));
            }
            if (AutoVerticalPlacement) {
                VerticalScrollBar->SetPosition(ClassesImports::Point(LocalPosition.X + ClientSize.X, LocalPosition.Y));
                VerticalScrollBar->SetSize(ClassesImports::Point(VerticalScrollBar->ClientSize.X, ClientSize.Y));
            } else {
                VerticalScrollBar->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 0)));
                VerticalScrollBar->SetSize(EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&VerticalScrollBarRect, 0))));
            }
        }
    }

    // Only active PositionModeW children contribute; scrollbars are excluded.
    void TPanelScrollBarGI::UpdateScrollRanges() {
        Types::TRect Bounds{};
        Types::TRect ChildBounds{};
        if (!Active) {
            return;
        }
        Bounds.Left = 0x7ffffff0;
        Bounds.Top = 0x7ffffff0;
        Bounds.Right = -0x7ffffff0;
        Bounds.Bottom = -0x7ffffff0;
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            if (Child != HorizontalScrollBar && Child != VerticalScrollBar && Child->PositionModeW == true && Child->Active == true) {
                ChildBounds = Child->GetLocalBounds();
                if (ChildBounds.Left < Bounds.Left) {
                    Bounds.Left = ChildBounds.Left;
                }
                if (ChildBounds.Top < Bounds.Top) {
                    Bounds.Top = ChildBounds.Top;
                }
                if (ChildBounds.Right > Bounds.Right) {
                    Bounds.Right = ChildBounds.Right;
                }
                if (ChildBounds.Bottom > Bounds.Bottom) {
                    Bounds.Bottom = ChildBounds.Bottom;
                }
            }
            Child = Child->NextSibling;
        }
        if (HorizontalScrollBar != nullptr) {
            HorizontalScrollBar->SetRange(Bounds.Left, Bounds.Right - 1);
            HorizontalScrollBar->SetPageSize(ClientSize.X);
            HorizontalScrollBar->SetPositionInternal(ScrollOffset.X);
        }
        if (VerticalScrollBar != nullptr) {
            VerticalScrollBar->SetRange(Bounds.Top, Bounds.Bottom - 1);
            VerticalScrollBar->SetPageSize(ClientSize.Y);
            VerticalScrollBar->SetPositionInternal(ScrollOffset.Y);
        }
    }

    void TPanelScrollBarGI::ScrollbarPositionChanged(GI_MessageLoop::TObjectGI* Sender) {
        SetScrollOffset(ClassesImports::Point(HorizontalScrollBar->Position, VerticalScrollBar->Position));
    }

    // Clamps the panel back to scrollbar positions when UnlimitedWorld is false.
    void TPanelScrollBarGI::PanelScrollChanged(GI_MessageLoop::TObjectGI* Sender) {
        HorizontalScrollBar->SetPositionInternal(ScrollOffset.X);
        VerticalScrollBar->SetPositionInternal(ScrollOffset.Y);
        if (!UnlimitedWorld) {
            SetScrollOffset(ClassesImports::Point(HorizontalScrollBar->Position, VerticalScrollBar->Position));
        }
    }

    void TPanelScrollBarGI::ScrollbarDestroyed(GI_MessageLoop::TObjectGI* Sender) {
        if (HorizontalScrollBar == Sender) {
            HorizontalScrollBar = nullptr;
        }
        if (VerticalScrollBar == Sender) {
            VerticalScrollBar = nullptr;
        }
    }

    void TPanelScrollBarGI_LoadFromConfigPath(TPanelScrollBarGI* Self, const pas::WideString& Path) {
        GI_Panel::TPanelGI_LoadFromConfigPath(Self, Path);
        Self->LoadScrollbarPanelProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TPanelScrollBarGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        LoadScrollbarPanelProperties(Block);
    }

    void TPanelScrollBarGI::LoadScrollbarPanelProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"StyleBarX"_wref.get()) > 0) {
            HorizontalScrollBar->SetConfigPath(Block->GetParam(u"StyleBarX"_wref.get()));
        }
        if (Block->CountParams(u"StyleBarY"_wref.get()) > 0) {
            VerticalScrollBar->SetConfigPath(Block->GetParam(u"StyleBarY"_wref.get()));
        }
        if (Block->CountParams(u"ActiveBarX"_wref.get()) > 0) {
            if (Block->GetParam(u"ActiveBarX"_wref.get()) == u"True") {
                SetHorizontalScrollbarEnabled(true);
            } else {
                SetHorizontalScrollbarEnabled(false);
            }
        }
        if (Block->CountParams(u"ActiveBarY"_wref.get()) > 0) {
            if (Block->GetParam(u"ActiveBarY"_wref.get()) == u"True") {
                SetVerticalScrollbarEnabled(true);
            } else {
                SetVerticalScrollbarEnabled(false);
            }
        }
        if (Block->CountParams(u"ExternalSB"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"ExternalSB"_wref.get())) == u"True") {
                SetScrollbarsOutside(true);
            } else {
                SetScrollbarsOutside(false);
            }
        }
        if (Block->CountParams(u"UnlimitedWorld"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"UnlimitedWorld"_wref.get())) == u"True") {
                SetUnlimitedWorldEnabled(true);
            } else {
                SetUnlimitedWorldEnabled(false);
            }
        }
        if (Block->CountParams(u"PosAutoBarX"_wref.get()) > 0) {
            AutoHorizontalPlacement = GI_Main::ParseEnabledNameGI(Block->GetParam(u"PosAutoBarX"_wref.get()));
        }
        if (Block->CountParams(u"PosAutoBarY"_wref.get()) > 0) {
            AutoVerticalPlacement = GI_Main::ParseEnabledNameGI(Block->GetParam(u"PosAutoBarY"_wref.get()));
        }
        if (Block->CountParams(u"RectBarX"_wref.get()) > 0) {
            HorizontalScrollBarRect = GI_Main::GetRectGI(Block->GetParam(u"RectBarX"_wref.get()));
        }
        if (Block->CountParams(u"RectBarY"_wref.get()) > 0) {
            VerticalScrollBarRect = GI_Main::GetRectGI(Block->GetParam(u"RectBarY"_wref.get()));
        }
        UpdateScrollbarPlacement();
        UpdateScrollRanges();
    }

    void TPanelScrollBarGI::p_destroy() {
        GI_PanelScrollBar::TPanelScrollBarGI_Destroy(this);
    }

    void TPanelScrollBarGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_PanelScrollBar::TPanelScrollBarGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_PanelScrollBar
