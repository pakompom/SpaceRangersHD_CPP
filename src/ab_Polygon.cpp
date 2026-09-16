#include "layout/ab_Polygon.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/ab_MainForm.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Polygon.hpp"
#include "units/ab_StopLine.hpp"

// Native anonymous RTTI and initialization-table evidence identify this unit.
// Ordinary routines occupy; is compiler finalization.
namespace ab_Polygon {
    ab_Polygon::PabPolygon FirstPolygon = nullptr;

    ab_Polygon::PabPolygon LastPolygon = nullptr;

    ab_Polygon::PabPolygon PolygonStorage{};

    pas::DynArray<ab_Polygon::TabOptGroup> PolygonGroups{};

    pas::DynArray<ab_Polygon::TabOptUnit> PolygonCells{};

    std::int32_t LongitudeCellCount{};

    std::int32_t PolarCellCount{};

    ab_Polygon::PabPolygonCell CurrentPolygonCell{};

    void ab_Polygon_Clear() {
        ab_Polygon::ab_Polygon_ClearVisibility();
        if (PolygonStorage != nullptr) {
            EC_Mem::FreeEC(PolygonStorage);
            PolygonStorage = nullptr;
        }
        FirstPolygon = nullptr;
        LastPolygon = nullptr;
    }

    std::int32_t ab_Polygon_Count() {
        std::int32_t Result = 0;
        PabPolygon Polygon = FirstPolygon;
        while (Polygon != nullptr) {
            ++Result;
            Polygon = Polygon->Next;
        }
        return Result;
    }

    void ab_Polygon_ClearVisibility() {
        std::int32_t Index{};
        {
            const std::int32_t cpp_last = PolygonCells.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    PolygonCells[Index].Points = nullptr;
                    PolygonCells[Index].Groups = nullptr;
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = PolygonGroups.length() - 1;
            if (0 <= cpp_last_2) {
                for (Index = 0; Index <= cpp_last_2; ++Index) {
                    PolygonGroups[Index].Polygons = nullptr;
                }
            }
        }
        PolygonGroups = nullptr;
        PolygonCells = nullptr;
        CurrentPolygonCell = nullptr;
    }

