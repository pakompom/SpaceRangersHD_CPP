#include "layout/abWall.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_Image.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/ab_Zone.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Math.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/abWall.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Hit.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_WorldImage.hpp"

namespace abWall {
    TabWall* FindStopPoint(ab_StopLine::PabStopPoint Point);

    std::uint32_t BarrierColor = 0x30ffac00u;

    pas::Array<std::uint32_t, 0, 1> BarrierHaloColors = pas::Array<std::uint32_t, 0, 1>{{0x40ffdb00u, 0x20ffac00u}};

    TabWall* ab_Wall_FindZone(ab_Zone::PabZone Zone) {
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabWall*>(Obj) != nullptr && reinterpret_cast<TabWall*>(Obj)->Zone == Zone) {
                return pas::checked_cast<TabWall*>(Obj);
            }
            Obj = Obj->Next;
        }
        return nullptr;
    }

    void ab_Wall_BuildBarrierImages() {
        ab_StopLine::PabStopLine ImageLine{};
        ab_StopLine::PabStopPoint First{};
        ab_StopLine::PabStopPoint Last{};
        std::int32_t Index{};
        ab_StopLine::PabStopLine Line = ab_StopLine::FirstStopLine;
        while (Line != nullptr) {
            if (Line->Collidable && abWall::FindStopPoint(Line->First) != nullptr && abWall::FindStopPoint(Line->Last) != nullptr) {
                ImageLine = ab_StopLine::ab_StopLine_Add();
                ImageLine->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Line));
                ImageLine->First = Line->First;
                ImageLine->Last = Line->Last;
                ImageLine->FirstColor = &BarrierColor;
                ImageLine->LastColor = &BarrierColor;
                ImageLine->Collidable = false;
                ImageLine->Visible = true;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 1); cpp_range.next(Index); ) {
                    First = ab_StopLine::ab_StopPoint_Add();
                    First->Radius = static_cast<long double>((Index + 1) * 20) + ab_Global::SphereRadius;
                    First->Longitude = ImageLine->First->Longitude;
                    First->PolarAngle = ImageLine->First->PolarAngle;
                    First->Kind = 1;
                    ab_StopLine::ab_StopPoint_UpdatePosition(First);
                    Last = ab_StopLine::ab_StopPoint_Add();
                    Last->Radius = static_cast<long double>((Index + 1) * 20) + ab_Global::SphereRadius;
                    Last->Longitude = ImageLine->Last->Longitude;
                    Last->PolarAngle = ImageLine->Last->PolarAngle;
                    Last->Kind = 1;
                    ab_StopLine::ab_StopPoint_UpdatePosition(Last);
                    ImageLine = ab_StopLine::ab_StopLine_Add();
                    ImageLine->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Line));
                    ImageLine->First = First;
                    ImageLine->Last = Last;
                    ImageLine->FirstColor = &BarrierHaloColors[Index];
                    ImageLine->LastColor = &BarrierHaloColors[Index];
                    ImageLine->Collidable = false;
                    ImageLine->Visible = true;
                }
            }
            Line = Line->Next;
        }
    }

    void TabWall_Create(TabWall* Self) {
        ab_Hit::TabHit_Create(Self);
        Self->TurnSpeedScale = 1.0;
        Self->DisruptUntilTick = 0;
        Self->Health = 200;
        Self->MaxHealth = 200;
        Self->WallCollisionEnabled = true;
    }

    void TabWall_Destroy(TabWall* Self) {
        if (Self->WorldImage != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Self->WorldImage);
            Self->WorldImage = nullptr;
        }
        ab_Hit::TabHit_Destroy(Self);
    }

    void TabWall::BindZone(ab_Zone::PabZone Value) {
        Zone = Value;
        if (Value->Name != u"") {
            WorldImage = ab_WorldImage::ab_WorldImage_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), pas::concat_wide({u"GAI,Bm.ABWall.", GR_Main::GiResourceSuffix(), u".", Value->Name}), u""_wref.get(), true);
            ab_WorldImage::ab_WorldImage_SetDepth(WorldImage, ab_Global::WorldImageFrontDepth, ab_Global::WorldImageBackDepth);
            DirectionFrameCount = EC_Str::CountDelimitedPartsW(Value->Name, u"_"_wref.get());
            DirectionFrameCount = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Value->Name, DirectionFrameCount - 1, u"_"_wref.get()));
        }
        DirectionFrameCount = 32;
        EffectOriginSpread = GR_Main::GiScalePixels(20);
        Mass = 1.0E+1;
        State.PolarAngleDegrees = 0.0;
        State.BearingDegrees = 0.0;
        CollisionRadius = 11.0;
        ZoneRadius = Value->Radius;
    }

    void TabWall::AttachVisual() {
    }

    void TabWall::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    void TabWall::ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) {
        ab_StopLine::PabStopLine Line{};
        ab_StopLine::PabStopLine Next{};
        ab_StopLine::PabStopLine Auxiliary{};
        std::uint8_t Changed{};
        if (Health > 0) {
            ab_Hit::TabHit::ApplyDamage(Amount, Source, Disrupt);
            if (Health <= 0 && StopPoint != nullptr) {
                Changed = false;
                Line = ab_StopLine::FirstStopLine;
                while (Line != nullptr) {
                    if (StopPoint == Line->First || StopPoint == Line->Last && Line->Collidable) {
                        Line->Collidable = false;
                        Changed = true;
                        Next = ab_StopLine::FirstStopLine;
                        while (Next != nullptr) {
                            Auxiliary = Next;
                            Next = Next->Next;
                            if (Auxiliary->UserValue == static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Line))) {
                                ab_StopLine::ab_StopLine_Delete(Auxiliary);
                            }
                        }
                    }
                    Line = Line->Next;
                }
                if (Changed) {
                    ab_StopLine::ab_StopLine_BuildCollisionList();
                }
            }
            if (Health <= 0) {
                Zone->DamagePerTick = 0;
                Zone->GravityStrength = 0;
            }
        }
    }

    void TabWall::UpdateState() {
        ab_Hit::TabHit::UpdateState();
    }

    void TabWall::Advance() {
        if (Health == 0) {
            Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
            Thrust = 0.0;
            if (WorldImage != nullptr) {
                ab_WorldImage::ab_WorldImage_Delete(WorldImage);
                WorldImage = nullptr;
            }
        } else if (WorldImage != nullptr) {
            ab_WorldImage::ab_WorldImage_SetPosition(WorldImage, GetWorldPosition());
        }
    }

    void TabWall::UpdateVisuals() {
        std::int32_t Frame{};
        float Value{};
        EC_Struct::TVector3D Position{};
        ab_Hit::TabHit::UpdateVisuals();
        if (WorldImage != nullptr && WorldImage->Image->GaiImageControl != nullptr) {
            Position = GetWorldPosition();
            Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Position));
            Value = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Position.X, -Position.Y));
            Frame = System::Round(pas::real_divide(Value, 3.6E+2L) * DirectionFrameCount);
            if (Frame >= DirectionFrameCount) {
                Frame = 0;
            }
            Value = pas::real_divide(System::Sqrt(pas::sqr(static_cast<pas::Extended>(Position.X)) + pas::sqr(static_cast<pas::Extended>(Position.Y))), ab_Global::SphereProjectedRadius);
            Frame += System::Round((pas::real_divide(WorldImage->Image->GaiImageControl->SequenceFrameCount, DirectionFrameCount) - 1.0L) * Value) * DirectionFrameCount;
            WorldImage->Image->GaiImageControl->SetSequenceFrame(Frame);
        }
    }

    TabWall* FindStopPoint(ab_StopLine::PabStopPoint Point) {
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<TabWall*>(Obj) != nullptr && reinterpret_cast<TabWall*>(Obj)->StopPoint == Point) {
                return pas::checked_cast<TabWall*>(Obj);
            }
            Obj = Obj->Next;
        }
        return nullptr;
    }

    void TabWall::p_destroy() {
        abWall::TabWall_Destroy(this);
    }

} // namespace abWall
