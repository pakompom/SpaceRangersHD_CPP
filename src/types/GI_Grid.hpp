#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Frame {
    struct TFrameGI;

} // namespace GI_Frame

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Grid {
    struct TGridRowGI;

    struct TGridGI;

    #pragma pack(push, 1)
    struct TGridRowGI {
        std::int32_t Height;
        std::int32_t AutoHeightMinimum;
        std::uint8_t AutoHeight;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    enum TGridTypeGI : std::uint8_t {
        gtHide = 0,
        gtCell = 1,
        gtRow = 2,
        gtCol = 3,
    };

    using TGridCanSelectCellEventGI = pas::Method<std::uint8_t(GI_MessageLoop::TObjectGI*, WindowsSdk::TPoint)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGridGI : GI_PanelScrollBar::TPanelScrollBarGI {
        PAS_CLASS_META(TGridGI, GI_PanelScrollBar::TPanelScrollBarGI, "TGridGI", 448)
        void p_destroy() override;
        void Clear() override;
        void LayoutCell(GI_MessageLoop::TObjectGI* Child);
        void LayoutCells();
        void UpdateGridExtent();
        // Type 0 hides lines, 1 draws both axes, 2 horizontal separators, 3 vertical separators; nonzero types include the outer border.
        void RebuildGridLines();
        void UpdateRowAutoHeight(std::int32_t RowIndex);
        void UpdateActiveCellVisibility();
        // New columns default to 100 pixels.
        void SetColumnCount(std::int32_t Value);
        // New rows default to 15 pixels.
        void SetRowCount(std::int32_t Value);
        void SetGridType(TGridTypeGI Value);
        std::int32_t GetColumnWidth(std::int32_t ColumnIndex);
        void SetColumnWidth(std::int32_t ColumnIndex, std::int32_t Width);
        std::int32_t GetRowHeight(std::int32_t RowIndex);
        // Also updates AutoHeightMinimum when the row's AutoHeight flag is set.
        void SetRowHeight(std::int32_t RowIndex, std::int32_t Height);
        // Does not validate RowIndex.
        void SetRowAutoHeightEnabled(std::int32_t RowIndex, std::uint8_t Enabled);
        // Raises for out-of-range coordinates or a missing cell label.
        GI_Label::TLabelGI* GetCell(std::int32_t CellX, std::int32_t CellY);
        void SetRowSelectEnabled(std::uint8_t Value);
        void SetColSelectEnabled(std::uint8_t Value);
        void SetBackgroundImagePath(pas::WideString Path);
        void SetActiveCellImagePath(pas::WideString Path);
        void SetActiveCellImageHalfAlpha(std::uint8_t Value);
        // Invalid coordinates become (-1,-1); valid cells are scrolled into view.
        void SetActiveCell(WindowsSdk::TPoint Cell);
        // Selection can be vetoed by the callback.
        void SelectCell(WindowsSdk::TPoint Cell);
        void CellClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t MouseState, WindowsSdk::TPoint Point);
        void ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) override;
        void OnFocusGained() override;
        void OnFocusLost() override;
        void ProcessKeyDown(std::int32_t Key) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadGridProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(WindowsSdk::TRect ClipRect) override;
        std::uint8_t cpp_padding[4];
        std::int32_t ColumnCount;
        std::int32_t RowCount;
        std::int32_t* ColumnWidths;
        pas::DynArray<TGridRowGI> Rows;
        pas::WideString FontName;
        std::uint32_t TextColor;
        TGridTypeGI GridType;
        std::uint8_t cpp_padding_2[3];
        std::uint32_t GridColor;
        GI_Image::TImageGI* BackgroundImage;
        GI_Image::TImageGI* ActiveCellImage;
        GI_Frame::TFrameGI* ActiveCellFrame;
        WindowsSdk::TPoint ActiveCell;
        std::uint8_t RowSelect;
        std::uint8_t ColSelect;
        std::uint8_t cpp_padding_3[2];
        GI_MessageLoop::TObjectNotifyEventGI SelectionChangedCallback;
        TGridCanSelectCellEventGI CanSelectCellCallback;
        GI_MessageLoop::TObjectNotifyEventGI RepeatedCellClickCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Cell UserValue packs the column below the row; decorations use -1.
    inline constexpr std::int32_t GridCellCoordinateMask = 0x0000ffff;

    inline constexpr std::int32_t GridCellRowShift = 16;

    inline constexpr std::int32_t GridDecorationTag = -1;

} // namespace GI_Grid
