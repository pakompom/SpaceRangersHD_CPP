#include "layout/ab_Item.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aItem.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_ShipAI.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aShip.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Item.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_Zone.hpp"

// TabItem VMT and helpers:; original unit boundary unresolved.
namespace ab_Item {
    void ab_Item_Update() {
        ab_Zone::PabZone Zone{};
        TabItem* Bonus{};
        std::int32_t Index{};
        std::int32_t Count{};
        float Distance{};
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject* Ship{};
        pas::Array<std::int32_t, 0, 7> Kinds{};
        if (ab_Ship::PlayerArcadeShip != nullptr) {
            if (pas::list_count(ab_Ship::PlayerArcadeShip->Enemies) > 0) {
                Zone = ab_Zone::FirstZone;
                while (Zone != nullptr) {
                    if ((Zone->BonusFlags & ab_Global::ArcadeBonusKindMask) != 0 && Zone->NextBonusTick >= 0 && Zone->NextBonusTick <= ab_Global::ArcadeTickCount) {
                        Zone->NextBonusTick = -1;
                        Count = 0;
                        for (Index = 0; Index <= 7; ++Index) {
                            if ((Zone->BonusFlags & pas::shl(1, Index)) != 0) {
                                Kinds[Count] = Index;
                                ++Count;
                            }
                        }
                        Bonus = pas::construct_call<TabItem>(TabItem_Create);
                        {
                            std::int32_t cpp_arg = Kinds[aMyFunction::RandomIntRange(0, Count - 1)];
                            std::uint8_t cpp_arg_2 = (Zone->BonusFlags & ab_Global::ArcadeHiddenBonusFlag) != 0;
                            TabItem* bonus = Bonus;
                            bonus->SetBonus(cpp_arg, cpp_arg_2, Zone);
                        }
                        Bonus->State.LongitudeDegrees = Zone->Longitude;
                        Bonus->State.PolarAngleDegrees = Zone->PolarAngle;
                        Bonus->State.BearingDegrees = aMyFunction::RandomIntRange(0, 359);
                        Distance = 0.0f;
                        ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&Bonus->State.LongitudeDegrees), pas::Var<double>(&Bonus->State.PolarAngleDegrees), pas::Var<double>(&Bonus->State.BearingDegrees), ab_Global::SphereRadius, Distance);
                        Bonus->AttachVisual();
                        ab_Object::ab_Object_Add(Bonus);
                    }
                    Zone = Zone->Next;
                }
            }
        }
        ab_Object::TabObject* NextObj = ab_Object::FirstArcadeObject;
        while (NextObj != nullptr) {
            Obj = NextObj;
            NextObj = NextObj->Next;
            if (pas::class_cast_if<TabItem*>(Obj) != nullptr && reinterpret_cast<TabItem*>(Obj)->BonusKind >= 0) {
                Bonus = reinterpret_cast<TabItem*>(Obj);
                Ship = ab_Object::FirstArcadeObject;
                while (Ship != nullptr) {
                    if (pas::class_cast_if<ab_Ship::TabShip*>(Ship) != nullptr && Ship->DistanceTo(Bonus) < 6.0E+1L) {
                        Bonus->Visual->DetachFromSpace();
                        Bonus->SpawnZone->NextBonusTick = ([&] {
                            std::int32_t cpp_right = 20 * aMyFunction::RandomIntRange(ab_Global::BonusRespawnSeconds[Bonus->SpawnZone->BonusRespawnClass * 2], ab_Global::BonusRespawnSeconds[Bonus->SpawnZone->BonusRespawnClass * 2 + 1]);
                            return ab_Global::ArcadeTickCount + cpp_right;
                        }());
                        if (Bonus->BonusKind == ab_Global::abkInvisibility && reinterpret_cast<ab_Ship::TabShip*>(Ship)->BonusTicks[Bonus->BonusKind] <= 0) {
                            reinterpret_cast<ab_Ship::TabShip*>(Ship)->RevealTicks = 0;
                        }
                        reinterpret_cast<ab_Ship::TabShip*>(Ship)->BonusTicks[Bonus->BonusKind] = 20 * ab_Global::BonusDurationSeconds[Bonus->BonusKind];
                        if (ab_Ship::PlayerArcadeShip == Ship) {
                            GR_Main::SoundManager->PlaySound(Globals::ArcadeItemSounds[Bonus->BonusKind]);
                        }
                        ab_Object::ab_Object_Delete(Bonus);
                        break;
                    }
                    Ship = Ship->Next;
                }
            }
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0 && aPlayer::GetPlayer() != nullptr && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook())) {
            NextObj = ab_Object::FirstArcadeObject;
            while (NextObj != nullptr) {
                Obj = NextObj;
                NextObj = NextObj->Next;
                if (pas::class_cast_if<TabItem*>(Obj) != nullptr && static_cast<TabItem*>(Obj)->Item != nullptr) {
                    if (pas::checked_cast<TabItem*>(Obj)->Item->Weight <= aPlayer::GetPlayer()->CargoFreeSpace && ab_Ship::PlayerArcadeShip->DistanceTo(Obj) < ab_Global::CargoPickupDistance) {
                        std::int32_t cpp_left = aShip::TShip_CalculateCargoHookPower(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook());
                        if (cpp_left >= pas::checked_cast<TabItem*>(Obj)->Item->Weight) {
                            ab_MainForm::TfAB::PickUpItem(pas::checked_cast<TabItem*>(Obj));
                            Globals::ArcadeBattleScreen->CancelCargoPickup();
                        }
                    }
                }
            }
        }
    }

    void ab_Item_Drop(ab_Object::TabObject* Origin, aItem::TItem* Item, std::int32_t MinDistance, std::int32_t MaxDistance) {
        TabItem* Dropped{};
        std::int32_t Attempt{};
        double Distance{};
        double Bearing{};
        ab_Object::TabObject* Obj{};
        Dropped = pas::construct_call<TabItem>(TabItem_Create);
        Dropped->SetItem(Item);
        for (Attempt = 0; Attempt <= 10; ++Attempt) {
            Distance = aMyFunction::RandomIntRange(MinDistance, MaxDistance);
            Bearing = aMyFunction::RandomIntRange(0, 359);
            Dropped->State = Origin->State;
            Dropped->State.BearingDegrees = Bearing;
            ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&Dropped->State.LongitudeDegrees), pas::Var<double>(&Dropped->State.PolarAngleDegrees), pas::Var<double>(&Dropped->State.BearingDegrees), ab_Global::SphereRadius, Distance);
            Obj = ab_Object::FirstArcadeObject;
            while (Obj != nullptr) {
                if (pas::class_cast_if<TabItem*>(Obj) != nullptr && Obj->DistanceTo(Dropped) < 2.0E+1L) {
                    break;
                }
                Obj = Obj->Next;
            }
            if (Obj == nullptr) {
                break;
            }
        }
        ab_Object::ab_Object_Add(Dropped);
        Dropped->AttachVisual();
    }

    TabItem* ab_Item_FindNearestBonus(ab_Zone::PabZone Origin) {
        double Distance{};
        TabItem* Result = nullptr;
        double BestDistance = 1.0E+20;
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabItem*>(Obj) != nullptr) {
                std::int32_t cpp_case = reinterpret_cast<TabItem*>(Obj)->BonusKind;
                if (cpp_case == ab_Global::abkRegeneration || cpp_case == ab_Global::abkSpeed || cpp_case >= ab_Global::abkDamage && cpp_case <= ab_Global::abkInvisibility) {
                    ab_Global::ComputeSphericalDistance(Distance, Origin->Longitude, Origin->PolarAngle, 0.0, Obj->State.LongitudeDegrees, Obj->State.PolarAngleDegrees, ab_Global::SphereRadius);
                    if (Distance < BestDistance) {
                        BestDistance = Distance;
                        Result = reinterpret_cast<TabItem*>(Obj);
                    }
                }
            }
            Obj = Obj->Next;
        }
        return Result;
    }

    TabItem* ab_Item_FindBonusRoute(ab_Zone::PabZone Origin, ab_Zone::PabZone& Zone) {
        double Distance{};
        ab_Zone::PabZone Route{};
        TabItem* Result = nullptr;
        Zone = nullptr;
        double BestDistance = 1.0E+20;
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabItem*>(Obj) != nullptr) {
                std::int32_t cpp_case = reinterpret_cast<TabItem*>(Obj)->BonusKind;
                if (cpp_case == ab_Global::abkRegeneration || cpp_case == ab_Global::abkSpeed || cpp_case >= ab_Global::abkDamage && cpp_case <= ab_Global::abkInvisibility) {
                    ab_Global::ComputeSphericalDistance(Distance, Origin->Longitude, Origin->PolarAngle, 0.0, Obj->State.LongitudeDegrees, Obj->State.PolarAngleDegrees, ab_Global::SphereRadius);
                    if (Distance < BestDistance) {
                        Route = ab_Zone::ab_Zone_FindReachableRouteZone(reinterpret_cast<TabItem*>(Obj)->SpawnZone);
                        if (Route != nullptr) {
                            if (aMyFunction::RandomIntRange(0, 2) == 0) {
                                Zone = Route;
                                BestDistance = Distance;
                                Result = reinterpret_cast<TabItem*>(Obj);
                            }
                        }
                    }
                }
            }
            Obj = Obj->Next;
        }
        return Result;
    }

    TabItem* ab_Item_FindRepairRoute(ab_Zone::PabZone Origin, ab_Zone::PabZone& Zone) {
        double Distance{};
        ab_Zone::PabZone Route{};
        TabItem* Result = nullptr;
        Zone = nullptr;
        double BestDistance = 1.0E+20;
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabItem*>(Obj) != nullptr && pas::in_range(reinterpret_cast<TabItem*>(Obj)->BonusKind, ab_Global::abkRegeneration, ab_Global::abkRegeneration)) {
                ab_Global::ComputeSphericalDistance(Distance, Origin->Longitude, Origin->PolarAngle, 0.0, Obj->State.LongitudeDegrees, Obj->State.PolarAngleDegrees, ab_Global::SphereRadius);
                if (Distance < BestDistance) {
                    Route = ab_Zone::ab_Zone_FindReachableRouteZone(reinterpret_cast<TabItem*>(Obj)->SpawnZone);
                    if (Route != nullptr) {
                        Zone = Route;
                        BestDistance = Distance;
                        Result = reinterpret_cast<TabItem*>(Obj);
                    }
                }
            }
            Obj = Obj->Next;
        }
        return Result;
    }

    void TabItem_Create(TabItem* Self) {
        ab_Object::TabObject_Create(Self);
        Self->BonusKind = -1;
        Self->HiddenBonus = false;
        Self->WallCollisionEnabled = false;
        Self->MaxSpeed = 0.0;
        Self->SpeedScale = 0.0;
        Self->Mass = 1.0E+1;
        Self->State.PolarAngleDegrees = 0.0;
        Self->State.BearingDegrees = 0.0;
        Self->CollisionRadius = 0.0;
        Self->Collidable = false;
    }

    void TabItem_Destroy(TabItem* Self) {
        ab_ShipAI::TabShipAI* Ship{};
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                Ship = pas::checked_cast<ab_ShipAI::TabShipAI*>(Obj);
                if (Ship->TargetBonus == Self) {
                    Ship->TargetBonus = nullptr;
                }
            }
            Obj = Obj->Next;
        }
        if (Self->Item != nullptr) {
            pas::free(Self->Item);
            Self->Item = nullptr;
        }
        if (Self->Visual != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Visual));
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabItem::SetItem(aItem::TItem* Value) {
        BonusKind = -1;
        Item = Value;
    }

    void TabItem::SetBonus(std::int32_t Kind, std::uint8_t Hidden, ab_Zone::PabZone Zone) {
        BonusKind = Kind;
        HiddenBonus = Hidden;
        SpawnZone = Zone;
        if (HiddenBonus) {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Container"_wref.get(), u"ItemAB.Unknown"_wref.get(), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> visual = pas::Var<SE_Space::TObjectSE*>(&Visual);
            SE_Space::RetainSpaceObject(visual, createSpaceObjectByName);
        } else {
            SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Container"_wref.get(), static_cast<pas::WideString>(pas::concat_ansi({"ItemAB.", SysUtils::IntToStr(Kind)})), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> visual_2 = pas::Var<SE_Space::TObjectSE*>(&Visual);
            SE_Space::RetainSpaceObject(visual_2, createSpaceObjectByName_2);
        }
    }

    void TabItem::AttachVisual() {
        if (Item != nullptr) {
            {
                SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                SE_Space::TSpaceSE* space = ab_Global::ArcadeSpaceProcess->Space;
                graphObject->AttachToSpace(space);
            }
            return;
        }
        if (Visual != nullptr) {
            Visual->AttachToSpace(ab_Global::ArcadeSpaceProcess->Space);
        }
    }

    void TabItem::DetachVisual() {
        if (Item != nullptr) {
            Item->GetGraphObject()->DetachFromSpace();
            return;
        }
        if (Visual != nullptr) {
            Visual->DetachFromSpace();
        }
    }

    void TabItem::UpdateState() {
        ab_Object::TabObject::UpdateState();
    }

    void TabItem::Advance() {
        ab_Object::TabObject::Advance();
    }

    void TabItem::UpdateVisuals() {
        EC_Struct::TVector3D Position{};
        ab_Object::TabObject::UpdateVisuals();
        Position = GetWorldPosition();
        Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Position));
        if (Item != nullptr) {
            {
                EC_Struct::TPointF makePointF = EC_Struct::MakePointF(Position.X, Position.Y);
                SE_Space::TObjectSE* graphObject = Item->GetGraphObject();
                graphObject->SetPosition(makePointF);
            }
            if (!ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                DetachVisual();
            } else {
                AttachVisual();
                {
                    float itemFrontDepth = ab_Global::ItemFrontDepth;
                    SE_Space::TObjectSE* graphObject_2 = Item->GetGraphObject();
                    graphObject_2->SetDepth(itemFrontDepth);
                }
            }
        } else if (Visual != nullptr) {
            Visual->SetPosition(EC_Struct::MakePointF(Position.X, Position.Y));
            if (!ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                DetachVisual();
            } else {
                AttachVisual();
                Visual->SetDepth(ab_Global::ItemFrontDepth);
            }
        }
    }

    void TabItem::p_destroy() {
        ab_Item::TabItem_Destroy(this);
    }

} // namespace ab_Item
