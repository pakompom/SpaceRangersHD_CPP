#include "layout/GI_Grid.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_Grid.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Line.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_PanelScrollBar.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_Grid {
    // Rows points into a Delphi dynamic array. ColumnWidths uses the EC heap.
    // Each cell is a TLabelGI child with column/row packed into the dword.
    void TGridGI_Create(TGridGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_PanelScrollBar::TPanelScrollBarGI_Create(Self, Owner);
        Self->TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->GridType = gtCell;
        Self->GridColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->ActiveCellFrame = pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, Self);
        Self->ActiveCellFrame->SetPositionModeW(true);
        Self->ActiveCellFrame->SetKind(GI_Frame::fkRect);
        Self->ActiveCellFrame->SetDepth(-3.0);
        Self->ActiveCellFrame->SetColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0));
        Self->ActiveCellFrame->UserValue = GridDecorationTag;
        Self->SetDragScrollingEnabled(true);
        Self->SetScrollbarsOutside(true);
        Self->SetUnlimitedWorldEnabled(false);
    }

    void TGridGI_Destroy(TGridGI* Self) {
        Self->Clear();
        GI_PanelScrollBar::TPanelScrollBarGI_Destroy(Self);
    }

    void TGridGI::Clear() {
        if (ControlName == u"DebugGrid") {
            SetColumnCount(0);
        }
        SetColumnCount(0);
        SetRowCount(0);
        if (ColumnWidths != nullptr) {
            EC_Mem::FreeEC(ColumnWidths);
        }
        ColumnWidths = nullptr;
        TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        GridType = gtCell;
        GridColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BackgroundImage = nullptr;
        ActiveCellImage = nullptr;
        ActiveCell.X = -1;
        ActiveCell.Y = -1;
    }

    void TGridGI::LayoutCell(GI_MessageLoop::TObjectGI* Child) {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t I{};
        if (pas::class_cast_if<GI_Label::TLabelGI*>(Child) != nullptr) {
            {
                std::int32_t cpp_arg = GetRowHeight(pas::shr(Child->UserValue, GridCellRowShift)) + 1;
                std::int32_t cpp_arg_2 = GetColumnWidth(Child->UserValue & GridCellCoordinateMask) + 1;
                Child->SetSize(ClassesImports::Point(cpp_arg_2, cpp_arg));
            }
            X = 0;
            Y = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, (Child->UserValue & GridCellCoordinateMask) - 1); cpp_range.next(I); ) {
                X += GetColumnWidth(I);
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::shr(Child->UserValue, GridCellRowShift) - 1); cpp_range_2.next(I); ) {
                Y += GetRowHeight(I);
            }
            Child->SetPosition(ClassesImports::Point(X, Y));
            if (!Child->PositionModeW) {
                Child->SetPositionModeW(true);
                Child->SetDepth(-1.0);
                {
                    GI_Label::TLabelGI* cpp_with = pas::checked_cast<GI_Label::TLabelGI*>(Child);
                    cpp_with->SetFontName(FontName);
                    cpp_with->SetTextAlignX(GI_Main::taxLeft);
                    cpp_with->SetTextAlignY(GI_Main::tayTop);
                    cpp_with->LeftButtonDownCallback = pas::bind_method<&TGridGI::CellClick>(this);
                    cpp_with->SetTextColor(TextColor);
                }
            }
        }
    }

    void TGridGI::LayoutCells() {
        GI_MessageLoop::TObjectGI* Current{};
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            Current = Child;
            Child = Child->NextSibling;
            LayoutCell(Current);
        }
    }

    void TGridGI::UpdateGridExtent() {
        std::int32_t I{};
        std::int32_t X = 0;
        std::int32_t Y = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColumnCount - 1); cpp_range.next(I); ) {
            X += GetColumnWidth(I);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, RowCount - 1); cpp_range_2.next(I); ) {
            Y += GetRowHeight(I);
        }
        if (BackgroundImage != nullptr) {
            BackgroundImage->SetSize(ClassesImports::Point(X, Y));
        }
        if (ActiveCell.X < 0 || ActiveCell.X >= ColumnCount || ActiveCell.Y < 0 || ActiveCell.Y >= RowCount) {
            ActiveCell = ClassesImports::Point(-1, -1);
        }
        RebuildGridLines();
        UpdateScrollRanges();
    }

    // Type 0 hides lines, 1 draws both axes, 2 horizontal separators, 3 vertical separators; nonzero types include the outer border.
    void TGridGI::RebuildGridLines() {
        GI_MessageLoop::TObjectGI* Current{};
        std::int32_t I{};
        std::int32_t Position{};
        GI_Line::TLineGI* Line{};
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            Current = Child;
            Child = Child->NextSibling;
            if (pas::class_cast_if<GI_Line::TLineGI*>(Current) != nullptr) {
                FreeOwnedChild(Current);
            }
        }
        if (GridType == gtHide || ColumnCount < 1 || RowCount < 1) {
            return;
        }
        std::int32_t X = 0;
        std::int32_t Y = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColumnCount - 1); cpp_range.next(I); ) {
            X += GetColumnWidth(I);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, RowCount - 1); cpp_range_2.next(I); ) {
            Y += GetRowHeight(I);
        }
        if ((GridType == gtCell || GridType == gtRow) && RowCount >= 2) {
            Position = GetRowHeight(0);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, RowCount - 1); cpp_range_3.next(I); ) {
                Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
                Line->SetPosition(ClassesImports::Point(0, Position));
                Line->SetSize(ClassesImports::Point(X + 1, 1));
                Line->SetPositionModeW(true);
                Line->SetColor(GridColor);
                Line->SetDepth(-2.0);
                Line->UserValue = GridDecorationTag;
                Position += GetRowHeight(I);
            }
        }
        if ((GridType == gtCell || GridType == gtCol) && ColumnCount >= 2) {
            Position = GetColumnWidth(0);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, ColumnCount - 1); cpp_range_4.next(I); ) {
                Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
                Line->SetPosition(ClassesImports::Point(Position, 0));
                Line->SetSize(ClassesImports::Point(1, Y + 1));
                Line->SetPositionModeW(true);
                Line->SetColor(GridColor);
                Line->SetDepth(-2.0);
                Line->UserValue = GridDecorationTag;
                Position += GetColumnWidth(I);
            }
        }
        Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
        Line->SetPosition(ClassesImports::Point(0, 0));
        Line->SetSize(ClassesImports::Point(X + 1, 1));
        Line->SetPositionModeW(true);
        Line->SetColor(GridColor);
        Line->SetDepth(-2.0);
        Line->UserValue = GridDecorationTag;
        Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
        Line->SetPosition(ClassesImports::Point(0, Y));
        Line->SetSize(ClassesImports::Point(X + 1, 1));
        Line->SetPositionModeW(true);
        Line->SetColor(GridColor);
        Line->SetDepth(-2.0);
        Line->UserValue = GridDecorationTag;
        Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
        Line->SetPosition(ClassesImports::Point(0, 0));
        Line->SetSize(ClassesImports::Point(1, Y + 1));
        Line->SetPositionModeW(true);
        Line->SetColor(GridColor);
        Line->SetDepth(-2.0);
        Line->UserValue = GridDecorationTag;
        Line = pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, this);
        Line->SetPosition(ClassesImports::Point(X, 0));
        Line->SetSize(ClassesImports::Point(1, Y + 1));
        Line->SetPositionModeW(true);
        Line->SetColor(GridColor);
        Line->SetDepth(-2.0);
        Line->UserValue = GridDecorationTag;
    }

    void TGridGI::UpdateRowAutoHeight(std::int32_t RowIndex) {
        std::int32_t CellHeight{};
        std::int32_t Height = Rows[RowIndex].AutoHeightMinimum;
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            if (pas::class_cast_if<GI_Label::TLabelGI*>(Child) != nullptr && pas::shr(Child->UserValue, GridCellRowShift) == RowIndex) {
                CellHeight = pas::checked_cast<GI_Label::TLabelGI*>(Child)->MeasureContentSize(nullptr).Y + 2;
                if (CellHeight > Height) {
                    Height = CellHeight;
                }
            }
            Child = Child->NextSibling;
        }
        SetRowHeight(RowIndex, Height);
    }

    void TGridGI::UpdateActiveCellVisibility() {
        WindowsSdk::TRect Rect{};
        if (ActiveCell.X >= ColumnCount || ActiveCell.Y >= RowCount) {
            ActiveCell = ClassesImports::Point(-1, -1);
        }
        Rect.Left = ActiveCellFrame->LocalPosition.X;
        Rect.Top = ActiveCellFrame->LocalPosition.Y;
        Rect.Right = ActiveCellFrame->LocalPosition.X + ActiveCellFrame->ClientSize.X;
        Rect.Bottom = ActiveCellFrame->LocalPosition.Y + ActiveCellFrame->ClientSize.Y;
        ScrollRectIntoView(Rect);
        if (ActiveCell.X < 0 || ActiveCell.Y < 0) {
            if (ActiveCellFrame != nullptr) {
                ActiveCellFrame->SetActive(false);
            }
            if (ActiveCellImage != nullptr) {
                ActiveCellImage->SetActive(false);
            }
        } else {
            if (ActiveCellFrame != nullptr) {
                ActiveCellFrame->SetActive(true);
            }
            if (ActiveCellImage != nullptr) {
                ActiveCellImage->SetActive(true);
            }
        }
    }

    // New columns default to 100 pixels.
    void TGridGI::SetColumnCount(std::int32_t Value) {
        GI_MessageLoop::TObjectGI* Child{};
        GI_MessageLoop::TObjectGI* Current{};
        std::int32_t X{};
        std::int32_t Y{};
        if (Value == ColumnCount) {
            return;
        }
        std::int32_t OldCount = ColumnCount;
        ColumnCount = Value;
        ColumnWidths = static_cast<std::int32_t*>(EC_Mem::ReAllocREC(ColumnWidths, Value * static_cast<std::int32_t>(sizeof(std::int32_t))));
        if (Value < OldCount) {
            Child = FirstChild;
            while (Child != nullptr) {
                Current = Child;
                Child = Child->NextSibling;
                if (pas::class_cast_if<GI_Label::TLabelGI*>(Current) != nullptr && (Current->UserValue & GridCellCoordinateMask) >= Value && Current->UserValue != GridDecorationTag) {
                    FreeOwnedChild(Current);
                }
            }
        } else {
            for (auto cpp_range = pas::for_to<std::int32_t>(OldCount, Value - 1); cpp_range.next(X); ) {
                EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(ColumnWidths, X * static_cast<std::int32_t>(sizeof(std::int32_t))), 100);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, RowCount - 1); cpp_range_2.next(Y); ) {
                    Child = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
                    Child->UserValue = X | pas::shl(Y, GridCellRowShift);
                    LayoutCell(Child);
                }
            }
        }
        UpdateGridExtent();
        Invalidate();
    }

    // New rows default to 15 pixels.
    void TGridGI::SetRowCount(std::int32_t Value) {
        GI_MessageLoop::TObjectGI* Child{};
        GI_MessageLoop::TObjectGI* Current{};
        std::int32_t X{};
        std::int32_t Y{};
        if (Value == RowCount) {
            return;
        }
        std::int32_t OldCount = RowCount;
        RowCount = Value;
        Rows.set_length(RowCount);
        if (Value < OldCount) {
            Child = FirstChild;
            while (Child != nullptr) {
                Current = Child;
                Child = Child->NextSibling;
                if (pas::class_cast_if<GI_Label::TLabelGI*>(Current) != nullptr && (pas::shr(Current->UserValue, GridCellRowShift) & GridCellCoordinateMask) >= Value && Current->UserValue != GridDecorationTag) {
                    FreeOwnedChild(Current);
                }
            }
        } else {
            for (auto cpp_range = pas::for_to<std::int32_t>(OldCount, Value - 1); cpp_range.next(Y); ) {
                Rows[Y].Height = 15;
                Rows[Y].AutoHeightMinimum = 15;
                Rows[Y].AutoHeight = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ColumnCount - 1); cpp_range_2.next(X); ) {
                    Child = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this);
                    Child->UserValue = X | pas::shl(Y, GridCellRowShift);
                    LayoutCell(Child);
                }
            }
        }
        UpdateActiveCellVisibility();
        UpdateGridExtent();
        Invalidate();
    }

    void TGridGI::SetGridType(TGridTypeGI Value) {
        if (Value != GridType) {
            GridType = Value;
            LayoutCells();
            UpdateGridExtent();
            Invalidate();
        }
    }

    std::int32_t TGridGI::GetColumnWidth(std::int32_t ColumnIndex) {
        if (ColumnIndex < 0 || ColumnIndex >= ColumnCount) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGridGI.GetSizeX. (", SysUtils::IntToStr(ColumnIndex), "<0) or (", SysUtils::IntToStr(ColumnIndex), ">0", SysUtils::IntToStr(ColumnCount), ")"})));
        }
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(ColumnWidths, ColumnIndex * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    void TGridGI::SetColumnWidth(std::int32_t ColumnIndex, std::int32_t Width) {
        if (GetColumnWidth(ColumnIndex) != Width) {
            EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(ColumnWidths, ColumnIndex * static_cast<std::int32_t>(sizeof(std::int32_t))), Width);
            LayoutCells();
            UpdateGridExtent();
            Invalidate();
        }
    }

    std::int32_t TGridGI::GetRowHeight(std::int32_t RowIndex) {
        if (RowIndex < 0 || RowIndex >= RowCount) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGridGI.GetSizeX. (", SysUtils::IntToStr(RowIndex), "<0) or (", SysUtils::IntToStr(RowIndex), ">=", SysUtils::IntToStr(RowCount), ")"})));
        }
        return Rows[RowIndex].Height;
    }

    // Also updates AutoHeightMinimum when the row's AutoHeight flag is set.
    void TGridGI::SetRowHeight(std::int32_t RowIndex, std::int32_t Height) {
        WindowsSdk::TPoint Cell{};
        if (GetRowHeight(RowIndex) != Height) {
            Rows[RowIndex].Height = Height;
            if (Rows[RowIndex].AutoHeight) {
                Rows[RowIndex].AutoHeightMinimum = Rows[RowIndex].Height;
            }
            LayoutCells();
            UpdateGridExtent();
            Cell = ActiveCell;
            ActiveCell.X = -2;
            SetActiveCell(Cell);
            Invalidate();
        }
    }

    // Does not validate RowIndex.
    void TGridGI::SetRowAutoHeightEnabled(std::int32_t RowIndex, std::uint8_t Enabled) {
        if (Rows[RowIndex].AutoHeight != Enabled) {
            Rows[RowIndex].AutoHeight = Enabled;
            UpdateRowAutoHeight(RowIndex);
        }
    }

    // Raises for out-of-range coordinates or a missing cell label.
    GI_Label::TLabelGI* TGridGI::GetCell(std::int32_t CellX, std::int32_t CellY) {
        if (CellX < 0 || ColumnCount <= CellX || CellY < 0 || RowCount <= CellY) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGridGI.GetCell. Cell=", SysUtils::IntToStr(CellX), ",", SysUtils::IntToStr(CellY), "  Count=", SysUtils::IntToStr(ColumnCount), ",", SysUtils::IntToStr(RowCount)})));
        }
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            if (pas::class_cast_if<GI_Label::TLabelGI*>(Child) != nullptr && (Child->UserValue & GridCellCoordinateMask) == CellX && pas::shr(Child->UserValue, GridCellRowShift) == CellY) {
                return pas::checked_cast<GI_Label::TLabelGI*>(Child);
            }
            Child = Child->NextSibling;
        }
        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TGridGI.GetCell. Cell=", SysUtils::IntToStr(CellX), ",", SysUtils::IntToStr(CellY), "  Count=", SysUtils::IntToStr(ColumnCount), ",", SysUtils::IntToStr(RowCount)})));
    }

    void TGridGI::SetRowSelectEnabled(std::uint8_t Value) {
        if (RowSelect != Value) {
            RowSelect = Value;
            Invalidate();
        }
    }

    void TGridGI::SetColSelectEnabled(std::uint8_t Value) {
        if (ColSelect != Value) {
            ColSelect = Value;
            Invalidate();
        }
    }

    void TGridGI::SetBackgroundImagePath(pas::WideString Path) {
        if (BackgroundImage != nullptr) {
            FreeOwnedChild(BackgroundImage);
            BackgroundImage = nullptr;
        }
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
        Image->UserValue = GridDecorationTag;
        Image->SetDepth(2.0);
        Image->SetImagePath(Path);
        Image->SetImageKindX(GI_Main::ikxLeftFill);
        Image->SetImageKindY(GI_Main::ikyTopFill);
        Image->SetPositionModeW(true);
        BackgroundImage = Image;
        UpdateGridExtent();
    }

    void TGridGI_SetActiveCellImagePath(TGridGI* Self, pas::WideString Path) {
        if (Path == u"") {
            if (Self->ActiveCellImage != nullptr) {
                Self->FreeOwnedChild(Self->ActiveCellImage);
                Self->ActiveCellImage = nullptr;
            }
        } else {
            if (Self->ActiveCellImage == nullptr) {
                Self->ActiveCellImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
            }
            Self->ActiveCellImage->SetImagePath(Path);
            Self->ActiveCellImage->SetImageKindX(GI_Main::ikxLeftFill);
            Self->ActiveCellImage->SetImageKindY(GI_Main::ikyTopFill);
            Self->ActiveCellImage->UserValue = GridDecorationTag;
            Self->ActiveCellImage->SetDepth(1.0);
            Self->ActiveCellImage->SetPositionModeW(true);
        }
        Self->UpdateGridExtent();
        Self->Invalidate();
    }

    void TGridGI::SetActiveCellImageHalfAlpha(std::uint8_t Value) {
        if (ActiveCellImage != nullptr) {
            ActiveCellImage->SetHalfAlpha(Value);
        }
    }

    // Invalid coordinates become (-1,-1); valid cells are scrolled into view.
    void TGridGI::SetActiveCell(WindowsSdk::TPoint Cell) {
        GI_Label::TLabelGI* LabelControl{};
        if (ActiveCell.X == Cell.X && ActiveCell.Y == Cell.Y) {
            return;
        }
        ActiveCell = Cell;
        if (ActiveCell.X >= 0 && ActiveCell.X < ColumnCount && ActiveCell.Y >= 0 && ActiveCell.Y < RowCount) {
            if (ActiveCellImage != nullptr) {
                if (static_cast<std::uint8_t>(RowSelect ^ 1) && static_cast<std::uint8_t>(ColSelect ^ 1)) {
                    LabelControl = GetCell(ActiveCell.X, ActiveCell.Y);
                    ActiveCellImage->SetPosition(LabelControl->LocalPosition);
                    ActiveCellImage->SetSize(LabelControl->ClientSize);
                } else if (RowSelect) {
                    LabelControl = GetCell(0, ActiveCell.Y);
                    ActiveCellImage->SetPosition(LabelControl->LocalPosition);
                    LabelControl = GetCell(ColumnCount - 1, ActiveCell.Y);
                    ActiveCellImage->SetSize(ClassesImports::Point(LabelControl->LocalPosition.X + LabelControl->ClientSize.X, LabelControl->ClientSize.Y));
                } else if (ColSelect) {
                    LabelControl = GetCell(ActiveCell.X, 0);
                    ActiveCellImage->SetPosition(LabelControl->LocalPosition);
                    LabelControl = GetCell(ActiveCell.X, RowCount - 1);
                    ActiveCellImage->SetSize(ClassesImports::Point(LabelControl->ClientSize.X, LabelControl->LocalPosition.Y + LabelControl->ClientSize.Y));
                }
            }
            if (static_cast<std::uint8_t>(RowSelect ^ 1) && static_cast<std::uint8_t>(ColSelect ^ 1)) {
                LabelControl = GetCell(ActiveCell.X, ActiveCell.Y);
                ActiveCellFrame->SetPosition(LabelControl->LocalPosition);
                ActiveCellFrame->SetSize(LabelControl->ClientSize);
            } else if (RowSelect) {
                LabelControl = GetCell(0, ActiveCell.Y);
                ActiveCellFrame->SetPosition(LabelControl->LocalPosition);
                LabelControl = GetCell(ColumnCount - 1, ActiveCell.Y);
                ActiveCellFrame->SetSize(ClassesImports::Point(LabelControl->LocalPosition.X + LabelControl->ClientSize.X, LabelControl->ClientSize.Y));
            } else if (ColSelect) {
                LabelControl = GetCell(ActiveCell.X, 0);
                ActiveCellFrame->SetPosition(LabelControl->LocalPosition);
                LabelControl = GetCell(ActiveCell.X, RowCount - 1);
                ActiveCellFrame->SetSize(ClassesImports::Point(LabelControl->ClientSize.X, LabelControl->LocalPosition.Y + LabelControl->ClientSize.Y));
            }
        } else {
            ActiveCell = ClassesImports::Point(-1, -1);
        }
        UpdateActiveCellVisibility();
        Invalidate();
    }

    // Selection can be vetoed by the callback.
    void TGridGI::SelectCell(WindowsSdk::TPoint Cell) {
        if (Cell.X < 0 || Cell.X >= ColumnCount || Cell.Y < 0 || Cell.Y >= RowCount) {
            return;
        }
        if (pas::assigned(CanSelectCellCallback)) {
            if (!CanSelectCellCallback(this, Cell)) {
                return;
            }
        }
        SetActiveCell(Cell);
        if (pas::assigned(SelectionChangedCallback)) {
            SelectionChangedCallback(this);
        }
    }

    void TGridGI::CellClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t MouseState, WindowsSdk::TPoint Point) {
        WindowsSdk::TPoint Cell{};
        std::uint8_t Repeated{};
        Cell = ClassesImports::Point(Sender->UserValue & GridCellCoordinateMask, pas::shr(Sender->UserValue, GridCellRowShift));
        if (pas::assigned(CanSelectCellCallback)) {
            if (!CanSelectCellCallback(this, Cell)) {
                return;
            }
        }
        if (ActiveCell.Y == Cell.Y && RowSelect || ActiveCell.X == Cell.X && ColSelect || ActiveCell.X == Cell.X && ActiveCell.Y == Cell.Y) {
            Repeated = true;
        } else {
            Repeated = false;
        }
        SetActiveCell(Cell);
        if (Repeated) {
            if (pas::assigned(RepeatedCellClickCallback)) {
                RepeatedCellClickCallback(this);
            }
        } else if (pas::assigned(SelectionChangedCallback)) {
            SelectionChangedCallback(this);
        }
    }

    void TGridGI::ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (IsOccludedAtPoint(Point)) {
            return;
        }
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (Active == true) {
            MessageLoop->SetFocusedControl(this);
        }
    }

    void TGridGI::OnFocusGained() {
        GI_MessageLoop::TObjectGI::OnFocusGained();
    }

    void TGridGI::OnFocusLost() {
        GI_MessageLoop::TObjectGI::OnFocusLost();
    }

    void TGridGI::ProcessKeyDown(std::int32_t Key) {
        if (Key == WindowsSdk::VK_LEFT) {
            SelectCell(ClassesImports::Point(ActiveCell.X - 1, ActiveCell.Y));
        } else if (Key == WindowsSdk::VK_RIGHT) {
            SelectCell(ClassesImports::Point(ActiveCell.X + 1, ActiveCell.Y));
        } else if (Key == WindowsSdk::VK_UP) {
            SelectCell(ClassesImports::Point(ActiveCell.X, ActiveCell.Y - 1));
        } else if (Key == WindowsSdk::VK_DOWN) {
            SelectCell(ClassesImports::Point(ActiveCell.X, ActiveCell.Y + 1));
        }
    }

    void TGridGI_LoadFromConfigPath(TGridGI* Self, const pas::WideString& Path) {
        GI_PanelScrollBar::TPanelScrollBarGI_LoadFromConfigPath(Self, Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        GI_Grid::TGridGI_LoadGridProperties(Self, Block);
    }

    void TGridGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_PanelScrollBar::TPanelScrollBarGI::LoadFromBlock(Block);
        GI_Grid::TGridGI_LoadGridProperties(this, Block);
    }

    void TGridGI_LoadGridProperties(TGridGI* Self, EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        EC_BlockPar::TBlockParEC* Properties{};
        EC_BlockPar::TBlockParEC* CellProperties{};
        GI_Label::TLabelGI* LabelControl{};
        std::int32_t I{};
        std::int32_t RowIndex{};
        std::int32_t Count{};
        std::int32_t CellX{};
        std::int32_t CellY{};
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            Self->FontName = EC_Str::TrimWideString(Block->GetParam(u"Font"_wref.get()));
        }
        if (Block->CountParams(u"TextColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"TextColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"GridType"_wref.get()) > 0) {
            Text = EC_Str::TrimWideString(Block->GetParam(u"GridType"_wref.get()));
            if (Text == u"Hide") {
                Self->SetGridType(gtHide);
            } else if (Text == u"Cell") {
                Self->SetGridType(gtCell);
            } else if (Text == u"Row") {
                Self->SetGridType(gtRow);
            } else if (Text == u"Col") {
                Self->SetGridType(gtCol);
            }
        }
        if (Block->CountParams(u"GridColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"GridColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->GridColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"CountX"_wref.get()) > 0) {
            Self->SetColumnCount(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"CountX"_wref.get()))));
        }
        if (Block->CountParams(u"CountY"_wref.get()) > 0) {
            Self->SetRowCount(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"CountY"_wref.get()))));
        }
        if (Block->CountBlocks(u"GridX"_wref.get()) > 0) {
            Properties = Block->GetBlock(u"GridX"_wref.get());
            Count = Properties->GetParamCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(Properties->GetParamValue(I)));
                std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(Properties->GetParamName(I)));
                Self->SetColumnWidth(strToInt_2, strToInt);
            }
        }
        if (Block->CountBlocks(u"GridY"_wref.get()) > 0) {
            Properties = Block->GetBlock(u"GridY"_wref.get());
            Count = Properties->GetParamCount();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                Text = EC_Str::TrimWideString(Properties->GetParamValue(I));
                RowIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(Properties->GetParamName(I)));
                if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) < 2) {
                    Self->Rows[RowIndex].AutoHeightMinimum = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
                    Self->SetRowHeight(RowIndex, SysUtils::StrToInt(static_cast<pas::AnsiString>(Text)));
                    Self->Rows[RowIndex].AutoHeight = false;
                } else {
                    if (EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()) == u"Auto") {
                        Self->SetRowAutoHeightEnabled(RowIndex, true);
                    } else {
                        Self->SetRowAutoHeightEnabled(RowIndex, false);
                    }
                    Self->SetRowHeight(RowIndex, SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()))));
                }
            }
        }
        if (Block->CountBlocks(u"GridCells"_wref.get()) > 0) {
            Properties = Block->GetBlock(u"GridCells"_wref.get());
            Count = Properties->GetParamCount();
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
                Text = Properties->GetParamName(I);
                RowIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                Self->GetCell(SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()))), RowIndex)->LoadTextLinesFromBlockParam(Properties, Text);
                if (Self->Rows[RowIndex].AutoHeight) {
                    Self->UpdateRowAutoHeight(RowIndex);
                }
            }
            Count = Properties->GetBlockCount();
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
                Text = Properties->GetBlockNameByIndex(I);
                CellX = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
                CellY = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                LabelControl = Self->GetCell(CellX, CellY);
                CellProperties = Properties->GetBlockByIndex(I);
                if (CellProperties->CountParams(u"WordWrap"_wref.get()) > 0) {
                    LabelControl->SetWordWrapEnabled(GI_Main::ParseEnabledNameGI(EC_Str::TrimWideString(CellProperties->GetParam(u"WordWrap"_wref.get()))));
                }
                if (CellProperties->CountParams(u"AlignY"_wref.get()) > 0) {
                    LabelControl->SetTextAlignY(GI_Main::ParseTextAlignYName(EC_Str::TrimWideString(CellProperties->GetParam(u"AlignY"_wref.get()))));
                }
                if (CellProperties->CountParams(u"AlignX"_wref.get()) > 0) {
                    LabelControl->SetTextAlignX(GI_Main::ParseTextAlignXName(EC_Str::TrimWideString(CellProperties->GetParam(u"AlignX"_wref.get()))));
                }
                if (CellProperties->CountParams(u"TextColor"_wref.get()) > 0) {
                    LabelControl->SetTextColor(GI_Main::GetColorGI(Block->GetParam(u"TextColor"_wref.get())));
                }
                if (CellProperties->CountParams(u"Image"_wref.get()) > 0) {
                    LabelControl->SetEmbeddedImagePath(CellProperties->GetParam(u"Image"_wref.get()));
                }
                if (CellProperties->CountParams(u"ImageKindX"_wref.get()) > 0) {
                    LabelControl->SetEmbeddedImageKindX(GI_Main::ParseImageKindXName(CellProperties->GetParam(u"ImageKindX"_wref.get())));
                }
                if (CellProperties->CountParams(u"ImageKindY"_wref.get()) > 0) {
                    LabelControl->SetEmbeddedImageKindY(GI_Main::ParseImageKindYName(CellProperties->GetParam(u"ImageKindY"_wref.get())));
                }
                if (CellProperties->CountParams(u"ImageHalfAlpha"_wref.get()) > 0) {
                    LabelControl->SetEmbeddedImageHalfAlpha(GI_Main::ParseEnabledNameGI(CellProperties->GetParam(u"ImageHalfAlpha"_wref.get())));
                }
                Self->UpdateRowAutoHeight(CellY);
            }
        }
        if (Block->CountParams(u"BackgroundImage"_wref.get()) > 0) {
            Self->SetBackgroundImagePath(Block->GetParam(u"BackgroundImage"_wref.get()));
        }
        if (Block->CountParams(u"RowSelect"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"RowSelect"_wref.get())) == u"True") {
                Self->SetRowSelectEnabled(true);
            } else {
                Self->SetRowSelectEnabled(false);
            }
        }
        if (Block->CountParams(u"ColSelect"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"ColSelect"_wref.get())) == u"True") {
                Self->SetColSelectEnabled(true);
            } else {
                Self->SetColSelectEnabled(false);
            }
        }
        if (Block->CountParams(u"ActiveCellImage"_wref.get()) > 0) {
            GI_Grid::TGridGI_SetActiveCellImagePath(Self, Block->GetParam(u"ActiveCellImage"_wref.get()));
        }
        if (Block->CountParams(u"ActiveCell"_wref.get()) > 0) {
            Text = Block->GetParam(u"ActiveCell"_wref.get());
            Self->SetActiveCell(([&] {
                std::int32_t strToInt_3 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                std::int32_t strToInt_4 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
                return ClassesImports::Point(strToInt_4, strToInt_3);
            }()));
        }
        if (Block->CountParams(u"ActiveCellImageHalfAlpha"_wref.get()) > 0) {
            Self->SetActiveCellImageHalfAlpha(GI_Main::ParseEnabledNameGI(Block->GetParam(u"ActiveCellImageHalfAlpha"_wref.get())));
        }
    }

    void TGridGI::Draw(WindowsSdk::TRect ClipRect) {
        GI_MessageLoop::TObjectGI::Draw(ClipRect);
    }

    void TGridGI::p_destroy() {
        GI_Grid::TGridGI_Destroy(this);
    }

    void TGridGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Grid::TGridGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Grid
