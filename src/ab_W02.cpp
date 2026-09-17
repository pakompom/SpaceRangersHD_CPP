#include "layout/ab_W02.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W02.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW02 projectile family:.
namespace ab_W02 {
    void TabW02_Create(TabW02* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->Mass = 1.0;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
    }

    void TabW02_Destroy(TabW02* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW02::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 100;
        ArmTick = ab_Global::ArcadeTickCount + 25;
        if (Offset != 0.0L) {
            State = ab_Global::AdvanceSphericalStateOnCurrentSphere(State, Offset);
        }
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w02_f"_wref.get(), u"GAI,Bm.AB.w02_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetFrameMode(Image, ab_WorldImage::afmRandomStart);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW02_Advance(TabW02* Self) {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 2) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 2) {
            Collision = Self->FindCollision();
            if (Self->Phase == 0 && Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if (Collision != nullptr && Self->Phase != 2) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Phase = 2;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w02a_f"_wref.get(), u"GAI,Bm.AB.w02a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Phase == 0 && ab_Global::ArcadeTickCount > Self->ArmTick) {
            Self->Phase = 1;
            Self->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            Self->Thrust = 0.0;
        } else if (Self->Phase == 1 && ab_Global::ArcadeTickCount > Self->ExpireTick) {
            Self->Phase = 2;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w02a_f"_wref.get(), u"GAI,Bm.AB.w02a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Phase == 1 && Self->SourceObject != nullptr) {
            Enemy = pas::checked_cast<ab_Ship::TabShip*>(Self->SourceObject)->FindNearestEnemyWithBearing(Self, Bearing);
            if (Enemy != nullptr && Bearing.Distance < 3.0E+2L) {
                Self->State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Self->State.BearingDegrees) + Bearing.BearingDeltaDegrees);
                Self->Thrust = 2.0;
                Self->MaxSpeed = 2.0;
            } else {
                Self->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
                Self->Thrust = 0.0;
            }
        } else if (Self->Phase == 2) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW02::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW02::p_destroy() {
        ab_W02::TabW02_Destroy(this);
    }

    void TabW02::virtual_TabObject_Advance() {
        ab_W02::TabW02_Advance(this);
    }

} // namespace ab_W02
