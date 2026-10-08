#include "layout/GI_StarFieldM.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_StarFieldM.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace GI_StarFieldM {
    void TStarFieldMGI_Create(TStarFieldMGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        std::int32_t I{};
        std::int32_t J{};
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->FocusPoint = EC_Struct::MakePointF(static_cast<float>(static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2.0),
                static_cast<float>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2.0));
        Self->ColorTable =
            static_cast<PMovingStarColorTable>(
                EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TMovingStarColorTable))));
        for (I = 0; I <= 15; ++I) {
            for (J = 0; J <= 31; ++J) {
                double cpp_arg = static_cast<float>(static_cast<float>(pas::random_real(&System::RandSeed) * 0.5) + 0.5);
                double cpp_arg_2 = static_cast<float>(static_cast<float>(pas::random_real(&System::RandSeed) * 0.5) + 0.5);
                double cpp_arg_3 = static_cast<float>(static_cast<float>(pas::random_real(&System::RandSeed) * 0.5) + 0.5);
                GR_GraphBuf::TPixelFormatGR* currentPixelFormat = GR_Main::CurrentPixelFormat;
                std::uint16_t packNormalizedRgb = currentPixelFormat->PackNormalizedRgb(cpp_arg, cpp_arg_2, cpp_arg_3);
                void* addPointerOffset = EC_Mem::AddPointerOffset(Self->ColorTable,
                    I * 32 * static_cast<std::int32_t>(sizeof(std::uint16_t)) + J *
                            static_cast<std::int32_t>(sizeof(std::uint16_t)));
                EC_Mem::WriteWordEC(addPointerOffset, packNormalizedRgb);
            }
        }
        Self->SeedStars();
    }

    void TStarFieldMGI_Destroy(TStarFieldMGI* Self) {
        if (Self->AnimationTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->AnimationTimer);
            Self->AnimationTimer = nullptr;
        }
        if (Self->ColorTable != nullptr) {
            EC_Mem::FreeEC(Self->ColorTable);
            Self->ColorTable = nullptr;
        }
        Self->ClearStars();
        GI_Panel::TPanelGI_Destroy(Self);
    }

    void TStarFieldMGI::OnActivate() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        AnimationTimer = MessageLoop->ScheduleCallbackTimer(50, 50,
                pas::bind_method<&TStarFieldMGI::AnimateStars>(this), 0);
    }

    void TStarFieldMGI::OnDeactivate() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
    }

    void TStarFieldMGI::ClearStars() {
        if (Stars != nullptr) {
            EC_Mem::FreeEC(Stars);
            Stars = nullptr;
        }
        StarCount = 0;
        Capacity = 0;
    }

    void TStarFieldMGI::GrowStars() {
        void* Tail{};
        Capacity += 64;
        Stars = static_cast<PMovingStarPixel>(EC_Mem::ReAllocREC(Stars,
                static_cast<std::int32_t>(sizeof(TMovingStarPixel)) * Capacity));
        Tail = EC_Mem::AddPointerOffset(Stars, static_cast<std::int32_t>(sizeof(TMovingStarPixel)) * (Capacity - 64));
        pas::fill_memory(Tail, static_cast<std::int32_t>(sizeof(TMovingStarPixel)) * 64, static_cast<std::uint8_t>(0));
    }

    PMovingStarPixel TStarFieldMGI::AllocateStar() {
        ++StarCount;
        if (StarCount > Capacity) {
            GrowStars();
        }
        return static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Stars,
            static_cast<std::int32_t>(sizeof(TMovingStarPixel)) * (StarCount - 1)));
    }

    void TStarFieldMGI::InitializeStar(PMovingStarPixel Star) {
        {
            double cpp_left = pas::random_real(&System::RandSeed);
            Star->Position.X = static_cast<float>(cpp_left * (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) - 1));
        }
        {
            double cpp_left_2 = pas::random_real(&System::RandSeed);
            Star->Position.Y = static_cast<float>(cpp_left_2 * (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) - 1));
        }
        Star->PixelPosition.X = pas::real_round(Star->Position.X);
        Star->PixelPosition.Y = pas::real_round(Star->Position.Y);
        float Factor = pas::random_real(&System::RandSeed);
        float Angle = Math::ArcTan2(static_cast<float>(static_cast<double>(Star->Position.X) - FocusPoint.X),
            -(static_cast<float>(static_cast<double>(Star->Position.Y) - FocusPoint.Y)));
        float DX = System::Sin(Angle);
        float DY = -System::Cos(Angle);
        Star->Direction.X = DX;
        Star->Direction.Y = DY;
        float Speed = static_cast<float>(static_cast<float>(0.5 * Factor) + 0.1);
        Star->Velocity.X = static_cast<float>(static_cast<double>(DX) * Speed);
        Star->Velocity.Y = static_cast<float>(static_cast<double>(DY) * Speed);
        Speed = static_cast<float>(static_cast<float>(0x1.3333333333334p-2 * Factor) + 0.1);
        Star->Acceleration.X = static_cast<float>(static_cast<double>(DX) * Speed);
        Star->Acceleration.Y = static_cast<float>(static_cast<double>(DY) * Speed);
        Star->ColorPosition = 0.0f;
        Star->ColorStep = static_cast<float>(static_cast<float>(4.0 * Factor) + 2.0);
        Star->PaletteIndex = 0;
        Star->Color = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(ColorTable,
                Star->PaletteIndex * 32 * static_cast<std::int32_t>(sizeof(std::uint16_t)) +
                    pas::real_round(Star->ColorPosition) * static_cast<std::int32_t>(sizeof(std::uint16_t))));
    }

    void TStarFieldMGI::SeedStars() {
        std::int32_t I{};
        PMovingStarPixel Star{};
        std::int32_t Count = 50;
        if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) < 768) {
            Count = pas::real_round(static_cast<float>(Count * 0.6103515625));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Star = AllocateStar();
            InitializeStar(Star);
        }
    }

    void TStarFieldMGI::AdvanceStars() {
        std::int32_t I{};
        std::int32_t X{};
        std::int32_t Y{};
        PMovingStarPixel Star = Stars;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            Star->Velocity.X = static_cast<float>(static_cast<double>(Star->Velocity.X) + Star->Acceleration.X);
            Star->Velocity.Y = static_cast<float>(static_cast<double>(Star->Velocity.Y) + Star->Acceleration.Y);
            Star->Position.X = static_cast<float>(static_cast<double>(Star->Position.X) + Star->Velocity.X);
            Star->Position.Y = static_cast<float>(static_cast<double>(Star->Position.Y) + Star->Velocity.Y);
            X = pas::real_round(Star->Position.X);
            Y = pas::real_round(Star->Position.Y);
            Star->PixelPosition.X = X;
            Star->PixelPosition.Y = Y;
            if (X < HitTestBounds.Left || X >= HitTestBounds.Right || Y < HitTestBounds.Top || Y >=
                    HitTestBounds.Bottom) {
                InitializeStar(Star);
            }
            if (Star->ColorPosition < 31.0) {
                Star->ColorPosition = static_cast<float>(static_cast<double>(Star->ColorPosition) + Star->ColorStep);
                if (Star->ColorPosition > 31.0) {
                    Star->ColorPosition = 31.0f;
                    Star->ColorStep = 0.0f;
                }
                Star->Color = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(ColorTable,
                        Star->PaletteIndex * 32 * static_cast<std::int32_t>(sizeof(std::uint16_t)) +
                            pas::real_round(Star->ColorPosition) * static_cast<std::int32_t>(sizeof(std::uint16_t))));
            }
            Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                    static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
        }
    }

    void TStarFieldMGI::RedirectStars() {
        std::int32_t I{};
        float Distance{};
        float Speed{};
        float DY{};
        float DX{};
        PMovingStarPixel Star = Stars;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
            DX = static_cast<float>(static_cast<double>(Star->Position.X) - FocusPoint.X);
            DY = static_cast<float>(static_cast<double>(Star->Position.Y) - FocusPoint.Y);
            Distance = System::Sqrt(static_cast<float>(static_cast<float>(static_cast<double>(DX) * DX) + static_cast<float>(static_cast<double>(DY) * DY)));
            DX = static_cast<float>(static_cast<double>(DX) / Distance);
            DY = static_cast<float>(static_cast<double>(DY) / Distance);
            Star->Direction.X = DX;
            Star->Direction.Y = DY;
            Speed = System::Sqrt(static_cast<float>(static_cast<float>(static_cast<double>(Star->Velocity.X) * Star->Velocity.X) +
                    static_cast<float>(static_cast<double>(Star->Velocity.Y) * Star->Velocity.Y)));
            Star->Velocity.X = static_cast<float>(static_cast<double>(Speed) * DX);
            Star->Velocity.Y = static_cast<float>(static_cast<double>(Speed) * DY);
            Speed = System::Sqrt(static_cast<float>(static_cast<float>(static_cast<double>(Star->Acceleration.X) * Star->Acceleration.X) +
                    static_cast<float>(static_cast<double>(Star->Acceleration.Y) * Star->Acceleration.Y)));
            Star->Acceleration.X = static_cast<float>(static_cast<double>(Speed) * DX);
            Star->Acceleration.Y = static_cast<float>(static_cast<double>(Speed) * DY);
            Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                    static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
        }
    }

    void TStarFieldMGI::AnimateStars(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Delta{};
        if (StarCount > 0) {
            --MotionTicks;
            if (MotionTicks < 0) {
                TargetFocusDistance = 0.0f;
                MotionTicks = 0;
            }
            if (TargetHeading != CurrentHeading || TargetFocusDistance != CurrentFocusDistance) {
                Delta = aMyFunction::HeadingDifferenceDegrees(CurrentHeading, TargetHeading);
                if (std::fabs(static_cast<double>(Delta)) <= 15.0) {
                    CurrentHeading = TargetHeading;
                } else if (Delta < 0.0) {
                    CurrentHeading = aMyFunction::WrapHeadingDegrees(static_cast<float>(CurrentHeading - 15.0));
                } else if (Delta > 0.0) {
                    CurrentHeading = aMyFunction::WrapHeadingDegrees(static_cast<float>(CurrentHeading + 15.0));
                }
                if (TargetFocusDistance < CurrentFocusDistance) {
                    CurrentFocusDistance = pas::real_max<double>(static_cast<double>(TargetFocusDistance),
                            static_cast<float>(CurrentFocusDistance - 9.0E+1));
                } else if (TargetFocusDistance > CurrentFocusDistance) {
                    CurrentFocusDistance = pas::real_min<double>(static_cast<double>(TargetFocusDistance),
                            static_cast<float>(CurrentFocusDistance + 6.0E+1));
                }
                FocusPoint.X = static_cast<float>(static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(CurrentHeading)) *
                        CurrentFocusDistance) + static_cast<float>(static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2.0));
                FocusPoint.Y = static_cast<float>(static_cast<float>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2.0) -
                        static_cast<float>(System::Cos(aMyFunction::HeadingDegreesToRadians(CurrentHeading)) * CurrentFocusDistance));
                RedirectStars();
            }
            AdvanceStars();
            Invalidate();
        }
    }

    void TStarFieldMGI::SetViewPosition(EC_Struct::TPointF Position) {
        float DX = static_cast<float>(static_cast<double>(Position.X) - ViewPosition.X);
        float DY = static_cast<float>(static_cast<double>(Position.Y) - ViewPosition.Y);
        if (static_cast<float>(static_cast<float>(static_cast<double>(DY) * DY) + static_cast<float>(static_cast<double>(DX) * DX)) >= 25.0) {
            if (DX != 0.0 || DY != 0.0) {
                TargetHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(DX, -DY));
                if (CurrentFocusDistance == 0.0) {
                    CurrentFocusDistance = TargetFocusDistance;
                }
                if (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 768) {
                    TargetFocusDistance = 1.8E+3f;
                } else {
                    TargetFocusDistance = 1.23E+3f;
                }
                MotionTicks = 5;
                RedirectStars();
            }
            ViewPosition = Position;
        }
    }

    void TStarFieldMGI::Invalidate() {
    }

    void TStarFieldMGI::ErasePreviousFrame() {
        PMovingStarPixel Star{};
        void* Buffer{};
        std::int32_t I{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
            if (!GlobalsV::SkipSavedPixelRestore) {
                if (!GlobalsV::BGImage) {
                    Star = Stars;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range.next(I); ) {
                        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Star->PreviousByteOffset), 0);
                        Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                                static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                    }
                } else {
                    Star = Stars;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, StarCount - 1); cpp_range_2.next(I); ) {
                        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Star->PreviousByteOffset),
                            Star->SavedPixel);
                        Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                                static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                    }
                }
            }
        }
    }

    void TStarFieldMGI::PrepareFrameDraw() {
        PMovingStarPixel Star{};
        std::int32_t I{};
        void* Buffer{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
            Star = Stars;
            I = StarCount;
            while (I > 0) {
                Star->ByteOffset = Star->PixelPosition.X * 2 + Star->PixelPosition.Y *
                            GR_Main::ScreenRenderBuffer->PitchBytes;
                if (GlobalsV::BGImage) {
                    Star->SavedPixel = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(Buffer, Star->ByteOffset));
                }
                Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                        static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                --I;
            }
        }
    }

    void TStarFieldMGI::DrawUpdateRects(Types::TRect ClipRect) {
        Draw(ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
    }

    void TStarFieldMGI::Draw(Types::TRect ClipRect) {
        void* Buffer{};
        PMovingStarPixel Pixel = Stars;
        std::int32_t Count = StarCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                GR_DX::QueueDrawPoint(Pixel->PixelPosition.X, Pixel->PixelPosition.Y,
                    GR_DX::Color565ToArgb(Pixel->Color), 255);
                Pixel = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Pixel,
                        static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                --Count;
            }
            GR_DX::FlushDrawPoints(nullptr);
        } else {
            Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
            while (Count > 0) {
                EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Pixel->ByteOffset), Pixel->Color);
                Pixel = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Pixel,
                        static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                --Count;
            }
        }
    }

    void TStarFieldMGI::CommitFrameDraw() {
        PMovingStarPixel Star{};
        std::int32_t I{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            Star = Stars;
            I = StarCount;
            while (I > 0) {
                Star->PreviousByteOffset = Star->ByteOffset;
                Star = static_cast<PMovingStarPixel>(EC_Mem::AddPointerOffset(Star,
                        static_cast<std::int32_t>(sizeof(TMovingStarPixel))));
                --I;
            }
        }
    }

    void TStarFieldMGI::p_destroy() {
        GI_StarFieldM::TStarFieldMGI_Destroy(this);
    }

} // namespace GI_StarFieldM
