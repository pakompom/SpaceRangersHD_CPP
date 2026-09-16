#include "layout/ab_W13.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W13.hpp"
#include "units/ab_WorldImage.hpp"

namespace ab_W13 {
    void TabW13_Create(TabW13* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->Mass = 1.0;
        Self->Thrust = 0.8;
        Self->CollisionRadius = 5.0;
        Self->Collidable = false;
    }

    void TabW13_Destroy(TabW13* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW13::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle, std::int32_t AGeneration, ab_Object::TabObject* Origin) {
        SourceObject = Owner;
        Damage = Amount;
        if (Origin != nullptr) {
            State = Origin->State;
        } else {
            State = Owner->State;
        }
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        if (Owner != nullptr) {
            Velocity = Owner->Velocity;
        } else {
            Velocity = Origin->Velocity;
        }
        Generation = AGeneration;
        if (Generation == 0) {
            ExpireTick = ab_Global::ArcadeTickCount + 25;
        } else {
            ExpireTick = ab_Global::ArcadeTickCount + 10;
        }
        if (Generation == 0) {
            Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w08_f"_wref.get(), u"GAI,Bm.AB.w08_s"_wref.get(), false);
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
        } else {
            Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w08b_f"_wref.get(), u"GAI,Bm.AB.w08b_s"_wref.get(), false);
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
        }
    }

    void TabW13::Advance() {
        TabW13* Child{};
        ab_Object::TabObject::Advance();
        if (!Exploding) {
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Exploding) {
            Collision = FindCollision();
            if (Collision == SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr) && static_cast<std::uint8_t>(Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
            } else if (Generation <= 1) {
                Child = pas::construct_call<TabW13>(TabW13_Create);
                ab_Object::ab_Object_Add(Child);
                {
                    float randomIntRange = aMyFunction::RandomIntRange(0, 360);
                    std::int32_t cpp_arg = Generation + 1;
                    std::int32_t cpp_arg_2 = Damage / 3;
                    ab_Object::TabObject* sourceObject = SourceObject;
                    Child->Launch(sourceObject, cpp_arg_2, randomIntRange, cpp_arg, this);
                }
                Child = pas::construct_call<TabW13>(TabW13_Create);
                ab_Object::ab_Object_Add(Child);
                {
                    float randomIntRange_2 = aMyFunction::RandomIntRange(0, 360);
                    std::int32_t cpp_arg_3 = Generation + 1;
                    std::int32_t cpp_arg_4 = Damage / 3;
                    ab_Object::TabObject* sourceObject_2 = SourceObject;
                    Child->Launch(sourceObject_2, cpp_arg_4, randomIntRange_2, cpp_arg_3, this);
                }
                Child = pas::construct_call<TabW13>(TabW13_Create);
                ab_Object::ab_Object_Add(Child);
                {
                    float randomIntRange_3 = aMyFunction::RandomIntRange(0, 360);
                    std::int32_t cpp_arg_5 = Generation + 1;
                    std::int32_t cpp_arg_6 = Damage / 3;
                    ab_Object::TabObject* sourceObject_3 = SourceObject;
                    Child->Launch(sourceObject_3, cpp_arg_6, randomIntRange_3, cpp_arg_5, this);
                }
            }
            Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w08a_f"_wref.get(), u"GAI,Bm.AB.w08a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (Exploding) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW13::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW13::p_destroy() {
        ab_W13::TabW13_Destroy(this);
    }

} // namespace ab_W13
