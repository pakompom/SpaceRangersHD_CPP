#include "layout/ab_W17.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W17.hpp"
#include "units/ab_WorldImage.hpp"

namespace ab_W17 {
    void TabW17_Create(TabW17* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 2.0E+1;
        Self->Mass = 0.1;
        Self->Thrust = 1.0;
        Self->CollisionRadius = 5.0;
        Self->Collidable = false;
    }

    void TabW17_Destroy(TabW17* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        if (Self->Partner != nullptr) {
            Self->Partner->Partner = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW17::Launch(ab_Object::TabObject* Owner, std::int32_t Amount) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        State.BearingDegrees = State.BearingDegrees + 3.0E+1L;
        Phase = 0;
        ExpireTick = ab_Global::ArcadeTickCount + 120;
        Partner = nullptr;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w17_f"_wref.get(), u"GAI,Bm.AB.w17_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
        TabW17* Other = pas::construct_call<TabW17>(TabW17_Create);
        ab_Object::ab_Object_Add(Other);
        Other->LaunchPartner(this, Owner, Amount);
    }

    void TabW17::LaunchPartner(TabW17* Other, ab_Object::TabObject* Owner, std::int32_t Amount) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        State.BearingDegrees = State.BearingDegrees - 3.0E+1L;
        Phase = 2;
        ExpireTick = ab_Global::ArcadeTickCount + 120;
        Partner = Other;
        Other->Partner = this;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w17b_f"_wref.get(), u"GAI,Bm.AB.w17b_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW17::Advance() {
        double Delta{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject::Advance();
        if (Phase != 1 && Phase != 3) {
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        if (pas::in_set<0, 0, 2, 2>(Phase) && DistanceTravelled > 2.0E+2L) {
            MaxSpeed = 11.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Phase != 1 && Phase != 3) {
            Collision = FindCollision();
            if (DistanceTravelled < 3.0E+2L && pas::in_set<0, 0, 2, 2>(Phase) && Collision == SourceObject) {
                Collision = nullptr;
            }
            if (Partner == Collision) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr) && Phase != 1 && Phase != 3) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
                if (Partner != nullptr) {
                    Partner->Velocity = Collision->Velocity;
                    {
                        pas::Extended cpp_left = Partner->BearingAndDistanceTo(Collision).BearingDeltaDegrees;
                        Partner->State.BearingDegrees = cpp_left + Partner->State.BearingDegrees;
                    }
                    Partner->MaxSpeed = Partner->MaxSpeed * 1.5L;
                    Partner->Thrust = 2.0;
                    Partner->Partner = nullptr;
                    Partner = nullptr;
                }
            }
            if (Phase == 0) {
                Phase = 1;
                ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w17a_f"_wref.get(), u"GAI,Bm.AB.w17a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
            } else {
                Phase = 3;
                ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w17c_f"_wref.get(), u"GAI,Bm.AB.w17c_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
            }
        } else if (pas::in_set<0, 0, 2, 2>(Phase) && Partner != nullptr) {
            cpp_with = BearingAndDistanceTo(Partner);
            Delta = cpp_with.BearingDeltaDegrees;
            while (Delta > 1.8E+2L) {
                Delta = Delta - 3.6E+2L;
            }
            while (Delta < -1.8E+2L) {
                Delta = Delta + 3.6E+2L;
            }
            if (cpp_with.Distance > 1.0E+2L && std::fabs(static_cast<pas::Extended>(Delta)) > 6.0E+1L) {
                if (Delta > 0.0L) {
                    Delta = Delta - 6.0E+1L;
                } else {
                    Delta = Delta + 6.0E+1L;
                }
                State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Delta);
            }
        } else if (Phase == 1 || Phase == 3) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW17::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW17::p_destroy() {
        ab_W17::TabW17_Destroy(this);
    }

} // namespace ab_W17
