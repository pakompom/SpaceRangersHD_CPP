#include "layout/GI_Panel.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_Panel {
    void TPanelGI_Create(TPanelGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->SkipOwnQueuedDraw = 1;
        Self->DragScrollingEnabled = false;
        Self->ScrollType = pstAll;
    }

    void TPanelGI_Destroy(TPanelGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPanelGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
        ScrollOffset.X = 0;
        ScrollOffset.Y = 0;
        Dragging = false;
        DragScrollingEnabled = false;
        ScrollType = pstAll;
        ScrollAxis = psaBoth;
    }

    // Only ModeW children are affected by scrolling.
    Types::TPoint TPanelGI::GetChildAbsolutePosition(Types::TPoint LocalPosition, std::uint8_t ModeW) {
        Types::TPoint Result{};
        if (!ModeW) {
            Result.X = AbsolutePosition.X + LocalPosition.X;
            Result.Y = AbsolutePosition.Y + LocalPosition.Y;
        } else {
            Result.X = AbsolutePosition.X + LocalPosition.X - ScrollOffset.X;
            Result.Y = AbsolutePosition.Y + LocalPosition.Y - ScrollOffset.Y;
        }
        return Result;
    }

    Types::TPoint TPanelGI::ToLocalPoint(Types::TPoint Point) {
        Types::TPoint Result{};
        Result.X = Point.X - AbsolutePosition.X + ScrollOffset.X;
        Result.Y = Point.Y - AbsolutePosition.Y + ScrollOffset.Y;
        return Result;
    }

    Types::TPoint TPanelGI::ToAbsolutePoint(Types::TPoint Point) {
        Types::TPoint Result{};
        Result.X = Point.X + AbsolutePosition.X - ScrollOffset.X;
        Result.Y = Point.Y + AbsolutePosition.Y - ScrollOffset.Y;
        return Result;
    }

    void TPanelGI::SetDragScrollingEnabled(std::uint8_t Value) {
        if (Value != DragScrollingEnabled) {
            DragScrollingEnabled = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
        }
    }

    void TPanelGI::SetScrollOffset(Types::TPoint Offset) {
        GI_MessageLoop::TObjectGI* Child{};
        Types::TPoint Delta{};
        Types::TRect DestRect{};
        Types::TRect SourceRect{};
        if (ScrollOffset.X == Offset.X && ScrollOffset.Y == Offset.Y) {
            return;
        }
        if (ScrollType == pstSimple) {
            ScrollOffset = Offset;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
        } else if (ScrollType == pstAll) {
            ScrollOffset = Offset;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
        } else if (ScrollType == pstObj) {
            MessageLoop->RegionDrawPending = true;
            MessageLoop->InvalidateMouseViewControls();
            Child = FirstChild;
            while (Child != nullptr) {
                if (Child->PositionModeW) {
                    Child->Invalidate();
                }
                Child = Child->NextSibling;
            }
            ScrollOffset = Offset;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Child = FirstChild;
            while (Child != nullptr) {
                if (Child->PositionModeW) {
                    Child->Invalidate();
                }
                Child = Child->NextSibling;
            }
        } else if (ScrollType == pstView) {
            Delta.X = ScrollOffset.X - Offset.X;
            Delta.Y = ScrollOffset.Y - Offset.Y;
            if (pas::abs(Delta.X) > ClientSize.X / 2 || pas::abs(Delta.Y) > ClientSize.Y / 2) {
                ScrollOffset = Offset;
                UpdateAbsolutePosition();
                UpdateSubtreeHitBounds();
                Invalidate();
                return;
            }
            MessageLoop->DrawQueuedUpdateRects();
            ScrollOffset = Offset;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            MessageLoop->RootUiObject->InvalidateScrollOverlap(HitTestBounds, Delta, this);
            DestRect = GetLocalBounds();
            SourceRect = DestRect;
            if (Delta.X > 0) {
                InvalidateRect(ClassesImports::Rect(DestRect.Left, DestRect.Top, DestRect.Left + Delta.X, DestRect.Bottom));
                DestRect.Left += Delta.X;
                SourceRect.Right -= Delta.X;
            } else if (Delta.X < 0) {
                InvalidateRect(ClassesImports::Rect(DestRect.Right + Delta.X, DestRect.Top, DestRect.Right, DestRect.Bottom));
                DestRect.Right += Delta.X;
                SourceRect.Left -= Delta.X;
            }
            if (Delta.Y > 0) {
                InvalidateRect(ClassesImports::Rect(DestRect.Left, DestRect.Top, DestRect.Right, DestRect.Top + Delta.Y));
                DestRect.Top += Delta.Y;
                SourceRect.Bottom -= Delta.Y;
            } else if (Delta.Y < 0) {
                InvalidateRect(ClassesImports::Rect(DestRect.Left, DestRect.Bottom + Delta.Y, DestRect.Right, DestRect.Bottom));
                DestRect.Bottom += Delta.Y;
                SourceRect.Top -= Delta.Y;
            }
            {
                void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                GR_Main::Ex_OKGR_CopySingleBuf_XY_XY_WORD(pixels, pitchBytes, DestRect.Left, DestRect.Top, SourceRect.Left, SourceRect.Top, SourceRect.Right - SourceRect.Left, SourceRect.Bottom - SourceRect.Top);
            }
        }
    }

    Types::TRect TPanelGI::GetVisibleContentRect() {
        Types::TRect Result{};
        Result.Left = ScrollOffset.X - OriginPoint.X;
        Result.Top = ScrollOffset.Y - OriginPoint.Y;
        Result.Right = Result.Left + ClientSize.X;
        Result.Bottom = Result.Top + ClientSize.Y;
        return Result;
    }

    void TPanelGI::ScrollRectIntoView(Types::TRect Rect) {
        Types::TPoint Offset{};
        Types::TRect Visible{};
        Offset = ScrollOffset;
        Visible = GetVisibleContentRect();
        if (Rect.Bottom > Visible.Bottom) {
            Offset.Y = Rect.Bottom - (Visible.Bottom - Visible.Top);
            SetScrollOffset(Offset);
        }
        Offset = ScrollOffset;
        Visible = GetVisibleContentRect();
        if (Rect.Top < Visible.Top) {
            Offset.Y = Rect.Top;
            SetScrollOffset(Offset);
        }
        Offset = ScrollOffset;
        Visible = GetVisibleContentRect();
        if (Rect.Right > Visible.Right) {
            Offset.X = Rect.Right - (Visible.Right - Visible.Left);
            SetScrollOffset(Offset);
        }
        Offset = ScrollOffset;
        Visible = GetVisibleContentRect();
        if (Rect.Left < Visible.Left) {
            Offset.X = Rect.Left;
            SetScrollOffset(Offset);
        }
    }

    void TPanelGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        Dragging = false;
    }

    void TPanelGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        std::int32_t X{};
        std::int32_t Y{};
        GI_MessageLoop::TObjectGI::ProcessMouseMove(KeyState, Point);
        if (Dragging == true) {
            if (Point.X != LastDragPoint.X || Point.Y != LastDragPoint.Y) {
                if (MessageLoop->IsCursorImageSelected(u"Main"_wref.get())) {
                    MessageLoop->SetCursorByName(u"Scroll"_wref.get());
                }
                if (ScrollAxis == psaHorizontal || ScrollAxis == psaBoth) {
                    X = ScrollOffset.X + LastDragPoint.X - Point.X;
                } else {
                    X = ScrollOffset.X;
                }
                if (ScrollAxis == psaVertical || ScrollAxis == psaBoth) {
                    Y = ScrollOffset.Y + LastDragPoint.Y - Point.Y;
                } else {
                    Y = ScrollOffset.Y;
                }
                SetScrollOffset(ClassesImports::Point(X, Y));
                LastDragPoint = Point;
                if (pas::assigned(ScrollChangedCallback)) {
                    ScrollChangedCallback(this);
                }
            }
        }
    }

    void TPanelGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        Dragging = false;
    }

    void TPanelGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        Dragging = false;
        if (MessageLoop->IsCursorImageSelected(u"Scroll"_wref.get())) {
            MessageLoop->SetCursorByName(u"Main"_wref.get());
        }
    }

    void TPanelGI::ProcessRightButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessRightButtonDown(KeyState, Point);
        if (static_cast<std::uint8_t>(IsOccludedAtPoint(Point) ^ 1) && DragScrollingEnabled == true) {
            Dragging = true;
            LastDragPoint = Point;
            if (MessageLoop->IsCursorImageSelected(u"Main"_wref.get())) {
                MessageLoop->SetCursorByName(u"Scroll"_wref.get());
            }
        }
    }

    void TPanelGI::ProcessRightButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessRightButtonUp(KeyState, Point);
        Dragging = false;
        if (MessageLoop->IsCursorImageSelected(u"Scroll"_wref.get())) {
            MessageLoop->SetCursorByName(u"Main"_wref.get());
        }
    }

    void TPanelGI::LoadFromConfigPath(const pas::WideString& Path) {
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"CenterWorld"_wref.get()) > 0) {
            Text = Block->GetParam(u"CenterWorld"_wref.get());
            ScrollOffset.X = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            ScrollOffset.Y = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
        }
        if (Block->CountParams(u"MoveWorld"_wref.get()) > 0) {
            DragScrollingEnabled = GI_Main::ParseEnabledNameGI(Block->GetParam(u"MoveWorld"_wref.get()));
        }
    }

    void TPanelGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        if (Block->CountParams(u"CenterWorld"_wref.get()) > 0) {
            Text = Block->GetParam(u"CenterWorld"_wref.get());
            ScrollOffset.X = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            ScrollOffset.Y = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
        }
        if (Block->CountParams(u"MoveWorld"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"MoveWorld"_wref.get())) == u"True") {
                DragScrollingEnabled = true;
            }
        }
        if (Block->CountParams(u"TypeScroll"_wref.get()) > 0) {
            Text = Block->GetParam(u"TypeScroll"_wref.get());
            if (Text == u"Simple") {
                ScrollType = pstSimple;
            } else if (Text == u"All") {
                ScrollType = pstAll;
            } else if (Text == u"Obj") {
                ScrollType = pstObj;
            } else if (Text == u"View") {
                ScrollType = pstView;
            }
        }
    }

    void TPanelGI::p_destroy() {
        GI_Panel::TPanelGI_Destroy(this);
    }

} // namespace GI_Panel
