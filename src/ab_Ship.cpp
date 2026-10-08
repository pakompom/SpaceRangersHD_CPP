#include "layout/ab_Ship.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aConst.hpp"
#include "types/aItem.hpp"
#include "types/aShip.hpp"
#include "types/abWall.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_ShipAI.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Hit.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_Space.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_W.hpp"
#include "units/ab_Zone.hpp"

namespace ab_Ship {
    ab_Ship::TabShip* PlayerArcadeShip = nullptr;

    ab_Ship::TabShip* KellerArcadeShip = nullptr;

    // Keeps Keller alive while present; exact visual role under review.
    ab_Ship::TabShip* KellerAuxiliaryShip = nullptr;

    std::uint8_t ArcadePaused{};

    std::uint8_t ArcadePauseWithShift{};

    void ab_Ship_RepelOverlaps() {
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject* Other{};
        double Bearing{};
        double Distance{};
        double Speed{};
        if ((ab_Global::ArcadeTickCount & 1) == 0) {
            Obj = ab_Object::FirstArcadeObject;
            while (Obj != nullptr) {
                if (ab_Hit::TabHit* abHit = pas::class_cast_if<ab_Hit::TabHit*>(Obj); abHit != nullptr &&
                    abHit->Health > 0) {
                    if (Obj->Active) {
                        if (!(pas::class_cast_if<abWall::TabWall*>(Obj) != nullptr)) {
                            Other = ab_Object::FirstArcadeObject;
                            while (Other != nullptr) {
                                if (Obj != Other && pas::class_cast_if<ab_Hit::TabHit*>(Other) != nullptr &&
                                    static_cast<ab_Hit::TabHit*>(Other)->Health > 0 && Other->Active &&
                                    (!(pas::class_cast_if<abWall::TabWall*>(Other) != nullptr) || Other->ZoneRadius >
                                            1.0)) {
                                    ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing),
                                        pas::Var<double>(&Distance), Obj->State.LongitudeDegrees,
                                        Obj->State.PolarAngleDegrees, 0.0, Other->State.LongitudeDegrees,
                                        Other->State.PolarAngleDegrees, ab_Global::SphereRadius);
                                    if (static_cast<float>(static_cast<double>(Obj->ZoneRadius) + Other->ZoneRadius) > Distance &&
                                        Distance > 0.0) {
                                        Speed = pas::real_max<double>(1.0,
                                                static_cast<float>(System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Obj->Velocity.X)) +
                                                    pas::sqr(static_cast<double>(Obj->Velocity.Y)))) / 2.0));
                                        Bearing =
                                            aMyFunction::HeadingDegreesToRadians(
                                                aMyFunction::WrapHeadingDegrees(static_cast<float>(Bearing + 1.8E+2)));
                                        Obj->Velocity.X = static_cast<float>(System::Sin(Bearing) * Speed);
                                        Obj->Velocity.Y = static_cast<float>(-System::Cos(Bearing) * Speed);
                                        if (ab_ShipAI::TabShipAI* abShipAI = pas::class_cast_if<ab_ShipAI::TabShipAI*>(
                                            Obj)) {
                                            abShipAI->NoticeCollision();
                                        }
                                    }
                                }
                                Other = Other->Next;
                            }
                        }
                    }
                }
                Obj = Obj->Next;
            }
        }
    }

    void TabShip_Create(TabShip* Self) {
        ab_Hit::TabHit_Create(Self);
        Self->TurnSpeedScale = 1.0;
        Self->DisruptUntilTick = 0;
        Self->Health = 200;
        Self->MaxHealth = 200;
        Self->TurnSpeed = 2.0;
        Self->WallCollisionEnabled = true;
        Self->GravityEnabled = true;
        Self->ZoneDamageEnabled = true;
        Self->Enemies = pas::make_object<pas::List>();
        Self->InitialEnemies = pas::make_object<pas::List>();
        Self->TrackedShips = pas::make_object<pas::List>();
        Self->TickCounter = 0;
        Self->ConvertedFromGameShip = false;
        Self->SpawnGraphKey = pas::WideString();
        Self->ScriptLabel = pas::WideString();
        Self->RewardObject = nullptr;
        Self->RandomRewardsDisabled = false;
    }

    void TabShip_Destroy(TabShip* Self) {
        TabShip* Ship{};
        std::int32_t Index{};
        std::int32_t Count{};
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (TabShip* abShip = pas::class_cast_if<TabShip*>(Obj)) {
                Ship = abShip;
                if (Ship->Enemies != nullptr) {
                    while (true) {
                        Index = pas::list_indexof(Ship->Enemies, reinterpret_cast<void*>(Self));
                        if (Index < 0) {
                            break;
                        }
                        pas::list_delete(Ship->Enemies, Index);
                        Index = pas::list_indexof(Ship->InitialEnemies, reinterpret_cast<void*>(Self));
                        if (Index >= 0) {
                            pas::list_put(Ship->InitialEnemies, Index, nullptr);
                        }
                    }
                    while (true) {
                        Index = pas::list_indexof(Ship->TrackedShips, reinterpret_cast<void*>(Self));
                        if (Index < 0) {
                            break;
                        }
                        pas::list_put(Ship->TrackedShips, Index, nullptr);
                    }
                }
                if (ab_ShipAI::TabShipAI* abShipAI = pas::class_cast_if<ab_ShipAI::TabShipAI*>(Ship); abShipAI !=
                    nullptr && abShipAI->TargetShip == Self) {
                    abShipAI->TargetShip = nullptr;
                }
            }
            Obj = Obj->Next;
        }
        if (Self == PlayerArcadeShip) {
            PlayerArcadeShip = nullptr;
        }
        if (Self == KellerArcadeShip) {
            KellerArcadeShip = nullptr;
        }
        if (Self == KellerAuxiliaryShip) {
            KellerAuxiliaryShip = nullptr;
        }
        if (PlayerArcadeShip != nullptr) {
            Index = 0;
            Count = pas::list_count(PlayerArcadeShip->Enemies);
            while (Index < Count) {
                if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(pas::list_at<pas::Object>(PlayerArcadeShip->Enemies,
                    Index)) != nullptr) {
                    break;
                }
                ++Index;
            }
            if (Index >= Count) {
                PlayerArcadeShip->StopThrust();
                PlayerArcadeShip->SetTurnInput(0.0);
                ab_Global::ArcadeAutopilotEnabled = false;
                Globals::ArcadeBattleScreen->UpdateAutopilotButtons();
                ab_Global::ArcadeEnemiesDefeated = true;
                Obj = ab_Object::FirstArcadeObject;
                while (Obj != nullptr) {
                    if (abWall::TabWall* abWall_2 = pas::class_cast_if<abWall::TabWall*>(Obj); abWall_2 != nullptr &&
                        abWall_2->Health > 0) {
                        abWall_2->Health = std::min<std::int32_t>(20, abWall_2->Health);
                    }
                    Obj = Obj->Next;
                }
                if (static_cast<std::uint8_t>(ab_Space::ArcadeKellerEncounter ^ 1) || KellerArcadeShip == nullptr) {
                    Globals::ArcadeBattleScreen->ShowVictory();
                }
            }
        }
        if (Self->Visual != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Visual));
        }
        if (Self->OffscreenMarker != nullptr) {
            pas::free(Self->OffscreenMarker);
            Self->OffscreenMarker = nullptr;
        }
        if (Self->OffscreenLabel != nullptr) {
            pas::free(Self->OffscreenLabel);
            Self->OffscreenLabel = nullptr;
        }
        if (Self->Enemies != nullptr) {
            pas::free(Self->Enemies);
            Self->Enemies = nullptr;
        }
        if (Self->InitialEnemies != nullptr) {
            pas::free(Self->InitialEnemies);
            Self->InitialEnemies = nullptr;
        }
        if (Self->TrackedShips != nullptr) {
            pas::free(Self->TrackedShips);
            Self->TrackedShips = nullptr;
        }
        if (Self->RewardObject != nullptr) {
            pas::free(Self->RewardObject);
        }
        Self->RewardObject = nullptr;
        ab_Hit::TabHit_Destroy(Self);
    }

    void TabShip::CreateShipVisual(const pas::WideString& GraphKey, std::int32_t Diameter) {
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, GraphKey,
                ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> visual = pas::Var<SE_Space::TObjectSE*>(&Visual);
            SE_Space::RetainSpaceObject(visual, createSpaceObjectByName);
        }
        pas::checked_cast<SE_Ship2::TShip2SE*>(Visual)->TailEmitIntervalMs = 10u;
        Visual->SetAlpha(255);
        if (this == PlayerArcadeShip && GlobalsV::ShipTail != 0 && ab_Global::ArcadeSpaceProcess->Space->AlphaShift ==
                0) {
            pas::checked_cast<SE_Ship2::TShip2SE*>(Visual)->SetTailMode(1);
        } else {
            pas::checked_cast<SE_Ship2::TShip2SE*>(Visual)->SetTailMode(0);
        }
        VisualDiameter = Diameter;
        EffectOriginSpread = GR_Main::GiScalePixels(VisualDiameter);
        Mass = 1.0E+1;
        State.PolarAngleDegrees = 0.0;
        State.BearingDegrees = 0.0;
        CollisionRadius = static_cast<float>(static_cast<float>(Diameter / 2.0) * 0x1.199999999999bp+0);
        ZoneRadius = static_cast<float>(0x1.ccccccccccccbp-1 * static_cast<float>(CollisionRadius));
        HasFiredWeapon = false;
    }

    void TabShip::CreateRuinsVisual(const pas::WideString& GraphKey, std::int32_t Diameter) {
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ruins"sv, GraphKey,
                ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> visual = pas::Var<SE_Space::TObjectSE*>(&Visual);
            SE_Space::RetainSpaceObject(visual, createSpaceObjectByName);
        }
        reinterpret_cast<SE_Ruins::TRuinsSE*>(Visual)->KeepSize = true;
        VisualDiameter = Diameter;
        EffectOriginSpread = GR_Main::GiScalePixels(VisualDiameter);
        Mass = 1.0E+1;
        State.PolarAngleDegrees = 0.0;
        State.BearingDegrees = 0.0;
        CollisionRadius = static_cast<float>(static_cast<float>(Diameter / 2.0) * 0x1.199999999999bp+0);
        ZoneRadius = static_cast<float>(0x1.ccccccccccccbp-1 * static_cast<float>(CollisionRadius));
    }

    void TabShip::AttachVisual() {
        if (Visual != nullptr) {
            Visual->AttachToSpace(ab_Global::ArcadeSpaceProcess->Space);
        }
    }

    void TabShip::DetachVisual() {
        if (Visual != nullptr) {
            Visual->DetachFromSpace();
        }
    }

    void TabShip::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        std::int32_t Index{};
        if (Visual != nullptr) {
            Visual->QueueImageLoad(PendingLoads, Owner);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
            ab_W::ab_Weapon_QueueImageLoad(&Weapons[Index], PendingLoads, Owner);
        }
    }

    TabShip* TabShip::FindNearestEnemy(ab_Object::TabObject* Origin) {
        std::int32_t Index{};
        TabShip* Ship{};
        double Distance{};
        TabShip* Result = nullptr;
        double BestDistance = 1.0E+20;
        {
            const std::int32_t cpp_last = pas::list_count(Enemies) - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    Ship = pas::list_at<TabShip>(Enemies, Index);
                    if (Ship->Health >= 1 && (Ship->BonusTicks[ab_Global::abkInvisibility] <= 0 || Ship->RevealTicks >
                                0)) {
                        Distance = Origin->DistanceTo(Ship);
                        if (Distance < BestDistance) {
                            BestDistance = Distance;
                            Result = Ship;
                        }
                    }
                }
            }
        }
        return Result;
    }

    TabShip* TabShip::FindNearestEnemyWithBearing(ab_Object::TabObject* Origin,
        ab_Global::TSphericalBearingDistance& Bearing) {
        std::int32_t Index{};
        TabShip* Ship{};
        ab_Global::TSphericalBearingDistance CandidateBearing{};
        TabShip* Result = nullptr;
        double BestDistance = 1.0E+20;
        {
            const std::int32_t cpp_last = pas::list_count(Enemies) - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    Ship = pas::list_at<TabShip>(Enemies, Index);
                    if (Ship->Health >= 1 && (Ship->BonusTicks[ab_Global::abkInvisibility] <= 0 || Ship->RevealTicks >
                                0)) {
                        CandidateBearing = Origin->BearingAndDistanceTo(Ship);
                        if (CandidateBearing.Distance < BestDistance) {
                            BestDistance = CandidateBearing.Distance;
                            Bearing = CandidateBearing;
                            Result = Ship;
                        }
                    }
                }
            }
        }
        return Result;
    }

    void TabShip::AddEnemy(TabShip* Ship) {
        pas::list_add(Enemies, reinterpret_cast<void*>(Ship));
        pas::list_add(InitialEnemies, reinterpret_cast<void*>(Ship));
    }

    void TabShip::AddTrackedShip(TabShip* Ship) {
        pas::list_add(TrackedShips, reinterpret_cast<void*>(Ship));
    }

    void TabShip::ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) {
        if (Health > 0) {
            if (BonusTicks[ab_Global::abkShield] > 0) {
                ab_Hit::TabHit::ApplyDamage(
                    pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(Amount) * ab_Global::ShieldDamageScale) * DamageTakenScale)),
                    Source, Disrupt);
            } else {
                ab_Hit::TabHit::ApplyDamage(pas::real_round(static_cast<float>(static_cast<double>(Amount) * DamageTakenScale)), Source,
                    Disrupt);
            }
        }
    }

    void TabShip::Brake() {
        ChangeSpeed(-1.0);
    }

    void TabShip::FirePrimary() {
        std::int32_t Score{};
        std::int32_t BestScore{};
        std::int32_t Index{};
        std::int32_t Candidate{};
        std::int32_t BestWeapon{};
        float DamageScale{};
        if (Health > 0 && WeaponCount > 0 && BonusTicks[ab_Global::abkWeaponLock] <= 0) {
            if (this != PlayerArcadeShip || LastPrimaryWeapon == PrimaryWeapon || ab_Global::WeaponSwitchDelayMs /
                    ab_Global::ArcadeTickMs <= ab_Global::ArcadeTickCount - LastPrimaryFireTick) {
                if (CanFireWeapon(PrimaryWeapon)) {
                    if (BonusTicks[ab_Global::abkInvisibility] > 0) {
                        RevealTicks = ab_Global::RevealAfterFiringMs / ab_Global::ArcadeTickMs;
                    }
                    LastPrimaryWeapon = PrimaryWeapon;
                    LastPrimaryFireTick = ab_Global::ArcadeTickCount;
                    Weapons[PrimaryWeapon].LastFireTick = ab_Global::ArcadeTickCount;
                    if (!(aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->AmmoModEnabled == 1 && this ==
                            PlayerArcadeShip)) {
                        Weapons[PrimaryWeapon].Ammo -= Weapons[PrimaryWeapon].AmmoCost;
                    }
                    if (BonusTicks[ab_Global::abkDamage] > 0) {
                        DamageScale = ab_Global::WeaponDamageBonusScale;
                    } else {
                        DamageScale = 1.0f;
                    }
                    DamageScale = static_cast<float>(static_cast<double>(DamageScale) * WeaponDamageScale);
                    ab_W::ab_Weapon_Fire(&Weapons[PrimaryWeapon], this, DamageScale);
                    if (Weapons[PrimaryWeapon].Ammo < Weapons[PrimaryWeapon].AmmoCost) {
                        BestScore = -1;
                        BestWeapon = -1;
                        Candidate = PrimaryWeapon;
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
                            if ((Weapons[Candidate].SlotData & aItem::EquipmentSecondaryFireFlag) == 0) {
                                Score = pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(Weapons[Candidate].Ammo) /
                                        Weapons[Candidate].MaxAmmo) * 1.0E+2));
                                if (CanFireWeapon(Candidate)) {
                                    Score += 100;
                                }
                                if (Score > BestScore) {
                                    BestScore = Score;
                                    BestWeapon = Candidate;
                                }
                            }
                            ++Candidate;
                            if (Candidate >= WeaponCount) {
                                Candidate = 0;
                            }
                        }
                        if (BestWeapon >= 0) {
                            PrimaryWeapon = BestWeapon;
                        }
                    }
                    HasFiredWeapon = true;
                }
            }
        }
    }

    void TabShip::FireSecondary() {
        std::int32_t Score{};
        std::int32_t BestScore{};
        std::int32_t Index{};
        std::int32_t Candidate{};
        std::int32_t BestWeapon{};
        float DamageScale{};
        if (Health > 0 && WeaponCount > 0 && BonusTicks[ab_Global::abkWeaponLock] <= 0) {
            if (this != PlayerArcadeShip || LastSecondaryWeapon == SecondaryWeapon || ab_Global::WeaponSwitchDelayMs /
                    ab_Global::ArcadeTickMs <= ab_Global::ArcadeTickCount - LastSecondaryFireTick) {
                if (CanFireWeapon(SecondaryWeapon)) {
                    if (BonusTicks[ab_Global::abkInvisibility] > 0) {
                        RevealTicks = ab_Global::RevealAfterFiringMs / ab_Global::ArcadeTickMs;
                    }
                    LastSecondaryWeapon = SecondaryWeapon;
                    LastSecondaryFireTick = ab_Global::ArcadeTickCount;
                    Weapons[SecondaryWeapon].LastFireTick = ab_Global::ArcadeTickCount;
                    if (!(aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->AmmoModEnabled == 1 && this ==
                            PlayerArcadeShip)) {
                        Weapons[SecondaryWeapon].Ammo -= Weapons[SecondaryWeapon].AmmoCost;
                    }
                    if (BonusTicks[ab_Global::abkDamage] > 0) {
                        DamageScale = ab_Global::WeaponDamageBonusScale;
                    } else {
                        DamageScale = 1.0f;
                    }
                    DamageScale = static_cast<float>(static_cast<double>(DamageScale) * WeaponDamageScale);
                    ab_W::ab_Weapon_Fire(&Weapons[SecondaryWeapon], this, DamageScale);
                    if (this == PlayerArcadeShip && Weapons[SecondaryWeapon].Ammo < Weapons[SecondaryWeapon].AmmoCost) {
                        BestScore = -1;
                        BestWeapon = -1;
                        Candidate = SecondaryWeapon;
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
                            if ((Weapons[Candidate].SlotData & aItem::EquipmentSecondaryFireFlag) != 0) {
                                Score = pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(Weapons[Candidate].Ammo) /
                                        Weapons[Candidate].MaxAmmo) * 1.0E+2));
                                if (CanFireWeapon(Candidate)) {
                                    Score += 100;
                                }
                                if (Score > BestScore) {
                                    BestScore = Score;
                                    BestWeapon = Candidate;
                                }
                            }
                            ++Candidate;
                            if (Candidate >= WeaponCount) {
                                Candidate = 0;
                            }
                        }
                        if (BestWeapon >= 0) {
                            SecondaryWeapon = BestWeapon;
                        }
                    }
                    HasFiredWeapon = true;
                }
            }
        }
    }

    void TabShip::FirePrimaryAt(ab_Object::TabObject* Target) {
        double Distance{};
        if (Health > 0 && BonusTicks[ab_Global::abkWeaponLock] <= 0) {
            Distance = DistanceTo(Target);
            if (Distance <= Weapons[PrimaryWeapon].Range) {
                FirePrimary();
            }
        }
    }

    void TabShip::FireSecondaryAt(ab_Object::TabObject* Target) {
        double Distance{};
        if (Health > 0 && BonusTicks[ab_Global::abkWeaponLock] <= 0) {
            Distance = DistanceTo(Target);
            if (Distance <= Weapons[SecondaryWeapon].Range) {
                FireSecondary();
            }
        }
    }

    void TabShip::SelectWeapon(std::int32_t Index) {
        if (this == PlayerArcadeShip) {
            if (Index < 0) {
                PrimaryWeapon = -1;
                SecondaryWeapon = -1;
            } else if (Index < WeaponCount) {
                if ((Weapons[Index].SlotData & aItem::EquipmentSecondaryFireFlag) == 0) {
                    PrimaryWeapon = Index;
                } else {
                    SecondaryWeapon = Index;
                }
            }
        } else if (PrimaryWeapon != Index) {
            PrimaryWeapon = Index;
        }
    }

    std::uint8_t TabShip::CanFireWeapon(std::int32_t Index) {
        std::uint8_t Result = false;
        if (BonusTicks[ab_Global::abkWeaponLock] <= 0 && Index >= 0 && Index < WeaponCount && Weapons[Index].AmmoCost <=
                Weapons[Index].Ammo) {
            return ab_Global::ArcadeTickCount - Weapons[Index].LastFireTick >= Weapons[Index].FireIntervalTicks;
        }
        return Result;
    }

    double TabShip::MinimumWeaponRange() {
        std::int32_t Index{};
        double Result = 1.0E+20;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
            Result = pas::real_min_scalar<double>(Result, Weapons[Index].Range);
        }
        return Result;
    }

    double TabShip::MaximumWeaponRange() {
        std::int32_t Index{};
        double Result = 0.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
            Result = pas::real_max_scalar<double>(Result, Weapons[Index].Range);
        }
        return Result;
    }

    void TabShip::AddWeapon(std::int32_t Kind) {
        if (WeaponCount < 5) {
            ab_W::ab_Weapon_Initialize(&Weapons[WeaponCount], Kind + 50);
            ++WeaponCount;
        }
    }

    void TabShip::UpdateState() {
        double Turn{};
        std::int32_t Index{};
        if (BonusTicks[ab_Global::abkRecharge] > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range.next(Index); ) {
                Weapons[Index].Ammo = std::min<std::int64_t>(
                        Weapons[Index].Ammo + pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(Weapons[Index].RechargePerTick) *
                            AmmoRechargeScale) * ab_Global::AmmoRechargeBonusScale)),
                        static_cast<std::int64_t>(Weapons[Index].MaxAmmo));
            }
        } else {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, WeaponCount - 1); cpp_range_2.next(Index); ) {
                Weapons[Index].Ammo = std::min<std::int64_t>(
                        Weapons[Index].Ammo + pas::real_round(static_cast<float>(static_cast<double>(Weapons[Index].RechargePerTick) *
                            AmmoRechargeScale)), static_cast<std::int64_t>(Weapons[Index].MaxAmmo));
            }
        }
        if (TurnInput != 0.0) {
            Turn = TurnInput;
            if (static_cast<float>(static_cast<double>(-TurnSpeed) * TurnSpeedScale) > Turn) {
                Turn = static_cast<float>(static_cast<double>(-TurnSpeed) * TurnSpeedScale);
            } else if (static_cast<float>(static_cast<double>(TurnSpeed) * TurnSpeedScale) < Turn) {
                Turn = static_cast<float>(static_cast<double>(TurnSpeed) * TurnSpeedScale);
            }
            State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(State.BearingDegrees) + Turn));
        }
        ab_Hit::TabHit::UpdateState();
        SpeedScale = static_cast<float>(static_cast<double>(SpeedScale) * MovementScale);
        TurnSpeedScale = static_cast<float>(static_cast<double>(TurnSpeedScale) * MovementScale);
        if (BonusTicks[ab_Global::abkSpeed] > 0) {
            SpeedScale = static_cast<float>(static_cast<double>(SpeedScale) * ab_Global::SpeedBonusScale);
        }
        if (BonusTicks[ab_Global::abkSlow] > 0) {
            SpeedScale = static_cast<float>(static_cast<double>(SpeedScale) * ab_Global::SpeedPenaltyScale);
        }
        if (BonusTicks[ab_Global::abkSpeed] > 0) {
            TurnSpeedScale = static_cast<float>(static_cast<double>(TurnSpeedScale) * ab_Global::SpeedBonusScale);
        }
        if (BonusTicks[ab_Global::abkSlow] > 0) {
            TurnSpeedScale = static_cast<float>(static_cast<double>(TurnSpeedScale) * ab_Global::SpeedPenaltyScale);
        }
    }

    void TabShip::Advance() {
        std::int32_t Index{};
        ab_Zone::PabZone Zone{};
        ab_Hit::TabHit::Advance();
        if (Health == 0) {
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            Thrust = 0.0;
        } else {
            for (Index = 0; Index <= 7; ++Index) {
                if (BonusTicks[Index] > 0) {
                    --BonusTicks[Index];
                }
            }
            if (BonusTicks[ab_Global::abkRegeneration] > 0) {
                Health = std::min<std::int32_t>(MaxHealth, Health + ab_Global::RegenerationHealthPerTick);
            }
            if (0.01 <= RegenerationRate) {
                if (RegenerationRate < 1.0E+1) {
                    if (pas::imod(TickCounter, pas::real_round(static_cast<float>(1.0E+1 / RegenerationRate))) == 0) {
                        Health = std::min<std::int32_t>(MaxHealth, Health + ab_Global::RegenerationHealthPerTick);
                    }
                }
                if (RegenerationRate >= 1.0E+1) {
                    Health = std::min<std::int64_t>(static_cast<std::int64_t>(MaxHealth),
                            Health + pas::real_round(static_cast<float>(static_cast<float>(static_cast<double>(ab_Global::RegenerationHealthPerTick) *
                                RegenerationRate) / 1.0E+1)));
                }
            }
            if (this == PlayerArcadeShip && TickCounter % 10 == 0) {
                if (aPlayer::GetPlayer() != nullptr) {
                    if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactDroid) > 0) {
                        Health = std::min<std::int32_t>(MaxHealth,
                                Health + ab_Global::RegenerationHealthPerTick *
                                    aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactDroid));
                    }
                }
            }
            if (BonusTicks[ab_Global::abkInvisibility] > 0 && RevealTicks > 0) {
                --RevealTicks;
            }
        }
        UpdateObstacleSensors();
        if ((ab_Global::ArcadeTickCount & 0x00000040) == 0) {
            if (ab_Zone::ab_Zone_IsInsideKind10(State.LongitudeDegrees, State.PolarAngleDegrees)) {
                if (WallCollisionEnabled) {
                    Zone = ab_Zone::ab_Zone_FindNearestOutside(State.LongitudeDegrees, State.PolarAngleDegrees);
                    if (Zone != nullptr) {
                        State.LongitudeDegrees = Zone->Longitude;
                        State.PolarAngleDegrees = Zone->PolarAngle;
                    }
                }
            }
        }
        TickCounter = (TickCounter + 1) % 10000;
    }

    void TabShip::UpdateVisuals() {
        Types::TPoint ViewSize{};
        double HorizonAlpha{};
        std::int32_t Alpha{};
        float InvisibilityAlpha{};
        EC_Struct::TVector3D Position{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Hit::TabHit::UpdateVisuals();
        Position = GetWorldPosition();
        Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix,
                pas::ConstRef<EC_Struct::TVector3D>(&Position));
        if (Visual != nullptr && Visual->IsAttachedToSpace()) {
            if (Position.Z <= ab_Global::SphereHorizonDepth && Position.Z > ab_Global::SphereFarHorizonDepth) {
                HorizonAlpha = static_cast<float>((static_cast<float>(static_cast<double>(ab_Global::SphereHorizonDepth) - Position.Z)) /
                        (static_cast<float>(static_cast<double>(ab_Global::SphereHorizonDepth) - ab_Global::SphereFarHorizonDepth)));
            } else if (Position.Z > ab_Global::SphereHorizonDepth && Position.Z < ab_Global::SphereNearHorizonDepth) {
                HorizonAlpha = static_cast<float>((static_cast<float>(static_cast<double>(ab_Global::SphereHorizonDepth) - Position.Z)) /
                        (static_cast<float>(static_cast<double>(ab_Global::SphereHorizonDepth) - ab_Global::SphereNearHorizonDepth)));
            } else {
                HorizonAlpha = 1.0;
            }
            Visual->SetPosition(EC_Struct::MakePointF(Position.X, Position.Y));
            if (Health > 0) {
                if (BonusTicks[ab_Global::abkInvisibility] > 0 && RevealTicks <= 0) {
                    if (this != PlayerArcadeShip) {
                        InvisibilityAlpha = ab_Global::OtherInvisibleAlpha;
                    } else {
                        InvisibilityAlpha = ab_Global::PlayerInvisibleAlpha;
                    }
                } else {
                    InvisibilityAlpha = 1.0f;
                }
                if (!ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                    Alpha = pas::real_round(static_cast<float>(static_cast<float>(128.0 * InvisibilityAlpha) * HorizonAlpha));
                    if (Visual->Size.X != EffectOriginSpread / 2 && (this != KellerArcadeShip || aGalaxy::Galaxy ==
                                nullptr || aGalaxy::Galaxy->KellerLeaveTurn == 0)) {
                        Visual->DetachFromSpace();
                        Visual->SetSize(ClassesImports::Point(EffectOriginSpread / 2, EffectOriginSpread / 2));
                        Visual->AttachToSpace(ab_Global::ArcadeSpaceProcess->Space);
                    }
                    Visual->SetDepth(ab_Global::ShipBackDepth);
                    if (SE_Ship2::TShip2SE* ship2SE = pas::class_cast_if<SE_Ship2::TShip2SE*>(Visual)) {
                        ship2SE->SetTailDepth(ab_Global::ShipTailBackDepth);
                    }
                } else {
                    Alpha = pas::real_round(static_cast<float>(static_cast<float>(255.0 * InvisibilityAlpha) * HorizonAlpha));
                    if (Visual->Size.X != EffectOriginSpread) {
                        Visual->DetachFromSpace();
                        Visual->SetSize(ClassesImports::Point(EffectOriginSpread, EffectOriginSpread));
                        Visual->AttachToSpace(ab_Global::ArcadeSpaceProcess->Space);
                    }
                    Visual->SetDepth(ab_Global::ShipFrontDepth);
                    if (SE_Ship2::TShip2SE* ship2SE_2 = pas::class_cast_if<SE_Ship2::TShip2SE*>(Visual)) {
                        ship2SE_2->SetTailDepth(ab_Global::ShipTailFrontDepth);
                    }
                }
            } else if (this != KellerArcadeShip) {
                Alpha = std::max<std::int32_t>(0, Visual->GetAlpha() - 10);
                if (Visual->GetAlpha() < 10) {
                    Visual->DetachFromSpace();
                }
            } else {
                Alpha = Visual->GetAlpha();
            }
            if (pas::in_set<1, 1, 3, 3>(ab_Global::ArcadeViewMode)) {
                Alpha = pas::real_round(aMyFunction::RemapClamped(ab_Global::SphereCameraDistance,
                        static_cast<float>(static_cast<double>(ab_Global::SphereRadius) + ab_Global::SphereNearCameraOffset),
                        static_cast<float>((static_cast<float>(static_cast<double>(ab_Global::SphereRadius) + ab_Global::SphereFarCameraOffset)) / 3.0), Alpha,
                        0.0));
            }
            Visual->SetAlpha(Alpha);
        }
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Visual) != nullptr) {
            {
                std::uint8_t headingDegreesToByte = aMyFunction::HeadingDegreesToByte(GetProjectedHeading(Position));
                SE_Space::TObjectSE* visual = Visual;
                visual->SetAngle(headingDegreesToByte);
            }
            {
                float cpp_arg = static_cast<float>(static_cast<float>(aMyFunction::RandomIntRange(0, 1) * 0x1.3333333333334p-2) + 2.0);
                SE_Ship2::TShip2SE* cpp_arg_2 = pas::checked_cast<SE_Ship2::TShip2SE*>(Visual);
                cpp_arg_2->OffsetTailsAlongHeading(cpp_arg);
            }
            pas::checked_cast<SE_Ship2::TShip2SE*>(Visual)->SetTailsEmitting(Thrust != 0.0);
            if (Visual->GetAngle() >= 254 || Visual->GetAngle() <= 2) {
                Visual->SetAngle(0);
            }
        }
        if (PlayerArcadeShip != nullptr) {
            if (OffscreenMarker != nullptr) {
                Position = PlayerArcadeShip->GetWorldPosition();
                Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix,
                        pas::ConstRef<EC_Struct::TVector3D>(&Position));
                Bearing = PlayerArcadeShip->BearingAndDistanceTo(this);
                Bearing.BearingDeltaDegrees =
                    aMyFunction::WrapHeadingDegrees(
                        static_cast<float>(static_cast<double>(PlayerArcadeShip->GetProjectedHeading(Position)) +
                            Bearing.BearingDeltaDegrees));
                if (Bearing.BearingDeltaDegrees > 1.8E+2) {
                    Bearing.BearingDeltaDegrees = static_cast<float>(Bearing.BearingDeltaDegrees - 3.6E+2);
                }
                ViewSize = Globals::ArcadeBattleScreen->WorldPanel->ClientSize;
                if (Bearing.BearingDeltaDegrees >= 0.0 && Bearing.BearingDeltaDegrees <= 45.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(
                        static_cast<float>((static_cast<float>(static_cast<float>(ViewSize.X / 2.0) - 3.0E+1)) * (static_cast<float>(Bearing.BearingDeltaDegrees / 45.0))), -(ViewSize.Y / 2) + 30));
                } else if (Bearing.BearingDeltaDegrees >= -45.0 && Bearing.BearingDeltaDegrees < 0.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(
                        static_cast<float>((static_cast<float>(static_cast<float>(ViewSize.X / 2.0) - 3.0E+1)) * (static_cast<float>(Bearing.BearingDeltaDegrees / 45.0))), -(ViewSize.Y / 2) + 30));
                } else if (Bearing.BearingDeltaDegrees >= 135.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(
                        static_cast<float>(static_cast<float>((static_cast<float>(1.8E+2 - Bearing.BearingDeltaDegrees)) / 45.0) * (static_cast<float>(static_cast<float>(ViewSize.X / 2.0) - 3.0E+1))),
                        ViewSize.Y / 2 - 30));
                } else if (Bearing.BearingDeltaDegrees <= -135.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(
                        static_cast<float>(static_cast<float>(-(static_cast<float>(1.8E+2 + Bearing.BearingDeltaDegrees)) / 45.0) * (static_cast<float>(static_cast<float>(ViewSize.X / 2.0) - 3.0E+1))),
                        ViewSize.Y / 2 - 30));
                } else if (Bearing.BearingDeltaDegrees >= 45.0 && Bearing.BearingDeltaDegrees <= 9.0E+1) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(ViewSize.X / 2 - 30,
                        static_cast<float>((static_cast<float>(static_cast<float>(ViewSize.Y / 2.0) - 3.0E+1)) * (static_cast<float>((static_cast<float>(Bearing.BearingDeltaDegrees - 9.0E+1)) / 45.0)))));
                } else if (Bearing.BearingDeltaDegrees >= 9.0E+1 && Bearing.BearingDeltaDegrees <= 135.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(ViewSize.X / 2 - 30,
                        static_cast<float>((static_cast<float>(static_cast<float>(ViewSize.Y / 2.0) - 3.0E+1)) * (static_cast<float>((static_cast<float>(Bearing.BearingDeltaDegrees - 9.0E+1)) / 45.0)))));
                } else if (Bearing.BearingDeltaDegrees <= -45.0 && Bearing.BearingDeltaDegrees >= -9.0E+1) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(-(ViewSize.X / 2) + 30,
                        static_cast<float>(static_cast<float>((static_cast<float>(static_cast<float>(-Bearing.BearingDeltaDegrees - 45.0) + -45.0)) / 45.0) * (static_cast<float>(static_cast<float>(ViewSize.Y / 2.0) - 3.0E+1)))));
                } else if (Bearing.BearingDeltaDegrees <= -9.0E+1 && Bearing.BearingDeltaDegrees >= -135.0) {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(-(ViewSize.X / 2) + 30,
                        static_cast<float>((static_cast<float>(static_cast<float>(ViewSize.Y / 2.0) - 3.0E+1)) * (static_cast<float>((static_cast<float>(-Bearing.BearingDeltaDegrees - 9.0E+1)) / 45.0)))));
                } else {
                    OffscreenMarker->SetPosition(EC_Struct::MakePointF(1.0E+2f, 1.0E+2f));
                }
                OffscreenLabel->SetPosition(EC_Struct::TruncatePointF(OffscreenMarker->Position));
                Bearing = BearingAndDistanceTo(PlayerArcadeShip);
                if (aMyFunction::PointDistanceSquared(OffscreenMarker->Position, Visual->Position) < 0.001) {
                    OffscreenMarker->SetAngle(0);
                } else {
                    OffscreenMarker->SetAngle(
                        aMyFunction::HeadingDegreesToByte(
                            aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(aMyFunction::PointBearingDegrees(
                                OffscreenMarker->Position, Visual->Position)) - Bearing.BearingDeltaDegrees))));
                }
            }
        }
    }

    void TabShip::UpdateAvoidanceDistances() {
        if (TurnSpeed == 0.0) {
            OuterAvoidanceDistance = 3.0E+1;
            MiddleAvoidanceDistance = 3.0E+1;
            InnerAvoidanceDistance = 3.0E+1;
            return;
        }
        OuterAvoidanceDistance = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(1.8E+2 / (static_cast<float>(static_cast<double>(TurnSpeed) * TurnSpeedScale))) * (static_cast<float>(5.0 * MaxSpeed))) /
                SystemImports::Pi) * 2.0);
        MiddleAvoidanceDistance = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(1.8E+2 / (static_cast<float>(static_cast<double>(TurnSpeed) * TurnSpeedScale))) * (static_cast<float>(static_cast<float>(5.0 * MaxSpeed) / 4.0))) /
                SystemImports::Pi) * 2.0);
        InnerAvoidanceDistance = 6.0E+1;
    }

    void TabShip::UpdateObstacleSensors() {
        std::int32_t Index{};
        if (ab_Global::ArcadeTickCount >= NextObstacleScanTick) {
            NextObstacleScanTick = ab_Global::ArcadeTickCount + 4;
            UpdateAvoidanceDistances();
            ab_StopLine::ab_StopLine_GetDistances(
                ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees,
                    State.BearingDegrees), pas::Var<double>(pas::byte_offset(&ObstacleDistances, 0 * sizeof(double))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 4 * sizeof(double))));
            ab_StopLine::ab_StopLine_GetDistances(
                ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees,
                    aMyFunction::WrapHeadingDegrees(static_cast<float>(State.BearingDegrees + 45.0))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 1 * sizeof(double))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 5 * sizeof(double))));
            ab_StopLine::ab_StopLine_GetDistances(
                ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees,
                    aMyFunction::WrapHeadingDegrees(static_cast<float>(State.BearingDegrees + 9.0E+1))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 2 * sizeof(double))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 6 * sizeof(double))));
            ab_StopLine::ab_StopLine_GetDistances(
                ab_Global::MakeSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees,
                    aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<float>(State.BearingDegrees + 9.0E+1) + 45.0))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 3 * sizeof(double))),
                pas::Var<double>(pas::byte_offset(&ObstacleDistances, 7 * sizeof(double))));
            for (Index = 0; Index <= 7; ++Index) {
                if (pas::load_unaligned<double>(pas::byte_offset(&ObstacleDistances, Index * sizeof(double))) >
                    OuterAvoidanceDistance) {
                    ObstacleLevels[Index] = 0;
                } else if (pas::load_unaligned<double>(pas::byte_offset(&ObstacleDistances, Index * sizeof(double))) >
                    MiddleAvoidanceDistance) {
                    ObstacleLevels[Index] = 1;
                } else if (pas::load_unaligned<double>(pas::byte_offset(&ObstacleDistances, Index * sizeof(double))) >
                    InnerAvoidanceDistance) {
                    ObstacleLevels[Index] = 2;
                } else {
                    ObstacleLevels[Index] = 3;
                }
            }
        }
    }

    void TabShip::p_destroy() {
        ab_Ship::TabShip_Destroy(this);
    }

} // namespace ab_Ship
