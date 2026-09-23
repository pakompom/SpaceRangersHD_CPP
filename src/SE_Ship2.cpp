#include "layout/SE_Ship2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_AlphaImage.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/GI_Tail.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Ship2.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"

// Native class and methods:.
namespace SE_Ship2 {
    void TShip2AnimSE_Destroy(TShip2AnimSE* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TShip2AnimSE::Clear() {
        if (Frames != nullptr) {
            Frames = nullptr;
        }
        if (Delays != nullptr) {
            Delays = nullptr;
        }
        Weight = 1;
        FrameCount = 0;
    }

    void TShip2AnimSE::Load(pas::WideString Specification) {
        std::int32_t FrameOffset{};
        std::int32_t Count{};
        std::int32_t Delay{};
        std::int32_t First{};
        std::int32_t Last{};
        pas::WideString RangeText{};
        Clear();
        std::int32_t Index = EC_Str::CountDelimitedPartsW(pas::view(Specification), u","sv);
        if (Index < 3) {
            pas::raise(pas::make_exception<pas::Exception>("Error in TShip2AnimSE.Load"_a));
        }
        Weight = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Specification), 0, u","sv)));
        Specification = EC_Str::ExtractDelimitedRangeW(pas::view(Specification), 1, Index - 1, u","sv);
        std::int32_t RangeCount = (EC_Str::CountDelimitedPartsW(pas::view(Specification), u"[]"sv) - 1) / 2;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range.next(Index); ) {
            RangeText = EC_Str::ExtractDelimitedPartW(pas::view(Specification), Index * 2 + 1, u"[]"sv);
            Delay = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(RangeText), 0, u",-"sv)));
            First = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(RangeText), 1, u",-"sv)));
            Last = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(RangeText), 2, u",-"sv)));
            Count = pas::abs(First - Last) + 1;
            FrameCount += Count;
            Frames.set_length(FrameCount);
            Delays.set_length(FrameCount);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(FrameOffset); ) {
                Frames[FrameCount - Count + FrameOffset] = First;
                Delays[FrameCount - Count + FrameOffset] = Delay;
                if (First < Last) {
                    ++First;
                } else {
                    --First;
                }
            }
        }
    }

    void TShip2SE_CreateEmpty(TShip2SE* Self) {
        SE_Space::TObjectSE_CreateEmpty(Self);
        Self->AlphaLimit = 255;
        Self->AngleOverride = -1;
    }

    void TShip2SE_Create(TShip2SE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition) {
        Self->AngleOverride = -1;
        if (EC_Str::CountDelimitedPartsW(pas::view(GraphKey), u","sv) > 1) {
            SE_Space::TObjectSE_Create(Self, EC_Str::ExtractDelimitedPartW(pas::view(GraphKey), 0, u","sv), UnusedPosition);
            Self->AlphaLimit = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(GraphKey), 1, u","sv)));
        } else {
            SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
            Self->AlphaLimit = 255;
        }
    }

    void TShip2SE_Destroy(TShip2SE* Self) {
        Self->StopStateTimer();
        Self->StopAnimationTimer();
        Self->CurrentAnimation = nullptr;
        Self->NextAnimation = nullptr;
        if (!Self->SharedAnimations) {
            while (Self->FirstAnimation != nullptr) {
                Self->DeleteAnimation(Self->LastAnimation);
            }
            while (Self->FirstReducedAnimation != nullptr) {
                Self->DeleteReducedAnimation(Self->LastReducedAnimation);
            }
        }
        Self->SharedAnimations = false;
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TShip2SE::CopyTo(SE_Space::TObjectSE* Destination) {
        std::int32_t Index{};
        SE_Space::TObjectSE::CopyTo(Destination);
        TShip2SE* Ship = pas::checked_cast<TShip2SE*>(Destination);
        Ship->ImagePath = ImagePath;
        Ship->ReducedImagePath = ReducedImagePath;
        Ship->ImageSize = ImageSize;
        Ship->ImageScale = ImageScale;
        Ship->ImageOrigin = ImageOrigin;
        Ship->ImageCenter = ImageCenter;
        Ship->Angle = Angle;
        Ship->Alpha = Alpha;
        Ship->AlphaLimit = AlphaLimit;
        Ship->MinimapImagePath = MinimapImagePath;
        Ship->MinimapImageOrigin = MinimapImageOrigin;
        Ship->StateIntervalMs = StateIntervalMs;
        Ship->SharedAnimations = true;
        Ship->FirstAnimation = FirstAnimation;
        Ship->LastAnimation = LastAnimation;
        Ship->DefaultAnimation = DefaultAnimation;
        Ship->FirstReducedAnimation = FirstReducedAnimation;
        Ship->LastReducedAnimation = LastReducedAnimation;
        Ship->DefaultReducedAnimation = DefaultReducedAnimation;
        Ship->TotalAnimationWeight = TotalAnimationWeight;
        Ship->TotalReducedAnimationWeight = TotalReducedAnimationWeight;
        for (Index = 1; Index <= 10; ++Index) {
            Ship->TailOrigins[Index] = TailOrigins[Index];
        }
        Ship->TailPrefix = TailPrefix;
        Ship->WeaponPortCount = WeaponPortCount;
        for (Index = 1; Index <= 10; ++Index) {
            Ship->WeaponPorts[Index] = WeaponPorts[Index];
        }
        Ship->SmallSize = SmallSize;
        Ship->LargeSize = LargeSize;
        Ship->TargetSizeScale = TargetSizeScale;
    }

    // Despite the diagnostic name, copies Self into Destination, which must be TShip2SE.
    void TShip2SE::CopyDataFromMirrorImage(SE_Space::TObjectSE* Destination) {
        std::int32_t Index{};
        TShip2SE* Ship = pas::checked_cast<TShip2SE*>(Destination);
        Ship->GraphKey = GraphKey;
        if (Ship->ImagePath != ImagePath || Ship->ReducedImagePath != ReducedImagePath) {
            GR_Main::AppendLogLineThreadSafe("Warning from CopyDataFromMirrorImage: image mismatch"_a);
            Ship->ImagePath = ImagePath;
            Ship->ReducedImagePath = ReducedImagePath;
        }
        Ship->MinimapImagePath = MinimapImagePath;
        TShip2AnimSE* SourceAnimation = FirstAnimation;
        TShip2AnimSE* DestinationAnimation = Ship->FirstAnimation;
        while (SourceAnimation != nullptr && DestinationAnimation != nullptr) {
            if (DefaultAnimation == SourceAnimation) {
                Ship->DefaultAnimation = DestinationAnimation;
            }
            DestinationAnimation->Weight = SourceAnimation->Weight;
            DestinationAnimation = DestinationAnimation->Next;
            SourceAnimation = SourceAnimation->Next;
        }
        if (SourceAnimation != nullptr || DestinationAnimation != nullptr) {
            GR_Main::AppendLogLineThreadSafe("Warning from CopyDataFromMirrorImage: animation mismatch"_a);
        }
        SourceAnimation = FirstReducedAnimation;
        DestinationAnimation = Ship->FirstReducedAnimation;
        while (SourceAnimation != nullptr && DestinationAnimation != nullptr) {
            if (DefaultReducedAnimation == SourceAnimation) {
                Ship->DefaultReducedAnimation = DestinationAnimation;
            }
            DestinationAnimation->Weight = SourceAnimation->Weight;
            DestinationAnimation = DestinationAnimation->Next;
            SourceAnimation = SourceAnimation->Next;
        }
        if (SourceAnimation != nullptr || DestinationAnimation != nullptr) {
            GR_Main::AppendLogLineThreadSafe("Warning from CopyDataFromMirrorImage: animation mismatch"_a);
        }
        Ship->TotalAnimationWeight = TotalAnimationWeight;
        Ship->TotalReducedAnimationWeight = TotalReducedAnimationWeight;
        for (Index = 1; Index <= 10; ++Index) {
            Ship->TailOrigins[Index] = TailOrigins[Index];
        }
        Ship->TailPrefix = TailPrefix;
        Ship->WeaponPortCount = WeaponPortCount;
        for (Index = 1; Index <= 10; ++Index) {
            Ship->WeaponPorts[Index] = WeaponPorts[Index];
        }
        Ship->SmallSize = SmallSize;
        Ship->LargeSize = LargeSize;
        Ship->TargetSizeScale = TargetSizeScale;
    }

    // Native diagnostic name: TShip2SE.Connect.
    void TShip2SE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        std::int32_t Index{};
        std::int32_t Stage = 0;
        try {
            if (IsAttachedToSpace()) {
                return;
            }
            ConfigureLoopSound(u"Ship"_wref.get());
            ConfigureRandomSound(u"Ship"_wref.get());
            Stage = 1;
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            Stage = 2;
            Image = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, nullptr);
            Stage = 3;
            if (Space->MapPanel != nullptr) {
                Space->MapPanel->AttachOwnedChild(Image);
            }
            Stage = 4;
            Image->SetPositionModeW(true);
            Image->SetDepthByName(DepthExpression);
            {
                std::int32_t trunc = System::Trunc(Position.Y);
                std::int32_t trunc_2 = System::Trunc(Position.X);
                Image->SetPosition(ClassesImports::Point(trunc_2, trunc));
            }
            if (AngleOverride >= 0) {
                Image->SetAngle(AngleOverride);
            } else {
                Image->SetAngle(Angle);
            }
            Image->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), ASpace->AlphaShift));
            Stage = 5;
            MinimapImage = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
            Stage = 6;
            MinimapImage->SetPositionModeW(true);
            MinimapImage->SetDepthByName(DepthExpression);
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
            MinimapImage->SetImagePath(MinimapImagePath);
            {
                WindowsSdk::TPoint contentSize = MinimapImage->GetContentSize();
                GI_MessageLoop::TObjectGI* minimapImage = MinimapImage;
                minimapImage->SetSize(contentSize);
            }
            {
                WindowsSdk::TPoint halfPoint = EC_Struct::HalfPoint(MinimapImage->GetContentSize());
                GI_MessageLoop::TObjectGI* minimapImage_2 = MinimapImage;
                minimapImage_2->SetOrigin(halfPoint);
            }
            Stage = 7;
            if (GlobalsV::AnimShipFull) {
                CurrentAnimation = FirstAnimation;
            } else {
                CurrentAnimation = FirstReducedAnimation;
            }
            NextAnimation = CurrentAnimation;
            CurrentFrameIndex = aMyFunction::RandomIntRange(0, CurrentAnimation->FrameCount - 1);
            Stage = 8;
            if (GlobalsV::AnimShipFull) {
                Image->SetImage(ImagePath, Size, EC_Struct::RoundPointF(GetOrbitCenter()));
            } else {
                Image->SetImage(ReducedImagePath, Size, EC_Struct::RoundPointF(GetOrbitCenter()));
            }
            Image->SetFrameIndex(CurrentAnimation->Frames[CurrentFrameIndex]);
            Stage = 9;
            if (TailMode > 0) {
                for (Index = 1; Index <= 10; ++Index) {
                    if (TailOrigins[Index].Y > 0.0L) {
                        Tails[Index] = pas::construct_call<GI_Tail::TTailGI>(GI_Tail::TTailGI_Create, Space->MapPanel);
                        if (TailEmitIntervalMs > 0) {
                            Tails[Index]->EmitIntervalMs = TailEmitIntervalMs;
                        }
                        Tails[Index]->SetSize(ClassesImports::Point(1000000, 1000000));
                        Tails[Index]->SetOrigin(EC_Struct::HalfPoint(Tails[Index]->ClientSize));
                        Tails[Index]->SetDepthByName(u"Tail"_wref.get());
                        Tails[Index]->SetPositionModeW(true);
                        Tails[Index]->SetImagePath(pas::concat_wide({u"Bm.Tail.", TailPrefix, u"0", pas::wide_int_to_str(TailMode - 1)}));
                        Tails[Index]->SetEmitting(Alpha == 255 && Space->AlphaShift == 0);
                    }
                }
            }
            Stage = 10;
            StartAnimationTimer();
            StartStateTimer();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe("TShip2SE.Connect"_a);
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(GraphKey));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"lastLabel=", EC_Str::IntToWideString(GI_RotateImage5::RotateImageConstructionStage)})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"self=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(this)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"sp=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(ASpace)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"FSpace=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Space)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"FImage=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Image)))})));
                if (Space != nullptr) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"PGI=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Space->MapPanel)))})));
                }
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TShip2SE.Connect, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TShip2SE::DetachFromSpace() {
        std::int32_t Index{};
        if (IsAttachedToSpace()) {
            StopStateTimer();
            StopAnimationTimer();
            CurrentAnimation = nullptr;
            NextAnimation = nullptr;
            Image->SetActive(false);
            pas::free(Image);
            Image = nullptr;
            pas::free(MinimapImage);
            MinimapImage = nullptr;
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] != nullptr) {
                    pas::free(Tails[Index]);
                    Tails[Index] = nullptr;
                }
            }
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TShip2SE::SetTailMode(std::int32_t Value) {
        std::int32_t Index{};
        TailMode = Value;
        for (Index = 1; Index <= 10; ++Index) {
            if (Tails[Index] != nullptr) {
                Tails[Index]->SetActive(Value > 0);
                if (Value > 0) {
                    if (TailMode == 1) {
                        if (([&] {
                            pas::WideString cpp_string = Tails[Index]->GetImagePath();
                            pas::WideString cpp_string_2 = pas::concat_wide({u"Bm.Tail.", TailPrefix, u"00"});
                            return cpp_string != cpp_string_2;
                        }())) {
                            Tails[Index]->SetImagePath(pas::concat_wide({u"Bm.Tail.", TailPrefix, u"00"}));
                            Tails[Index]->SetEmitting(Alpha == 255 && Space->AlphaShift == 0);
                            continue;
                        }
                    }
                    if (TailMode == 2) {
                        if (([&] {
                            pas::WideString cpp_string_3 = Tails[Index]->GetImagePath();
                            pas::WideString cpp_string_4 = pas::concat_wide({u"Bm.Tail.", TailPrefix, u"01"});
                            return cpp_string_3 != cpp_string_4;
                        }())) {
                            Tails[Index]->SetImagePath(pas::concat_wide({u"Bm.Tail.", TailPrefix, u"01"}));
                            Tails[Index]->SetEmitting(Alpha == 255 && Space->AlphaShift == 0);
                        }
                    }
                }
            }
        }
    }

    pas::WideString TShip2SE::GetImagePath() {
        if (GlobalsV::AnimShipFull) {
            return ImagePath;
        }
        return ReducedImagePath;
    }

    void TShip2SE::SetSize(Types::TPoint Value) {
        if (Size.X != Value.X || Size.Y != Value.Y) {
            SE_Space::TObjectSE::SetSize(Value);
            ImageScale.X = pas::real_divide(Size.X, ImageSize.X);
            ImageScale.Y = pas::real_divide(Size.Y, ImageSize.Y);
        }
    }

    void TShip2SE::SetPosition(EC_Struct::TPointF APosition) {
        Types::TPoint PixelPosition{};
        std::int32_t Index{};
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            {
                std::int32_t trunc = System::Trunc(APosition.Y);
                std::int32_t trunc_2 = System::Trunc(APosition.X);
                PixelPosition = ClassesImports::Point(trunc_2, trunc);
            }
            Image->SetPosition(PixelPosition);
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] != nullptr) {
                    Tails[Index]->EmitterPosition = ImagePointToWorld(TailOrigins[Index]);
                }
            }
        }
    }

    void TShip2SE::SetTailDepth(float Value) {
        std::int32_t Index{};
        if (TailMode > 0) {
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] == nullptr) {
                    break;
                }
                Tails[Index]->SetDepth(Value);
            }
        }
    }

    void TShip2SE::SetDepth(float Value) {
        Image->SetDepth(Value);
    }

    float TShip2SE::GetDepth() {
        return Image->Depth;
    }

    std::uint8_t TShip2SE::GetAngle() {
        return Angle;
    }

    void TShip2SE::SetAngle(std::uint8_t Value) {
        std::int32_t Index{};
        EC_Struct::TPointF Velocity{};
        Angle = Value;
        if (IsAttachedToSpace()) {
            if (AngleOverride >= 0) {
                Image->SetAngle(AngleOverride);
            } else {
                Image->SetAngle(Angle);
            }
            Velocity.X = 0.0f;
            Velocity.Y = 0.0f;
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] != nullptr) {
                    Tails[Index]->EmitterPosition = ImagePointToWorld(TailOrigins[Index]);
                    Tails[Index]->SegmentVelocity = Velocity;
                }
            }
        }
    }

    void TShip2SE::OffsetTailsAlongHeading(float Distance) {
        float Radians{};
        EC_Struct::TPointF Delta{};
        std::int32_t Index{};
        if (IsAttachedToSpace()) {
            Radians = pas::real_divide(GetAngle(), 256.0L) * pas::constant(2.0L * SystemImports::Pi) + SystemImports::Pi;
            Delta.X = System::Sin(Radians) * Distance;
            Delta.Y = System::Cos(Radians) * -Distance;
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] != nullptr) {
                    Tails[Index]->OffsetSegments(Delta);
                }
            }
        }
    }

    void TShip2SE::OffsetTails(EC_Struct::TPointF Delta) {
        std::int32_t Index{};
        for (Index = 1; Index <= 10; ++Index) {
            if (Tails[Index] != nullptr) {
                Tails[Index]->OffsetSegments(Delta);
            }
        }
    }

    void TShip2SE::SetTailsEmitting(std::uint8_t Value) {
        std::int32_t Index{};
        for (Index = 1; Index <= 10; ++Index) {
            if (Tails[Index] != nullptr) {
                Tails[Index]->SetEmitting(Value);
            }
        }
    }

    std::uint8_t TShip2SE::GetAlpha() {
        return Alpha;
    }

    void TShip2SE::SetAlpha(std::uint8_t Value) {
        std::int32_t Index{};
        Alpha = Value;
        if (IsAttachedToSpace()) {
            Image->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), Space->AlphaShift));
            for (Index = 1; Index <= 10; ++Index) {
                if (Tails[Index] != nullptr) {
                    Tails[Index]->SetEmitting(Alpha == 255 && Space->AlphaShift == 0);
                }
            }
        }
    }

    EC_Struct::TPointF TShip2SE::GetOrbitCenter() {
        EC_Struct::TPointF Result{};
        Result.X = static_cast<long double>(ImageOrigin.X) * ImageScale.X;
        Result.Y = static_cast<long double>(ImageOrigin.Y) * ImageScale.Y;
        return Result;
    }

    EC_Struct::TPointF TShip2SE::ScaleImagePoint(EC_Struct::TPointF Point) {
        EC_Struct::TPointF Result{};
        Result.X = (static_cast<long double>(Point.X) - ImageOrigin.X) * ImageScale.X;
        Result.Y = (static_cast<long double>(Point.Y) - ImageOrigin.Y) * ImageScale.Y;
        return Result;
    }

    EC_Struct::TPointF TShip2SE::ImagePointToWorld(EC_Struct::TPointF Point) {
        EC_Struct::TPointF Result{};
        Point = ScaleImagePoint(Point);
        double Radians = pas::real_divide(Angle, 256.0L) * aMyFunction::GameTwoPi;
        double Sine = System::Sin(Radians);
        double Cosine = System::Cos(Radians);
        Result.X = static_cast<long double>(Point.X) * Cosine - static_cast<long double>(Point.Y) * Sine + Position.X;
        Result.Y = static_cast<long double>(Point.X) * Sine + static_cast<long double>(Point.Y) * Cosine + Position.Y;
        return Result;
    }

    EC_Struct::TPointF TShip2SE::GetTargetPoint(std::uint8_t Heading, std::int32_t Seed) {
        EC_Struct::TPointF Result{};
        EC_Struct::TPointF Point{};
        {
            float cpp_arg = pas::imod(Seed, ImageSize.X);
            float cpp_arg_2 = pas::imod(Seed * 45452 + 3247, ImageSize.Y);
            Point = ScaleImagePoint(EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
        }
        Point.X = static_cast<long double>(Point.X) * TargetSizeScale;
        Point.Y = static_cast<long double>(Point.Y) * TargetSizeScale;
        double Radians = pas::real_divide(Heading, 256.0L) * aMyFunction::GameTwoPi;
        double Sine = System::Sin(Radians);
        double Cosine = System::Cos(Radians);
        Result.X = static_cast<long double>(Point.X) * Cosine - static_cast<long double>(Point.Y) * Sine + Position.X;
        Result.Y = static_cast<long double>(Point.X) * Sine + static_cast<long double>(Point.Y) * Cosine + Position.Y;
        return Result;
    }

    EC_Struct::TPointF TShip2SE::GetWeaponPortPoint(std::uint8_t Heading, std::uint32_t Seed) {
        EC_Struct::TPointF Result{};
        EC_Struct::TPointF Point{};
        if (WeaponPortCount < 1) {
            Result = Position;
            return Result;
        }
        Point = ScaleImagePoint(WeaponPorts[1 + pas::imod(pas::sqr(static_cast<std::int32_t>(Seed)) / 11, WeaponPortCount)]);
        double Radians = pas::real_divide(Heading, 256.0L) * aMyFunction::GameTwoPi;
        double Sine = System::Sin(Radians);
        double Cosine = System::Cos(Radians);
        Result.X = static_cast<long double>(Point.X) * Cosine - static_cast<long double>(Point.Y) * Sine + Position.X;
        Result.Y = static_cast<long double>(Point.X) * Sine + static_cast<long double>(Point.Y) * Cosine + Position.Y;
        return Result;
    }

    std::uint8_t TShip2SE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Image->HitTestPixel(Image->MessageLoop->GetCursorPoint());
    }

    TShip2AnimSE* TShip2SE::AddAnimation() {
        TShip2AnimSE* Animation = pas::make_object<TShip2AnimSE>();
        if (LastAnimation != nullptr) {
            LastAnimation->Next = Animation;
        }
        Animation->Prev = LastAnimation;
        Animation->Next = nullptr;
        LastAnimation = Animation;
        if (FirstAnimation == nullptr) {
            FirstAnimation = Animation;
        }
        return Animation;
    }

    void TShip2SE::DeleteAnimation(TShip2AnimSE* Animation) {
        if (Animation->Prev != nullptr) {
            Animation->Prev->Next = Animation->Next;
        }
        if (Animation->Next != nullptr) {
            Animation->Next->Prev = Animation->Prev;
        }
        if (LastAnimation == Animation) {
            LastAnimation = Animation->Prev;
        }
        if (FirstAnimation == Animation) {
            FirstAnimation = Animation->Next;
        }
        pas::free(Animation);
    }

    TShip2AnimSE* TShip2SE::AddReducedAnimation() {
        TShip2AnimSE* Animation = pas::make_object<TShip2AnimSE>();
        if (LastReducedAnimation != nullptr) {
            LastReducedAnimation->Next = Animation;
        }
        Animation->Prev = LastReducedAnimation;
        Animation->Next = nullptr;
        LastReducedAnimation = Animation;
        if (FirstReducedAnimation == nullptr) {
            FirstReducedAnimation = Animation;
        }
        return Animation;
    }

    void TShip2SE::DeleteReducedAnimation(TShip2AnimSE* Animation) {
        if (Animation->Prev != nullptr) {
            Animation->Prev->Next = Animation->Next;
        }
        if (Animation->Next != nullptr) {
            Animation->Next->Prev = Animation->Prev;
        }
        if (LastReducedAnimation == Animation) {
            LastReducedAnimation = Animation->Prev;
        }
        if (FirstReducedAnimation == Animation) {
            FirstReducedAnimation = Animation->Next;
        }
        pas::free(Animation);
    }

    void TShip2SE::StartAnimationTimer() {
        StopAnimationTimer();
        std::int32_t Delay = CurrentAnimation->Delays[CurrentFrameIndex];
        AnimationTimer = Space->Screen->ScheduleCallbackTimer(Delay, Delay, pas::bind_method<&TShip2SE::AdvanceAnimation>(this), 0);
    }

    void TShip2SE::StopAnimationTimer() {
        if (AnimationTimer != nullptr) {
            Space->Screen->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
    }

    void TShip2SE::StartStateTimer() {
        StopStateTimer();
        SelectNextAnimation(nullptr, 0);
        StateTimer = Space->Screen->ScheduleCallbackTimer(StateIntervalMs, StateIntervalMs, pas::bind_method<&TShip2SE::SelectNextAnimation>(this), 0);
    }

    void TShip2SE::StopStateTimer() {
        if (StateTimer != nullptr) {
            Space->Screen->CancelCallbackTimer(StateTimer);
            StateTimer = nullptr;
        }
    }

    void TShip2SE::AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        {
            WindowsSdk::TRect& cpp_with = Image->HitTestBounds;
            if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) * -0.1L > cpp_with.Right || static_cast<std::uint32_t>(GR_Main::GameScreenWidth) * 1.1L < cpp_with.Left || static_cast<std::uint32_t>(GR_Main::GameScreenHeight) * -0.1L > cpp_with.Bottom || static_cast<std::uint32_t>(GR_Main::GameScreenHeight) * 1.1L < cpp_with.Top) {
                SetTailsEmitting(false);
                return;
            }
        }
        SetTailsEmitting(true);
        ++CurrentFrameIndex;
        if (CurrentAnimation->FrameCount > CurrentFrameIndex) {
            Image->SetFrameIndex(CurrentAnimation->Frames[CurrentFrameIndex]);
            StartAnimationTimer();
            return;
        }
        CurrentFrameIndex = 0;
        if (GlobalsV::AnimShipFull) {
            if (CurrentAnimation == LastAnimation) {
                CurrentAnimation = DefaultAnimation;
            } else {
                CurrentAnimation = NextAnimation;
            }
            NextAnimation = DefaultAnimation;
        } else {
            if (CurrentAnimation == LastReducedAnimation) {
                CurrentAnimation = DefaultReducedAnimation;
            } else {
                CurrentAnimation = NextAnimation;
            }
            NextAnimation = DefaultReducedAnimation;
        }
        Image->SetFrameIndex(CurrentAnimation->Frames[CurrentFrameIndex]);
        StartAnimationTimer();
    }

    void TShip2SE::SelectNextAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Weight{};
        TShip2AnimSE* Animation{};
        if (GlobalsV::AnimShipFull) {
            if (NextAnimation == LastAnimation) {
                return;
            }
            Weight = pas::random(TotalAnimationWeight, &System::RandSeed);
            Animation = FirstAnimation;
            while (Animation != nullptr) {
                if (Weight < Animation->Weight) {
                    NextAnimation = Animation;
                    break;
                }
                Weight -= Animation->Weight;
                Animation = Animation->Next;
            }
        } else {
            if (NextAnimation == LastReducedAnimation) {
                return;
            }
            Weight = pas::random(TotalReducedAnimationWeight, &System::RandSeed);
            Animation = FirstReducedAnimation;
            while (Animation != nullptr) {
                if (Weight < Animation->Weight) {
                    NextAnimation = Animation;
                    break;
                }
                Weight -= Animation->Weight;
                Animation = Animation->Next;
            }
        }
    }

    void TShip2SE::DrawMap() {
        SE_Process::TProcessSE* CurrentProcess = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, CurrentProcess->RadarCenter)) < pas::sqr(CurrentProcess->RadarRange) || AlternateImagePath != u"" && CurrentProcess->RadarRange > 0 || aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Graphic == this) {
            MinimapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
    }

    void TShip2SE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Index{};
        TShip2AnimSE* Animation{};
        TShip2AnimSE* ReducedAnimation{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        SharedAnimations = false;
        if (Block->CountParams(u"AngleOverride"_wref.get()) > 0) {
            AngleOverride = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"AngleOverride"sv)));
        } else {
            AngleOverride = -1;
        }
        SetAngle(0);
        SetAlpha(255);
        ImagePath = Block->GetParam(u"Image"sv);
        ReducedImagePath = Block->GetParam(u"ImageS"sv);
        MinimapImagePath = Block->GetParam(u"ImageMap"sv);
        if (Block->CountParams(u"ImageI"_wref.get()) > 0) {
            AlternateImagePath = Block->GetParam(u"ImageI"sv);
        } else {
            AlternateImagePath = pas::WideString();
        }
        if (Block->CountParams(u"PanelPartnerImage"_wref.get()) > 0) {
            PanelPartnerImage = Block->GetParam(u"PanelPartnerImage"sv);
        } else {
            PanelPartnerImage = pas::WideString();
        }
        ImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImage"sv)));
        MinimapImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImageMap"sv)));
        ImageSize = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SizeImage"sv)));
        ImageCenter = EC_Struct::PointToPointF(GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeCenterImage"sv))));
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 10); cpp_range.next(Index); ) {
            TailOrigins[Index] = EC_Struct::MakePointF(0.0f, 0.0f);
            if (Block->CountParams(pas::concat_wide({u"Tail", EC_Str::IntToWideString(Index)})) > 0) {
                TailOrigins[Index] = EC_Struct::PointToPointF(GI_Main::GetPointGI(pas::view(Block->GetParam(pas::view(pas::concat_wide({u"Tail", EC_Str::IntToWideString(Index)}))))));
            }
        }
        if (Block->CountParams(u"TailPrefix"_wref.get()) > 0) {
            TailPrefix = Block->GetParam(u"TailPrefix"sv);
        } else {
            TailPrefix = pas::WideString();
        }
        WeaponPortCount = 0u;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 10); cpp_range_2.next(Index); ) {
            if (Block->CountParams(pas::concat_wide({u"WeaponPort", EC_Str::IntToWideString(Index)})) <= 0) {
                break;
            }
            WeaponPorts[Index] = EC_Struct::PointToPointF(GI_Main::GetPointGI(pas::view(Block->GetParam(pas::view(pas::concat_wide({u"WeaponPort", EC_Str::IntToWideString(Index)}))))));
            ++WeaponPortCount;
        }
        StateIntervalMs = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"StateTime"sv)));
        EC_BlockPar::TBlockParEC* AnimBlock = Block->GetBlock(u"Anim"sv);
        TShip2AnimSE* Normal = AddAnimation();
        Normal->Load(AnimBlock->GetParam(u"Normal"sv));
        TotalAnimationWeight = Normal->Weight;
        DefaultAnimation = LastAnimation;
        Index = 0;
        while (AnimBlock->CountParams(pas::wide_int_to_str(Index)) > 0) {
            Animation = AddAnimation();
            Animation->Load(AnimBlock->GetParam(pas::view(pas::wide_int_to_str(Index))));
            TotalAnimationWeight += Animation->Weight;
            if (LastAnimation->Weight > DefaultAnimation->Weight) {
                DefaultAnimation = LastAnimation;
            }
            ++Index;
        }
        AnimBlock = Block->GetBlock(u"AnimS"sv);
        TShip2AnimSE* ReducedNormal = AddReducedAnimation();
        ReducedNormal->Load(AnimBlock->GetParam(u"Normal"sv));
        TotalReducedAnimationWeight = ReducedNormal->Weight;
        DefaultReducedAnimation = LastReducedAnimation;
        Index = 0;
        while (AnimBlock->CountParams(pas::wide_int_to_str(Index)) > 0) {
            ReducedAnimation = AddReducedAnimation();
            ReducedAnimation->Load(AnimBlock->GetParam(pas::view(pas::wide_int_to_str(Index))));
            TotalReducedAnimationWeight += ReducedAnimation->Weight;
            if (LastReducedAnimation->Weight > DefaultReducedAnimation->Weight) {
                DefaultReducedAnimation = LastReducedAnimation;
            }
            ++Index;
        }
        if (Block->CountParams(u"SizeSmall"_wref.get()) > 0) {
            SmallSize = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"SizeSmall"sv)));
        } else {
            SmallSize = 0;
        }
        if (Block->CountParams(u"SizeLarge"_wref.get()) > 0) {
            LargeSize = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"SizeLarge"sv)));
        } else {
            LargeSize = 0;
        }
        if (Block->CountParams(u"TargetSizeK"_wref.get()) > 0) {
            TargetSizeScale = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"TargetSizeK"sv));
        } else {
            TargetSizeScale = 0.3f;
        }
        SetSize(ClassesImports::Point(64, 64));
    }

    void TShip2SE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
        if (Block->CountParams(u"Angle"_wref.get()) > 0) {
            SetAngle(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Angle"sv))));
        }
    }

    void TShip2SE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_RotateImage5::TRotateImage5GI* MainImage = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, Owner);
        if (GlobalsV::AnimShipFull) {
            GI_RotateImage5::TRotateImage5GI::QueueImagePath(PendingLoads, ImagePath);
        } else {
            GI_RotateImage5::TRotateImage5GI::QueueImagePath(PendingLoads, ReducedImagePath);
        }
        pas::free(MainImage);
        GI_AlphaImage::TAlphaImageGI* MapImage = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Owner);
        MapImage->SetImagePath(MinimapImagePath);
        MapImage->QueueImageLoad(PendingLoads);
        pas::free(MapImage);
    }

    void TShip2AnimSE::p_destroy() {
        SE_Ship2::TShip2AnimSE_Destroy(this);
    }

    void TShip2SE::p_destroy() {
        SE_Ship2::TShip2SE_Destroy(this);
    }

} // namespace SE_Ship2
