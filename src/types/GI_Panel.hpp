#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Panel {
    struct TPanelGI;

    enum TPanelScrollTypeGI : std::uint8_t {
        pstSimple = 0,
        pstAll = 1,
        pstObj = 2,
        pstView = 3,
    };

    enum TPanelScrollAxisGI : std::uint8_t {
        psaHorizontal = 0,
        psaVertical = 1,
        psaBoth = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPanelGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TPanelGI, GI_MessageLoop::TObjectGI, "TPanelGI", 316)
        void p_destroy() override;
        void Clear() override;
        // Only ModeW children are affected by scrolling.
        Types::TPoint GetChildAbsolutePosition(Types::TPoint LocalPosition, std::uint8_t ModeW) override;
        Types::TPoint ToLocalPoint(Types::TPoint Point) override;
        Types::TPoint ToAbsolutePoint(Types::TPoint Point) override;
        void SetDragScrollingEnabled(std::uint8_t Value);
        virtual void SetScrollOffset(Types::TPoint Offset);
        Types::TRect GetVisibleContentRect();
        void ScrollRectIntoView(Types::TRect Rect);
        void OnActivate() override;
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessRightButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessRightButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t DragScrollingEnabled;
        TPanelScrollTypeGI ScrollType;
        std::uint8_t Dragging;
        Types::TPoint LastDragPoint;
        std::uint8_t cpp_padding[5];
        GI_MessageLoop::TObjectNotifyEventGI ScrollChangedCallback;
        TPanelScrollAxisGI ScrollAxis;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Panel
