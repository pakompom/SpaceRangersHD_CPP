#include "layout/ab_W18.hpp"
#include "units/EC_Struct.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W18.hpp"
#include "units/ab_WorldImage.hpp"

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

    void TabW18::Advance() {
        ab_Object::TabObject* Collision{};
        ab_Object::TabObject* NextNeighbor{};
        ab_Object::TabObject* Obj{};
        float PositiveDelta{};
        float NegativeDelta{};
        float Delta{};
        ab_Object::TabObject::Advance();
        if (!Exploding) {
            if (SourceObject != nullptr) {
                if (OrbitRadius < 1.5E+2L) {
                    OrbitRadius = OrbitRadius + 0.5L;
                }
                if (OrbitRadius < 1.0E+2L) {
                    OrbitRadius = OrbitRadius + 0.5L;
                }
                if (OrbitRadius < 5.0E+1L) {
                    OrbitRadius = OrbitRadius + 0.5L;
                }
                if (ab_Global::ArcadeTickCount % 3 == 1) {
                    OrbitAngle = aMyFunction::WrapHeadingDegrees(OrbitAngle + 3.75L + AngleCorrection);
                    AngleCorrection = 0.0f;
                } else {
                    OrbitAngle = aMyFunction::WrapHeadingDegrees(OrbitAngle + 3.75L);
                }
                State = ab_Global::AdvanceSphericalStateOnCurrentSphere(ab_Global::MakeSphericalBearingState(SourceObject->State.LongitudeDegrees, SourceObject->State.PolarAngleDegrees, OrbitAngle), OrbitRadius);
                if (ab_Global::ArcadeTickCount % 3 == 0) {
                    Collision = nullptr;
                    NextNeighbor = nullptr;
                    PositiveDelta = 0.0f;
                    NegativeDelta = 0.0f;
                    Obj = ab_Object::FirstArcadeObject;
                    while (Obj != nullptr) {
                        if (pas::class_cast_if<TabW18*>(Obj) != nullptr && Obj != this && static_cast<TabW18*>(Obj)->SourceObject == SourceObject) {
                            Delta = aMyFunction::WrapSignedHeadingDegrees(static_cast<long double>(pas::checked_cast<TabW18*>(Obj)->OrbitAngle) - OrbitAngle);
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
                        pas::checked_cast<TabW18*>(Collision)->AngleCorrection = pas::checked_cast<TabW18*>(Collision)->AngleCorrection - (1.8E+2L + NegativeDelta) * 0.03L;
                    }
                    if (NextNeighbor != nullptr) {
                        pas::checked_cast<TabW18*>(NextNeighbor)->AngleCorrection = pas::checked_cast<TabW18*>(NextNeighbor)->AngleCorrection + (1.8E+2L - PositiveDelta) * 0.03L;
                    }
                }
            }
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            ab_WorldImage::ab_WorldImage_SetPosition(Image, GetWorldPosition());
        }
        Collision = nullptr;
        if (!Exploding) {
            Collision = FindCollision();
            if (Collision == SourceObject) {
                Collision = nullptr;
            }
        }
        if ((ab_Global::ArcadeTickCount > ExpireTick || Collision != nullptr || SourceObject == nullptr) && static_cast<std::uint8_t>(Exploding ^ 1)) {
            if (Collision != nullptr) {
                Collision->ApplyDamage(Damage, SourceObject, false);
                Collision->State.BearingDegrees = Collision->State.BearingDegrees - 3.0E+1L;
            }
            Exploding = true;
            ab_WorldImage::ab_WorldImage_Set(Image, GetWorldPosition(), u"GAI,Bm.AB.w18a_f"_wref.get(), u"GAI,Bm.AB.w18a_s"_wref.get());
            ab_WorldImage::ab_WorldImage_SetDepth(Image, ab_Global::HitFrontDepth, ab_Global::HitBackDepth);
            ab_WorldImage::ab_WorldImage_SetLooping(Image, false);
        } else if (Exploding) {
            DeletionPending = Image->Finished;
        }
    }

    void TabW18::UpdateVisuals() {
        ab_Object::TabObject::UpdateVisuals();
    }

    void TabW18::p_destroy() {
        ab_W18::TabW18_Destroy(this);
    }

} // namespace ab_W18
