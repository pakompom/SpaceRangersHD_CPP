#include "layout/ab_W06.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W06.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW06 projectile family:.
namespace ab_W06 {
    void TabW06_Create(TabW06* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->Mass = 1.0;
        Self->Thrust = 1.2;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
        Self->TurnSpeed = 1.0E+1f;
    }

    void TabW06_Destroy(TabW06* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW06::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        double Heading{};
        double HeadingDelta{};
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 20;
        if (Offset != 0.0L) {
            Heading = aMyFunction::WrapHeadingDegrees(State.BearingDegrees + 9.0E+1L);
            HeadingDelta = aMyFunction::HeadingDifferenceDegrees(Heading, State.BearingDegrees);
            ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&State.LongitudeDegrees), pas::Var<double>(&State.PolarAngleDegrees), pas::Var<double>(&Heading), ab_Global::SphereRadius, Offset);
            State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Heading) + HeadingDelta);
        }
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w06_f"_wref.get(), u"GAI,Bm.AB.w06_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW06_Advance(TabW06* Self) {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Object::TabObject_Advance(Self);
        if (!Self->Exploding) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Self->Exploding) {
            Collision = Self->FindCollision();
            if (Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && static_cast<std::uint8_t>(Self->Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w06a_f"_wref.get(), u"GAI,Bm.AB.w06a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (static_cast<std::uint8_t>(Self->Exploding ^ 1) && Collision == nullptr) {
            if (Self->SourceObject != nullptr) {
                Enemy = pas::checked_cast<ab_Ship::TabShip*>(Self->SourceObject);
                Enemy = Enemy->FindNearestEnemyWithBearing(Self, Bearing);
                if (Enemy != nullptr) {
                    if (Bearing.Distance < 4.0E+2L) {
                        if (Bearing.BearingDeltaDegrees < -Self->TurnSpeed) {
                            Bearing.BearingDeltaDegrees = -Self->TurnSpeed;
                        } else if (Bearing.BearingDeltaDegrees > Self->TurnSpeed) {
                            Bearing.BearingDeltaDegrees = Self->TurnSpeed;
                        }
                        Self->State.BearingDegrees = static_cast<long double>(Self->State.BearingDegrees) + Bearing.BearingDeltaDegrees;
                    }
                }
            }
        } else if (Self->Exploding) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW06::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW06::p_destroy() {
        ab_W06::TabW06_Destroy(this);
    }

    void TabW06::virtual_TabObject_Advance() {
        ab_W06::TabW06_Advance(this);
    }

} // namespace ab_W06
