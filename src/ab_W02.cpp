#include "layout/ab_W02.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W02.hpp"
#include "units/ab_WorldImage.hpp"

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

    void TabW02::Advance() {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Object::TabObject::Advance();
        if (Phase != 2) {
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Phase != 2) {
            Collision = FindCollision();
            if (Phase == 0 && Collision == SourceObject) {
                Collision = nullptr;
            }
        }
        if (Collision != nullptr && Phase != 2) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
            }
            Phase = 2;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w02a_f"_wref.get(), u"GAI,Bm.AB.w02a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (Phase == 0 && ab_Global::ArcadeTickCount > ArmTick) {
            Phase = 1;
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            Thrust = 0.0;
        } else if (Phase == 1 && ab_Global::ArcadeTickCount > ExpireTick) {
            Phase = 2;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w02a_f"_wref.get(), u"GAI,Bm.AB.w02a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (Phase == 1 && SourceObject != nullptr) {
            Enemy = pas::checked_cast<ab_Ship::TabShip*>(SourceObject)->FindNearestEnemyWithBearing(this, Bearing);
            if (Enemy != nullptr && Bearing.Distance < 3.0E+2L) {
                State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Bearing.BearingDeltaDegrees);
                Thrust = 2.0;
                MaxSpeed = 2.0;
            } else {
                Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
                Thrust = 0.0;
            }
        } else if (Phase == 2) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW02::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW02::p_destroy() {
        ab_W02::TabW02_Destroy(this);
    }

} // namespace ab_W02
