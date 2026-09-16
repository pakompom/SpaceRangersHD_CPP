#include "layout/ab_W07.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W07.hpp"
#include "units/ab_WorldImage.hpp"

namespace ab_W07 {
    void TabW07_Create(TabW07* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 12.0;
        Self->TurnSpeed = 1.0E+1f;
        Self->Mass = 1.0;
        Self->Thrust = 0.7;
        Self->CollisionRadius = 1.0;
        Self->Collidable = true;
        Self->TrailImages = pas::make_object<pas::List>();
    }

    void TabW07_Destroy(TabW07* Self) {
        std::int32_t Index{};
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        if (Self->TrailImages != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->TrailImages) - 1); cpp_range.next(Index); ) {
                ab_WorldImage::ab_WorldImage_Delete(pas::list_at<ab_WorldImage::TabWorldImage>(Self->TrailImages, Index));
            }
            pas::free(Self->TrailImages);
            Self->TrailImages = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW07::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        double Heading{};
        double HeadingDelta{};
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 300;
        if (Offset != 0.0L) {
            Heading = aMyFunction::WrapHeadingDegrees(State.BearingDegrees + 9.0E+1L);
            HeadingDelta = aMyFunction::HeadingDifferenceDegrees(Heading, State.BearingDegrees);
            ab_Global::AdvanceSphericalBearingState(State.LongitudeDegrees, State.PolarAngleDegrees, Heading, ab_Global::SphereRadius, Offset);
            State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Heading) + HeadingDelta);
        }
        TrailDistance = 0.0f;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w07_f"_wref.get(), u"GAI,Bm.AB.w07_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW07::Advance() {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject::Advance();
        if (!Exploding) {
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        if (DistanceTravelled > 4.0E+2L) {
            MaxSpeed = 5.0;
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Exploding) {
            Collision = FindCollision();
            if (DistanceTravelled < 4.0E+2L && Collision == SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr) && static_cast<std::uint8_t>(Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
            }
            Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w07b_f"_wref.get(), u"GAI,Bm.AB.w07b_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (static_cast<std::uint8_t>(Exploding ^ 1) && Collision == nullptr) {
            if (SourceObject != nullptr) {
                Enemy = pas::checked_cast<ab_Ship::TabShip*>(SourceObject);
                Enemy = Enemy->FindNearestEnemy(this);
                if (Enemy != nullptr) {
                    cpp_with = BearingAndDistanceTo(Enemy);
                    if (cpp_with.BearingDeltaDegrees < -TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = -TurnSpeed;
                    } else if (cpp_with.BearingDeltaDegrees > TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = TurnSpeed;
                    }
                    State.BearingDegrees = static_cast<long double>(State.BearingDegrees) + cpp_with.BearingDeltaDegrees;
                }
            }
        } else if (Exploding) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW07::UpdateVisuals() {
        ab_WorldImage::PabWorldImage Entry{};
        std::int32_t Index{};
        ab_Object::TabObject::UpdateVisuals();
        if (static_cast<std::uint8_t>(Exploding ^ 1) && DistanceTravelled > TrailDistance) {
            TrailDistance = 0.0f;
            Entry = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(TrailImages) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<ab_WorldImage::TabWorldImage>(TrailImages, Index);
                if (Entry->Finished) {
                    break;
                }
                Entry = nullptr;
            }
            if (Entry == nullptr) {
                Entry = ab_WorldImage::ab_WorldImage_Create(GetWorldPosition(), u"GAI,Bm.AB.w07a_f"_wref.get(), u"GAI,Bm.AB.w07a_s"_wref.get(), false);
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                pas::list_add(TrailImages, static_cast<void*>(Entry));
            } else {
                ab_WorldImage::ab_WorldImage_Set(Entry, GetWorldPosition(), u"GAI,Bm.AB.w07a_f"_wref.get(), u"GAI,Bm.AB.w07a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
            }
        }
    }

    void TabW07::p_destroy() {
        ab_W07::TabW07_Destroy(this);
    }

} // namespace ab_W07
