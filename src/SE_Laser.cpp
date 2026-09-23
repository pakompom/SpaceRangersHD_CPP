#include "layout/SE_Laser.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aMyFunction.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_RotateImage2.hpp"
#include "units/Math.hpp"
#include "units/SE_Laser.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"

namespace SE_Laser {
    void TLaserSE_Destroy(TLaserSE* Self) {
        if (Self->FrameImages != nullptr) {
            pas::free(Self->FrameImages);
            Self->FrameImages = nullptr;
        }
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TLaserSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (!IsAttachedToSpace()) {
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            RebuildSegments();
        }
    }

    void TLaserSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            ClearSegments();
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TLaserSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            RebuildSegments();
        }
    }

    void TLaserSE::RebuildSegments() {
        ClearSegments();
        double Angle = Math::ArcTan2(static_cast<long double>(TargetPosition.X) - Position.X, -(static_cast<long double>(TargetPosition.Y) - Position.Y));
        double AngleSin = System::Sin(Angle);
        double AngleCos = System::Cos(Angle);
        std::int32_t ImageAngle = System::Round(pas::real_divide(Angle, aMyFunction::GamePi) * 127.0L) & 0x000000ff;
        double Distance = pas::real_divide(SegmentSize, 2.0L);
        double BeamLength = System::Sqrt(pas::sqr(static_cast<long double>(TargetPosition.X) - Position.X) + pas::sqr(static_cast<long double>(TargetPosition.Y) - Position.Y));
        Segments = pas::make_object<pas::List>();
        GI_RotateImage2::TRotateImage2GI* Segment = nullptr;
        while (Distance < BeamLength) {
            Segment = pas::construct_call<GI_RotateImage2::TRotateImage2GI>(GI_RotateImage2::TRotateImage2GI_Create, Space->MapPanel);
            Segment->SetPositionModeW(true);
            Segment->SetDepthByName(DepthExpression);
            EndPosition = EC_Struct::MakePointF(static_cast<long double>(AngleSin) * Distance + Position.X, Position.Y - static_cast<long double>(AngleCos) * Distance);
            Segment->SetPosition(EC_Struct::TruncatePointF(EndPosition));
            Segment->SetAngle(ImageAngle);
            Segment->SetAlpha(192);
            {
                Types::TPoint point = ClassesImports::Point(SegmentSize / 2, SegmentSize / 2);
                pas::WideString textAt = FrameImages->GetTextAt(0);
                Types::TPoint point_2 = ClassesImports::Point(SegmentSize, SegmentSize);
                Segment->SetImage(std::move(textAt), point_2, point);
            }
            pas::list_add(Segments, reinterpret_cast<void*>(Segment));
            Distance = static_cast<long double>(Distance) + SegmentSize - 4.0L;
        }
        // Native code retains this empty check of the final segment.
        static_cast<void>(Segment != nullptr);
        {
            float cpp_arg = pas::real_divide(SegmentSize, 2.0L) * AngleSin + EndPosition.X;
            float cpp_arg_2 = EndPosition.Y - pas::real_divide(SegmentSize, 2.0L) * AngleCos;
            EndPosition = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
        }
        FrameIndex = 0;
        UpdateSegmentImages();
        StartAnimationTimer();
    }

    void TLaserSE::ClearSegments() {
        std::int32_t Index{};
        GI_MessageLoop::TObjectGI* Segment{};
        StopAnimationTimer();
        if (Segments != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Segments) - 1); cpp_range.next(Index); ) {
                Segment = pas::list_at<GI_MessageLoop::TObjectGI>(Segments, Index);
                Segment->SetActive(false);
                Space->MapPanel->FreeOwnedChild(Segment);
            }
            pas::free(Segments);
        }
    }

    void TLaserSE::UpdateSegmentImages() {
        std::int32_t Index{};
        GI_RotateImage2::TRotateImage2GI* Segment{};
        if (Segments != nullptr && FrameIndex >= 0 && FrameImages != nullptr && FrameIndex < FrameImages->GetCount()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Segments) - 1); cpp_range.next(Index); ) {
                Segment = pas::list_at<GI_RotateImage2::TRotateImage2GI>(Segments, Index);
                {
                    Types::TPoint point = ClassesImports::Point(SegmentSize / 2, SegmentSize / 2);
                    pas::WideString textAt = FrameImages->GetTextAt(FrameIndex);
                    Types::TPoint point_2 = ClassesImports::Point(SegmentSize, SegmentSize);
                    Segment->SetImage(std::move(textAt), point_2, point);
                }
            }
        }
    }

    void TLaserSE::StartAnimationTimer() {
        StopAnimationTimer();
        if (!ManualAnimation) {
            AnimationTimer = Space->Screen->ScheduleCallbackTimer(FrameInterval, FrameInterval, pas::bind_method<&TLaserSE::AdvanceAnimationTimer>(this), 0);
        }
    }

    void TLaserSE::StopAnimationTimer() {
        if (AnimationTimer != nullptr) {
            Space->Screen->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
    }

    void TLaserSE::AdvanceAnimationTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ++FrameIndex;
        if (FrameIndex < FrameImages->GetCount()) {
            UpdateSegmentImages();
        } else {
            FrameIndex = 0;
            UpdateSegmentImages();
        }
    }

    void TLaserSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Index{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        if (FrameImages != nullptr) {
            pas::free(FrameImages);
            FrameImages = nullptr;
        }
        FrameImages = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        FrameInterval = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Time"sv)));
        Index = 0;
        while (Block->CountParams(EC_Str::IntToWideString(Index)) > 0) {
            {
                const pas::WideString& trimWideString = EC_Str::TrimWideString(Block->GetParam(pas::view(EC_Str::IntToWideString(Index))));
                EC_Str::TStringsEC* frameImages = FrameImages;
                frameImages->Add(trimWideString);
            }
            ++Index;
        }
        SegmentSize = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"RadiusUnit"sv)));
    }

    void TLaserSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
        if (Block->CountParams(u"PosDes"_wref.get()) > 0) {
            TargetPosition = EC_Struct::PointToPointF(GI_Main::GetPointGI(pas::view(Block->GetParam(u"PosDes"sv))));
        }
        if (Block->CountParams(u"ManualAnim"_wref.get()) > 0) {
            ManualAnimation = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"ManualAnim"sv)));
        }
    }

    void TLaserSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        std::int32_t Index{};
        GI_RotateImage2::TRotateImage2GI* Segment = pas::construct_call<GI_RotateImage2::TRotateImage2GI>(GI_RotateImage2::TRotateImage2GI_Create, Owner);
        std::int32_t Count = FrameImages->GetCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            {
                Types::TPoint point = ClassesImports::Point(SegmentSize / 2, SegmentSize / 2);
                pas::WideString textAt = FrameImages->GetTextAt(Index);
                Types::TPoint point_2 = ClassesImports::Point(SegmentSize, SegmentSize);
                Segment->SetImage(std::move(textAt), point_2, point);
            }
            Segment->QueueImageLoad(PendingLoads);
        }
        pas::free(Segment);
    }

    void TLaserSE::p_destroy() {
        SE_Laser::TLaserSE_Destroy(this);
    }

} // namespace SE_Laser
