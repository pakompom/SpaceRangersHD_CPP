#include "layout/ab_Hit.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"
#include "types/abWall.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_ShipAI.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Hit.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Ship.hpp"

namespace ab_Hit {
    // Native objects are TabShip instances.
    pas::Array<ab_Hit::TabHit*, 0, 3> KellerFragments{};

    // Advance and KellerBreakupComplete iterate the four fragment slots, indices 0..3.
    pas::Array<double, 0, 3> KellerFragmentDistances{};

    // Initialized by breakup; later use under review.
    pas::Array<double, 0, 3> KellerFragmentValuesAC{};

    std::int32_t KellerBreakupTicks{};

    std::uint8_t KellerSplitActive{};

    std::uint8_t KellerFinishRequested{};

    std::uint8_t KellerDeathPending{};

    void TabHit_Create(TabHit* Self) {
        ab_Object::TabObject_Create(Self);
        Self->DisruptUntilTick = 0;
        Self->Health = 200;
        Self->MaxHealth = 200;
        Self->Effects = pas::make_object<pas::List>();
        Self->StateCC = true;
    }

    void TabHit_Destroy(TabHit* Self) {
        std::int32_t Index{};
        pas::Object* Effect{};
        if (Self->Effects != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->Effects) - 1); cpp_range.next(Index); ) {
                Effect = pas::list_at<pas::Object>(Self->Effects, Index);
                pas::free(Effect);
            }
            pas::free(Self->Effects);
            Self->Effects = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabHit::ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) {
        std::int32_t Frame{};
        GI_MessageLoop::TObjectGI* Effect{};
        std::uint8_t Invulnerable{};
        GI_GAI::TgaiGI* Animation{};
        if (Health > 0) {
            Invulnerable = aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->GodModEnabled == 1 && ab_Ship::PlayerArcadeShip == this;
            if (this != KellerFragments[0] && this != KellerFragments[1] && this != KellerFragments[2] && this != KellerFragments[3]) {
                if (Source != nullptr) {
                    if (pas::class_cast_if<ab_Ship::TabShip*>(this) != nullptr && pas::class_cast_if<ab_Ship::TabShip*>(Source) != nullptr) {
                        if (pas::list_indexof(pas::checked_cast<ab_Ship::TabShip*>(Source)->Enemies, reinterpret_cast<void*>(this)) < 0) {
                            Amount = Amount / 4;
                        }
                    }
                }
            }
            if (ab_Ship::PlayerArcadeShip == this && aPlayer::GetPlayer() != nullptr) {
                if (ab_Global::ArcadeAutopilotEnabled) {
                    Amount = System::Round(Amount * 0.7L);
                }
                if (ab_Ship::KellerArcadeShip != nullptr) {
                    Amount = System::Round(static_cast<long double>(aMyFunction::RemapClamped(aPlayer::GetPlayer()->BlackHoleKillCount + aPlayer::GetPlayer()->HyperspaceKillCount, 5.0, 7.0E+1, 0.1, 1.0)) * Amount);
                }
                Amount = System::Round(static_cast<long double>(Amount) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[6]].ArcadeDamageTakenScale);
            }
            if (ab_Ship::KellerArcadeShip == this && (ab_Ship::PlayerArcadeShip == nullptr || ab_Ship::PlayerArcadeShip->Health <= 0)) {
                Amount = 0;
            }
            if (Disrupt) {
                if (ab_Global::ArcadeTickCount < DisruptUntilTick) {
                    DisruptUntilTick += Amount;
                    if (ab_Ship::PlayerArcadeShip == this && DisruptUntilTick - ab_Global::ArcadeTickCount > 350) {
                        DisruptUntilTick = ab_Global::ArcadeTickCount + 350;
                    }
                } else {
                    DisruptUntilTick = ab_Global::ArcadeTickCount + Amount;
                }
                if (!Invulnerable) {
                    Health = std::max<std::int32_t>(0, Health - 2);
                }
            } else if (!Invulnerable) {
                Health = std::max<std::int32_t>(0, Health - Amount);
            }
            if (ab_Ship::KellerAuxiliaryShip != nullptr && ab_Ship::KellerArcadeShip == this && Health <= 0) {
                Health = 100;
            }
            if (ab_Ship::KellerArcadeShip == this && Health == 0) {
                Animation = pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::checked_cast<ab_Ship::TabShip*>(this)->Visual)->Animation;
                Frame = Animation->SequenceFrame;
                {
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"[20,0-", SysUtils::IntToStr(Animation->GetMainImageFrameCount() - 1), "][20,0-", SysUtils::IntToStr(Animation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* animation = Animation;
                    animation->LoadFrameSequenceFromText(cpp_arg);
                }
                Animation->SetSequenceFrame(Frame);
                Animation->CycleCompleteCallback = pas::bind_method<&TabHit::KellerBreakupComplete>(this);
                StateCC = false;
            }
            if (Health > 0 || ab_Ship::KellerArcadeShip != this) {
                if (Health <= 0) {
                    if (ab_Global::IsDepthBeforeSphereHorizon(GetProjectedPosition().Z)) {
                        Effect = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Globals::ArcadeBattleScreen->WorldPanel);
                        pas::list_add(Effects, reinterpret_cast<void*>(Effect));
                        if (ab_Global::IsDepthBeforeSphereHorizon(GetProjectedPosition().Z)) {
                            if (ab_Ship::KellerArcadeShip == this || ab_Ship::KellerAuxiliaryShip == this) {
                                reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SetImagePath(u"Bm.Weapon.ExplM"_wref.get());
                            } else {
                                reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SetImagePath(static_cast<pas::WideString>(pas::concat_ansi({"Bm.Weapon.Expl", SysUtils::IntToStr(aMyFunction::RandomIntRange(0, 1))})));
                            }
                            Effect->SetDepth(ab_Global::ExplosionFrontDepth);
                        } else {
                            reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SetImagePath(u"Bm.AB.expl0_s"_wref.get());
                            Effect->SetDepth(ab_Global::ExplosionBackDepth);
                        }
                        reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SequenceIndex = 0;
                        Effect->UpdateAutoGeometry();
                        Effect->SetSize(reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->GetContentSize());
                        Effect->SetOrigin(EC_Struct::HalfPoint(Effect->ClientSize));
                        reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->CycleCompleteCallback = pas::bind_method<&TabHit::ExplosionComplete>(this);
                        reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->RestartPlayback();
                        Effect->SetActive(true);
                        if (pas::class_cast_if<abWall::TabWall*>(this) != nullptr) {
                            if (aMyFunction::RandomIntRange(0, 1) == 0) {
                                GR_Main::SoundManager->PlaySound(u"Sound.ab_Expl0"_wref.get());
                            } else {
                                GR_Main::SoundManager->PlaySound(u"Sound.ab_Expl1"_wref.get());
                            }
                        } else {
                            auto cpp_arg_2 = pas::borrow(Globals::ArcadeExplosionSounds[aMyFunction::RandomIntRange(0, Globals::ArcadeExplosionSounds.length() - 1)]);
                            GR_Sound::TSoundControl* soundManager = GR_Main::SoundManager;
                            soundManager->PlaySound(cpp_arg_2.get());
                        }
                    } else {
                        DeletionPending = true;
                    }
                } else if (Source != nullptr && pas::list_count(Effects) < 5 || pas::list_count(Effects) < 1) {
                    Effect = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Globals::ArcadeBattleScreen->WorldPanel);
                    pas::list_add(Effects, reinterpret_cast<void*>(Effect));
                    if (ab_Global::IsDepthBeforeSphereHorizon(GetProjectedPosition().Z)) {
                        reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SetImagePath(u"Bm.AB.hit00_f"_wref.get());
                        Effect->SetDepth(ab_Global::HitFrontDepth);
                    } else {
                        reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SetImagePath(u"Bm.AB.hit00_s"_wref.get());
                        Effect->SetDepth(ab_Global::HitBackDepth);
                    }
                    reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->SequenceIndex = 0;
                    Effect->UpdateAutoGeometry();
                    Effect->SetSize(reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->GetContentSize());
                    {
                        std::int32_t cpp_right_2 = aMyFunction::RandomIntRange(-EffectOriginSpread / 4, EffectOriginSpread / 4);
                        std::int32_t cpp_arg_3 = Effect->ClientSize.Y / 2 + cpp_right_2;
                        std::int32_t cpp_right = aMyFunction::RandomIntRange(-EffectOriginSpread / 4, EffectOriginSpread / 4);
                        std::int32_t cpp_arg_4 = Effect->ClientSize.X / 2 + cpp_right;
                        Effect->SetOrigin(ClassesImports::Point(cpp_arg_4, cpp_arg_3));
                    }
                    reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->CycleCompleteCallback = pas::bind_method<&TabHit::HitEffectComplete>(this);
                    reinterpret_cast<GI_GAI::TgaiGI*>(Effect)->RestartPlayback();
                    Effect->SetActive(true);
                    if (ab_Ship::PlayerArcadeShip == this) {
                        auto cpp_arg_5 = pas::borrow(Globals::ArcadeHitSounds[aMyFunction::RandomIntRange(0, Globals::ArcadeHitSounds.length() - 1)]);
                        GR_Sound::TSoundControl* soundManager_2 = GR_Main::SoundManager;
                        soundManager_2->PlaySound(cpp_arg_5.get());
                    }
                }
            }
        }
    }

    void TabHit::UpdateState() {
        if (ab_Global::ArcadeTickCount < DisruptUntilTick) {
            if (ab_Ship::PlayerArcadeShip == this) {
                SpeedScale = 0.7;
                TurnSpeedScale = 0.6;
            } else {
                SpeedScale = 0.5;
                TurnSpeedScale = 0.4;
            }
        } else {
            SpeedScale = 1.0;
            TurnSpeedScale = 1.0;
        }
        ab_Object::TabObject::UpdateState();
    }

    void TabHit::Advance() {
        std::int32_t Index{};
        double ArcDistance{};
        GI_GAI::TgaiGI* OutwardAnimation{};
        GI_GAI::TgaiGI* InwardAnimation{};
        GI_GAI::TgaiGI* DeathAnimation{};
        ab_ShipAI::TabShipAI* Ship{};
        ab_Global::TSphericalBearingState Source{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Object::TabObject::Advance();
        if (Health == 0) {
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            Thrust = 0.0;
        }
        if (ab_Ship::KellerArcadeShip == this && KellerFragments[0] != nullptr) {
            ++KellerBreakupTicks;
            if (KellerBreakupTicks > 150 && aPlayer::GetPlayer() != nullptr && aKling::KellerShip != nullptr && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0 && KellerFinishRequested) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 3); cpp_range.next(Index); ) {
                    KellerFragments[Index]->ApplyDamage(KellerFragments[Index]->Health, this, false);
                    KellerFragments[Index] = nullptr;
                }
                DeletionPending = true;
                aGalaxy::Galaxy->CheckIntegrityChecksum1(650);
                ++aPlayer::GetPlayer()->DominatorKillsByType[0];
                aKling::KellerShip->ScriptItemsAct(61, nullptr, nullptr, 0);
                pas::free(aKling::KellerShip);
                aGalaxy::Galaxy->PrimeIntegrityChecksum1(651);
            } else {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(Index); ) {
                    Source = State;
                    Source.BearingDegrees = 0.0;
                    Bearing = ab_Global::GetSphericalBearingAndDistance(Source, KellerFragments[Index]->State);
                    if (KellerBreakupTicks < 150) {
                        KellerFragmentDistances[Index] = pas::real_max<double>(KellerFragmentDistances[Index], Bearing.Distance);
                        OutwardAnimation = pas::checked_cast<SE_Ruins::TRuinsSE*>(reinterpret_cast<ab_Ship::TabShip*>(KellerFragments[Index])->Visual)->Animation;
                        OutwardAnimation->SetSequenceFrame(System::Round(OutwardAnimation->SequenceFrameCount / 2 * pas::real_divide(KellerBreakupTicks, 1.5E+2L)));
                    } else {
                        InwardAnimation = pas::checked_cast<SE_Ruins::TRuinsSE*>(reinterpret_cast<ab_Ship::TabShip*>(KellerFragments[Index])->Visual)->Animation;
                        InwardAnimation->SetSequenceFrame(std::min<std::int64_t>(static_cast<std::int64_t>(InwardAnimation->SequenceFrameCount - 1), System::Round(InwardAnimation->SequenceFrameCount / 2 * pas::real_divide(KellerBreakupTicks, 1.5E+2L))));
                        KellerFragments[Index]->Velocity.X = 0.0f;
                        KellerFragments[Index]->Velocity.Y = 0.0f;
                        KellerFragments[Index]->State = State;
                        KellerFragments[Index]->State.BearingDegrees = aMyFunction::WrapHeadingDegrees(Bearing.BearingDeltaDegrees);
                        ArcDistance = (1.0L - pas::real_divide(KellerBreakupTicks - 150, 1.5E+2L)) * KellerFragmentDistances[Index];
                        ab_Global::AdvanceSphericalBearingState(KellerFragments[Index]->State.LongitudeDegrees, KellerFragments[Index]->State.PolarAngleDegrees, KellerFragments[Index]->State.BearingDegrees, ab_Global::SphereRadius, ArcDistance);
                        if (KellerBreakupTicks >= 300) {
                            KellerFragments[Index]->DeletionPending = true;
                            KellerFragments[Index] = nullptr;
                        }
                    }
                }
                if (KellerFragments[0] == nullptr) {
                    KellerSplitActive = false;
                    StateCC = true;
                    Health = MaxHealth;
                    pas::checked_cast<ab_Ship::TabShip*>(this)->AttachVisual();
                    if (KellerDeathPending) {
                        DeathAnimation = pas::checked_cast<SE_Ruins::TRuinsSE*>(ab_Ship::KellerArcadeShip->Visual)->Animation;
                        DeathAnimation->SetImagePath(u"Bm.Ruins.Keller_Out"_wref.get());
                        DeathAnimation->SequenceIndex = 0;
                        DeathAnimation->UpdateAutoGeometry();
                        DeathAnimation->SetSize(DeathAnimation->GetContentSize());
                        DeathAnimation->SetOrigin(EC_Struct::HalfPoint(DeathAnimation->ClientSize));
                        DeathAnimation->CycleCompleteCallback = pas::bind_method<&TabHit::KellerDeathComplete>(this);
                        Ship = pas::checked_cast<ab_ShipAI::TabShipAI*>(ab_Ship::KellerArcadeShip);
                        Ship->WallCollisionEnabled = false;
                        Ship->GravityEnabled = false;
                        Ship->ZoneDamageEnabled = false;
                        Ship->Collidable = false;
                        Ship->Active = false;
                        Ship->StateCC = false;
                        Ship->Velocity.X = 0.0f;
                        Ship->Velocity.Y = 0.0f;
                        Ship->MaxSpeed = 0.0;
                        Ship->Thrust = 0.0;
                        Ship->AIEnabled = false;
                    }
                }
            }
        }
    }

    void TabHit::UpdateVisuals() {
        GI_MessageLoop::TObjectGI* Effect{};
        std::int32_t Index{};
        EC_Struct::TVector3D Position{};
        ab_Object::TabObject::UpdateVisuals();
        Position = GetWorldPosition();
        Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Position));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Effects) - 1); cpp_range.next(Index); ) {
            Effect = pas::list_at<GI_MessageLoop::TObjectGI>(Effects, Index);
            {
                std::int32_t round = System::Round(Position.Y);
                std::int32_t round_2 = System::Round(Position.X);
                Effect->SetPosition(ClassesImports::Point(round_2, round));
            }
        }
    }

    void TabHit::ExplosionComplete(GI_MessageLoop::TObjectGI* Sender) {
        pas::list_delete(Effects, pas::list_indexof(Effects, reinterpret_cast<void*>(Sender)));
        pas::free(Sender);
        if (pas::list_count(Effects) <= 0) {
            if (Health <= 0) {
                DeletionPending = true;
            }
        }
    }

    void TabHit::KellerBreakupComplete(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        ab_Ship::TabShip* Ship{};
        double Angle{};
        double SourceLongitude{};
        double SourcePolarAngle{};
        double TargetLongitude{};
        double TargetPolarAngle{};
        double Bearing{};
        double Distance{};
        Types::TPoint TargetPoint{};
        Types::TPoint SourcePoint{};
        GI_GAI::TgaiGI* FragmentAnimation{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 3); cpp_range.next(Index); ) {
            KellerFragments[Index] = nullptr;
            KellerFragmentDistances[Index] = 0.0;
            KellerFragmentValuesAC[Index] = 0.0;
        }
        KellerBreakupTicks = 0;
        KellerFinishRequested = false;
        SourcePoint = Globals::ArcadeBattleScreen->WorldPanel->ToAbsolutePoint(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::checked_cast<ab_Ship::TabShip*>(this)->Visual)->Animation->LocalPosition);
        if (!Globals::ArcadeBattleScreen->ScreenPointToSphere(SourcePoint, SourceLongitude, SourcePolarAngle)) {
            SourceLongitude = -1.0E+20;
            SourcePolarAngle = -1.0E+20;
        }
        GI_GAI::TgaiGI* Animation = pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::checked_cast<ab_Ship::TabShip*>(this)->Visual)->Animation;
        Animation->CycleCompleteCallback = nullptr;
        pas::checked_cast<ab_Ship::TabShip*>(this)->DetachVisual();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(Index); ) {
            Ship = pas::construct_call<ab_Ship::TabShip>(ab_Ship::TabShip_Create);
            ab_Object::ab_Object_Add(Ship);
            Ship->CreateRuinsVisual(static_cast<pas::WideString>(pas::concat_ansi({"Ruins.Keller_P", SysUtils::IntToStr(Index + 1)})), 128);
            Ship->MaxSpeed = 8.0;
            Ship->TurnSpeed = 1.8;
            Ship->Thrust = 0.0;
            Ship->Health = 1000000000;
            Ship->MaxHealth = 1000000000;
            Ship->WeaponCount = 0;
            Ship->PrimaryWeapon = 0;
            Ship->State = State;
            Ship->State.BearingDegrees = aMyFunction::RandomIntRange(0, 359);
            Ship->CollisionRadius = 0.0;
            Ship->WallCollisionEnabled = false;
            Ship->GravityEnabled = false;
            Ship->ZoneDamageEnabled = false;
            Ship->Collidable = false;
            Ship->Active = false;
            Ship->StateCC = false;
            Ship->MaxSpeed = 1.0E+1;
            KellerFragments[Index] = Ship;
            if (Index == 0) {
                Angle = aMyFunction::HeadingDegreesToRadians(0.0);
            } else if (Index == 1) {
                Angle = aMyFunction::HeadingDegreesToRadians(2.4E+2);
            } else if (Index == 2) {
                Angle = aMyFunction::HeadingDegreesToRadians(1.2E+2);
            } else {
                Angle = aMyFunction::HeadingDegreesToRadians(2.7E+2);
            }
            Ship->Velocity.X = System::Sin(Angle) * 1.0E+1L;
            Ship->Velocity.Y = System::Cos(Angle) * -1.0E+1L;
            if (SourceLongitude > -1.0E+10L) {
                TargetPoint.X = SourcePoint.X + System::Round(System::Sin(Angle) * 1.0E+2L);
                TargetPoint.Y = SourcePoint.Y - System::Round(System::Cos(Angle) * 1.0E+2L);
                if (Globals::ArcadeBattleScreen->ScreenPointToSphere(TargetPoint, TargetLongitude, TargetPolarAngle)) {
                    ab_Global::ComputeSphericalBearingAndDistance(Bearing, pas::Var<double>(&Distance), SourceLongitude, SourcePolarAngle, 0.0, TargetLongitude, TargetPolarAngle, ab_Global::SphereRadius);
                    Bearing = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(Bearing));
                    Ship->Velocity.X = System::Sin(Bearing) * 1.0E+1L;
                    Ship->Velocity.Y = -System::Cos(Bearing) * 1.0E+1L;
                    Ship->State.BearingDegrees = Bearing;
                }
            }
            Ship->AttachVisual();
            Ship->UpdateState();
            Ship->Advance();
            Ship->UpdateVisuals();
            FragmentAnimation = pas::checked_cast<SE_Ruins::TRuinsSE*>(Ship->Visual)->Animation;
            FragmentAnimation->StopAutoPlayback();
        }
    }

    void TabHit::KellerDeathComplete(GI_MessageLoop::TObjectGI* Sender) {
        DeletionPending = true;
        pas::checked_cast<ab_Ship::TabShip*>(this)->DetachVisual();
    }

    void TabHit::HitEffectComplete(GI_MessageLoop::TObjectGI* Sender) {
        pas::list_delete(Effects, pas::list_indexof(Effects, reinterpret_cast<void*>(Sender)));
        pas::free(Sender);
    }

    void TabHit::p_destroy() {
        ab_Hit::TabHit_Destroy(this);
    }

} // namespace ab_Hit
