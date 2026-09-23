#include "layout/SE_Sputnik.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Globals.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Planet.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Sputnik.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Sputnik {
    // Does nothing when satellite graphics are disabled.
    void TSputnikSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (!GlobalsV::SputnikShow) {
            return;
        }
        if (IsAttachedToSpace()) {
            return;
        }
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        PlanetControl = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Space->MapPanel);
        PlanetControl->SetPositionModeW(true);
        {
            std::int32_t trunc = System::Trunc(Position.Y);
            std::int32_t trunc_2 = System::Trunc(Position.X);
            PlanetControl->SetPosition(ClassesImports::Point(trunc_2, trunc));
        }
        PlanetControl->SetSurfaceMapOffset(SurfaceMapOffset);
        RebuildOrbitTransform();
        UpdateOrbitDisplay();
        OrbitTimer = Space->Screen->ScheduleCallbackTimer(OrbitTimerInterval, OrbitTimerInterval, pas::bind_method<&TSputnikSE::AdvanceOrbitTimer>(this), 0);
        RotationTimer = Space->Screen->ScheduleCallbackTimer(RotationTimerInterval, RotationTimerInterval, pas::bind_method<&TSputnikSE::AdvanceRotationTimer>(this), 0);
    }

    void TSputnikSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (OrbitTimer != nullptr) {
            Space->Screen->CancelCallbackTimer(OrbitTimer);
            OrbitTimer = nullptr;
        }
        if (RotationTimer != nullptr) {
            Space->Screen->CancelCallbackTimer(RotationTimer);
            RotationTimer = nullptr;
        }
        pas::free(PlanetControl);
        PlanetControl = nullptr;
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TSputnikSE::SetOrbitCenter(EC_Struct::TPointF Center) {
        OrbitCenter = Center;
        UpdateOrbitDisplay();
    }

    EC_Struct::TPointF TSputnikSE::GetOrbitCenter() {
        EC_Struct::TPointF Result{};
        Result = OrbitCenter;
        return Result;
    }

    // Returns a new buffer owned by the caller; excludes OrbitAngle.
    EC_Buf::TBufEC* TSputnikSE::BuildStateBuffer() {
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddAnsiChar(DepthOrder);
        Buffer->AddSingle(OrbitInclination);
        Buffer->AddSingle(OrbitRotation);
        Buffer->AddSingle(OrbitAngleStep);
        Buffer->AddDWord(OrbitTimerInterval);
        Buffer->AddSingle(OrbitRadius);
        Buffer->AddIntegerValue(MinDisplayRadius);
        Buffer->AddIntegerValue(MaxDisplayRadius);
        Buffer->AddDWord(RotationTimerInterval);
        Buffer->AddIntegerValue(SurfaceMapStep);
        return Buffer;
    }

    // Rewinds Buffer to zero and rebuilds the orbit transform and display position.
    void TSputnikSE::LoadStateBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->SetPosition(0);
        DepthOrder = EC_Buf::TBufEC_GetByte(Buffer);
        OrbitInclination = EC_Buf::TBufEC_GetSingle(Buffer);
        OrbitRotation = EC_Buf::TBufEC_GetSingle(Buffer);
        OrbitAngleStep = EC_Buf::TBufEC_GetSingle(Buffer);
        OrbitTimerInterval = EC_Buf::TBufEC_GetUInt32(Buffer);
        OrbitRadius = EC_Buf::TBufEC_GetSingle(Buffer);
        MinDisplayRadius = EC_Buf::TBufEC_GetInt32(Buffer);
        MaxDisplayRadius = EC_Buf::TBufEC_GetInt32(Buffer);
        RotationTimerInterval = EC_Buf::TBufEC_GetUInt32(Buffer);
        SurfaceMapStep = EC_Buf::TBufEC_GetInt32(Buffer);
        RebuildOrbitTransform();
        UpdateOrbitDisplay();
    }

    void TSputnikSE::RebuildOrbitTransform() {
        float Angle = aMyFunction::HeadingDegreesToRadians(OrbitRotation);
        RotationCos = System::Cos(Angle);
        RotationSin = System::Sin(Angle);
        Angle = aMyFunction::HeadingDegreesToRadians(OrbitInclination);
        InclinationCos = System::Cos(Angle);
        InclinationSin = System::Sin(Angle);
        MaxOrbitDepth = std::fabs(static_cast<long double>(-InclinationSin) * OrbitRadius);
        MinOrbitDepth = -MaxOrbitDepth;
    }

    // Requires a nonzero depth range when attached; updates position, apparent radius and drawing depth.
    void TSputnikSE::UpdateOrbitDisplay() {
        Globals::TSputnikTempl* Template{};
        if (!IsAttachedToSpace()) {
            return;
        }
        float Angle = aMyFunction::HeadingDegreesToRadians(OrbitAngle);
        float OrbitX = System::Sin(Angle) * OrbitRadius;
        float OrbitY = System::Cos(Angle) * -OrbitRadius;
        float X = static_cast<long double>(InclinationCos) * RotationCos * OrbitX + static_cast<long double>(-RotationSin) * OrbitY + OrbitCenter.X;
        float Y = static_cast<long double>(InclinationCos) * RotationSin * OrbitX + static_cast<long double>(OrbitY) * RotationCos + OrbitCenter.Y;
        float Z = static_cast<long double>(-InclinationSin) * OrbitX;
        Position = EC_Struct::MakePointF(X, Y);
        DisplayRadius = System::Round(pas::real_divide(static_cast<long double>(Z) - MinOrbitDepth, static_cast<long double>(MaxOrbitDepth) - MinOrbitDepth) * (MaxDisplayRadius - MinDisplayRadius) + MinDisplayRadius);
        if (DisplayRadius < MinDisplayRadius) {
            DisplayRadius = MinDisplayRadius;
        } else if (DisplayRadius > MaxDisplayRadius) {
            DisplayRadius = MaxDisplayRadius;
        }
        if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) < 768) {
            DisplayRadius = System::Round(pas::real_divide(DisplayRadius * 800, 1024.0L));
        }
        LightAngle = System::Round(pas::real_divide(pas::real_divide(Math::ArcTan2(-Position.X, Position.Y) * 1.8E+2L, aMyFunction::GamePi) * 256.0L, 3.6E+2L));
        std::int32_t Index = DisplayRadius - GlobalsV::MinimumSatelliteTemplateRadius;
        Template = pas::list_at<Globals::TSputnikTempl>(GlobalsV::SatelliteRenderTemplates, Index);
        PlanetControl->SetImageFromTemplate(Template->MaskName, ImagePath, Template->Radius);
        PlanetControl->SetLightAngle(LightAngle);
        PlanetControl->SetPosition(EC_Struct::TruncatePointF(Position));
        PlanetControl->SetOrigin(ClassesImports::Point(Template->Radius, Template->Radius));
        if (Z < 0.0L) {
            PlanetControl->SetDepth(static_cast<long double>(DepthOrder) + GlobalsV::PlanetDepth + 1.0L);
        } else {
            PlanetControl->SetDepth(static_cast<long double>(GlobalsV::PlanetDepth) - DepthOrder - 1.0L);
        }
    }

    void TSputnikSE::AdvanceOrbitTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        OrbitAngle = aMyFunction::WrapHeadingDegrees(static_cast<long double>(OrbitAngle) + OrbitAngleStep);
        UpdateOrbitDisplay();
    }

    void TSputnikSE::AdvanceRotationTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        SurfaceMapOffset += SurfaceMapStep;
        PlanetControl->SetSurfaceMapOffset(SurfaceMapOffset);
    }

    void TSputnikSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"sv);
    }

    void TSputnikSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TSputnikSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        Globals::TSputnikTempl* Template{};
        Template = pas::list_at<Globals::TSputnikTempl>(GlobalsV::SatelliteRenderTemplates, 0);
        {
            GI_Planet::TPlanetGI* cpp_with = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Owner);
            cpp_with->SetImageFromTemplate(Template->MaskName, ImagePath, Template->Radius);
            cpp_with->QueueImageLoad(PendingLoads);
            pas::free(cpp_with);
        }
    }

} // namespace SE_Sputnik
