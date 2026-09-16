#include "layout/GR_Rect.hpp"
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Rect.hpp"

namespace GR_Rect {
    void TRectGR_Create(TRectGR* Self) {
        pas::object_create(Self);
    }

    void TRectGR_Destroy(TRectGR* Self) {
        pas::object_destroy(Self);
    }

    void TArrayRectGR_Create(TArrayRectGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TArrayRectGR_Destroy(TArrayRectGR* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TArrayRectGR::Clear() {
        TRectGR* Removed{};
        TRectGR* Node = FirstRect;
        while (Node != nullptr) {
            Removed = Node;
            Node = Node->Next;
            pas::free(Removed);
        }
        FirstRect = nullptr;
        LastRect = nullptr;
    }

    TRectGR* TArrayRectGR::AllocateRectNode() {
        TRectGR* Node = pas::construct_call<TRectGR>(TRectGR_Create);
        if (LastRect != nullptr) {
            LastRect->Next = Node;
        }
        Node->Prev = LastRect;
        Node->Next = nullptr;
        LastRect = Node;
        if (FirstRect == nullptr) {
            FirstRect = Node;
        }
        return Node;
    }

    void TArrayRectGR::RemoveRectNode(TRectGR* RectNode) {
        if (RectNode->Prev != nullptr) {
            RectNode->Prev->Next = RectNode->Next;
        }
        if (RectNode->Next != nullptr) {
            RectNode->Next->Prev = RectNode->Prev;
        }
        if (LastRect == RectNode) {
            LastRect = RectNode->Prev;
        }
        if (FirstRect == RectNode) {
            FirstRect = RectNode->Next;
        }
        pas::free(RectNode);
    }

    // Maintains nonoverlapping coverage.
    void TArrayRectGR::AddRect(Types::TRect Rect) {
        TRectGR* Removed{};
        TRectGR* Node = LastRect;
        while (Node != nullptr) {
            {
                Types::TRect& cpp_with = Node->Bounds;
                if (Rect.Left >= cpp_with.Left && Rect.Right <= cpp_with.Right && Rect.Top >= cpp_with.Top && Rect.Bottom <= cpp_with.Bottom) {
                    return;
                }
            }
            Node = Node->Prev;
        }
        Node = LastRect;
        while (Node != nullptr) {
            Types::TRect& cpp_with_2 = Node->Bounds;
            if (cpp_with_2.Left >= Rect.Left && cpp_with_2.Right <= Rect.Right && cpp_with_2.Top >= Rect.Top && cpp_with_2.Bottom <= Rect.Bottom) {
                Removed = Node;
                Node = Node->Prev;
                RemoveRectNode(Removed);
            } else {
                Node = Node->Prev;
            }
        }
        InsertRectFragment(Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
    }

    void TArrayRectGR::InsertRectFragment(std::int32_t Left, std::int32_t Top, std::int32_t Right, std::int32_t Bottom) {
        std::int32_t ExistingLeft{};
        std::int32_t ExistingTop{};
        std::int32_t ExistingRight{};
        std::int32_t ExistingBottom{};
        TRectGR* Node = LastRect;
        while (Node != nullptr) {
            ExistingLeft = Node->Bounds.Left;
            ExistingRight = Node->Bounds.Right;
            ExistingTop = Node->Bounds.Top;
            ExistingBottom = Node->Bounds.Bottom;
            if (Left >= ExistingLeft && Right <= ExistingRight && Top >= ExistingTop && Bottom <= ExistingBottom) {
                return;
            }
            if (Left < ExistingRight && Right > ExistingLeft && Top < ExistingBottom && Bottom > ExistingTop) {
                break;
            }
            Node = Node->Prev;
        }
        if (Node == nullptr) {
            Node = AllocateRectNode();
            Node->Bounds.Left = Left;
            Node->Bounds.Top = Top;
            Node->Bounds.Right = Right;
            Node->Bounds.Bottom = Bottom;
            return;
        }
        std::int32_t OutsideEdges = 0;
        if (Left < ExistingLeft) {
            OutsideEdges |= 1;
        }
        if (Right > ExistingRight) {
            OutsideEdges |= 8;
        }
        if (Top < ExistingTop) {
            OutsideEdges |= 16;
        }
        if (Bottom > ExistingBottom) {
            OutsideEdges |= 128;
        }
        if (OutsideEdges == 1) {
            InsertRectFragment(Left, Top, ExistingLeft, Bottom);
        } else if (OutsideEdges == 8) {
            InsertRectFragment(ExistingRight, Top, Right, Bottom);
        } else if (OutsideEdges == 16) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
        } else if (OutsideEdges == 128) {
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
        } else if (OutsideEdges == 9) {
            InsertRectFragment(Left, Top, ExistingLeft, Bottom);
            InsertRectFragment(ExistingRight, Top, Right, Bottom);
        } else if (OutsideEdges == 144) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
        } else if (OutsideEdges == 17) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(Left, ExistingTop, ExistingLeft, Bottom);
        } else if (OutsideEdges == 24) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(ExistingRight, ExistingTop, Right, Bottom);
        } else if (OutsideEdges == 129) {
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
            InsertRectFragment(Left, Top, ExistingLeft, ExistingBottom);
        } else if (OutsideEdges == 136) {
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
            InsertRectFragment(ExistingRight, Top, Right, ExistingBottom);
        } else if (OutsideEdges == 145) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(Left, ExistingTop, ExistingLeft, ExistingBottom);
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
        } else if (OutsideEdges == 152) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(ExistingRight, ExistingTop, Right, ExistingBottom);
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
        } else if (OutsideEdges == 25) {
            InsertRectFragment(Left, Top, Right, ExistingTop);
            InsertRectFragment(Left, ExistingTop, ExistingLeft, Bottom);
            InsertRectFragment(ExistingRight, ExistingTop, Right, Bottom);
        } else if (OutsideEdges == 137) {
            InsertRectFragment(Left, ExistingBottom, Right, Bottom);
            InsertRectFragment(Left, Top, ExistingLeft, ExistingBottom);
            InsertRectFragment(ExistingRight, Top, Right, ExistingBottom);
        }
    }

    void TArrayRectGR::AddScreenClippedRect(Types::TRect Rect, Types::TPoint UnusedPoint1, Types::TPoint UnusedPoint2) {
        Types::TRect Clipped{};
        if (EC_Struct::IntersectRects(Clipped, Rect, GR_Main::GameScreenRect)) {
            AddRect(Clipped);
        }
    }

    void TRectGR::p_destroy() {
        GR_Rect::TRectGR_Destroy(this);
    }

    void TArrayRectGR::p_destroy() {
        GR_Rect::TArrayRectGR_Destroy(this);
    }

} // namespace GR_Rect
