#include "layout/GI_StarFieldImg.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Cache.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_StarFieldImg.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/aMyFunction.hpp"

// Native class ownership follows reference/unit_ownership.json.
namespace GI_StarFieldImg {
    void TStarFieldImgGI_Create(TStarFieldImgGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ReservedDirty = true;
        {
            float cpp_arg = pas::real_divide(static_cast<std::uint32_t>(GR_Main::GameScreenWidth), 2.0L);
            float cpp_arg_2 = pas::real_divide(static_cast<std::uint32_t>(GR_Main::GameScreenHeight), 2.0L);
            Self->FocusPoint = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
        }
    }

    void TStarFieldImgGI_Destroy(TStarFieldImgGI* Self) {
        if (Self->AnimationTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->AnimationTimer);
            Self->AnimationTimer = nullptr;
        }
        Self->ClearStars();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TStarFieldImgGI::ClearStars() {
        if (Stars != nullptr) {
            EC_Mem::FreeEC(Stars);
            Stars = nullptr;
        }
        StarCount = 0;
        Capacity = 0;
    }

    // Adds 64 zeroed entries.
    void TStarFieldImgGI::GrowStars() {
        void* Tail{};
        Capacity += 64;
        Stars = static_cast<PStarFieldImageGI>(EC_Mem::ReAllocREC(Stars, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * Capacity));
        Tail = EC_Mem::AddPointerOffset(Stars, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * (Capacity - 64));
        pas::fill_memory(Tail, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * 64, static_cast<std::uint8_t>(0));
    }

    PStarFieldImageGI TStarFieldImgGI::AllocateStar() {
        ++StarCount;
        if (StarCount > Capacity) {
            GrowStars();
        }
        return static_cast<PStarFieldImageGI>(EC_Mem::AddPointerOffset(Stars, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * (StarCount - 1)));
    }

    // Copies particles only, not camera or timer state.
    void TStarFieldImgGI::CopyStarsFrom(TStarFieldImgGI* Source) {
        ClearStars();
        if (Source->StarCount > 0) {
            StarCount = Source->StarCount;
            Capacity = StarCount;
            Stars = static_cast<PStarFieldImageGI>(EC_Mem::ReAllocREC(Stars, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * Capacity));
            Windows::CopyMemory(Stars, Source->Stars, static_cast<std::int32_t>(sizeof(TStarFieldImageGI)) * StarCount);
        }
    }

