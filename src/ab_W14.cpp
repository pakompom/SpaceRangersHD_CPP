#include "layout/ab_W14.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_Image.hpp"
#include "types/SystemImports.hpp"
#include "types/ab_Ship.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W14.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW14 projectile family:.
namespace ab_W14 {
    void TabW14_Create(TabW14* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 25.0;
        Self->Mass = 1.0;
        Self->Thrust = 1.5;
        Self->CollisionRadius = 5.0;
        Self->Collidable = true;
    }

    void TabW14_Destroy(TabW14* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW14::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle) {
        SourceObject = Owner;
        Damage = Amount;
        State = Owner->State;
        State.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(State.BearingDegrees) + Angle);
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 150;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w14_f"_wref.get(), u"GAI,Bm.AB.w14_s"_wref.get(), true);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW14_Advance(TabW14* Self) {
        ab_Object::TabObject_Advance(Self);
        if (Self->Phase != 1) {
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        ab_Object::TabObject* Collision = nullptr;
        if (Self->Phase != 1) {
            Collision = Self->FindCollision();
            if (Collision != nullptr && Self->SourceObject != nullptr && pas::class_cast_if<ab_Ship::TabShip*>(Collision) != nullptr && pas::list_indexof(reinterpret_cast<ab_Ship::TabShip*>(Self->SourceObject)->Enemies, reinterpret_cast<void*>(Collision)) < 0) {
                Collision = nullptr;
            } else if (Collision == Self->SourceObject) {
                Collision = nullptr;
            } else if (pas::class_cast_if<TabW14*>(Collision) != nullptr) {
                Collision = nullptr;
            }
        }
        // Native launch sets ExpireTick, but flight expires by half-circumference.
        if ((Self->DistanceTravelled > SystemImports::Pi * ab_Global::SphereRadius || Collision != nullptr) && Self->Phase != 1) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
            }
            Self->Phase = 1;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w14a_f"_wref.get(), u"GAI,Bm.AB.w14a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Phase == 1) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW14::UpdateVisuals() {
        std::int32_t Frame{};
        EC_Struct::TVector3D Position{};
        ab_Object::TabObject::UpdateVisuals();
        if (Phase == 0) {
            Position = GetWorldPosition();
            Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Position));
            Frame = System::Round(([&] {
                pas::Extended cpp_left = pas::real_divide(GetProjectedHeading(Position), 3.6E+2L);
                return cpp_left * Image->Image->GaiImageControl->SequenceFrameCount;
            }()));
            if (Frame >= Image->Image->GaiImageControl->SequenceFrameCount) {
                Frame = 0;
            }
            Image->Image->GaiImageControl->SetSequenceFrame(Frame);
        }
    }

    void TabW14::p_destroy() {
        ab_W14::TabW14_Destroy(this);
    }

    void TabW14::virtual_TabObject_Advance() {
        ab_W14::TabW14_Advance(this);
    }

} // namespace ab_W14
