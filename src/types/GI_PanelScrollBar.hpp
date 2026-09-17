#pragma once
#include "runtime_support.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_ScrollBar {
    struct TScrollBarGI;

} // namespace GI_ScrollBar

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPanelScrollBarGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TPanelScrollBarGI, GI_Panel::TPanelGI, "TPanelScrollBarGI", 364)
        void p_destroy() override;
        void Clear() override;
        void SetVerticalScrollBarConfigPath(pas::WideString Path);
        void SetHorizontalScrollbarEnabled(std::uint8_t Value);
        std::uint8_t IsVerticalScrollbarEnabled();
        void SetVerticalScrollbarEnabled(std::uint8_t Value);
        // The panel retains ownership of scrollbars parented outside it.
        void SetScrollbarsOutside(std::uint8_t Value);
        void SetUnlimitedWorldEnabled(std::uint8_t Value);
        void SetSize(Types::TPoint Size) override;
        void SetOrigin(Types::TPoint Origin) override;
        void SetScrollOffset(Types::TPoint Offset) override;
        void SetDepth(double NewDepth) override;
        void UpdateScrollbarPlacement();
        // Only active PositionModeW children contribute; scrollbars are excluded.
        void UpdateScrollRanges();
        void ScrollbarPositionChanged(GI_MessageLoop::TObjectGI* Sender);
        // Clamps the panel back to scrollbar positions when UnlimitedWorld is false.
        void PanelScrollChanged(GI_MessageLoop::TObjectGI* Sender);
        void ScrollbarDestroyed(GI_MessageLoop::TObjectGI* Sender);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadScrollbarPanelProperties(EC_BlockPar::TBlockParEC* Block);
        std::uint8_t cpp_padding[4];
        GI_ScrollBar::TScrollBarGI* HorizontalScrollBar;
        GI_ScrollBar::TScrollBarGI* VerticalScrollBar;
        std::uint8_t AutoHorizontalPlacement;
        std::uint8_t AutoVerticalPlacement;
        Types::TRect HorizontalScrollBarRect;
        Types::TRect VerticalScrollBarRect;
        std::uint8_t ScrollbarsOutside;
        std::uint8_t UnlimitedWorld;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_PanelScrollBar
