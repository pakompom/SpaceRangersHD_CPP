#include "layout/ab_W16.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W16.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW16 projectile family:.
namespace ab_W16 {
    void TabW16_Create(TabW16* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 12.0;
        Self->Mass = 1.0;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 5.0;
        Self->Collidable = false;
    }

    void TabW16_Destroy(TabW16* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabW16*>(Obj) != nullptr && static_cast<TabW16*>(Obj)->ParentProjectile == Self) {
                pas::checked_cast<TabW16*>(Obj)->ParentProjectile = nullptr;
            }
            Obj = Obj->Next;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW16::Launch(ab_Object::TabObject* Owner, std::int32_t Amount) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 120;
        ParentProjectile = nullptr;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w16_f"_wref.get(), u"GAI,Bm.AB.w16_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW16::LaunchChild(TabW16* Parent, float Angle) {
        SourceObject = Parent->SourceObject;
        Damage = Parent->Damage / 5;
        State = Parent->State;
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
        MaxSpeed = 1.0E+1;
        Thrust = 2.5;
        Phase = 2;
        ExpireTick = ab_Global::ArcadeTickCount + 50;
        ParentProjectile = Parent;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w16b_f"_wref.get(), u"GAI,Bm.AB.w16b_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW16_Advance(TabW16* Self) {
        TabW16* Child{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 1 && Self->Phase != 3) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        if (Self->Phase == 0 && Self->DistanceTravelled > 1.0E+2L) {
            Self->MaxSpeed = 13.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 1 && Self->Phase != 3) {
            Collision = Self->FindCollision();
            if (Collision == Self->SourceObject && Self->Phase == 0 && Self->DistanceTravelled < 2.0E+2L) {
                Collision = nullptr;
            }
            if (Collision == Self->SourceObject && Self->Phase == 2 && Self->ParentProjectile != nullptr && Self->ParentProjectile->DistanceTravelled < 6.0E+2L) {
                Collision = nullptr;
            }
            if (Collision != nullptr) {
                if (pas::class_cast_if<TabW16*>(Collision) != nullptr) {
                    if (pas::checked_cast<TabW16*>(Collision)->ParentProjectile == Self || Self->ParentProjectile == Collision || Self->ParentProjectile != nullptr && pas::checked_cast<TabW16*>(Collision)->ParentProjectile == Self->ParentProjectile) {
                        Collision = nullptr;
                    }
                }
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && Self->Phase != 1 && Self->Phase != 3) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            if (Self->Phase == 0) {
                Self->Phase = 1;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w16a_f"_wref.get(), u"GAI,Bm.AB.w16a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            } else {
                Self->Phase = 3;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w16c_f"_wref.get(), u"GAI,Bm.AB.w16c_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            }
        } else if (Self->Phase == 0 && Self->DistanceTravelled > 5.0E+1L && Self->SourceObject != nullptr) {
            Child = pas::construct_call<TabW16>(TabW16_Create);
            ab_Object::ab_Object_Add(Child);
            Child->LaunchChild(Self, pas::random(360, &System::RandSeed));
        } else if (Self->Phase == 2 && Self->ParentProjectile != nullptr) {
            cpp_with = Self->BearingAndDistanceTo(Self->ParentProjectile);
            if (cpp_with.Distance > 1.0E+1L) {
                Self->State.BearingDegrees = static_cast<long double>(Self->State.BearingDegrees) + cpp_with.BearingDeltaDegrees;
                Self->MaxSpeed = Self->ParentProjectile->MaxSpeed * 1.5L;
            } else if (cpp_with.Distance < 5.0L) {
                Self->State.BearingDegrees = pas::random(360, &System::RandSeed);
            } else {
                Self->MaxSpeed = Self->MaxSpeed * 0.6L;
            }
        } else if (Self->Phase == 1 || Self->Phase == 3) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW16::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW16::p_destroy() {
        ab_W16::TabW16_Destroy(this);
    }

    void TabW16::virtual_TabObject_Advance() {
        ab_W16::TabW16_Advance(this);
    }

} // namespace ab_W16
