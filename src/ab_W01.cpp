#include "layout/ab_W01.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W01.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW01 projectile family:.
namespace ab_W01 {
    void TabW01_Create(TabW01* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->ValueB0 = 5.0E+2f;
        Self->TurnSpeed = 4.0f;
        Self->Mass = 1.0;
        Self->Thrust = 1.2;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
        Self->TrailImages = pas::make_object<pas::List>();
    }

    void TabW01_Destroy(TabW01* Self) {
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

    void TabW01::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        double Heading{};
        double HeadingDelta{};
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        LastTrailPosition = GetWorldPosition();
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 30;
        if (Offset != 0.0L) {
            Heading = aMyFunction::WrapHeadingDegrees(State.BearingDegrees + 9.0E+1L);
            HeadingDelta = aMyFunction::HeadingDifferenceDegrees(Heading, State.BearingDegrees);
            ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&State.LongitudeDegrees), pas::Var<double>(&State.PolarAngleDegrees), pas::Var<double>(&Heading), ab_Global::SphereRadius, Offset);
            State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Heading) + HeadingDelta);
        }
        TrailDistance = 0.0f;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w01_f"_wref.get(), u"GAI,Bm.AB.w01_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW01_Advance(TabW01* Self) {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance cpp_with{};
        ab_Object::TabObject_Advance(Self);
        if (!Self->Exploding) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Self->Exploding) {
            Collision = Self->FindCollision();
            if (Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr) && static_cast<std::uint8_t>(Self->Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w01b_f"_wref.get(), u"GAI,Bm.AB.w01b_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (static_cast<std::uint8_t>(Self->Exploding ^ 1) && Collision == nullptr) {
            if (Self->SourceObject != nullptr) {
                Enemy = pas::checked_cast<ab_Ship::TabShip*>(Self->SourceObject);
                Enemy = Enemy->FindNearestEnemy(Self);
                if (Enemy != nullptr) {
                    cpp_with = Self->BearingAndDistanceTo(Enemy);
                    if (cpp_with.BearingDeltaDegrees < -Self->TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = -Self->TurnSpeed;
                    } else if (cpp_with.BearingDeltaDegrees > Self->TurnSpeed) {
                        cpp_with.BearingDeltaDegrees = Self->TurnSpeed;
                    }
                    Self->State.BearingDegrees = static_cast<long double>(Self->State.BearingDegrees) + cpp_with.BearingDeltaDegrees;
                }
            }
        } else if (Self->Exploding) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW01::UpdateVisuals() {
        ab_WorldImage::PabWorldImage Entry{};
        std::int32_t Index{};
        EC_Struct::TVector3D Position{};
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
                Entry = ab_WorldImage::ab_WorldImage_Create(GetWorldPosition(), u"GAI,Bm.AB.w01a_f"_wref.get(), u""_wref.get(), false);
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                pas::list_add(TrailImages, static_cast<void*>(Entry));
            } else {
                ab_WorldImage::ab_WorldImage_Set(Entry, GetWorldPosition(), u"GAI,Bm.AB.w01a_f"_wref.get(), u""_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
            }
            Entry = nullptr;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(TrailImages) - 1); cpp_range_2.next(Index); ) {
                Entry = pas::list_at<ab_WorldImage::TabWorldImage>(TrailImages, Index);
                if (Entry->Finished) {
                    break;
                }
                Entry = nullptr;
            }
            Position = GetWorldPosition();
            {
                double cpp_arg = pas::real_divide(static_cast<long double>(Position.X) + LastTrailPosition.X, 2.0L);
                double cpp_arg_2 = pas::real_divide(static_cast<long double>(Position.Y) + LastTrailPosition.Y, 2.0L);
                double cpp_arg_3 = pas::real_divide(static_cast<long double>(Position.Z) + LastTrailPosition.Z, 2.0L);
                Position = EC_Struct::MakeVector3D(cpp_arg, cpp_arg_2, cpp_arg_3);
            }
            if (Entry == nullptr) {
                Entry = ab_WorldImage::ab_WorldImage_Create(Position, u"GAI,Bm.AB.w01a_f"_wref.get(), u""_wref.get(), false);
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                pas::list_add(TrailImages, static_cast<void*>(Entry));
            } else {
                ab_WorldImage::ab_WorldImage_Set(Entry, Position, u"GAI,Bm.AB.w01a_f"_wref.get(), u""_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
            }
            LastTrailPosition = GetWorldPosition();
        }
    }

    void TabW01::p_destroy() {
        ab_W01::TabW01_Destroy(this);
    }

    void TabW01::virtual_TabObject_Advance() {
        ab_W01::TabW01_Advance(this);
    }

} // namespace ab_W01
