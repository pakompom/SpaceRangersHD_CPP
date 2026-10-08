#include "layout/ab_Object.hpp"
#include "types/EC_Buf.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SystemImports.hpp"
#include "types/ab_Hit.hpp"
#include "types/ab_MainForm.hpp"
#include "types/ab_ShipAI.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_Zone.hpp"

// Native TabObject VMT and methods:.
namespace ab_Object {
    ab_Object::TabObject* FirstArcadeObject = nullptr;

    ab_Object::TabObject* LastArcadeObject = nullptr;

    void ab_Object_Clear() {
        while (!(FirstArcadeObject == nullptr)) {
            ab_Object::ab_Object_Delete(LastArcadeObject);
        }
    }

    void ab_Object_Add(TabObject* Obj) {
        if (LastArcadeObject != nullptr) {
            LastArcadeObject->Next = Obj;
        }
        Obj->Prev = LastArcadeObject;
        Obj->Next = nullptr;
        LastArcadeObject = Obj;
        if (FirstArcadeObject == nullptr) {
            FirstArcadeObject = Obj;
        }
    }

    void ab_Object_Delete(TabObject* Obj) {
        if (Obj->Prev != nullptr) {
            Obj->Prev->Next = Obj->Next;
        }
        if (Obj->Next != nullptr) {
            Obj->Next->Prev = Obj->Prev;
        }
        if (LastArcadeObject == Obj) {
            LastArcadeObject = Obj->Prev;
        }
        if (FirstArcadeObject == Obj) {
            FirstArcadeObject = Obj->Next;
        }
        pas::free(Obj);
    }

