#include "layout/ab_W04.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W04.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW04 projectile family:.
namespace ab_W04 {
    void TabW04_Create(TabW04* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->Mass = 1.0;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
    }

    void TabW04_Destroy(TabW04* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW04::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 25;
        if (Offset != 0.0L) {
            State = ab_Global::AdvanceSphericalStateOnCurrentSphere(State, Offset);
        }
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w04_f"_wref.get(), u"GAI,Bm.AB.w04_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetFrameMode(Image, ab_WorldImage::afmRandomStart);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW04::LaunchChild(TabW04* Parent, float Angle) {
        SourceObject = Parent->SourceObject;
        Damage = Parent->Damage / 3;
        State = Parent->State;
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
        ExpireTick = ab_Global::ArcadeTickCount + 30;
        AimTick = ab_Global::ArcadeTickCount + 8;
        Thrust = 1.0;
        Phase = 2;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w04b_f"_wref.get(), u"GAI,Bm.AB.w04b_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetFrameMode(Image, ab_WorldImage::afmRandomStart);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW04_Advance(TabW04* Self) {
        ab_Ship::TabShip* Enemy{};
        TabW04* Child{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 1 && Self->Phase != 4) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 1 && Self->Phase != 4) {
            Collision = Self->FindCollision();
            if (Self->Phase == 0 && Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && Self->Phase != 1 && Self->Phase != 4) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            } else if (Self->Phase == 0) {
                Child = pas::construct_call<TabW04>(TabW04_Create);
                ab_Object::ab_Object_Add(Child);
                Child->LaunchChild(Self, 35.0f);
                Child = pas::construct_call<TabW04>(TabW04_Create);
                ab_Object::ab_Object_Add(Child);
                Child->LaunchChild(Self, 155.0f);
                Child = pas::construct_call<TabW04>(TabW04_Create);
                ab_Object::ab_Object_Add(Child);
                Child->LaunchChild(Self, 275.0f);
            }
            if (Self->Phase == 0) {
                Self->Phase = 1;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w04a_f"_wref.get(), u"GAI,Bm.AB.w04a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            } else {
                Self->Phase = 4;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w04c_f"_wref.get(), u"GAI,Bm.AB.w04c_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            }
        } else if (Self->Phase == 2 && ab_Global::ArcadeTickCount > Self->AimTick) {
            if (Self->SourceObject != nullptr) {
                Enemy = pas::checked_cast<ab_Ship::TabShip*>(Self->SourceObject)->FindNearestEnemyWithBearing(Self, Bearing);
            } else {
                Enemy = nullptr;
            }
            if (Enemy != nullptr && Bearing.Distance < 5.0E+2L) {
                Self->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
                Self->State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Self->State.BearingDegrees) + Bearing.BearingDeltaDegrees);
                Self->Thrust = 3.0;
            }
            Self->Phase = 3;
        } else if (Self->Phase == 1 || Self->Phase == 4) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW04::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW04::p_destroy() {
        ab_W04::TabW04_Destroy(this);
    }

    void TabW04::virtual_TabObject_Advance() {
        ab_W04::TabW04_Advance(this);
    }

} // namespace ab_W04
