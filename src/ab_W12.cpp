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
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->TrailImages) - 1); cpp_range.next(Index); ) {
                ab_WorldImage::ab_WorldImage_Delete(pas::list_at<ab_WorldImage::TabWorldImage>(Self->TrailImages, Index));
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
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w12_f"_wref.get(), u"GAI,Bm.AB.w12_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW12_Advance(TabW12* Self) {
        ab_Object::TabObject_Advance(Self);
        if (!Self->Exploding) {
            Self->OrbitAngle = aMyFunction::WrapHeadingDegrees(Self->OrbitAngle + 5.0L);
            Self->OrbitRadius = Self->OrbitRadius + 3.0L;
            if (Self->SourceObject != nullptr) {
                Self->State = ab_Global::AdvanceSphericalStateOnCurrentSphere(ab_Global::MakeSphericalBearingState(Self->SourceObject->State.LongitudeDegrees, Self->SourceObject->State.PolarAngleDegrees, Self->OrbitAngle), Self->OrbitRadius);
            }
            Self->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (!Self->Exploding) {
            Collision = Self->FindCollision();
            if (Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr || Self->SourceObject == nullptr) && static_cast<std::uint8_t>(Self->Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w12b_f"_wref.get(), u"GAI,Bm.AB.w12b_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Exploding) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW12::UpdateVisuals() {
        ab_WorldImage::PabWorldImage Entry{};
        std::int32_t Index{};
        ab_Object::TabObject::UpdateVisuals();
        if (!Exploding) {
            Entry = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(TrailImages) - 1); cpp_range.next(Index); ) {
                Entry = pas::list_at<ab_WorldImage::TabWorldImage>(TrailImages, Index);
                if (Entry->Finished) {
                    break;
                }
                Entry = nullptr;
            }
            if (Entry == nullptr) {
                Entry = ab_WorldImage::ab_WorldImage_Create(GetWorldPosition(), u"GAI,Bm.AB.w12a_f"_wref.get(), u"GAI,Bm.AB.w12a_s"_wref.get(), false);
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
                pas::list_add(TrailImages, static_cast<void*>(Entry));
            } else {
                ab_WorldImage::ab_WorldImage_Set(Entry, GetWorldPosition(), u"GAI,Bm.AB.w12a_f"_wref.get(), u"GAI,Bm.AB.w12a_s"_wref.get());
                ab_WorldImage::ab_WorldImage_SetDepth(Entry, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
                ab_WorldImage::ab_WorldImage_SetLooping(Entry, false);
            }
        }
    }

    void TabW12::p_destroy() {
        ab_W12::TabW12_Destroy(this);
    }

    void TabW12::virtual_TabObject_Advance() {
        ab_W12::TabW12_Advance(this);
    }

} // namespace ab_W12
