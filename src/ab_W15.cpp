#include "layout/ab_W15.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W15.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW15 projectile family:.
namespace ab_W15 {
    std::int32_t W15ProjectileCount = 0;

    void TabW15_Create(TabW15* Self) {
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 12.0;
        Self->Mass = 1.0;
        Self->Thrust = 1.5;
        Self->TurnSpeed = 1.0f;
        Self->CollisionRadius = 5.0;
        Self->Collidable = true;
        ++W15ProjectileCount;
        if (W15ProjectileCount > 50) {
            Obj = ab_Object::FirstArcadeObject;
            while (Obj != nullptr) {
                if (pas::class_cast_if<TabW15*>(Obj) != nullptr && reinterpret_cast<TabW15*>(Obj)->Phase != 2) {
                    pas::checked_cast<TabW15*>(Obj)->Explode();
                    break;
                }
                Obj = Obj->Next;
            }
        }
    }

    void TabW15_Destroy(TabW15* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        --W15ProjectileCount;
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW15::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 1500;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w15_f"_wref.get(), u"GAI,Bm.AB.w15_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW15::Explode() {
        Phase = 2;
        ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w15a_f"_wref.get(), u"GAI,Bm.AB.w15a_s"_wref.get());
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
        ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
    }

    void TabW15_Advance(TabW15* Self) {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 2) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        if (ab_Global::ArcadeTickCount % 150 == 0) {
            std::int32_t cpp_left = aMyFunction::RandomIntRange(25, 40);
            Self->TurnBias = cpp_left * (aMyFunction::RandomIntRange(0, 1) * 2 - 1);
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 2) {
            Collision = Self->FindCollision();
            if (Collision != nullptr && Self->SourceObject != nullptr && pas::class_cast_if<ab_Ship::TabShip*>(Collision) != nullptr && pas::list_indexof(reinterpret_cast<ab_Ship::TabShip*>(Self->SourceObject)->Enemies, reinterpret_cast<void*>(Collision)) < 0) {
                Collision = nullptr;
            }
            if (Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && Self->Phase != 2) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Phase = 2;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w15a_f"_wref.get(), u"GAI,Bm.AB.w15a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Phase == 0 && Self->DistanceTravelled > 4.0E+2L) {
            Self->MaxSpeed = 5.0;
            Self->Phase = 1;
        } else if (Self->Phase == 1) {
            if (Self->SourceObject != nullptr) {
                Enemy = pas::checked_cast<ab_Ship::TabShip*>(Self->SourceObject);
                Enemy = Enemy->FindNearestEnemy(Self);
                if (Enemy != nullptr) {
                    cpp_with = Self->BearingAndDistanceTo(Enemy);
                    cpp_with.BearingDeltaDegrees = static_cast<long double>(cpp_with.BearingDeltaDegrees) + Self->TurnBias;
                    if (cpp_with.BearingDeltaDegrees < -Self->TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = -Self->TurnSpeed;
                    } else if (cpp_with.BearingDeltaDegrees > Self->TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = Self->TurnSpeed;
                    }
                    Self->State.BearingDegrees = static_cast<long double>(Self->State.BearingDegrees) + cpp_with.BearingDeltaDegrees;
                }
            }
        } else if (Self->Phase == 2) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW15::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW15::p_destroy() {
        ab_W15::TabW15_Destroy(this);
    }

    void TabW15::virtual_TabObject_Advance() {
        ab_W15::TabW15_Advance(this);
    }

} // namespace ab_W15
