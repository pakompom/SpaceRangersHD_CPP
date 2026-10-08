#include "layout/ab_W12.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W12.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW12 projectile family:.
namespace ab_W12 {
    void TabW12_Create(TabW12* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->Mass = 1.0;
        Self->Thrust = 0.0;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
        Self->TrailImages = pas::make_object<pas::List>();
    }

    void TabW12_Destroy(TabW12* Self) {
        std::int32_t Index{};
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        if (Self->TrailImages != nullptr) {
            {
                const std::int32_t cpp_last = pas::list_count(Self->TrailImages) - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        ab_WorldImage::ab_WorldImage_Delete(pas::list_at<ab_WorldImage::TabWorldImage>(
                            Self->TrailImages, Index));
                    }
                }
            }
            pas::free(Self->TrailImages);
            Self->TrailImages = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW12::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle) {
        SourceObject = Owner;
        Damage = Amount;
        OrbitAngle = Angle;
        OrbitRadius = 1.0f;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 100;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0),
                u"GAI,Bm.AB.w12_f"_wref.get(), u"GAI,Bm.AB.w12_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW12::Advance() {
        ab_Object::TabObject::Advance();
        if (!Exploding) {
            OrbitAngle = aMyFunction::WrapHeadingDegrees(static_cast<float>(OrbitAngle + 5.0));
            OrbitRadius = static_cast<float>(OrbitRadius + 3.0);
            if (SourceObject != nullptr) {
                State = ab_Global::AdvanceSphericalStateOnCurrentSphere(
                        ab_Global::MakeSphericalBearingState(SourceObject->State.LongitudeDegrees,
                            SourceObject->State.PolarAngleDegrees, OrbitAngle), OrbitRadius);
            }
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Exploding) {
            Collision = FindCollision();
            if (Collision == SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr || SourceObject == nullptr) &&
            static_cast<std::uint8_t>(Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
            }
            Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w12b_f"_wref.get(),
                u"GAI,Bm.AB.w12b_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (Exploding) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW12::UpdateVisuals() {
        ab_WorldImage::PabWorldImage Entry{};
        std::int32_t Index{};
        ab_Object::TabObject::UpdateVisuals();
        if (!Exploding) {
            Entry = nullptr;
            {
                const std::int32_t cpp_last = pas::list_count(TrailImages) - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        Entry = pas::list_at<ab_WorldImage::TabWorldImage>(TrailImages, Index);
                        if (Entry->Finished) {
                            break;
                        }
                        Entry = nullptr;
                    }
                }
            }
            if (Entry == nullptr) {
                Entry = ab_WorldImage::ab_WorldImage_Create(GetWorldPosition(), u"GAI,Bm.AB.w12a_f"_wref.get(),
                        u"GAI,Bm.AB.w12a_s"_wref.get(), false);
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                pas::list_add(TrailImages, static_cast<void*>(Entry));
            } else {
                ab_WorldImage::ab_WorldImage_Set(Entry, GetWorldPosition(), u"GAI,Bm.AB.w12a_f"_wref.get(),
                    u"GAI,Bm.AB.w12a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
            }
        }
    }

    void TabW12::p_destroy() {
        ab_W12::TabW12_Destroy(this);
    }

} // namespace ab_W12
