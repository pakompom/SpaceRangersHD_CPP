#include "layout/aGroup.hpp"
#include "types/SystemImports.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGroup.hpp"
#include "units/aMyFunction.hpp"

namespace aGroup {
    void TGroup_Create(TGroup* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->CreatedTurn = aGalaxy::Galaxy->CurrentTurn;
            Self->GenerationSeed = aMyFunction::SeededRandomIntRange(100000, SystemImports::MaxInt, aGalaxy::Galaxy->GenerationSeed * aGalaxy::Galaxy->CurrentTurn);
            Self->RandomState = Self->GenerationSeed;
        }
        Self->Ships = pas::make_object<pas::List>();
        Self->Route.set_length(0);
        Self->Route = nullptr;
        Self->TargetStar = nullptr;
        Self->AssemblyStar = nullptr;
    }

    void TGroup_Destroy(TGroup* Self) {
        if (Self->Ships != nullptr) {
            pas::free(Self->Ships);
            Self->Ships = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TGroup::Save(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        TGroupRouteOrder Order{};
        Buffer->AddWideChar(CreatedTurn);
        Buffer->AddDWord(GenerationSeed);
        Buffer->AddDWord(RandomState);
        Buffer->AddAnsiChar('\000');
        std::int32_t Count = pas::list_count(Ships);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            Buffer->AddDWord(Ship->Id);
        }
        Count = Route.length();
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Order = Route[I];
            Buffer->AddAnsiChar(Order.Kind);
            if (Order.Kind == 3) {
                Buffer->AddDWord(pas::checked_cast<aGalaxy::TStar*>(Order.Target)->Id);
            } else if (Order.Kind == 4) {
                Buffer->AddDWord(pas::checked_cast<aGalaxy::THole*>(Order.Target)->Id);
            } else if (Order.Kind == 2) {
                if (pas::class_cast_if<aShip::TShip*>(Order.Target) != nullptr) {
                    Buffer->AddDWord(static_cast<std::uint32_t>(pas::checked_cast<aShip::TShip*>(Order.Target)->Id) | 0x80000000u);
                } else {
                    Buffer->AddDWord(pas::checked_cast<aPlanet::TPlanet*>(Order.Target)->Id);
                }
            } else if (Order.Kind == 6) {
                Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(Order.Target)->Id);
            } else {
                Buffer->AddDWord(0u);
            }
            Buffer->AddSingle(Order.Destination.X);
            Buffer->AddSingle(Order.Destination.Y);
            Buffer->AddAnsiChar(Order.WaitMode);
            Buffer->AddIntegerValue(Order.WaitUntilTurn);
        }
    }

    // Ships initially contains serialized IDs, pending reference resolution. Does not clear existing entries.
    void TGroup::Load(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        CreatedTurn = EC_Buf::TBufEC_GetWord(Buffer);
        std::int32_t OldestTurn = Galaxy->CurrentTurn - 1000;
        while (CreatedTurn < OldestTurn) {
            CreatedTurn += 0x00010000;
        }
        GenerationSeed = EC_Buf::TBufEC_GetUInt32(Buffer);
        RandomState = EC_Buf::TBufEC_GetUInt32(Buffer);
        EC_Buf::TBufEC_GetByte(Buffer);
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err TGroup.Load FShips"_a));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            void* uInt32 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            pas::List* ships = Ships;
            pas::list_add(ships, uInt32);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        if (Count < 0 || Count > 10000) {
            pas::raise(pas::make_exception<pas::Abort>("Err TGroup.Load FOrders"_a));
        }
        Route.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Route[I].Kind = EC_Buf::TBufEC_GetByte(Buffer);
            Route[I].Target = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
            Route[I].Destination.X = EC_Buf::TBufEC_GetSingle(Buffer);
            Route[I].Destination.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            Route[I].WaitMode = EC_Buf::TBufEC_GetByte(Buffer);
            Route[I].WaitUntilTurn = EC_Buf::TBufEC_GetInt32(Buffer);
        }
    }

    // Rebinds ship and route target IDs after Load.
    void TGroup::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            pas::list_put(Ships, I, reinterpret_cast<void*>(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Ships, I))), true)))));
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            Ship->LiberationGroup = this;
        }
        {
            const std::int32_t cpp_last = Route.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    if (Route[I].Kind == 3) {
                        Route[I].Target = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(Galaxy->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)))));
                    } else if (Route[I].Kind == 4) {
                        Route[I].Target = pas::checked_cast<aGalaxy::THole*>(static_cast<pas::Object*>(Galaxy->IdToHole(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)))));
                    } else if (Route[I].Kind == 2) {
                        if ((static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)) & 0x80000000u) == 0x80000000u) {
                            Route[I].Target = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)) & 0x7fffffff, true)));
                        } else {
                            Route[I].Target = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)), true)));
                        }
                    } else if (Route[I].Kind == 6) {
                        Route[I].Target = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Galaxy->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Route[I].Target)), true)));
                    } else {
                        Route[I].Target = nullptr;
                    }
                }
            }
        }
    }

    // Appends Ship, assigns LiberationGroup and resets its order index.
    void TGroup::AddShip(aShip::TShip* Ship) {
        pas::list_add(Ships, reinterpret_cast<void*>(Ship));
        Ship->LiberationGroup = this;
        Ship->LiberationGroupRouteIndex = 0;
    }

    // May remove and free Self when empty or older than 150 days.
    void TGroup_NextDay(TGroup* Self) {
        if (pas::list_count(Self->Ships) == 0) {
            pas::list_delete(aGalaxy::Galaxy->LiberationGroups, pas::list_indexof(aGalaxy::Galaxy->LiberationGroups, reinterpret_cast<void*>(Self)));
            pas::free(Self);
        } else if (aGalaxy::Galaxy->CurrentTurn > Self->CreatedTurn + 150) {
            aGroup::TGroup_Disband(Self);
        }
    }

    // Detaches member ships, removes Self from Galaxy.LiberationGroups, and frees Self.
    void TGroup_Disband(TGroup* Self) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        {
            const std::int32_t cpp_first = pas::list_count(Self->Ships) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Ship = pas::list_at<aShip::TShip>(Self->Ships, I);
                    Ship->LeaveLiberationGroup();
                }
            }
        }
        pas::list_delete(aGalaxy::Galaxy->LiberationGroups, pas::list_indexof(aGalaxy::Galaxy->LiberationGroups, reinterpret_cast<void*>(Self)));
        pas::free(Self);
    }

    // Chooses TargetStar and a Coalition AssemblyStar within 28 parsecs. Failure disbands and frees Self.
    std::uint8_t TGroup::SelectLiberationTarget() {
        std::uint8_t Result{};
        TargetStar = aGalaxy::Galaxy->SelectStarForLiberationAttack(FindCentralMemberStar(), aGalaxyStruct::sfCoalition);
        std::int32_t Attempts = 0;
        while (TargetStar == nullptr || TargetStar->HasLiberationGroupOrder() || aGalaxy::TGalaxy::HasMilitaryBaseAssignedToStar(TargetStar)) {
            if (Attempts > 10) {
                TargetStar = nullptr;
                AssemblyStar = nullptr;
                aGroup::TGroup_Disband(this);
                return false;
            }
            TargetStar = aGalaxy::Galaxy->SelectStarForLiberationAttack(FindCentralMemberStar(), aGalaxyStruct::sfCoalition);
            ++Attempts;
        }
        AssemblyStar = TargetStar->FindNearestStarByFaction(aGalaxyStruct::sfCoalition, false);
        if (AssemblyStar == nullptr || aMyFunction::PointDistance(AssemblyStar->Position, TargetStar->Position) > 28.0L) {
            TargetStar = nullptr;
            AssemblyStar = nullptr;
            Result = false;
            aGroup::TGroup_Disband(this);
            return Result;
        }
        return true;
    }

    // Builds staging, landing and attack orders and publishes news. May disband and free Self when no suitable staging planet exists.
    std::uint8_t TGroup::BuildLiberationOrders() {
        pas::WideString Text{};
        std::uint8_t Result = true;
        if (!(TargetStar != nullptr && AssemblyStar != nullptr || SelectLiberationTarget())) {
            return Result;
        }
        Route.set_length(4);
        {
            TGroupRouteOrder& cpp_with = Route[0];
            cpp_with.Kind = 3;
            cpp_with.Target = AssemblyStar;
            cpp_with.WaitMode = 0;
            cpp_with.WaitUntilTurn = 0;
        }
        aPlanet::TPlanet* Planet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(AssemblyStar->FindFirstInhabitedPlanet()));
        if (Planet == nullptr) {
            Result = false;
            aGroup::TGroup_Disband(this);
            return Result;
        }
        {
            TGroupRouteOrder& cpp_with_2 = Route[1];
            cpp_with_2.Kind = 2;
            cpp_with_2.Target = Planet;
            cpp_with_2.WaitMode = 0;
            cpp_with_2.WaitUntilTurn = 0;
        }
        {
            TGroupRouteOrder& cpp_with_3 = Route[2];
            cpp_with_3.Kind = 1;
            cpp_with_3.Target = nullptr;
            pas::store_unaligned<EC_Struct::TPointF>(&cpp_with_3.Destination, AssemblyStar->GetBoundaryPointTowardStar(TargetStar));
            cpp_with_3.WaitMode = 3;
            {
                std::int32_t cpp_right = aMyFunction::NextRandomIntRange(45, 55, RandomState);
                cpp_with_3.WaitUntilTurn = aGalaxy::Galaxy->CurrentTurn + cpp_right;
            }
        }
        {
            TGroupRouteOrder& cpp_with_4 = Route[3];
            cpp_with_4.Kind = 3;
            cpp_with_4.Target = TargetStar;
            cpp_with_4.WaitMode = 0;
            cpp_with_4.WaitUntilTurn = 0;
        }
        if (TargetStar->Status.CustomFaction != u"") {
            Text = aConst::PickLocalizedTextVariant(pas::concat_wide({u"GalaxyNews.Group.WarriorLiberator.Create", TargetStar->Status.CustomFaction}), RandomState * (aGalaxy::Galaxy->CurrentTurn % 71));
        } else if (TargetStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Group.WarriorLiberator.CreatePirates"_wref.get(), RandomState * (aGalaxy::Galaxy->CurrentTurn % 71));
        } else {
            Text = aConst::PickLocalizedTextVariant(u"GalaxyNews.Group.WarriorLiberator.Create"_wref.get(), RandomState * (aGalaxy::Galaxy->CurrentTurn % 71));
        }
        aMyFunction::ReplaceTextToken(Text, u"<StarNormal>"_w, AssemblyStar->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<StarEnemy>"_w, TargetStar->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<SectorNormal>"_w, AssemblyStar->Constellation->GetName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<SectorEnemy>"_w, TargetStar->Constellation->GetName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(Route[2].WaitUntilTurn), u"<color=255,240,100>"_w);
        aGalaxy::Galaxy->AddPlanetNews(26, Text);
        return Result;
    }

    // Requires a nonempty member list; same route index, no land/jump order, and within 300 units.
    std::uint8_t TGroup::AreShipsAssembled() {
        std::int32_t I{};
        aShip::TShip* Ship = pas::list_at<aShip::TShip>(Ships, 0);
        std::int32_t RouteIndex = Ship->LiberationGroupRouteIndex;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Ships, I);
            if (Ship->LiberationGroupRouteIndex != RouteIndex || static_cast<std::uint8_t>(pas::is_one_of<aShip::soNone, aShip::soMove>(Ship->Order) ^ 1) || aMyFunction::PointDistanceSquared(Ship->Position, Route[RouteIndex].Destination) > 9.0E+4L) {
                return false;
            }
        }
        return true;
    }

    // Advances members in reverse order, detaching completed members.
    void TGroup::AdvanceRouteForShips() {
        std::int32_t I{};
        aShip::TShip* Ship{};
        {
            const std::int32_t cpp_first = pas::list_count(Ships) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Ship = pas::list_at<aShip::TShip>(Ships, I);
                    ++Ship->LiberationGroupRouteIndex;
                    if (Ship->LiberationGroupRouteIndex >= Route.length()) {
                        Ship->LeaveLiberationGroup();
                    } else {
                        Ship->ProcessLiberationGroupRoute();
                    }
                }
            }
        }
    }

    pas::WideString TGroup::GetShipGreeting(void* Ship) {
        pas::WideString Result{};
        aGalaxy::TStar* Star{};
        if (static_cast<aShip::TShip*>(Ship)->LiberationGroupRouteIndex != 0) {
            Star = pas::checked_cast<aGalaxy::TStar*>(Route[3].Target);
            if (aGalaxy::Galaxy->CurrentTurn < Route[2].WaitUntilTurn) {
                auto name = pas::borrow(Star->Name);
                pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(Route[2].WaitUntilTurn);
                pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"ShipGreetings.Group.WarriorLiberatorBefore"_wref.get(), aMyFunction::NextRandomIntRange(100, 1000, RandomState));
                return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<StarEnemy>"_w, name.get(), u"<Date>"_w, std::move(formatTurnDate));
            }
            auto name_2 = pas::borrow(Star->Name);
            pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(Route[2].WaitUntilTurn);
            pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"ShipGreetings.Group.WarriorLiberatorAfter"_wref.get(), aMyFunction::NextRandomIntRange(100, 1000, RandomState));
            return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_2), u"<color=255,240,100>"_w, u"<StarEnemy>"_w, name_2.get(), u"<Date>"_w, std::move(formatTurnDate_2));
        }
        return Result;
    }

    // Chooses a member's current star minimizing summed rounded distances to all galaxy planets.
    aGalaxy::TStar* TGroup::FindCentralMemberStar() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Distance{};
        aShip::TShip* Ship{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Result = nullptr;
        std::int32_t BestDistance = SystemImports::MaxInt;
        {
            const std::int32_t cpp_first = pas::list_count(Ships) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Ship = pas::list_at<aShip::TShip>(Ships, I);
                    Distance = 0;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(J); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, J);
                        Distance += System::Round(aMyFunction::PointDistance(Ship->CurrentStar->Position, Planet->CurrentStar->Position));
                    }
                    if (BestDistance > Distance) {
                        Result = Ship->CurrentStar;
                        BestDistance = Distance;
                    }
                }
            }
        }
        return Result;
    }

    void TGroup::p_destroy() {
        aGroup::TGroup_Destroy(this);
    }

} // namespace aGroup
