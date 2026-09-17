#include "layout/ab_W18.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W18.hpp"
#include "units/ab_WorldImage.hpp"

// Native TabW18 projectile family:.
namespace ab_W18 {
    void TabW18_Create(TabW18* Self) {
        ab_Object::TabObject_Create(Self);
        Self->MaxSpeed = 1.0E+2;
        Self->AngleCorrection = 0.0f;
        Self->Mass = 1.0;
        Self->Thrust = 0.0;
        Self->CollisionRadius = 1.0;
        Self->Collidable = false;
    }

    void TabW18_Destroy(TabW18* Self) {
        if (Self->Image != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->Image);
            Self->Image = nullptr;
        }
        ab_Object::TabObject_Destroy(Self);
    }

    void TabW18::Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle) {
        SourceObject = Owner;
        Damage = Amount;
        OrbitAngle = Angle;
        OrbitRadius = 1.0f;
        State = Owner->State;
        Velocity = Owner->Velocity;
        ExpireTick = ab_Global::ArcadeTickCount + 1000;
        Image = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), u"GAI,Bm.AB.w18_f"_wref.get(), u"GAI,Bm.AB.w18_s"_wref.get(), false);
        ab_WorldImage::ab_WorldImage_SetFrameMode(Image, ab_WorldImage::afmRandomStart);
        ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
    }

    void TabW18_Advance(TabW18* Self) {
        ab_Object::TabObject* Collision{};
        ab_Object::TabObject* NextNeighbor{};
        ab_Object::TabObject* Obj{};
        float PositiveDelta{};
        float NegativeDelta{};
        float Delta{};
        ab_Object::TabObject_Advance(Self);
        if (!Self->Exploding) {
            if (Self->SourceObject != nullptr) {
                if (Self->OrbitRadius < 1.5E+2L) {
                    Self->OrbitRadius = Self->OrbitRadius + 0.5L;
                }
                if (Self->OrbitRadius < 1.0E+2L) {
                    Self->OrbitRadius = Self->OrbitRadius + 0.5L;
                }
                if (Self->OrbitRadius < 5.0E+1L) {
                    Self->OrbitRadius = Self->OrbitRadius + 0.5L;
                }
                if (ab_Global::ArcadeTickCount % 3 == 1) {
                    Self->OrbitAngle = aMyFunction::WrapHeadingDegrees(Self->OrbitAngle + 3.75L + Self->AngleCorrection);
                    Self->AngleCorrection = 0.0f;
                } else {
                    Self->OrbitAngle = aMyFunction::WrapHeadingDegrees(Self->OrbitAngle + 3.75L);
                }
                Self->State = ab_Global::AdvanceSphericalStateOnCurrentSphere(ab_Global::MakeSphericalBearingState(Self->SourceObject->State.LongitudeDegrees, Self->SourceObject->State.PolarAngleDegrees, Self->OrbitAngle), Self->OrbitRadius);
                if (ab_Global::ArcadeTickCount % 3 == 0) {
                    Collision = nullptr;
                    NextNeighbor = nullptr;
                    PositiveDelta = 0.0f;
                    NegativeDelta = 0.0f;
                    Obj = ab_Object::FirstArcadeObject;
                    while (Obj != nullptr) {
                        if (pas::class_cast_if<TabW18*>(Obj) != nullptr && Obj != Self && static_cast<TabW18*>(Obj)->SourceObject == Self->SourceObject) {
                            Delta = aMyFunction::WrapSignedHeadingDegrees(static_cast<long double>(pas::checked_cast<TabW18*>(Obj)->OrbitAngle) - Self->OrbitAngle);
                            if (Delta < 0.0L && (Collision == nullptr || NegativeDelta < Delta)) {
                                NegativeDelta = Delta;
                                Collision = Obj;
                            }
                            if (Delta >= 0.0L && (NextNeighbor == nullptr || PositiveDelta > Delta)) {
                                PositiveDelta = Delta;
                                NextNeighbor = Obj;
                            }
                        }
                        Obj = Obj->Next;
                    }
                    if (Collision != nullptr) {
                        auto& cpp_target = pas::checked_cast<TabW18*>(Collision)->AngleCorrection;
                        cpp_target = pas::checked_cast<TabW18*>(Collision)->AngleCorrection - (1.8E+2L + NegativeDelta) * 0.03L;
                    }
                    if (NextNeighbor != nullptr) {
                        auto& cpp_target_2 = pas::checked_cast<TabW18*>(NextNeighbor)->AngleCorrection;
                        cpp_target_2 = pas::checked_cast<TabW18*>(NextNeighbor)->AngleCorrection + (1.8E+2L - PositiveDelta) * 0.03L;
                    }
                }
            }
            Self->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            ab_WorldImage::ab_WorldImage_SetPosition(Self->Image, Self->GetWorldPosition());
        }
        Collision = nullptr;
        if (!Self->Exploding) {
            Collision = Self->FindCollision();
            if (Collision == Self->SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > Self->ExpireTick || Collision != nullptr || Self->SourceObject == nullptr) && static_cast<std::uint8_t>(Self->Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Self->Damage, Self->SourceObject, false);
                Collision->State.BearingDegrees = Collision->State.BearingDegrees - 3.0E+1L;
            }
            Self->Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Self->Image, Self->GetWorldPosition(), u"GAI,Bm.AB.w18a_f"_wref.get(), u"GAI,Bm.AB.w18a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Self->Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Self->Image, false);
        } else if (Self->Exploding) {
            Self->DeletionPending = Self->Image->Finished;
        }
    }

    void TabW18::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW18::p_destroy() {
        ab_W18::TabW18_Destroy(this);
    }

    void TabW18::virtual_TabObject_Advance() {
        ab_W18::TabW18_Advance(this);
    }

} // namespace ab_W18
