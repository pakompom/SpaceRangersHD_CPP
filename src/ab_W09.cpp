#include "layout/ab_W09.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W09.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW09 projectile family:.
namespace ab_W09 {
    void TabW09_Create(TabW09* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 12.0;
        Self->Mass = 1.0;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 5.0;
        Self->Collidable = false;
    }

    void TabW09_Destroy(TabW09* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW09::Launch(ab_Object::TabObject* Owner, std::int32_t Amount) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 120;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w09_f"_wref.get(), u"GAI,Bm.AB.w09_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW09::LaunchChild(TabW09* Parent, float Angle) {
        SourceObject = Parent->SourceObject;
        Damage = Parent->Damage / 20;
        State = Parent->State;
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
        MaxSpeed = 1.0E+2;
        Thrust = 2.5;
        Phase = 2;
        ExpireTick = ab_Global::ArcadeTickCount + 20;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w09b_f"_wref.get(), u"GAI,Bm.AB.w09b_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW09_Advance(TabW09* Self) {
        std::int32_t Index{};
        ab_Ship::TabShip* Enemy{};
        TabW09* Child{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 1 && Self->Phase != 3) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        if (Self->Phase == 0 && Self->DistanceTravelled > 1.0E+2L) {
            Self->MaxSpeed = 13.0;
        }
        if (Self->Phase == 0 && Self->DistanceTravelled > 3.0E+2L) {
            Self->MaxSpeed = 8.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 1 && Self->Phase != 3) {
            Collision = Self->FindCollision();
            if (Self->DistanceTravelled < 2.0E+2L && Self->Phase == 0 && Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && Self->Phase != 1 && Self->Phase != 3) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            if (Self->Phase == 0) {
                Self->Phase = 1;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w09a_f"_wref.get(), u"GAI,Bm.AB.w09a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            } else {
                Self->Phase = 3;
                ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w09c_f"_wref.get(), u"GAI,Bm.AB.w09c_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
            }
        } else if (Self->Phase == 0 && ab_Global::ArcadeTickCount % 3 == 0 && Self->SourceObject != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(reinterpret_cast<ab_Ship::TabShip*>(Self->SourceObject)->Enemies) - 1); cpp_range.next(Index); ) {
                Enemy = pas::list_at<ab_Ship::TabShip>(reinterpret_cast<ab_Ship::TabShip*>(Self->SourceObject)->Enemies, Index);
                cpp_with = Self->BearingAndDistanceTo(Enemy);
                if (cpp_with.Distance < 4.0E+2L && Enemy->Health > 0) {
                    Child = pas::construct_call<TabW09>(TabW09_Create);
                    ab_Object::ab_Object_Add(Child);
                    Child->LaunchChild(Self, cpp_with.BearingDeltaDegrees);
                }
            }
        } else if (Self->Phase == 1 || Self->Phase == 3) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW09::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW09::p_destroy() {
        ab_W09::TabW09_Destroy(this);
    }

    void TabW09::virtual_TabObject_Advance() {
        ab_W09::TabW09_Advance(this);
    }

} // namespace ab_W09
