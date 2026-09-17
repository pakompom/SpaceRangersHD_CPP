#include "layout/ab_W03.hpp"
#include "units/EC_Struct.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W03.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW03 projectile family:.
namespace ab_W03 {
    std::int32_t ProjectileCount = 0;

    void TabW03_Create(TabW03* Self) {
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 12.0;
        Self->Mass = 1.0;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
        ++ProjectileCount;
        if (ProjectileCount > 30) {
            Obj = ab_Object::FirstArcadeObject;
            while (Obj != nullptr) {
                if (pas::class_cast_if<TabW03*>(Obj) != nullptr && static_cast<std::uint8_t>(reinterpret_cast<TabW03*>(Obj)->Exploding ^ 1)) {
                    pas::checked_cast<TabW03*>(Obj)->Explode();
                    break;
                }
                Obj = Obj->Next;
            }
        }
    }

    void TabW03_Destroy(TabW03* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        --ProjectileCount;
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW03::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        if (Offset != 0.0L) {
            State = ab_Global::AdvanceSphericalStateOnCurrentSphere(State, Offset);
        }
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w03_f"_wref.get(), u"GAI,Bm.AB.w03_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetFrameMode(Image, ab_WorldImage::afmRandomStart);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW03::Explode() {
        Exploding = true;
        ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w03a_f"_wref.get(), u"GAI,Bm.AB.w03a_s"_wref.get());
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
        ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
    }

    void TabW03_Advance(TabW03* Self) {
        ab_Object::TabObject_Advance(Self);
        if (!Self->Exploding) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        if (Self->DistanceTravelled > 5.0E+2L) {
            Self->MaxSpeed = 8.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Self->Exploding) {
            Collision = Self->FindCollision();
            if (Self->DistanceTravelled < 5.0E+2L && Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if (Collision != nullptr && static_cast<std::uint8_t>(Self->Exploding ^ 1)) {
            if (Collision != nullptr) {
                {
                    float cpp_arg = pas::real_divide(Self->Velocity.X, 2.0L);
                    float cpp_arg_2 = pas::real_divide(Self->Velocity.Y, 2.0L);
                    Collision->Velocity = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
                }
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Explode();
        } else if (Self->Exploding) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW03::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW03::p_destroy() {
        ab_W03::TabW03_Destroy(this);
    }

    void TabW03::virtual_TabObject_Advance() {
        ab_W03::TabW03_Advance(this);
    }

} // namespace ab_W03
