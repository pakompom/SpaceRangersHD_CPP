#include "layout/ab_Space.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Hit.hpp"
#include "units/ab_Item.hpp"
#include "units/ab_MainForm.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_ShipAI.hpp"
#include "units/ab_Space.hpp"
#include "units/ab_W.hpp"

// Native TabSpace VMT; field recovery is incomplete.
namespace ab_Space {
    float FindApproachDanger(TabSpace* Space, float Accumulated);

    ab_Space::TabSpace* FirstArcadeSpace = nullptr;

    ab_Space::TabSpace* LastArcadeSpace = nullptr;

    ab_Space::TabSpace* CurrentArcadeSpace = nullptr;

    // Destination selected before entering a space.
    ab_Space::TabSpace* NextArcadeSpace = nullptr;

    ab_Space::TabSpace* StartArcadeSpace = nullptr;

    ab_Space::TabSpace* EndArcadeSpace = nullptr;

    ab_Space::TabSpace* HoveredArcadeSpace = nullptr;

    ab_Space::PabSpaceLink FirstArcadeSpaceLink = nullptr;

    ab_Space::PabSpaceLink LastArcadeSpaceLink = nullptr;

    std::uint8_t ArcadeKellerEncounter{};

    void ab_Space_UpdateApproachDanger() {
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            Space->UpdateApproachDanger();
            Space = Space->Next;
        }
        Space = FirstArcadeSpace;
        while (Space != nullptr) {
            TabSpace::PruneApproachDanger();
            Space = Space->Next;
        }
    }

    void ab_Space_CreateImages() {
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            Space->CreateImage();
            Space = Space->Next;
        }
    }

    void ab_Space_ClearImages() {
        ab_Space::ab_SpaceLink_ClearImages();
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            Space->ClearImage();
            Space = Space->Next;
        }
    }

    void ab_SpaceLink_Invalidate() {
        std::int32_t Index{};
        Types::TRect Bounds{};
        std::int32_t OffsetX = Globals::ArcadeBattleScreen->WorldCenterX - ab_Global::ArcadeMapViewPosition.X;
        std::int32_t OffsetY = Globals::ArcadeBattleScreen->WorldCenterY - ab_Global::ArcadeMapViewPosition.Y;
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            Bounds.Left = 1000000000;
            Bounds.Right = -1000000000;
            Bounds.Top = 1000000000;
            Bounds.Bottom = -1000000000;
            for (Index = 0; Index <= 10; ++Index) {
                Bounds.Left = std::min<std::int32_t>(Bounds.Left, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, Index * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))));
                Bounds.Top = std::min<std::int32_t>(Bounds.Top, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, Index * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))));
                Bounds.Right = std::max<std::int32_t>(Bounds.Right, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, Index * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))));
                Bounds.Bottom = std::max<std::int32_t>(Bounds.Bottom, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, Index * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))));
            }
            Bounds.Left += OffsetX;
            Bounds.Top += OffsetY;
            Bounds.Right += OffsetX;
            Bounds.Bottom += OffsetY;
            Globals::ArcadeBattleScreen->QueueUpdateRect(Bounds);
            Link = Link->Next;
        }
    }

    void ab_SpaceLink_BuildGeometry() {
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Last{};
        float DirectionY{};
        float DirectionX{};
        float WidthY{};
        float WidthX{};
        float InverseLength{};
        float ArrowLength{};
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            First = EC_Struct::PointToPointF(Link->First->MapPosition);
            Last = EC_Struct::PointToPointF(Link->Last->MapPosition);
            DirectionX = static_cast<long double>(Last.X) - First.X;
            DirectionY = static_cast<long double>(Last.Y) - First.Y;
            InverseLength = pas::real_divide(1.0L, System::Sqrt(pas::sqr(static_cast<pas::Extended>(DirectionX)) + pas::sqr(static_cast<pas::Extended>(DirectionY))));
            DirectionX = static_cast<long double>(DirectionX) * InverseLength;
            DirectionY = static_cast<long double>(DirectionY) * InverseLength;
            ArrowLength = GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * 0.5L;
            First.X = static_cast<long double>(GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius)) * DirectionX * 1.1L + First.X;
            First.Y = static_cast<long double>(GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius)) * DirectionY * 1.1L + First.Y;
            Last.X = Last.X - (GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * 1.1L + ArrowLength) * DirectionX;
            Last.Y = Last.Y - (GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * 1.1L + ArrowLength) * DirectionY;
            WidthX = static_cast<long double>(GR_Main::GiScalePixels(7)) * DirectionX;
            WidthY = static_cast<long double>(GR_Main::GiScalePixels(7)) * DirectionY;
            {
                std::int32_t round = System::Round(static_cast<long double>(First.Y) + WidthX);
                std::int32_t round_2 = System::Round(static_cast<long double>(First.X) - WidthY);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), ClassesImports::Point(round_2, round));
            }
            {
                std::int32_t round_3 = System::Round(static_cast<long double>(First.Y) - WidthX);
                std::int32_t round_4 = System::Round(static_cast<long double>(First.X) + WidthY);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 1 * sizeof(Types::TPoint)), ClassesImports::Point(round_4, round_3));
            }
            {
                std::int32_t round_5 = System::Round(static_cast<long double>(Last.Y) - WidthX - DirectionY * 2.0L);
                std::int32_t round_6 = System::Round(static_cast<long double>(Last.X) + WidthY - DirectionX * 2.0L);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), ClassesImports::Point(round_6, round_5));
            }
            {
                std::int32_t round_7 = System::Round(static_cast<long double>(Last.Y) + WidthX - DirectionY * 2.0L);
                std::int32_t round_8 = System::Round(static_cast<long double>(Last.X) - WidthY - DirectionX * 2.0L);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 3 * sizeof(Types::TPoint)), ClassesImports::Point(round_8, round_7));
            }
            {
                std::int32_t round_9 = System::Round(0.7L * WidthX + First.Y);
                std::int32_t round_10 = System::Round(First.X - 0.7L * WidthY);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 7 * sizeof(Types::TPoint)), ClassesImports::Point(round_10, round_9));
            }
            {
                std::int32_t round_11 = System::Round(First.Y - 0.7L * WidthX);
                std::int32_t round_12 = System::Round(0.7L * WidthY + First.X);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 8 * sizeof(Types::TPoint)), ClassesImports::Point(round_12, round_11));
            }
            {
                std::int32_t round_13 = System::Round(Last.Y - 0.7L * WidthX - DirectionY * 2.0L);
                std::int32_t round_14 = System::Round(0.7L * WidthY + Last.X - DirectionX * 2.0L);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 9 * sizeof(Types::TPoint)), ClassesImports::Point(round_14, round_13));
            }
            {
                std::int32_t round_15 = System::Round(0.7L * WidthX + Last.Y - DirectionY * 2.0L);
                std::int32_t round_16 = System::Round(Last.X - 0.7L * WidthY - DirectionX * 2.0L);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 10 * sizeof(Types::TPoint)), ClassesImports::Point(round_16, round_15));
            }
            WidthX = static_cast<long double>(DirectionX) * ArrowLength * 0.4L;
            WidthY = static_cast<long double>(DirectionY) * ArrowLength * 0.4L;
            {
                std::int32_t round_17 = System::Round(static_cast<long double>(Last.Y) + WidthX);
                std::int32_t round_18 = System::Round(static_cast<long double>(Last.X) - WidthY);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 4 * sizeof(Types::TPoint)), ClassesImports::Point(round_18, round_17));
            }
            {
                std::int32_t round_19 = System::Round(static_cast<long double>(DirectionY) * ArrowLength + Last.Y);
                std::int32_t round_20 = System::Round(static_cast<long double>(DirectionX) * ArrowLength + Last.X);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 5 * sizeof(Types::TPoint)), ClassesImports::Point(round_20, round_19));
            }
            {
                std::int32_t round_21 = System::Round(static_cast<long double>(Last.Y) - WidthX);
                std::int32_t round_22 = System::Round(static_cast<long double>(Last.X) + WidthY);
                pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Link->Points, 6 * sizeof(Types::TPoint)), ClassesImports::Point(round_22, round_21));
            }
            Link = Link->Next;
        }
    }

    void ab_SpaceLink_Draw() {
        GR_Main::TTriangleRasterizer16 Rasterizer{};
        std::int32_t ColorOffset{};
        Rasterizer = GR_Main::TriangleRasterizer16;
        std::int32_t OffsetX = Globals::ArcadeBattleScreen->WorldCenterX - ab_Global::ArcadeMapViewPosition.X;
        std::int32_t OffsetY = Globals::ArcadeBattleScreen->WorldCenterY - ab_Global::ArcadeMapViewPosition.Y;
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            ColorOffset = Link->Last->AppearanceIndex * 6;
            if (GlobalsV::HardwareRenderingEnabled) {
                GR_DX::DrawGradientLine(pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 7 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 7 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, 0x40ffffffu, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 10 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 10 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, 0xc0ffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                GR_DX::DrawGradientLine(pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 8 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 8 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, 0x40ffffffu, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 9 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 9 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, 0xc0ffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                GR_DX::DrawColoredTriangle(pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 3], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 1 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 1 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 3], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 2], true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                GR_DX::DrawColoredTriangle(pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 2], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 3 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 3 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 2], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 3], true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                GR_DX::DrawColoredTriangle(pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 4 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 4 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 1], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 5 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 5 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset], pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 6 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX, pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 6 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY, ab_Global::ArcadeMapPalette[ColorOffset + 1], true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
            } else {
                {
                    std::int32_t cpp_arg = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 7 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_2 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 10 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t cpp_arg_3 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 10 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    WindowsSdk::TRect gameScreenRect = GR_Main::GameScreenRect;
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t cpp_arg_4 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 7 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::DrawGradientLine16Clipped(pixels, pitchBytes, cpp_arg_4, cpp_arg, 0x40ffffffu, cpp_arg_2, cpp_arg_3, 0xc0ffffffu, gameScreenRect);
                }
                {
                    std::int32_t cpp_arg_5 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 8 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_6 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 9 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t cpp_arg_7 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 9 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    WindowsSdk::TRect gameScreenRect_2 = GR_Main::GameScreenRect;
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t cpp_arg_8 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 8 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::DrawGradientLine16Clipped(pixels_2, pitchBytes_2, cpp_arg_8, cpp_arg_5, 0x40ffffffu, cpp_arg_6, cpp_arg_7, 0xc0ffffffu, gameScreenRect_2);
                }
                {
                    std::uint32_t cpp_arg_9 = ab_Global::ArcadeMapPalette[ColorOffset + 2];
                    std::int32_t cpp_arg_10 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_11 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_12 = ab_Global::ArcadeMapPalette[ColorOffset + 3];
                    std::int32_t cpp_arg_13 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 1 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_14 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 1 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_15 = ab_Global::ArcadeMapPalette[ColorOffset + 3];
                    std::int32_t cpp_arg_16 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_17 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
                    Rasterizer(pixels_3, pitchBytes_3, cpp_arg_17, cpp_arg_16, cpp_arg_15, cpp_arg_14, cpp_arg_13, cpp_arg_12, cpp_arg_11, cpp_arg_10, cpp_arg_9, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                }
                {
                    std::uint32_t cpp_arg_18 = ab_Global::ArcadeMapPalette[ColorOffset + 3];
                    std::int32_t cpp_arg_19 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_20 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 0 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_21 = ab_Global::ArcadeMapPalette[ColorOffset + 2];
                    std::int32_t cpp_arg_22 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 3 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_23 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 3 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_24 = ab_Global::ArcadeMapPalette[ColorOffset + 2];
                    std::int32_t cpp_arg_25 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_26 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 2 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                    Rasterizer(pixels_4, pitchBytes_4, cpp_arg_26, cpp_arg_25, cpp_arg_24, cpp_arg_23, cpp_arg_22, cpp_arg_21, cpp_arg_20, cpp_arg_19, cpp_arg_18, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                }
                {
                    std::uint32_t cpp_arg_27 = ab_Global::ArcadeMapPalette[ColorOffset + 1];
                    std::int32_t cpp_arg_28 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 6 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_29 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 6 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_30 = ab_Global::ArcadeMapPalette[ColorOffset];
                    std::int32_t cpp_arg_31 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 5 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_32 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 5 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::uint32_t cpp_arg_33 = ab_Global::ArcadeMapPalette[ColorOffset + 1];
                    std::int32_t cpp_arg_34 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 4 * sizeof(Types::TPoint)), offsetof(Types::TPoint, Y))) + OffsetY;
                    std::int32_t cpp_arg_35 = pas::load_unaligned<std::int32_t>(pas::byte_offset(pas::byte_offset(&Link->Points, 4 * sizeof(Types::TPoint)), offsetof(Types::TPoint, X))) + OffsetX;
                    std::int32_t pitchBytes_5 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* pixels_5 = GR_Main::ScreenRenderBuffer->GetPixels();
                    Rasterizer(pixels_5, pitchBytes_5, cpp_arg_35, cpp_arg_34, cpp_arg_33, cpp_arg_32, cpp_arg_31, cpp_arg_30, cpp_arg_29, cpp_arg_28, cpp_arg_27, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                }
            }
            Link = Link->Next;
        }
    }

    void ab_SpaceLink_ClearImages() {
        // The native routine retains this traversal without any per-link action.
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            Link = Link->Next;
        }
    }

    void ab_Space_Clear() {
        ab_Space::ab_SpaceLink_Clear();
        while (!(FirstArcadeSpace == nullptr)) {
            ab_Space::ab_Space_Delete(LastArcadeSpace);
        }
        CurrentArcadeSpace = nullptr;
        NextArcadeSpace = nullptr;
        StartArcadeSpace = nullptr;
        EndArcadeSpace = nullptr;
    }

    TabSpace* ab_Space_Add() {
        TabSpace* Space = pas::construct_call<TabSpace>(TabSpace_Create);
        if (LastArcadeSpace != nullptr) {
            LastArcadeSpace->Next = Space;
        }
        Space->Prev = LastArcadeSpace;
        Space->Next = nullptr;
        LastArcadeSpace = Space;
        if (FirstArcadeSpace == nullptr) {
            FirstArcadeSpace = Space;
        }
        return Space;
    }

    void ab_Space_Delete(TabSpace* Space) {
        PabSpaceLink Removing{};
        if (Space->Prev != nullptr) {
            Space->Prev->Next = Space->Next;
        }
        if (Space->Next != nullptr) {
            Space->Next->Prev = Space->Prev;
        }
        if (LastArcadeSpace == Space) {
            LastArcadeSpace = Space->Prev;
        }
        if (FirstArcadeSpace == Space) {
            FirstArcadeSpace = Space->Next;
        }
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            Removing = Link;
            Link = Link->Next;
            if (Removing->First == Space || Removing->Last == Space) {
                ab_Space::ab_SpaceLink_Delete(Removing);
            }
        }
        pas::free(Space);
    }

    TabSpace* ab_Space_Find(Types::TPoint GridPosition) {
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            if (Space->GridPosition.X == GridPosition.X && Space->GridPosition.Y == GridPosition.Y) {
                return Space;
            }
            Space = Space->Next;
        }
        return nullptr;
    }

    void ab_Space_RecountLinks() {
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            Space->RecountLinks();
            Space = Space->Next;
        }
    }

    void ab_SpaceLink_Clear() {
        while (!(FirstArcadeSpaceLink == nullptr)) {
            ab_Space::ab_SpaceLink_Delete(LastArcadeSpaceLink);
        }
    }

    PabSpaceLink ab_SpaceLink_Add() {
        PabSpaceLink Link = static_cast<PabSpaceLink>(EC_Mem::AllocClearEC(static_cast<std::int32_t>(sizeof(TabSpaceLink))));
        if (LastArcadeSpaceLink != nullptr) {
            LastArcadeSpaceLink->Next = Link;
        }
        Link->Prev = LastArcadeSpaceLink;
        Link->Next = nullptr;
        LastArcadeSpaceLink = Link;
        if (FirstArcadeSpaceLink == nullptr) {
            FirstArcadeSpaceLink = Link;
        }
        return Link;
    }

    void ab_SpaceLink_Delete(PabSpaceLink Link) {
        if (Link->Prev != nullptr) {
            Link->Prev->Next = Link->Next;
        }
        if (Link->Next != nullptr) {
            Link->Next->Prev = Link->Prev;
        }
        if (LastArcadeSpaceLink == Link) {
            LastArcadeSpaceLink = Link->Prev;
        }
        if (FirstArcadeSpaceLink == Link) {
            FirstArcadeSpaceLink = Link->Next;
        }
        EC_Mem::FreeEC(Link);
    }

    void ab_SpaceLink_Connect(TabSpace* First, TabSpace* Last) {
        PabSpaceLink Link = ab_Space::ab_SpaceLink_Add();
        Link->First = First;
        Link->Last = Last;
    }

    PabSpaceLink ab_SpaceLink_Find(TabSpace* First, TabSpace* Last) {
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            if (Link->First == First && Link->Last == Last || Link->First == Last && Link->Last == First) {
                return Link;
            }
            Link = Link->Next;
        }
        return nullptr;
    }

    PabSpaceLink ab_SpaceLink_FindExit(TabSpace* First, std::int32_t ExitIndex) {
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            if (Link->First == First && Link->ExitIndex == ExitIndex) {
                return Link;
            }
            Link = Link->Next;
        }
        return nullptr;
    }

    void ab_Space_Update() {
        TabSpace* Space = FirstArcadeSpace;
        while (Space != nullptr) {
            Space->Update();
            Space = Space->Next;
        }
    }

    void TabSpace_Create(TabSpace* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Color28 = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 0);
        Self->Color2C = GR_Main::CurrentPixelFormat->PackRgbBytes(155, 155, 0);
        Self->Objects = pas::make_object<pas::List>();
        Self->ImageActive = false;
    }

    void TabSpace_Destroy(TabSpace* Self) {
        Self->ClearVisuals();
        Self->ClearObjects();
        pas::free(Self->Objects);
        Self->Objects = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TabSpace::ClearVisuals() {
        ClearImage();
    }

    // Empty native update hook.
    void TabSpace::UpdateVisuals() {
    }

    void TabSpace::CreateImage() {
        ImageActive = true;
        if (Image == nullptr && this != StartArcadeSpace && this != EndArcadeSpace) {
            Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Globals::ArcadeBattleScreen->WorldPanel);
            Image->SetDepth(3.0E+1);
            if (Danger <= 0.0L) {
                const pas::WideString& cpp_arg = pas::concat_wide({u"Bm.ABClot.", GR_Main::GiResourceSuffix(), u"0_", pas::wide_int_to_str(aMyFunction::RandomIntRange(0, 1))});
                GI_GAI::TgaiGI* image = Image;
                image->SetImagePath(cpp_arg);
            } else if (static_cast<long double>(Danger) + ApproachDanger < ab_Global::ArcadeHighDangerThreshold) {
                const pas::WideString& cpp_arg_2 = pas::concat_wide({u"Bm.ABClot.", GR_Main::GiResourceSuffix(), u"1_", pas::wide_int_to_str(aMyFunction::RandomIntRange(0, 3))});
                GI_GAI::TgaiGI* image_2 = Image;
                image_2->SetImagePath(cpp_arg_2);
            } else {
                const pas::WideString& cpp_arg_3 = pas::concat_wide({u"Bm.ABClot.", GR_Main::GiResourceSuffix(), u"2_", pas::wide_int_to_str(aMyFunction::RandomIntRange(0, 1))});
                GI_GAI::TgaiGI* image_3 = Image;
                image_3->SetImagePath(cpp_arg_3);
            }
            {
                WindowsSdk::TPoint contentSize = Image->GetContentSize();
                GI_GAI::TgaiGI* image_4 = Image;
                image_4->SetSize(contentSize);
            }
            Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
            Image->SequenceIndex = 0;
            Image->UpdateAutoGeometry();
            {
                std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, Image->SequenceFrameCount - 1);
                GI_GAI::TgaiGI* image_5 = Image;
                image_5->SetSequenceFrame(randomIntRange);
            }
            Image->SetActive(true);
            Image->RestartPlayback();
        }
    }

    void TabSpace::ClearImage() {
        if (Image != nullptr) {
            pas::free(Image);
            Image = nullptr;
        }
        ImageActive = false;
    }

    void TabSpace::Update() {
        TabSpace::UpdateVisuals();
    }

    void TabSpace::ClearObjects() {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range.next(Index); ) {
            pas::free(pas::list_at<pas::Object>(Objects, Index));
        }
        pas::list_clear(Objects);
    }

    void TabSpace::PopulateObjects() {
        ab_ShipAI::TabShipAI* Ship{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t Kind{};
        std::int32_t VisualIndex{};
        std::int32_t Hitpoints{};
        std::int32_t WeaponIndex{};
        aItem::TItem* Item{};
        ab_Item::TabItem* ArcadeItem{};
        float Scale{};
        ClearObjects();
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        {
            std::int32_t cpp_case = static_cast<std::int32_t>(System::Round(Danger));
            if (cpp_case == 0) {
                if (Globals::ArcadeBattleScreen->RandomRange(0, 100) > 90) {
                    Count = Globals::ArcadeBattleScreen->RandomRange(1, 2);
                } else {
                    Count = 0;
                }
            } else if (cpp_case >= 1 && cpp_case <= 20) {
                if (Globals::ArcadeBattleScreen->RandomRange(0, 100) > 80) {
                    Count = Globals::ArcadeBattleScreen->RandomRange(2, 3);
                } else {
                    Count = 0;
                }
            } else if (cpp_case >= 21 && cpp_case <= 30) {
                if (Globals::ArcadeBattleScreen->RandomRange(0, 100) > 50) {
                    Count = Globals::ArcadeBattleScreen->RandomRange(2, 3);
                } else {
                    Count = 0;
                }
            } else if (cpp_case >= 31 && cpp_case <= 60) {
                if (Globals::ArcadeBattleScreen->RandomRange(0, 100) > 10) {
                    Count = Globals::ArcadeBattleScreen->RandomRange(2, 4);
                } else {
                    Count = 0;
                }
            } else if (cpp_case >= 61 && cpp_case <= 90) {
                if (Globals::ArcadeBattleScreen->RandomRange(0, 100) > 5) {
                    Count = Globals::ArcadeBattleScreen->RandomRange(2, 5);
                } else {
                    Count = 0;
                }
            } else if (cpp_case >= 91 && cpp_case <= 100) {
                Count = Globals::ArcadeBattleScreen->RandomRange(4, 5);
            } else {
                Count = Globals::ArcadeBattleScreen->RandomRange(4, 5);
            }
        }
        if (Count > 0) {
            pas::Extended cpp_left = Globals::ArcadeBattleScreen->RandomRange(0, 100);
            if (cpp_left < aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 2.0E+1, 3.0E+2, 0.0, 9.0E+1)) {
                Count = 0;
            }
        }
        if (aPlayer::GetPlayer()->HyperspaceKillCount < pas::real_divide(4.0L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor)) {
            Count = std::min<std::int32_t>(Count, 2);
        } else if (aPlayer::GetPlayer()->HyperspaceKillCount < pas::real_divide(1.0E+1L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor)) {
            Count = std::min<std::int32_t>(Count, 3);
        }
        if (aPlayer::GetPlayer()->HyperspaceKillCount == 0) {
            Count = 1;
        }
        std::int32_t Minimum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 0.0, 6.0));
        std::int32_t Maximum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 5.0, 11.0));
        VisualIndex = Globals::ArcadeBattleScreen->RandomRange(1, 5);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            {
                std::int32_t cpp_case_2 = aPlayer::GetPlayer()->HyperspaceKillCount;
                if (cpp_case_2 == 0) {
                    Scale = 0.2f;
                } else if (cpp_case_2 >= 1 && cpp_case_2 <= 4) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 1.0, 4.0, 0.3, 0.5);
                } else if (cpp_case_2 >= 5 && cpp_case_2 <= 12) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 5.0, 12.0, 0.5, 0.8);
                } else if (cpp_case_2 >= 13 && cpp_case_2 <= 30) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 13.0, 3.0E+1, 0.8, 1.0);
                } else if (cpp_case_2 >= 31 && cpp_case_2 <= 80) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 31.0, 8.0E+1, 1.0, 1.3);
                } else {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->HyperspaceKillCount, 81.0, 1.0E+2, 1.3, 1.6);
                }
            }
            Scale = static_cast<long double>(aMyFunction::RemapClamped(Danger, 0.0, 1.0E+2, 0.8, 1.2)) * Scale;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->Wealth, pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0L), aGalaxy::Galaxy->MaxRangerWealth, 0.8, 1.1)) * Scale;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->StrengthInBestRanger, 0.2, 1.0, 0.5, 1.2)) * Scale;
            Scale = static_cast<long double>(Scale) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            if (Index == 0) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(1.0, 1.3)) * Scale;
            } else if (Index == 1) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.7, 1.1)) * Scale;
            } else {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.2, 0.6)) * Scale;
            }
            Hitpoints = System::Round(pas::real_max<pas::Extended>(1.5E+2L, pas::real_min<pas::Extended>(aPlayer::GetPlayer()->GetHull()->Weight * 1.2L, 525.0L * Scale)));
            {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"Ship.HS.", SysUtils::IntToStr(aMyFunction::IncrementWrapped(VisualIndex, 1, 5))}));
                std::int32_t round = System::Round(aMyFunction::RemapClamped(Hitpoints, 1.5E+2, 9.0E+2, 5.0E+1, 8.0E+1));
                Ship->CreateShipVisual(cpp_arg, round);
            }
            Hitpoints = System::Round(static_cast<long double>(aGalaxy::Galaxy->GetArcadeHitpointsModifier()) * Hitpoints);
            Ship->MaxHealth = Hitpoints;
            Ship->Health = Hitpoints;
            Ship->MaxSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 7.0, 5.0);
            Ship->MaxSpeed = static_cast<long double>(Ship->MaxSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->TurnSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 3.0, 2.0);
            Ship->TurnSpeed = static_cast<long double>(Ship->TurnSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->Thrust = 0.0;
            Ship->WeaponCount = 0;
            Kind = Globals::ArcadeBattleScreen->RandomRange(Minimum, Maximum);
            Ship->AddWeapon(Kind);
            if (Index == 0) {
                Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                if (aPlayer::GetPlayer()->StrengthInBestRanger > 0.9L || aPlayer::GetPlayer()->WealthInBestRanger > 0.9L) {
                    Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                }
            }
            if (aPlayer::GetPlayer()->StrengthInBestRanger > 0.7L || aPlayer::GetPlayer()->WealthInBestRanger > 0.7L) {
                Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
            }
            {
                pas::Extended cpp_left_2 = Globals::ArcadeBattleScreen->RandomRange(0, System::Round(Danger));
                if (cpp_left_2 < Danger) {
                    Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Ship->WeaponCount - 1); cpp_range_2.next(WeaponIndex); ) {
                Ship->Weapons[WeaponIndex].Damage = System::Round(static_cast<long double>(aGalaxy::Galaxy->GetArcadeDamageModifier()) * Ship->Weapons[WeaponIndex].Damage);
            }
            Ship->PrimaryWeapon = 0;
            Ship->EncounterTag = 1;
            pas::list_add(Objects, reinterpret_cast<void*>(Ship));
        }
        Kind = 0;
        std::int32_t MineralBudget = std::min<std::int32_t>(aConst::CargoHookLevelStats[8].PickupPower, pas::idiv(aPlayer::GetPlayer()->Wealth / 40, aConst::GoodsMarket[4].AveragePrice));
        MineralBudget = System::Round(static_cast<long double>(aMyFunction::RemapClamped(static_cast<long double>(Danger) + ApproachDanger, 0.0, 2.5E+2, 0.2, 1.2)) * MineralBudget);
        MineralBudget = System::Round(static_cast<long double>(aMyFunction::RemapClamped(Count, 0.0, 4.0, 0.8, 1.2)) * MineralBudget);
        for (Index = 1; Index <= 8; ++Index) {
            Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
            {
                aItem::TGoods* cpp_with = pas::checked_cast<aItem::TGoods*>(Item);
                {
                    pas::Extended cpp_left_3 = Globals::ArcadeBattleScreen->RandomFloat(0.6, 1.3);
                    std::int64_t round_2 = System::Round(cpp_left_3 * aConst::CargoHookLevelStats[aGalaxy::Galaxy->TechLevel].PickupPower);
                    std::int64_t randomRange = static_cast<std::int64_t>(Globals::ArcadeBattleScreen->RandomRange(MineralBudget / 8, MineralBudget / 2));
                    Count = std::min<std::int64_t>(round_2, randomRange) + 1;
                }
                Kind += Count;
                cpp_with->Init(aConst::t_Minerals, Count);
                cpp_with->NaturalFlag = true;
            }
            ArcadeItem = pas::construct_call<ab_Item::TabItem>(ab_Item::TabItem_Create);
            ArcadeItem->SetItem(Item);
            pas::list_add(Objects, reinterpret_cast<void*>(ArcadeItem));
            if (Kind > MineralBudget) {
                break;
            }
        }
    }

    void TabSpace::PopulateHoleEncounter() {
        aItem::TItem* Item{};
        ab_Item::TabItem* ArcadeItem{};
        std::int32_t Index{};
        std::int32_t WeaponIndex{};
        std::int32_t Kind{};
        std::int32_t VisualIndex{};
        std::int32_t Hitpoints{};
        ab_ShipAI::TabShipAI* Ship{};
        float Scale{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        std::int32_t Count = Globals::ArcadeBattleScreen->RandomRange(2, 4);
        Count = std::min<std::int32_t>(6, Count + System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 4.0, 8.0, 0.0, 2.0)));
        if (aPlayer::GetPlayer()->BlackHoleKillCount == 0) {
            Count = 1;
        } else if (aPlayer::GetPlayer()->BlackHoleKillCount < pas::real_divide(5.0L, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor)) {
            Count = std::min<std::int32_t>(Count, 2);
        }
        std::int32_t Minimum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->RefreshTechLevel(), 3.0, 8.0, 0.0, 6.0));
        std::int32_t Maximum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->RefreshTechLevel(), 3.0, 8.0, 6.0, 15.0));
        VisualIndex = Globals::ArcadeBattleScreen->RandomRange(0, 2);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            {
                std::int32_t cpp_case = aPlayer::GetPlayer()->BlackHoleKillCount;
                if (cpp_case == 0) {
                    Scale = 0.1f;
                } else if (cpp_case >= 1 && cpp_case <= 5) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 1.0, 5.0, 0.5, 0.6);
                } else if (cpp_case >= 6 && cpp_case <= 12) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 6.0, 12.0, 0.6, 0.8);
                } else if (cpp_case >= 13 && cpp_case <= 23) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 13.0, 23.0, 0.8, 1.0);
                } else if (cpp_case >= 24 && cpp_case <= 40) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 24.0, 4.0E+1, 1.0, 1.3);
                } else {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 41.0, 1.0E+2, 1.3, 1.6);
                }
            }
            Scale = static_cast<long double>(Scale) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->Wealth, pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0L), aGalaxy::Galaxy->MaxRangerWealth, 0.8, 1.1)) * Scale;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->StrengthInBestRanger, 0.2, 1.0, 0.5, 1.1)) * Scale;
            if (Index == 0) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(1.0, 1.5)) * Scale;
            } else if (Index == 1) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.7, 1.1)) * Scale;
            } else {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.2, 0.6)) * Scale;
            }
            if (aPlayer::GetPlayer()->BlackHoleKillCount == 0) {
                Hitpoints = 150;
            } else {
                Hitpoints = System::Round(pas::real_max<pas::Extended>(1.5E+2L, pas::real_min<pas::Extended>(aPlayer::GetPlayer()->GetHull()->Weight * 1.2L, 525.0L * Scale)));
            }
            {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"Ship.X.", SysUtils::IntToStr(aMyFunction::IncrementWrapped(VisualIndex, 0, 2))}));
                std::int32_t round = System::Round(aMyFunction::RemapClamped(Hitpoints, 1.5E+2, 9.0E+2, 5.0E+1, 8.0E+1));
                Ship->CreateShipVisual(cpp_arg, round);
            }
            if (aGalaxy::Galaxy != nullptr) {
                Hitpoints = System::Round(static_cast<long double>(aGalaxy::Galaxy->GetArcadeHitpointsModifier()) * Hitpoints);
            }
            Ship->MaxHealth = Hitpoints;
            Ship->Health = Hitpoints;
            Ship->MaxSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 7.0, 5.0);
            Ship->MaxSpeed = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 0.0, 3.0E+1, 0.6, 1.0)) * Ship->MaxSpeed;
            Ship->MaxSpeed = static_cast<long double>(Ship->MaxSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->TurnSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 4.0, 3.0);
            Ship->TurnSpeed = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 0.0, 3.0E+1, 0.6, 1.0)) * Ship->TurnSpeed;
            Ship->TurnSpeed = static_cast<long double>(Ship->TurnSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->Thrust = 0.0;
            Ship->WeaponCount = 0;
            Kind = Globals::ArcadeBattleScreen->RandomRange(Minimum, Maximum);
            Ship->AddWeapon(Kind);
            Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
            if (Index == 0) {
                Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                if (aPlayer::GetPlayer()->BlackHoleKillCount > 0 && aPlayer::GetPlayer()->StrengthInBestRanger > 0.9L || aPlayer::GetPlayer()->WealthInBestRanger > 0.9L) {
                    Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                }
            }
            if (aPlayer::GetPlayer()->BlackHoleKillCount > 0) {
                if (aPlayer::GetPlayer()->StrengthInBestRanger > 0.7L || aPlayer::GetPlayer()->WealthInBestRanger > 0.7L) {
                    Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                }
                {
                    pas::Extended cpp_left = Globals::ArcadeBattleScreen->RandomRange(0, System::Round(Danger));
                    if (cpp_left < Danger) {
                        Ship->AddWeapon(aMyFunction::IncrementWrapped(Kind, Minimum, Maximum));
                    }
                }
            }
            if (aGalaxy::Galaxy != nullptr) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Ship->WeaponCount - 1); cpp_range_2.next(WeaponIndex); ) {
                    Ship->Weapons[WeaponIndex].Damage = System::Round(static_cast<long double>(aGalaxy::Galaxy->GetArcadeDamageModifier()) * Ship->Weapons[WeaponIndex].Damage);
                }
            }
            Ship->PrimaryWeapon = 0;
            Ship->EncounterTag = 1;
            pas::list_add(Objects, reinterpret_cast<void*>(Ship));
        }
        Kind = 0;
        std::int32_t MineralBudget = std::min<std::int32_t>(aConst::CargoHookLevelStats[8].PickupPower, pas::idiv(aPlayer::GetPlayer()->Wealth / 40, aConst::GoodsMarket[4].AveragePrice));
        MineralBudget = System::Round(static_cast<long double>(aMyFunction::RemapClamped(Count, 2.0, 4.0, 0.8, 1.2)) * MineralBudget);
        for (Index = 1; Index <= 8; ++Index) {
            Item = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
            {
                aItem::TGoods* cpp_with = pas::checked_cast<aItem::TGoods*>(Item);
                {
                    pas::Extended cpp_left_2 = Globals::ArcadeBattleScreen->RandomFloat(0.6, 1.3);
                    std::int64_t round_2 = System::Round(cpp_left_2 * aConst::CargoHookLevelStats[aGalaxy::Galaxy->TechLevel].PickupPower);
                    std::int64_t randomRange = static_cast<std::int64_t>(Globals::ArcadeBattleScreen->RandomRange(MineralBudget / 8, MineralBudget / 2));
                    Count = std::min<std::int64_t>(round_2, randomRange) + 1;
                }
                Kind += Count;
                cpp_with->Init(aConst::t_Minerals, Count);
                cpp_with->NaturalFlag = true;
            }
            ArcadeItem = pas::construct_call<ab_Item::TabItem>(ab_Item::TabItem_Create);
            ArcadeItem->SetItem(Item);
            pas::list_add(Objects, reinterpret_cast<void*>(ArcadeItem));
            if (Kind > MineralBudget) {
                break;
            }
        }
    }

    void TabSpace::PopulateScriptedEncounter() {
        std::int32_t Kind{};
        float Scale{};
        std::int32_t Index{};
        ab_ShipAI::TabShipAI* Ship{};
        std::int32_t Minimum{};
        std::int32_t Maximum{};
        std::int32_t J{};
        ab_Ship::TabShip* OtherShip{};
        auto InitializeScriptedEncounterShip = [&]() -> void {
            std::int32_t WeaponIndex{};
            std::int32_t Hitpoints{};
            {
                std::int32_t cpp_case = aPlayer::GetPlayer()->BlackHoleKillCount;
                if (cpp_case == 0) {
                    Scale = 0.1f;
                } else if (cpp_case >= 1 && cpp_case <= 5) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 1.0, 5.0, 0.5, 0.6);
                } else if (cpp_case >= 6 && cpp_case <= 12) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 6.0, 12.0, 0.6, 0.8);
                } else if (cpp_case >= 13 && cpp_case <= 23) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 13.0, 23.0, 0.8, 1.0);
                } else if (cpp_case >= 24 && cpp_case <= 40) {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 24.0, 4.0E+1, 1.0, 1.3);
                } else {
                    Scale = aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 41.0, 1.0E+2, 1.3, 1.6);
                }
            }
            Scale = static_cast<long double>(Scale) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->Wealth, pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0L), aGalaxy::Galaxy->MaxRangerWealth, 0.8, 1.1)) * Scale;
            Scale = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->StrengthInBestRanger, 0.2, 1.0, 0.5, 1.1)) * Scale;
            if (Index == 0) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(1.0, 1.5)) * Scale;
            } else if (Index == 1) {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.7, 1.1)) * Scale;
            } else {
                Scale = static_cast<long double>(Globals::ArcadeBattleScreen->RandomFloat(0.2, 0.6)) * Scale;
            }
            if (Ship->ConvertedFromGameShip) {
                Ship->Health = Ship->Health * Ship->HealthScalePercent / 100;
                Ship->MaxHealth = Ship->MaxHealth * Ship->HealthScalePercent / 100;
            } else {
                Hitpoints = System::Round(pas::real_max<pas::Extended>(1.5E+2L, pas::real_min<pas::Extended>(aPlayer::GetPlayer()->GetHull()->Weight * 1.2L, 525.0L * Scale)));
                Hitpoints = Ship->HealthScalePercent * Hitpoints / 100;
                if (Ship->SpawnGraphKey.read(1) == u'R') {
                    Ship->CreateRuinsVisual(Ship->SpawnGraphKey, 128);
                } else {
                    Ship->CreateShipVisual(Ship->SpawnGraphKey, System::Round(aMyFunction::RemapClamped(Hitpoints, 1.5E+2, 9.0E+2, 5.0E+1, 8.0E+1)));
                }
                Ship->MaxHealth = Hitpoints;
                Ship->Health = Hitpoints;
            }
            Ship->MaxSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 7.0, 5.0);
            Ship->MaxSpeed = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 0.0, 3.0E+1, 0.6, 1.0)) * Ship->MaxSpeed;
            Ship->MaxSpeed = static_cast<long double>(Ship->MaxSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->TurnSpeed = aMyFunction::RemapClamped(Ship->VisualDiameter, 5.0E+1, 8.0E+1, 4.0, 3.0);
            Ship->TurnSpeed = static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount, 0.0, 3.0E+1, 0.6, 1.0)) * Ship->TurnSpeed;
            Ship->TurnSpeed = static_cast<long double>(Ship->TurnSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->Thrust = 0.0;
            if (!Ship->ConvertedFromGameShip) {
                Ship->WeaponCount = 0;
                Kind = Globals::ArcadeBattleScreen->RandomRange(Minimum, Maximum);
                Ship->AddWeapon(Kind);
                {
                    std::int32_t incrementWrapped = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                    ab_Ship::TabShip* ship = Ship;
                    ship->AddWeapon(incrementWrapped);
                }
                if (Index == 0) {
                    {
                        std::int32_t incrementWrapped_2 = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                        ab_Ship::TabShip* ship_2 = Ship;
                        ship_2->AddWeapon(incrementWrapped_2);
                    }
                    if (aPlayer::GetPlayer()->BlackHoleKillCount > 0 && aPlayer::GetPlayer()->StrengthInBestRanger > 0.9L || aPlayer::GetPlayer()->WealthInBestRanger > 0.9L) {
                        std::int32_t incrementWrapped_3 = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                        ab_Ship::TabShip* ship_3 = Ship;
                        ship_3->AddWeapon(incrementWrapped_3);
                    }
                }
                if (aPlayer::GetPlayer()->BlackHoleKillCount > 0) {
                    if (aPlayer::GetPlayer()->StrengthInBestRanger > 0.7L || aPlayer::GetPlayer()->WealthInBestRanger > 0.7L) {
                        std::int32_t incrementWrapped_4 = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                        ab_Ship::TabShip* ship_4 = Ship;
                        ship_4->AddWeapon(incrementWrapped_4);
                    }
                    {
                        pas::Extended cpp_left = Globals::ArcadeBattleScreen->RandomRange(0, System::Round(this->Danger));
                        if (cpp_left < this->Danger) {
                            std::int32_t incrementWrapped_5 = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                            ab_Ship::TabShip* ship_5 = Ship;
                            ship_5->AddWeapon(incrementWrapped_5);
                        }
                    }
                }
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Ship->WeaponCount - 1); cpp_range.next(WeaponIndex); ) {
                Ship->Weapons[WeaponIndex].Damage = Ship->Weapons[WeaponIndex].Damage * Ship->DamageScalePercent / 100;
            }
            Ship->PrimaryWeapon = 0;
            Ship->EncounterTag = 1;
        };
        if (aPlayer::GetPlayer() == nullptr || ab_MainForm::ActiveArcadeRequestShips == nullptr) {
            return;
        }
        Minimum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->RefreshTechLevel(), 3.0, 8.0, 0.0, 6.0));
        Maximum = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->RefreshTechLevel(), 3.0, 8.0, 6.0, 15.0));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_MainForm::ActiveArcadeRequestShips) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<ab_ShipAI::TabShipAI>(ab_MainForm::ActiveArcadeRequestShips, Index);
            InitializeScriptedEncounterShip();
            if (Ship->Team == 1) {
                ab_Ship::PlayerArcadeShip->AddTrackedShip(Ship);
            } else {
                ab_Ship::PlayerArcadeShip->AddEnemy(Ship);
                Ship->AddEnemy(ab_Ship::PlayerArcadeShip);
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ab_MainForm::ActiveArcadeRequestShips) - 1); cpp_range_2.next(J); ) {
                if (J != Index) {
                    OtherShip = pas::list_at<ab_Ship::TabShip>(ab_MainForm::ActiveArcadeRequestShips, J);
                    if (Ship->Team != OtherShip->Team) {
                        Ship->AddEnemy(OtherShip);
                    }
                }
            }
            pas::list_add(Objects, reinterpret_cast<void*>(Ship));
        }
        pas::list_clear(ab_MainForm::ActiveArcadeRequestShips);
    }

    void TabSpace::PopulateKellerEncounter() {
        ab_ShipAI::TabShipAI* Ship{};
        std::int32_t I{};
        std::int32_t VisualIndex{};
        std::int32_t Minimum{};
        std::int32_t Maximum{};
        std::int32_t Kind{};
        Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
        Ship->CreateRuinsVisual(u"Ruins.Keller"_wref.get(), 128);
        {
            std::int32_t cpp_case = aPlayer::GetPlayer()->BlackHoleKillCount + aPlayer::GetPlayer()->HyperspaceKillCount;
            if (cpp_case >= 0 && cpp_case <= 10) {
                Ship->MaxHealth = std::max<std::int32_t>(2000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->Weight, 2.0L)));
            } else if (cpp_case >= 11 && cpp_case <= 20) {
                Ship->MaxHealth = std::max<std::int32_t>(2000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->Weight, 1.6L)));
            } else if (cpp_case >= 21 && cpp_case <= 52) {
                Ship->MaxHealth = std::max<std::int32_t>(2000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->Weight, 1.3L)));
            } else {
                Ship->MaxHealth = std::max<std::int32_t>(2000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->Weight, 1.0L)));
            }
        }
        {
            std::int32_t cpp_case_2 = aPlayer::GetPlayer()->BlackHoleKillCount + aPlayer::GetPlayer()->HyperspaceKillCount;
            if (cpp_case_2 >= 0 && cpp_case_2 <= 10) {
                Ship->Health = std::max<std::int32_t>(1000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->HullPoints, 2.0L)));
            } else if (cpp_case_2 >= 11 && cpp_case_2 <= 20) {
                Ship->Health = std::max<std::int32_t>(1000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->HullPoints, 1.5L)));
            } else if (cpp_case_2 >= 21 && cpp_case_2 <= 52) {
                Ship->Health = std::max<std::int32_t>(1000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->HullPoints, 1.3L)));
            } else {
                Ship->Health = std::max<std::int32_t>(1000, aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(aKling::KellerShip->GetHull()->HullPoints, 1.0L)));
            }
        }
        Ship->Health = System::Round(static_cast<long double>(Ship->Health) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor);
        if (Ship->Health > Ship->MaxHealth) {
            Ship->Health = Ship->MaxHealth;
        }
        Ship->MaxSpeed = 6.0;
        Ship->MaxSpeed = static_cast<long double>(Ship->MaxSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
        Ship->TurnSpeed = 3.0;
        Ship->TurnSpeed = static_cast<long double>(Ship->TurnSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
        Ship->Thrust = 0.0;
        Ship->WeaponCount = aKling::KellerShip->WeaponCount;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aKling::KellerShip->WeaponCount - 1); cpp_range.next(I); ) {
            ab_W::ab_Weapon_InitializeFromInfo(&Ship->Weapons[I], aKling::KellerShip->Weapons[I + 1]->GetWeaponInfo());
        }
        Ship->PrimaryWeapon = 0;
        Ship->EncounterTag = 1;
        pas::list_add(Objects, reinterpret_cast<void*>(Ship));
        ab_Ship::KellerArcadeShip = Ship;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(I); ) {
            ab_Hit::KellerFragments[I] = nullptr;
            ab_Hit::KellerFragmentDistances[I] = 0.0;
            ab_Hit::KellerFragmentValuesAC[I] = 0.0;
        }
        ab_Hit::KellerBreakupTicks = 0;
        ab_Hit::KellerSplitActive = false;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(aGalaxy::Galaxy->DifficultyLevels[6])); cpp_range_3.next(I); ) {
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            if (I == 0) {
                Ship->CreateShipVisual(u"Ship.Keller.K1"_wref.get(), 100);
                Ship->Health = ab_Ship::KellerArcadeShip->Health / 2;
            } else {
                VisualIndex = Globals::ArcadeBattleScreen->RandomRange(3, 5);
                {
                    std::int32_t cpp_right = Globals::ArcadeBattleScreen->RandomRange(0, 10);
                    std::int32_t cpp_arg = 90 - 10 * VisualIndex + cpp_right;
                    const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::concat_ansi({"Ship.Keller.K", SysUtils::IntToStr(VisualIndex)}));
                    ab_Ship::TabShip* ship = Ship;
                    ship->CreateShipVisual(cpp_arg_2, cpp_arg);
                }
                Ship->Health = System::Round(static_cast<long double>(100 * (8 - VisualIndex)) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor);
            }
            Ship->MaxHealth = Ship->Health;
            Ship->MaxSpeed = 7.0;
            Ship->MaxSpeed = static_cast<long double>(Ship->MaxSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->TurnSpeed = 4.0;
            Ship->TurnSpeed = static_cast<long double>(Ship->TurnSpeed) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].QuestTimeAndExperienceFactor;
            Ship->Thrust = 0.0;
            Ship->WeaponCount = 0;
            if (I == 0) {
                Minimum = 8;
                Maximum = 13;
            } else {
                Minimum = 5;
                Maximum = 10;
            }
            Kind = Globals::ArcadeBattleScreen->RandomRange(Minimum, Maximum);
            Ship->AddWeapon(Kind);
            {
                std::int32_t incrementWrapped = aMyFunction::IncrementWrapped(Kind, Minimum, Maximum);
                ab_Ship::TabShip* ship_2 = Ship;
                ship_2->AddWeapon(incrementWrapped);
            }
            Ship->PrimaryWeapon = 0;
            Ship->EncounterTag = 1;
            pas::list_add(Objects, reinterpret_cast<void*>(Ship));
        }
    }

    void TabSpace::UpdateApproachDanger() {
        if (Danger <= 0.0L) {
            ApproachDanger = 0.0;
            return;
        }
        ApproachDanger = ab_Space::FindApproachDanger(this, 0.0f);
    }

    float FindApproachDanger(TabSpace* Space, float Accumulated) {
        float Candidate{};
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            if (Link->Last == Space && Link->First->Danger <= 0.0L) {
                return Accumulated;
            }
            Link = Link->Next;
        }
        float Result = 1.0E+20f;
        Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            if (Link->Last == Space) {
                Candidate = ab_Space::FindApproachDanger(Link->First, static_cast<long double>(Accumulated) + Link->First->Danger);
                if (Candidate < Result) {
                    Result = Candidate;
                }
            }
            Link = Link->Next;
        }
        return Result;
    }

    // Native instance receiver is unused; visits the complete graph.
    void TabSpace::PruneApproachDanger() {
        PabSpaceLink Link{};
        TabSpace* Space{};
        std::uint8_t Changed = true;
        while (Changed) {
            Changed = false;
            Space = FirstArcadeSpace;
            while (Space != nullptr) {
                if (Space->ApproachDanger > 0.0L) {
                    Link = FirstArcadeSpaceLink;
                    while (Link != nullptr) {
                        if (Link->First == Space && Link->Last->ApproachDanger > 0.0L) {
                            break;
                        }
                        Link = Link->Next;
                    }
                    if (Link != nullptr) {
                        Link = FirstArcadeSpaceLink;
                        while (Link != nullptr) {
                            if (Link->Last == Space && Link->First->ApproachDanger > 0.0L) {
                                break;
                            }
                            Link = Link->Next;
                        }
                        if (Link == nullptr) {
                            Space->ApproachDanger = 0.0;
                            Changed = true;
                        }
                    }
                }
                Space = Space->Next;
            }
        }
    }

    pas::WideString TabSpace::GetDangerText() {
        pas::WideString Result{};
        if (Danger == 0.0L) {
            return aConst::LocalizedColorText(u"FormAB.DangerMini"_wref.get());
        } else if (Danger < 4.0E+1L) {
            return aConst::LocalizedColorText(u"FormAB.DangerSmall"_wref.get());
        } else if (Danger < 7.0E+1L) {
            return aConst::LocalizedColorText(u"FormAB.DangerAverage"_wref.get());
        } else if (Danger < 9.0E+1L) {
            return aConst::LocalizedColorText(u"FormAB.DangerBig"_wref.get());
        } else {
            return aConst::LocalizedColorText(u"FormAB.DangerHuge"_wref.get());
        }
    }

    void TabSpace::RecountLinks() {
        IncomingCount = 0;
        OutgoingCount = 0;
        PabSpaceLink Link = FirstArcadeSpaceLink;
        while (Link != nullptr) {
            if (Link->First == this) {
                ++OutgoingCount;
            } else if (Link->Last == this) {
                ++IncomingCount;
            }
            Link = Link->Next;
        }
    }

    void TabSpace::p_destroy() {
        ab_Space::TabSpace_Destroy(this);
    }

} // namespace ab_Space
