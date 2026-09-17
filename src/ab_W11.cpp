#include "layout/ab_W11.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W11.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW11 projectile family:.
namespace ab_W11 {
    void TabW11_Create(TabW11* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 4.0E+1;
        Self->TurnSpeed = 0.2f;
        Self->Mass = 1.0;
        Self->Thrust = 0.8;
        Self->CollisionRadius = 5.0;
        Self->Collidable = false;
        Self->TrailImages = pas::make_object<pas::List>();
    }

    void TabW11_Destroy(TabW11* Self) {
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

    void TabW11::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset) {
        double Heading{};
        double HeadingDelta{};
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        LastTrailPosition = GetWorldPosition();
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 70;
        if (Offset != 0.0L) {
            Heading = aMyFunction::WrapHeadingDegrees(State.BearingDegrees + 9.0E+1L);
            HeadingDelta = aMyFunction::HeadingDifferenceDegrees(Heading, State.BearingDegrees);
            ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&State.LongitudeDegrees), pas::Var<double>(&State.PolarAngleDegrees), pas::Var<double>(&Heading), ab_Global::SphereRadius, Offset);
            State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Heading) + HeadingDelta);
        }
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w11_f"_wref.get(), u"GAI,Bm.AB.w11_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW11::Advance() {
        ab_Ship::TabShip* Enemy{};
        ab_Global::TSphericalBearingDistance cpp_with{};
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
            }
            Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w11b_f"_wref.get(), u"GAI,Bm.AB.w11b_s"_wref.get());
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

    void TabW11::UpdateVisuals() {
        ab_WorldImage::PabWorldImage Entry{};
        std::int32_t Index{};
        std::int32_t Sample{};
        EC_Struct::TVector3D Position{};
        EC_Struct::TVector3D TrailPosition{};
        EC_Struct::TVector3D Delta{};
        ab_Object::TabObject::UpdateVisuals();
        if (!Exploding) {
            Position = GetWorldPosition();
            Delta.X = pas::real_divide(-(static_cast<long double>(LastTrailPosition.X) - Position.X), 4.0L);
            Delta.Y = pas::real_divide(-(static_cast<long double>(LastTrailPosition.Y) - Position.Y), 4.0L);
            Delta.Z = pas::real_divide(-(static_cast<long double>(LastTrailPosition.Z) - Position.Z), 4.0L);
            for (Sample = 0; Sample <= 3; ++Sample) {
                Entry = nullptr;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(TrailImages) - 1); cpp_range.next(Index); ) {
                    Entry = pas::list_at<ab_WorldImage::TabWorldImage>(TrailImages, Index);
                    if (Entry->Finished) {
                        break;
                    }
                    Entry = nullptr;
                }
                TrailPosition = EC_Struct::MakeVector3D(static_cast<long double>(Sample) * Delta.X + Position.X, static_cast<long double>(Sample) * Delta.Y + Position.Y, static_cast<long double>(Sample) * Delta.Z + Position.Z);
                if (Entry == nullptr) {
                    Entry = ab_WorldImage::ab_WorldImage_Create(TrailPosition, u"GAI,Bm.AB.w11a_f"_wref.get(), u"GAI,Bm.AB.w11a_s"_wref.get(), false);
                    ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                    ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                    pas::list_add(TrailImages, static_cast<void*>(Entry));
                } else {
                    ab_WorldImage::ab_WorldImage_Set(Entry, TrailPosition, u"GAI,Bm.AB.w11a_f"_wref.get(), u"GAI,Bm.AB.w11a_s"_wref.get());
                    ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                    ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                }
            }
            LastTrailPosition = Position;
        }
    }

    void TabW11::p_destroy() {
        ab_W11::TabW11_Destroy(this);
    }

} // namespace ab_W11