    void TStarFieldImgGI::InitializeStar(PStarFieldImageGI Star) {
        float Factor{};
        EC_Struct::TPointF Intersection{};
        if (aMyFunction::RandomIntRange(0, 1) == 0) {
            {
                pas::Extended cpp_left = pas::random_real(&System::RandSeed);
                Star->Position.X = cpp_left * (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) - 1);
            }
            {
                pas::Extended cpp_left_2 = pas::random_real(&System::RandSeed);
                Star->Position.Y = cpp_left_2 * (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) - 1);
            }
        } else {
            {
                pas::Extended cpp_left_4 = pas::random_real(&System::RandSeed);
                pas::Extended cpp_left_3 = cpp_left_4 * (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) - 1) * 0.5L;
                Star->Position.X = cpp_left_3 + static_cast<std::uint32_t>(GR_Main::GameScreenWidth) * 0.25L;
            }
            {
                pas::Extended cpp_left_6 = pas::random_real(&System::RandSeed);
                pas::Extended cpp_left_5 = cpp_left_6 * (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) - 1) * 0.5L;
                Star->Position.Y = cpp_left_5 + static_cast<std::uint32_t>(GR_Main::GameScreenHeight) * 0.25L;
            }
        }
        std::uint8_t Outside = FocusPoint.X < 0.0L || static_cast<long double>(FocusPoint.X) >= static_cast<std::uint32_t>(GR_Main::GameScreenWidth) || FocusPoint.Y < 0.0L || static_cast<long double>(FocusPoint.Y) >= static_cast<std::uint32_t>(GR_Main::GameScreenHeight);
        if (Outside) {
            if (aMyFunction::SegmentIntersectsRectEdges(EC_Struct::MakePointF(Star->Position.X, Star->Position.Y), FocusPoint, EC_Struct::MakePointF(0.0f, 0.0f), EC_Struct::MakePointF(static_cast<std::uint32_t>(GR_Main::GameScreenWidth) - 1, static_cast<std::uint32_t>(GR_Main::GameScreenHeight) - 1), Intersection)) {
                Factor = pas::random_real(&System::RandSeed) * 0.5L + 0.5L;
                Star->Position.X = (static_cast<long double>(Intersection.X) - Star->Position.X) * Factor + Star->Position.X;
                Star->Position.Y = (static_cast<long double>(Intersection.Y) - Star->Position.Y) * Factor + Star->Position.Y;
            }
        }
        Star->PixelPosition.X = System::Round(Star->Position.X);
        Star->PixelPosition.Y = System::Round(Star->Position.Y);
        Factor = pas::random_real(&System::RandSeed);
        float Angle = Math::ArcTan2(static_cast<long double>(Star->Position.X) - FocusPoint.X, -(static_cast<long double>(Star->Position.Y) - FocusPoint.Y));
        float DX = System::Sin(Angle);
        float DY = -System::Cos(Angle);
        Star->Direction.X = DX;
        Star->Direction.Y = DY;
        float Speed = 0.25L * Factor + 0.05L;
        if (Outside) {
            Speed = Speed * 4.0L;
        }
        Star->Velocity.X = static_cast<long double>(DX) * Speed;
        Star->Velocity.Y = static_cast<long double>(DY) * Speed;
        Speed = 0.15L * Factor + 0.05L;
        if (Outside) {
            Speed = Speed * 2.0L;
        }
        Star->Acceleration.X = static_cast<long double>(DX) * Speed;
        Star->Acceleration.Y = static_cast<long double>(DY) * Speed;
        Star->TemplateIndex = aMyFunction::RandomIntRange(0, GlobalsV::StarFieldImageTemplates.length() - 1);
        EC_CacheGAI::TCGaiEC* Data = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Star->TemplateIndex].CacheControl));
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Star->TemplateIndex].CacheControl)->Release();
            };
            pas::store_unaligned<WindowsSdk::TPoint>(&Star->ImageSize, Data->GetCanvasSize());
            pas::store_unaligned<WindowsSdk::TPoint>(&Star->ImageOffset, EC_Struct::HalfPoint(Star->ImageSize));
            Star->FrameIndex = 0;
            Star->FramePosition = 0.0f;
            Star->LastFrame = System::Round((Data->GetSequenceFrameCount(0) - 1) * (Factor * 0.5L + 0.2L));
            Star->FrameStep = (1.0L + Factor) * pas::real_divide(Star->LastFrame, 1.0E+2L);
            if (Outside) {
                Star->FrameStep = Star->FrameStep * 4.0L;
            }
        }
    }

    // Clears/reseeds the animated image stars and advances 201 warm-up steps.
    void TStarFieldImgGI::SeedStars() {
        std::int32_t I{};
        PStarFieldImageGI Star{};
        ClearStars();
        std::int32_t Count = 20;
        if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) < 768) {
            Count = System::Round(Count * 0.6103515625L);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Star = AllocateStar();
            InitializeStar(Star);
        }
        for (I = 0; I <= 200; ++I) {
            AdvanceStars();
        }
    }

    void TStarFieldImgGI::AdvanceStars() {
        std::int32_t I{};
        std::int32_t X{};
        std::int32_t Y{};
        PStarFieldImageGI Star = Stars;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star->Velocity.X = static_cast<long double>(Star->Velocity.X) + Star->Acceleration.X;
            Star->Velocity.Y = static_cast<long double>(Star->Velocity.Y) + Star->Acceleration.Y;
            Star->Position.X = static_cast<long double>(Star->Position.X) + Star->Velocity.X;
            Star->Position.Y = static_cast<long double>(Star->Position.Y) + Star->Velocity.Y;
            X = System::Round(Star->Position.X);
            Y = System::Round(Star->Position.Y);
            Star->PixelPosition.X = X;
            Star->PixelPosition.Y = Y;
            Star->FramePosition = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Star->LastFrame), static_cast<long double>(Star->FramePosition) + Star->FrameStep);
            Star->FrameIndex = System::Round(Star->FramePosition);
            if (X < HitTestBounds.Left - 30 || X >= HitTestBounds.Right + 30 || Y < HitTestBounds.Top - 30 || Y >= HitTestBounds.Bottom + 30) {
                InitializeStar(Star);
            }
            Star = static_cast<PStarFieldImageGI>(EC_Mem::AddPointerOffset(Star, static_cast<std::int32_t>(sizeof(TStarFieldImageGI))));
        }
    }

    void TStarFieldImgGI::RedirectStars() {
        std::int32_t I{};
        float Distance{};
        float Speed{};
        float DY{};
        float DX{};
        PStarFieldImageGI Star = Stars;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            DX = static_cast<long double>(Star->Position.X) - FocusPoint.X;
            DY = static_cast<long double>(Star->Position.Y) - FocusPoint.Y;
            Distance = System::Sqrt(static_cast<long double>(DX) * DX + static_cast<long double>(DY) * DY);
            DX = pas::real_divide(DX, Distance);
            DY = pas::real_divide(DY, Distance);
            Star->Direction.X = DX;
            Star->Direction.Y = DY;
            Speed = System::Sqrt(static_cast<long double>(Star->Velocity.X) * Star->Velocity.X + static_cast<long double>(Star->Velocity.Y) * Star->Velocity.Y);
            Star->Velocity.X = static_cast<long double>(Speed) * DX;
            Star->Velocity.Y = static_cast<long double>(Speed) * DY;
            Speed = System::Sqrt(static_cast<long double>(Star->Acceleration.X) * Star->Acceleration.X + static_cast<long double>(Star->Acceleration.Y) * Star->Acceleration.Y);
            Star->Acceleration.X = static_cast<long double>(Speed) * DX;
            Star->Acceleration.Y = static_cast<long double>(Speed) * DY;
            Star = static_cast<PStarFieldImageGI>(EC_Mem::AddPointerOffset(Star, static_cast<std::int32_t>(sizeof(TStarFieldImageGI))));
        }
    }

    void TStarFieldImgGI::AnimateStars(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Delta{};
        if (StarCount > 0) {
            --MotionTicks;
            if (MotionTicks < 0) {
                TargetFocusDistance = 0.0f;
                MotionTicks = 0;
            }
            if (TargetHeading != CurrentHeading || TargetFocusDistance != CurrentFocusDistance) {
                Delta = aMyFunction::HeadingDifferenceDegrees(CurrentHeading, TargetHeading);
                if (std::fabs(static_cast<pas::Extended>(Delta)) <= 15.0L) {
                    CurrentHeading = TargetHeading;
                } else if (Delta < 0.0L) {
                    CurrentHeading = aMyFunction::WrapHeadingDegrees(CurrentHeading - 15.0L);
                } else if (Delta > 0.0L) {
                    CurrentHeading = aMyFunction::WrapHeadingDegrees(CurrentHeading + 15.0L);
                }
                if (TargetFocusDistance < CurrentFocusDistance) {
                    CurrentFocusDistance = pas::real_max<pas::Extended>(static_cast<pas::Extended>(TargetFocusDistance), CurrentFocusDistance - 1.5E+2L);
                } else if (TargetFocusDistance > CurrentFocusDistance) {
                    CurrentFocusDistance = pas::real_min<pas::Extended>(static_cast<pas::Extended>(TargetFocusDistance), CurrentFocusDistance + 1.0E+2L);
                }
                {
                    pas::Extended cpp_left = System::Sin(aMyFunction::HeadingDegreesToRadians(CurrentHeading)) * CurrentFocusDistance;
                    FocusPoint.X = cpp_left + pas::real_divide(static_cast<std::uint32_t>(GR_Main::GameScreenWidth), 2.0L);
                }
                {
                    pas::Extended cpp_right = System::Cos(aMyFunction::HeadingDegreesToRadians(CurrentHeading)) * CurrentFocusDistance;
                    FocusPoint.Y = pas::real_divide(static_cast<std::uint32_t>(GR_Main::GameScreenHeight), 2.0L) - cpp_right;
                }
                RedirectStars();
            }
            AdvanceStars();
            Invalidate();
        }
    }

    void TStarFieldImgGI::SetViewPosition(EC_Struct::TPointF Position) {
        float DX = static_cast<long double>(Position.X) - ViewPosition.X;
        float DY = static_cast<long double>(Position.Y) - ViewPosition.Y;
        if (static_cast<long double>(DY) * DY + static_cast<long double>(DX) * DX >= 25.0L) {
            if (DX != 0.0L || DY != 0.0L) {
                TargetHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(DX, -DY));
                if (CurrentFocusDistance == 0.0L) {
                    CurrentFocusDistance = TargetFocusDistance;
                }
                if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 768) {
                    TargetFocusDistance = 3.0E+3f;
                } else {
                    TargetFocusDistance = 2.05E+3f;
                }
                MotionTicks = 5;
                RedirectStars();
            }
            ViewPosition = Position;
        }
    }

    void TStarFieldImgGI::Invalidate() {
        std::int32_t I{};
        WindowsSdk::TRect Bounds{};
        PStarFieldImageGI Star = Stars;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Bounds.Left = Star->PixelPosition.X + Star->ImageOffset.X;
            Bounds.Top = Star->PixelPosition.Y + Star->ImageOffset.Y;
            Bounds.Right = Bounds.Left + Star->ImageSize.X;
            Bounds.Bottom = Bounds.Top + Star->ImageSize.Y;
            MessageLoop->QueueUpdateRect(Bounds);
            Star = static_cast<PStarFieldImageGI>(EC_Mem::AddPointerOffset(Star, static_cast<std::int32_t>(sizeof(TStarFieldImageGI))));
        }
    }

    void TStarFieldImgGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        AnimationTimer = MessageLoop->ScheduleCallbackTimer(50, 50, pas::bind_method<&TStarFieldImgGI::AnimateStars>(this), 0);
    }

    void TStarFieldImgGI::OnDeactivate() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        GI_MessageLoop::TObjectGI::OnDeactivate();
    }

    void TStarFieldImgGI_LoadFromConfigPath(TStarFieldImgGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        TStarFieldImgGI::ApplyStarConfig(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TStarFieldImgGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TStarFieldImgGI::ApplyStarConfig(Block);
    }

    // Native empty extension hook.
    void TStarFieldImgGI::ApplyStarConfig(EC_BlockPar::TBlockParEC* Block) {
    }

    void TStarFieldImgGI::UpdateAutoGeometry() {
    }

    void TStarFieldImgGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        PStarFieldImageGI Star{};
        std::int32_t I{};
        EC_CacheGAI::TCGaiEC* Data{};
        GR_gi::TgiGR* Frame{};
        WindowsSdk::TPoint Origin{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect Intersection{};
        {
            const std::int32_t cpp_last = GlobalsV::StarFieldImageTemplates.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    GlobalsV::StarFieldImageTemplates[I].CachedData = nullptr;
                }
            }
        }
        {
            std::exception_ptr cpp_error{};
            try {
                {
                    const std::int32_t cpp_last_2 = GlobalsV::StarFieldImageTemplates.length() - 1;
                    if (0 <= cpp_last_2) {
                        for (I = 0; I <= cpp_last_2; ++I) {
                            GlobalsV::StarFieldImageTemplates[I].CachedData = EC_CacheGAI::AcquireCachedGai(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[I].CacheControl));
                        }
                    }
                }
                Star = Stars;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
                    Bounds.Left = Star->PixelPosition.X + Star->ImageOffset.X;
                    Bounds.Top = Star->PixelPosition.Y + Star->ImageOffset.Y;
                    Bounds.Right = Bounds.Left + Star->ImageSize.X;
                    Bounds.Bottom = Bounds.Top + Star->ImageSize.Y;
                    if (EC_Struct::IntersectRects(Intersection, Bounds, ClipRect)) {
                        Data = reinterpret_cast<EC_CacheGAI::TCGaiEC*>(GlobalsV::StarFieldImageTemplates[Star->TemplateIndex].CachedData);
                        if (GlobalsV::HardwareRenderingEnabled) {
                            Origin = Data->GetFrameOrigin(Data->GetSequenceFrameIndex(0, Star->FrameIndex));
                            {
                                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateFrameSurface = (Data->GetOrCreateFrameSurface(Data->GetSequenceFrameIndex(0, Star->FrameIndex), cpp_result), cpp_result);
                                std::int32_t cpp_arg = Origin.Y + Bounds.Top;
                                std::int32_t cpp_arg_2 = Origin.X + Bounds.Left;
                                GR_DX::DrawTexture(orCreateFrameSurface, cpp_arg_2, cpp_arg, 255, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            }
                        } else {
                            Frame = Data->LoadFrameGi(Data->GetSequenceFrameIndex(0, Star->FrameIndex));
                            Frame->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Bounds.Left + Frame->GetBoundsRect().Left - Data->GetBoundsRect().Left, Bounds.Top + Frame->GetBoundsRect().Top - Data->GetBoundsRect().Top, ClipRect, 0, 255);
                        }
                    }
                    Star = static_cast<PStarFieldImageGI>(EC_Mem::AddPointerOffset(Star, static_cast<std::int32_t>(sizeof(TStarFieldImageGI))));
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            {
                const std::int32_t cpp_last_3 = GlobalsV::StarFieldImageTemplates.length() - 1;
                if (0 <= cpp_last_3) {
                    for (I = 0; I <= cpp_last_3; ++I) {
                        if (reinterpret_cast<EC_CacheGAI::TCGaiEC*>(GlobalsV::StarFieldImageTemplates[I].CachedData) != nullptr) {
                            reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[I].CacheControl)->Release();
                            GlobalsV::StarFieldImageTemplates[I].CachedData = nullptr;
                        }
                    }
                }
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TStarFieldImgGI::p_destroy() {
        GI_StarFieldImg::TStarFieldImgGI_Destroy(this);
    }

    void TStarFieldImgGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_StarFieldImg::TStarFieldImgGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_StarFieldImg
