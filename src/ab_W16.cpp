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

    void TabW16::Advance() {
        TabW16* Child{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject::Advance();
        if (Phase != 1 && Phase != 3) {
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        if (Phase == 0 && DistanceTravelled > 1.0E+2L) {
            MaxSpeed = 13.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Phase != 1 && Phase != 3) {
            Collision = FindCollision();
            if (Collision == SourceObject && Phase == 0 && DistanceTravelled < 2.0E+2L) {
                Collision = nullptr;
            }
            if (Collision == SourceObject && Phase == 2 && ParentProjectile != nullptr && ParentProjectile->DistanceTravelled < 6.0E+2L) {
                Collision = nullptr;
            }
            if (Collision != nullptr) {
                if (pas::class_cast_if<TabW16*>(Collision) != nullptr) {
                    if (pas::checked_cast<TabW16*>(Collision)->ParentProjectile == this || ParentProjectile == Collision || ParentProjectile != nullptr && pas::checked_cast<TabW16*>(Collision)->ParentProjectile == ParentProjectile) {
                        Collision = nullptr;
                    }
                }
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr) && Phase != 1 && Phase != 3) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
            }
            if (Phase == 0) {
                Phase = 1;
                ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w16a_f"_wref.get(), u"GAI,Bm.AB.w16a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
            } else {
                Phase = 3;
                ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w16c_f"_wref.get(), u"GAI,Bm.AB.w16c_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
            }
        } else if (Phase == 0 && DistanceTravelled > 5.0E+1L && SourceObject != nullptr) {
            Child = pas::construct_call<TabW16>(TabW16_Create);
            ab_Object::ab_Object_Add(Child);
            Child->LaunchChild(this, pas::random(360, &System::RandSeed));
        } else if (Phase == 2 && ParentProjectile != nullptr) {
            cpp_with = BearingAndDistanceTo(ParentProjectile);
            if (cpp_with.Distance > 1.0E+1L) {
                State.BearingDegrees = static_cast<long double>(State.BearingDegrees) + cpp_with.BearingDeltaDegrees;
                MaxSpeed = ParentProjectile->MaxSpeed * 1.5L;
            } else if (cpp_with.Distance < 5.0L) {
                State.BearingDegrees = pas::random(360, &System::RandSeed);
            } else {
                MaxSpeed = MaxSpeed * 0.6L;
            }
        } else if (Phase == 1 || Phase == 3) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW16::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW16::p_destroy() {
        ab_W16::TabW16_Destroy(this);
    }

} // namespace ab_W16
