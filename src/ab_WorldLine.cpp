#include "layout/ab_WorldLine.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/ab_MainForm.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/Globals.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_WorldLine.hpp"

// Grouped by native diagnostic prefix; original source-unit boundaries remain unresolved.
namespace ab_WorldLine {
    std::uint32_t WorldLineHeap = 0u;

    ab_WorldLine::PabWorldLine FirstWorldLine = nullptr;

    ab_WorldLine::PabWorldLine LastWorldLine = nullptr;

    void ab_WorldLine_Clear() {
        while (!(FirstWorldLine == nullptr)) {
            ab_WorldLine::ab_WorldLine_Delete(LastWorldLine);
        }
        if (WorldLineHeap != 0) {
            WindowsSdk::HeapDestroy(WorldLineHeap);
            WorldLineHeap = 0u;
        }
    }

    // Allocates and links a node owned by the world list.
    PabWorldLine ab_WorldLine_Add() {
        if (WorldLineHeap == 0) {
            WorldLineHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (WorldLineHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_WorldLine_Add.HeapCreate"_a));
            }
        }
        PabWorldLine Line = static_cast<PabWorldLine>(EC_Mem::AllocClearFromHeapEC(WorldLineHeap, static_cast<std::int32_t>(sizeof(TabWorldLine))));
        if (LastWorldLine != nullptr) {
            LastWorldLine->Next = Line;
        }
        Line->Prev = LastWorldLine;
        Line->Next = nullptr;
        LastWorldLine = Line;
        if (FirstWorldLine == nullptr) {
            FirstWorldLine = Line;
        }
        return Line;
    }

    void ab_WorldLine_Delete(PabWorldLine Line) {
        if (Line->Prev != nullptr) {
            Line->Prev->Next = Line->Next;
        }
        if (Line->Next != nullptr) {
            Line->Next->Prev = Line->Prev;
        }
        if (LastWorldLine == Line) {
            LastWorldLine = Line->Prev;
        }
        if (FirstWorldLine == Line) {
            FirstWorldLine = Line->Next;
        }
        if (Line->Segment != nullptr) {
            Globals::ArcadeBattleScreen->WorldLines->RetireSegment(Line->Segment);
            Line->Segment = nullptr;
        }
        if (WorldLineHeap != 0) {
            EC_Mem::FreeFromHeapEC(WorldLineHeap, Line);
        }
    }

    PabWorldLine ab_WorldLine_Create(EC_Struct::TVector3D First, EC_Struct::TVector3D Last, std::int32_t Kind, std::uint32_t FrontColor, std::uint32_t BackColor, std::uint8_t ShowBehindSphere) {
        PabWorldLine Line = ab_WorldLine::ab_WorldLine_Add();
        pas::store_unaligned<EC_Struct::TVector3D>(&Line->First, First);
        pas::store_unaligned<EC_Struct::TVector3D>(&Line->Last, Last);
        Line->FrontColor = FrontColor;
        Line->BackColor = BackColor;
        Line->Kind = Kind;
        Line->ShowBehindSphere = ShowBehindSphere;
        return Line;
    }

    void ab_WorldLine_Set(PabWorldLine Line, EC_Struct::TVector3D First, EC_Struct::TVector3D Last, std::int32_t Kind, std::uint32_t FrontColor, std::uint32_t BackColor, std::uint8_t ShowBehindSphere) {
        pas::store_unaligned<EC_Struct::TVector3D>(&Line->First, First);
        pas::store_unaligned<EC_Struct::TVector3D>(&Line->Last, Last);
        Line->FrontColor = FrontColor;
        Line->BackColor = BackColor;
        if (Line->Kind != Kind) {
            Line->Kind = Kind;
            if (Line->Segment != nullptr) {
                Globals::ArcadeBattleScreen->WorldLines->RetireSegment(Line->Segment);
                Line->Segment = nullptr;
            }
        }
        Line->ShowBehindSphere = ShowBehindSphere;
    }

    void ab_WorldLine_Update() {
        PabWorldLine Line{};
        std::uint8_t Behind{};
        EC_Struct::TVector3D First{};
        EC_Struct::TVector3D Last{};
        EC_Struct::TVector3D Center{};
        Center = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
        Center = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Center));
        Line = FirstWorldLine;
        while (Line != nullptr) {
            if (Line->Kind == 0) {
                Line = Line->Next;
                continue;
            }
            First = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->First));
            Last = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->Last));
            Behind = static_cast<std::uint8_t>(ab_Global::IsDepthBeforeSphereHorizon(First.Z) ^ 1) || static_cast<std::uint8_t>(ab_Global::IsDepthBeforeSphereHorizon(Last.Z) ^ 1);
            if (Behind && static_cast<std::uint8_t>(Line->ShowBehindSphere ^ 1)) {
                if (Line->Segment != nullptr) {
                    Globals::ArcadeBattleScreen->WorldLines->RetireSegment(Line->Segment);
                    Line->Segment = nullptr;
                }
            } else {
                if (Line->Segment == nullptr) {
                    {
                        std::int32_t round_3 = System::Round(Last.Y);
                        std::int32_t round_4 = System::Round(Last.X);
                        WindowsSdk::TPoint point = ClassesImports::Point(round_4, round_3);
                        std::int32_t round = System::Round(First.Y);
                        std::int32_t round_2 = System::Round(First.X);
                        WindowsSdk::TPoint point_2 = ClassesImports::Point(round_2, round);
                        Line->Segment = Globals::ArcadeBattleScreen->WorldLines->AddLine(point_2, point, Line->FrontColor);
                    }
                    Line->Segment->Animated = Line->Kind == 2;
                    if (Line->Kind < 3) {
                        Line->Segment->Kind = 0;
                    } else if (Line->Kind == 3) {
                        Line->Segment->Kind = 1;
                    } else {
                        Line->Segment->Kind = 2;
                    }
                } else {
                    Line->Segment->First.X = System::Round(First.X);
                    Line->Segment->First.Y = System::Round(First.Y);
                    Line->Segment->Last.X = System::Round(Last.X);
                    Line->Segment->Last.Y = System::Round(Last.Y);
                    Globals::ArcadeBattleScreen->WorldLines->UpdateSegmentLength(Line->Segment);
                }
                if (Behind) {
                    Line->Segment->Color = Line->BackColor;
                    Line->Segment->EndColor = Line->BackEndColor;
                } else {
                    Line->Segment->Color = Line->FrontColor;
                    Line->Segment->EndColor = Line->FrontEndColor;
                }
            }
            Line = Line->Next;
        }
    }

} // namespace ab_WorldLine