    void ab_Polygon_LoadVisibility(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        std::int32_t ItemIndex{};
        pas::DynArray<PabPolygon> Polygons{};
        void* Cursor{};
        std::int32_t PointIndex{};
        PabPolygonGroup Group{};
        ab_Polygon::ab_Polygon_ClearVisibility();
        std::int32_t Count = ab_Polygon::ab_Polygon_Count();
        Polygons.set_length(Count);
        Index = 0;
        PabPolygon Polygon = FirstPolygon;
        while (Polygon != nullptr) {
            Polygons[Index] = Polygon;
            ++Index;
            Polygon = Polygon->Next;
        }
        LongitudeCellCount = EC_Buf::TBufEC_GetInt32(Buffer);
        PolarCellCount = EC_Buf::TBufEC_GetInt32(Buffer);
        PolygonCells.set_length(LongitudeCellCount * PolarCellCount);
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        PolygonGroups.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PolygonGroups.length() - 1); cpp_range.next(Index); ) {
            Group = &PolygonGroups[Index];
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            Group->Polygons.set_length(Count);
            Cursor = static_cast<std::uint8_t*>(Buffer->Data) + Buffer->Position;
            {
                const std::int32_t cpp_last = Group->Polygons.length() - 1;
                if (0 <= cpp_last) {
                    for (ItemIndex = 0; ItemIndex <= cpp_last; ++ItemIndex) {
                        Group->Polygons[ItemIndex] = Polygons[pas::load_unaligned<std::uint16_t>(static_cast<System::PWord>(Cursor))];
                        Cursor = static_cast<std::uint8_t*>(Cursor) + 2;
                    }
                }
            }
            Buffer->SetPosition(Buffer->Position + Count * 2);
            Cursor = static_cast<std::uint8_t*>(Buffer->Data) + Buffer->Position;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 0 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, X)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 0 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Y)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 0 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Z)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 1 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, X)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 1 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Y)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 1 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Z)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 2 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, X)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 2 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Y)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 2 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Z)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 3 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, X)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 3 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Y)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Cursor = static_cast<std::uint8_t*>(Cursor) + 4;
            pas::store_unaligned<double>(pas::byte_offset(pas::byte_offset(&Group->Corners, 3 * sizeof(EC_Struct::TVector3D)), offsetof(EC_Struct::TVector3D, Z)), static_cast<double>(pas::load_unaligned<float>(static_cast<System::PSingle>(Cursor))));
            Buffer->SetPosition(Buffer->Position + 48);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, LongitudeCellCount * PolarCellCount - 1); cpp_range_2.next(Index); ) {
            CurrentPolygonCell = &PolygonCells[Index];
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            CurrentPolygonCell->Points.set_length(Count);
            Cursor = static_cast<std::uint8_t*>(Buffer->Data) + Buffer->Position;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(ItemIndex); ) {
                PointIndex = pas::load_unaligned<std::uint16_t>(static_cast<System::PWord>(Cursor));
                CurrentPolygonCell->Points[ItemIndex] = ab_StopLine::StopPointIndex[PointIndex];
                Cursor = static_cast<std::uint8_t*>(Cursor) + 2;
            }
            Buffer->SetPosition(Buffer->Position + Count * 2);
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            CurrentPolygonCell->Groups.set_length(Count);
            Cursor = static_cast<std::uint8_t*>(Buffer->Data) + Buffer->Position;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(ItemIndex); ) {
                CurrentPolygonCell->Groups[ItemIndex] = &PolygonGroups[pas::load_unaligned<std::uint16_t>(static_cast<System::PWord>(Cursor))];
                Cursor = static_cast<std::uint8_t*>(Cursor) + 2;
            }
            Buffer->SetPosition(Buffer->Position + Count * 2);
        }
        Polygons = nullptr;
        CurrentPolygonCell = nullptr;
    }

    void ab_Polygon_SelectVisibilityCell() {
        std::int32_t LongitudeIndex{};
        std::int32_t PolarIndex{};
        LongitudeIndex = System::Round(pas::real_divide(ab_Global::SphereViewState.LongitudeDegrees, 3.6E+2L) * LongitudeCellCount);
        if (LongitudeIndex >= LongitudeCellCount) {
            LongitudeIndex = 0;
        }
        PolarIndex = System::Round(pas::real_divide(ab_Global::SphereViewState.PolarAngleDegrees, 1.8E+2L) * (PolarCellCount - 1));
        if (PolarIndex >= PolarCellCount) {
            GR_Main::RaiseWideMessage(u"ab_OptCur"_wref.get());
        }
        CurrentPolygonCell = &PolygonCells[LongitudeIndex * PolarCellCount + PolarIndex];
    }

    void ab_Polygon_ProjectVisiblePoints() {
        std::int32_t Index{};
        ab_StopLine::PabStopPoint Point{};
        EC_Struct::TVector3D Projected{};
        if (CurrentPolygonCell == nullptr) {
            return;
        }
        if (CurrentPolygonCell->Points == nullptr) {
            return;
        }
        std::int32_t Count = CurrentPolygonCell->Points.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Point = CurrentPolygonCell->Points[Index];
            Projected = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Point->Position));
            Point->Projected = true;
            Point->ScreenX = Globals::ArcadeBattleScreen->WorldCenterX + System::Round(Projected.X);
            Point->ScreenY = Globals::ArcadeBattleScreen->WorldCenterY + System::Round(Projected.Y);
        }
    }

    void ab_Polygon_QueueUpdateRects() {
        std::int32_t Index{};
        PabPolygonGroup Group{};
        double MinX{};
        double MaxX{};
        double MinY{};
        double MaxY{};
        EC_Struct::TVector3D Projected{};
        if (CurrentPolygonCell == nullptr) {
            return;
        }
        if (CurrentPolygonCell->Groups == nullptr) {
            return;
        }
        std::int32_t CenterX = Globals::ArcadeBattleScreen->WorldCenterX;
        std::int32_t CenterY = Globals::ArcadeBattleScreen->WorldCenterY;
        std::int32_t Count = CurrentPolygonCell->Groups.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Group = CurrentPolygonCell->Groups[Index];
            Projected = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(pas::byte_offset(&Group->Corners, 0 * sizeof(EC_Struct::TVector3D))));
            MinX = Projected.X;
            MaxX = Projected.X;
            MinY = Projected.Y;
            MaxY = Projected.Y;
            Projected = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(pas::byte_offset(&Group->Corners, 1 * sizeof(EC_Struct::TVector3D))));
            if (Projected.X < MinX) {
                MinX = Projected.X;
            } else if (Projected.X > MaxX) {
                MaxX = Projected.X;
            }
            if (Projected.Y < MinY) {
                MinY = Projected.Y;
            } else if (Projected.Y > MaxY) {
                MaxY = Projected.Y;
            }
            Projected = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(pas::byte_offset(&Group->Corners, 2 * sizeof(EC_Struct::TVector3D))));
            if (Projected.X < MinX) {
                MinX = Projected.X;
            } else if (Projected.X > MaxX) {
                MaxX = Projected.X;
            }
            if (Projected.Y < MinY) {
                MinY = Projected.Y;
            } else if (Projected.Y > MaxY) {
                MaxY = Projected.Y;
            }
            Projected = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(pas::byte_offset(&Group->Corners, 3 * sizeof(EC_Struct::TVector3D))));
            if (Projected.X < MinX) {
                MinX = Projected.X;
            } else if (Projected.X > MaxX) {
                MaxX = Projected.X;
            }
            if (Projected.Y < MinY) {
                MinY = Projected.Y;
            } else if (Projected.Y > MaxY) {
                MaxY = Projected.Y;
            }
            {
                std::int32_t cpp_arg = CenterY + System::Round(MaxY) + 1;
                std::int32_t cpp_arg_2 = CenterX + System::Round(MaxX) + 1;
                std::int32_t cpp_arg_3 = CenterY + System::Round(MinY);
                std::int32_t cpp_arg_4 = CenterX + System::Round(MinX);
                Globals::ArcadeBattleScreen->QueueUpdateRect(ClassesImports::Rect(cpp_arg_4, cpp_arg_3, cpp_arg_2, cpp_arg));
            }
        }
    }

    void ab_Polygon_Draw() {
        GR_Main::TTriangleRasterizer16 Rasterizer{};
        std::int32_t GroupIndex{};
        std::int32_t PolygonIndex{};
        std::int32_t PolygonCount{};
        PabPolygonGroup Group{};
        PabPolygon Polygon{};
        Rasterizer = GR_Main::TriangleRasterizer16;
        if (CurrentPolygonCell == nullptr) {
            return;
        }
        if (CurrentPolygonCell->Groups == nullptr) {
            return;
        }
        std::int32_t GroupCount = CurrentPolygonCell->Groups.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, GroupCount - 1); cpp_range.next(GroupIndex); ) {
            Group = CurrentPolygonCell->Groups[GroupIndex];
            PolygonCount = Group->Polygons.length() - 1 + 1;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PolygonCount - 1); cpp_range_2.next(PolygonIndex); ) {
                Polygon = Group->Polygons[PolygonIndex];
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawColoredTriangle(pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX, pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY, pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color)))), pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX, pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY, pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color)))), pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX, pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY, pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color)))), true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                } else {
                    std::uint32_t cpp_arg = pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color))));
                    std::int32_t screenY = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY;
                    std::int32_t screenX = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 2 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX;
                    std::uint32_t cpp_arg_2 = pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color))));
                    std::int32_t screenY_2 = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY;
                    std::int32_t screenX_2 = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 1 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX;
                    std::uint32_t cpp_arg_3 = pas::load_unaligned<std::uint32_t>(pas::load_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color))));
                    std::int32_t screenY_3 = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenY;
                    std::int32_t screenX_3 = pas::load_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, 0 * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)))->ScreenX;
                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    Rasterizer(pixels, pitchBytes, screenX_3, screenY_3, cpp_arg_3, screenX_2, screenY_2, cpp_arg_2, screenX, screenY, cpp_arg, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                }
            }
        }
    }

    void ab_Polygon_Load(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        std::int32_t Vertex{};
        ab_Polygon::ab_Polygon_Clear();
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Count < 1) {
            return;
        }
        PolygonStorage = static_cast<PabPolygon>(EC_Mem::AllocClearEC(Count * static_cast<std::int32_t>(sizeof(TabPolygon))));
        PabPolygon Polygon = PolygonStorage;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (LastPolygon != nullptr) {
                LastPolygon->Next = Polygon;
            }
            Polygon->Prev = LastPolygon;
            Polygon->Next = nullptr;
            LastPolygon = Polygon;
            if (FirstPolygon == nullptr) {
                FirstPolygon = Polygon;
            }
            Polygon->MapValue30 = EC_Buf::TBufEC_GetInt32(Buffer);
            for (Vertex = 0; Vertex <= 2; ++Vertex) {
                pas::store_unaligned<ab_StopLine::PabStopPoint>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, Vertex * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Point)), ab_StopLine::StopPointIndex[EC_Buf::TBufEC_GetInt32(Buffer)]);
                pas::store_unaligned<System::PCardinal>(pas::byte_offset(pas::byte_offset(&Polygon->Vertices, Vertex * sizeof(TabPolygonVertex)), offsetof(TabPolygonVertex, Color)), static_cast<System::PCardinal>(static_cast<void*>(([&] {
                    std::int32_t cpp_right = EC_Buf::TBufEC_GetInt32(Buffer);
                    return static_cast<std::uint8_t*>(ab_Global::ArcadeMapColorBuffer->Data) + cpp_right;
                }()))));
            }
            Polygon = static_cast<PabPolygon>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Polygon) + static_cast<std::int32_t>(sizeof(TabPolygon))));
        }
    }

} // namespace ab_Polygon