    void ab_Object_QueueImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        TabObject* Obj = FirstArcadeObject;
        while (Obj != nullptr) {
            Obj->QueueImageLoad(PendingLoads, Owner);
            Obj = Obj->Next;
        }
    }

    void ab_Object_UpdateSounds() {
        std::int32_t Index{};
        TabObject* Playing{};
        TabObject* Closest{};
        float BestDistance{};
        float Distance{};
        float Volume{};
        pas::Array<std::int32_t, 0, 14> Counts{};
        EC_Struct::TVector3D Position{};
        for (Index = 0; Index <= 14; ++Index) {
            Counts[Index] = 0;
        }
        TabObject* Obj = FirstArcadeObject;
        while (Obj != nullptr) {
            if (Obj->SoundDelay > 0) {
                --Obj->SoundDelay;
            }
            if (Obj->SoundDelay == 0 && Obj->SoundGroup >= 9000 && Obj->SoundGroup <= 9014) {
                ++Counts[Obj->SoundGroup - 9000];
            }
            Obj = Obj->Next;
        }
        for (Index = 0; Index <= 14; ++Index) {
            if (Counts[Index] > 0) {
                Closest = nullptr;
                Playing = nullptr;
                BestDistance = 1.0E+20f;
                Obj = FirstArcadeObject;
                while (Obj != nullptr) {
                    if (Obj->SoundDelay == 0 && Index + 9000 == Obj->SoundGroup) {
                        if (Obj->Sound != nullptr) {
                            Playing = Obj;
                        }
                        Position = Obj->GetProjectedPosition();
                        if (ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                            Distance = static_cast<float>(pas::sqr(static_cast<double>(Position.X)) +
                                    pas::sqr(static_cast<double>(Position.Y)));
                            if (Distance < BestDistance) {
                                BestDistance = Distance;
                                Closest = Obj;
                            }
                        }
                    }
                    Obj = Obj->Next;
                }
                if (Playing != nullptr && Closest != Playing) {
                    pas::free(Playing->Sound);
                    Playing->Sound = nullptr;
                }
                if (Closest != nullptr) {
                    if (Closest->Sound == nullptr) {
                        Closest->Sound =
                            pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
                        Closest->Sound->Configure(Closest->SoundPath, Closest->SoundGroup, true);
                    }
                    Volume = static_cast<float>((static_cast<float>(System::Sqrt(BestDistance) - 1.0E+2)) * 0.0033333333333333335);
                    if (Volume < 0.0) {
                        Volume = 0.0f;
                    } else if (Volume > 1.0) {
                        Volume = 1.0f;
                    }
                    Closest->Sound->SetVolume(static_cast<float>(1.0 - Volume));
                }
            }
        }
    }

    void TabObject_Create(TabObject* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->MaxSpeed = 5.0;
        Self->SpeedScale = 1.0;
        Self->DistanceTravelled = 0.0;
        Self->WallCollisionEnabled = false;
        Self->GravityEnabled = false;
        Self->ZoneDamageEnabled = false;
        Self->Active = true;
        Self->Collidable = true;
        Self->SoundDelay = -1;
        Self->InitialRandomSeed = aMyFunction::AdvanceRandomSeed(Globals::ArcadeBattleScreen->RandomSeed);
        Self->RandomState = Self->InitialRandomSeed;
        Self->WeaponDamageScale = 1.0f;
        Self->AmmoRechargeScale = 1.0f;
        Self->MovementScale = 1.0f;
        Self->GravityScale = 1.0f;
        Self->RegenerationRate = 0.0f;
        Self->DamageTakenScale = 1.0f;
        Self->LuckScale = 1.0f;
    }

    void TabObject_Destroy(TabObject* Self) {
        if (Self->Sound != nullptr) {
            pas::free(Self->Sound);
            Self->Sound = nullptr;
        }
        TabObject* Obj = FirstArcadeObject;
        while (Obj != nullptr) {
            if (Obj->SourceObject == Self) {
                Obj->SourceObject = nullptr;
            }
            Obj = Obj->Next;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    EC_Struct::TVector3D TabObject::GetWorldPosition() {
        EC_Struct::TVector3D Result{};
        Result = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(State.LongitudeDegrees),
                aMyFunction::HeadingDegreesToRadians(State.PolarAngleDegrees), ab_Global::SphereRadius);
        return Result;
    }

    EC_Struct::TVector3D TabObject::GetProjectedPosition() {
        EC_Struct::TVector3D Result{};
        Result = GetWorldPosition();
        Result = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix,
                pas::ConstRef<EC_Struct::TVector3D>(&Result));
        return Result;
    }

    double TabObject::DistanceTo(TabObject* Other) {
        double TargetPolar = aMyFunction::HeadingDegreesToRadians(Other->State.PolarAngleDegrees);
        double SourcePolar = aMyFunction::HeadingDegreesToRadians(State.PolarAngleDegrees);
        double LongitudeDelta = aMyFunction::HeadingDegreesToRadians(
            aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(Other->State.LongitudeDegrees) -
                State.LongitudeDegrees)));
        double Value = static_cast<float>(static_cast<float>(System::Cos(TargetPolar) * System::Cos(SourcePolar)) + static_cast<float>(static_cast<float>(System::Sin(TargetPolar) *
                System::Sin(SourcePolar)) * System::Cos(LongitudeDelta)));
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double ArcAngle = MathImports::ArcCos(Value);
        return static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(ArcAngle / pas::constant(2.0 * SystemImports::Pi)) * 2.0) * SystemImports::Pi) * ab_Global::SphereRadius);
    }

    ab_Global::TSphericalBearingDistance TabObject::BearingAndDistanceTo(TabObject* Other) {
        ab_Global::TSphericalBearingDistance Result{};
        Result = ab_Global::GetSphericalBearingAndDistance(State, Other->State);
        return Result;
    }

    double TabObject::GetProjectedHeading(EC_Struct::TVector3D Position) {
        ab_Global::TSphericalBearingState ForwardState{};
        EC_Struct::TVector3D ForwardPosition{};
        ForwardState = State;
        ForwardState = ab_Global::AdvanceSphericalStateOnCurrentSphere(ForwardState, 1.0E+1);
        ForwardPosition = ab_Global::SphericalToVector3D(
                aMyFunction::HeadingDegreesToRadians(ForwardState.LongitudeDegrees),
                aMyFunction::HeadingDegreesToRadians(ForwardState.PolarAngleDegrees), ab_Global::SphereRadius);
        ForwardPosition = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix,
                pas::ConstRef<EC_Struct::TVector3D>(&ForwardPosition));
        if (Position.X == ForwardPosition.X && Position.Y == ForwardPosition.Y) {
            return 0.0;
        }
        return aMyFunction::PointBearingDegrees(EC_Struct::MakePointF(Position.X, Position.Y),
            EC_Struct::MakePointF(ForwardPosition.X, ForwardPosition.Y));
    }

    void TabObject::ChangeSpeed(double Delta) {
        double Speed = System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Velocity.X)) +
            pas::sqr(static_cast<double>(Velocity.Y))));
        if (Speed == 0.0) {
            return;
        }
        double Angle = Math::ArcTan2(Velocity.X, -Velocity.Y);
        Speed = pas::real_max<double>(0.0, static_cast<float>(static_cast<double>(Speed) + Delta));
        Velocity.X = static_cast<float>(System::Sin(Angle) * Speed);
        Velocity.Y = static_cast<float>(-System::Cos(Angle) * Speed);
    }

    std::uint8_t TabObject::CollidesWith(TabObject* Other) {
        if (CollisionRadius <= 0.0 || Other->CollisionRadius <= 0.0) {
            return false;
        }
        if (static_cast<std::uint8_t>(Collidable ^ 1) && static_cast<std::uint8_t>(Other->Collidable ^ 1)) {
            return false;
        }
        if (ab_Hit::TabHit* abHit = pas::class_cast_if<ab_Hit::TabHit*>(Other); abHit != nullptr && abHit->Health <=
                0) {
            return false;
        }
        return static_cast<float>(static_cast<double>(CollisionRadius) + Other->CollisionRadius) >= DistanceTo(Other);
    }

    TabObject* TabObject::FindCollision() {
        if ((ab_Global::ArcadeTickCount & 1) != 0) {
            return nullptr;
        }
        TabObject* Obj = FirstArcadeObject;
        while (Obj != nullptr) {
            if (Obj != this && CollidesWith(Obj)) {
                return Obj;
            }
            Obj = Obj->Next;
        }
        return nullptr;
    }

    void TabObject::ApplyDamage(std::int32_t Amount, TabObject* Source, std::uint8_t Disrupt) {
    }

    void TabObject::UpdateState() {
    }

    void TabObject::Advance() {
        EC_Struct::TPointF Force{};
        double TravelBearing{};
        double HeadingDelta{};
        double ReflectedSpeed{};
        float Factor{};
        float Limit{};
        ab_Zone::PabZone Zone{};
        double Bearing{};
        double Distance{};
        double UnusedResult{};
        Force = EC_Struct::MakePointF(0.0f, 0.0f);
        if (Thrust != 0.0) {
            Force.X = static_cast<float>(Force.X + static_cast<float>(static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(State.BearingDegrees)) * Thrust) *
                        MovementScale));
            Force.Y = static_cast<float>(Force.Y - static_cast<float>(static_cast<float>(System::Cos(aMyFunction::HeadingDegreesToRadians(State.BearingDegrees)) * Thrust) *
                        MovementScale));
        }
        if (GravityEnabled) {
            Zone = ab_Zone::ab_Zone_FindNearestEnabled(State.LongitudeDegrees, State.PolarAngleDegrees);
            if (Zone != nullptr) {
                ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing), pas::Var<double>(&Distance),
                    State.LongitudeDegrees, State.PolarAngleDegrees, 0.0, Zone->Longitude, Zone->PolarAngle,
                    ab_Global::SphereRadius);
                Limit = 2.0f;
                if (ab_Ship::PlayerArcadeShip == this) {
                    if (static_cast<float>(pas::sqr(static_cast<double>(Velocity.X)) + pas::sqr(static_cast<double>(Velocity.Y))) >
                        pas::sqr(2.0)) {
                        Limit = 8.0f;
                    }
                }
                if (Zone->GravityStrength < 0) {
                    Factor = -pas::real_min<double>(static_cast<float>(Limit / 2.0),
                            static_cast<float>(static_cast<float>(static_cast<double>(pas::abs(Zone->GravityStrength)) * Mass) /
                                (static_cast<float>(pas::sqr(static_cast<double>(Distance)) + 0.1))));
                } else {
                    Factor = pas::real_min<double>(static_cast<double>(Limit),
                            static_cast<float>(static_cast<float>(static_cast<double>(pas::abs(Zone->GravityStrength)) * Mass) /
                                (static_cast<float>(pas::sqr(static_cast<double>(Distance)) + 0.1))));
                }
                Bearing = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(Bearing));
                Force.X = static_cast<float>(Force.X + static_cast<float>(static_cast<float>(System::Sin(Bearing) * Factor) * GravityScale));
                Force.Y = static_cast<float>(Force.Y - static_cast<float>(static_cast<float>(System::Cos(Bearing) * Factor) * GravityScale));
            }
        }
        if (ZoneDamageEnabled && pas::checked_cast<ab_Hit::TabHit*>(this)->Health > 0) {
            Zone = ab_Zone::FirstZone;
            while (Zone != nullptr) {
                if (Zone->DamagePerTick != 0) {
                    ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing),
                        pas::Var<double>(&Distance), Zone->Longitude, Zone->PolarAngle, 0.0, State.LongitudeDegrees,
                        State.PolarAngleDegrees, ab_Global::SphereRadius);
                    if (static_cast<float>(static_cast<float>(static_cast<double>(Zone->Radius) + ZoneRadius) + 5.0) > Distance) {
                        if (Zone->DamagePerTick < 0) {
                            std::int32_t min = ([&] {
                                std::int32_t maxHealth = pas::checked_cast<ab_Hit::TabHit*>(this)->MaxHealth;
                                std::int32_t cpp_arg = pas::checked_cast<ab_Hit::TabHit*>(this)->Health +
                                    -Zone->DamagePerTick;
                                return std::min<std::int32_t>(maxHealth, cpp_arg);
                            }());
                            static_cast<ab_Hit::TabHit*>(this)->Health = min;
                        } else {
                            ApplyDamage(Zone->DamagePerTick, nullptr, false);
                            if (ab_ShipAI::TabShipAI* abShipAI = pas::class_cast_if<ab_ShipAI::TabShipAI*>(this)) {
                                abShipAI->NoticeDamagingZone(Zone);
                            }
                        }
                    }
                }
                Zone = Zone->Next;
            }
        }
        Factor = static_cast<float>(1.0 / Mass);
        Velocity.X = static_cast<float>(Velocity.X + static_cast<float>(static_cast<double>(Force.X) * Factor));
        Velocity.Y = static_cast<float>(Velocity.Y + static_cast<float>(static_cast<double>(Force.Y) * Factor));
        double ArcDistance = System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Velocity.X)) +
            pas::sqr(static_cast<double>(Velocity.Y))));
        if (static_cast<float>(static_cast<double>(MaxSpeed) * SpeedScale) < ArcDistance) {
            ArcDistance = static_cast<float>(static_cast<double>(MaxSpeed) * SpeedScale);
        }
        ArcDistance = pas::real_max<double>(0.0,
                static_cast<float>(static_cast<double>(ArcDistance) - aMyFunction::RemapClamped(ArcDistance, 0.0, MaxSpeed,
                    ab_Global::SphereLowSpeedDrag, ab_Global::SphereHighSpeedDrag)));
        TravelBearing = aMyFunction::PointBearingDegrees(EC_Struct::MakePointF(0.0f, 0.0f), Velocity);
        if (ab_Ship::PlayerArcadeShip == this) {
            ab_Ship::PlayerArcadeShip->TurnSpeed = aMyFunction::RemapClamped(ArcDistance, 0.0, MaxSpeed,
                    ab_Global::PlayerSlowTurnSpeed, ab_Global::PlayerFastTurnSpeed);
            Limit = ab_Global::PlayerDriftTurnStep;
            Factor = aMyFunction::HeadingDifferenceDegrees(TravelBearing, State.BearingDegrees);
            if (std::fabs(static_cast<double>(Factor)) < 9.0E+1) {
                if (-Limit > Factor) {
                    TravelBearing = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearing) - Limit));
                } else if (Factor > Limit) {
                    TravelBearing = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearing) + Limit));
                }
            } else {
                Factor = aMyFunction::HeadingDifferenceDegrees(TravelBearing,
                        aMyFunction::WrapHeadingDegrees(static_cast<float>(State.BearingDegrees + 1.8E+2)));
                if (-Limit > Factor) {
                    TravelBearing = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearing) - Limit));
                } else if (Factor > Limit) {
                    TravelBearing = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearing) + Limit));
                }
            }
        }
        if (ArcDistance != 0.0) {
            if (WallCollisionEnabled) {
                {
                    ab_Global::TSphericalBearingState advanceSphericalStateAlongBearing = ab_Global::AdvanceSphericalStateAlongBearing(
                        State, TravelBearing, ArcDistance);
                    ab_Global::TSphericalBearingState state = State;
                    if (ab_StopLine::ab_StopLine_ReflectMovement(state, advanceSphericalStateAlongBearing, HeadingDelta,
                        ReflectedSpeed, UnusedResult)) {
                        ArcDistance = ReflectedSpeed;
                        TravelBearing = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearing) +
                                HeadingDelta));
                    }
                }
                State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(State.BearingDegrees) +
                        UnusedResult));
            }
            if (ArcDistance > 0.0) {
                State = ab_Global::AdvanceSphericalStateAndTravelBearing(State, TravelBearing, ArcDistance);
                DistanceTravelled = static_cast<float>(static_cast<double>(DistanceTravelled) + ArcDistance);
                Velocity.X = static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(TravelBearing)) * ArcDistance);
                Velocity.Y = static_cast<float>(-System::Cos(aMyFunction::HeadingDegreesToRadians(TravelBearing)) * ArcDistance);
            } else {
                Velocity.X = 0.0f;
                Velocity.Y = 0.0f;
            }
        } else {
            Velocity.X = 0.0f;
            Velocity.Y = 0.0f;
        }
    }

    void TabObject::UpdateVisuals() {
    }

    void TabObject::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    std::int32_t TabObject::RandomRange(std::int32_t BoundA, std::int32_t BoundB) {
        RandomState = EC_Buf::SeedRngMultiplier * (RandomState % EC_Buf::SeedRngQuotient) - EC_Buf::SeedRngRemainder *
                    (RandomState / EC_Buf::SeedRngQuotient);
        std::int32_t Result = static_cast<std::int32_t>(RandomState) - 1;
        if (Result < 0) {
            Result = -Result;
        }
        if (BoundA <= BoundB) {
            return BoundA + pas::imod(Result, BoundB - BoundA + 1);
        }
        return BoundB + pas::imod(Result, BoundA - BoundB + 1);
    }

    void TabObject::p_destroy() {
        ab_Object::TabObject_Destroy(this);
    }

} // namespace ab_Object
