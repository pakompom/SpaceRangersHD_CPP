#include "layout/GI_PSWeapon11Desintegrator.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon11Desintegrator.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_PSWeapon11Desintegrator {
    pas::DynArray<GI_PSWeapon11Desintegrator::TDesintegratorPalette> DesintegratorPalettes{};

    void LoadDesintegratorPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.10.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        DesintegratorPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 0); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        DesintegratorPalettes[Index][ColorIndex] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                }
            }
        }
    }

    void TPSWeapon11Desintegrator_Create(TPSWeapon11Desintegrator* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 4;
        Self->Wavelength = 32;
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->PhaseMask = Self->Wavelength - 1;
        Self->UpdateProjectionBounds();
        Self->PendingSparkSteps = 0;
        Self->Color = DesintegratorPalettes[APaletteIndex][0];
        Self->RemainingTicks = 50;
        Self->LifetimeTicks = Self->RemainingTicks;
    }

    void TPSWeapon11Desintegrator_Destroy(TPSWeapon11Desintegrator* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon11Desintegrator::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon11Desintegrator::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon11Desintegrator::UpdateProjectionBounds() {
        std::int32_t DY = -(TargetPoint.Y - LocalPosition.Y);
        if (DY == 0) {
            ++DY;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, DY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        float A = static_cast<long double>(-HalfWidth * 2) * Cosine - static_cast<long double>(-Distance) * Sine;
        float B = static_cast<long double>(HalfWidth * 2) * Cosine - static_cast<long double>(-Distance) * Sine;
        float C = static_cast<long double>(-HalfWidth * 2) * Cosine;
        float D = static_cast<long double>(HalfWidth * 2) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = static_cast<long double>(-HalfWidth * 2) * Sine + static_cast<long double>(-Distance) * Cosine;
        B = static_cast<long double>(HalfWidth * 2) * Sine + static_cast<long double>(-Distance) * Cosine;
        C = static_cast<long double>(-HalfWidth * 2) * Sine;
        D = static_cast<long double>(HalfWidth * 2) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon11Desintegrator::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X - 32;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y - 32;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X + 32;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y + 32;
    }

    Types::TRect TPSWeapon11Desintegrator::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PDesintegratorParticle TPSWeapon11Desintegrator::AddParticle() {
        PDesintegratorParticle Particle = static_cast<PDesintegratorParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TDesintegratorParticle))));
        if (LastParticle != nullptr) {
            LastParticle->Next = Particle;
        }
        Particle->Prev = LastParticle;
        Particle->Next = nullptr;
        LastParticle = Particle;
        if (FirstParticle == nullptr) {
            FirstParticle = Particle;
        }
        return Particle;
    }

    void TPSWeapon11Desintegrator::RemoveParticle(PDesintegratorParticle Particle) {
        if (Particle != nullptr) {
            if (Particle->Prev != nullptr) {
                Particle->Prev->Next = Particle->Next;
            }
            if (Particle->Next != nullptr) {
                Particle->Next->Prev = Particle->Prev;
            }
            if (LastParticle == Particle) {
                LastParticle = Particle->Prev;
            }
            if (FirstParticle == Particle) {
                FirstParticle = Particle->Next;
            }
            EC_Mem::FreeEC(Particle);
        }
    }

    void TPSWeapon11Desintegrator::ClearParticles() {
        PDesintegratorParticle Current{};
        PDesintegratorParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon11Desintegrator::Invalidate() {
    }

    void TPSWeapon11Desintegrator::InvalidateRect(Types::TRect Rect) {
        Types::TPoint Target{};
        Types::TRect Intersection{};
        MessageLoop->UpdateRects->AddScreenClippedRect(HitTestBounds, Parent->ToAbsolutePoint(LocalPosition), Parent->ToAbsolutePoint(TargetPoint));
        Target = Parent->ToAbsolutePoint(TargetPoint);
        Rect.Left = Target.X - 32;
        Rect.Right = Target.X + 32;
        Rect.Top = Target.Y - 32;
        Rect.Bottom = Target.Y + 32;
        if (EC_Struct::IntersectRects(Intersection, Rect, GR_Main::GameScreenRect)) {
            MessageLoop->QueueUpdateRect(Intersection);
        }
    }

    void TPSWeapon11Desintegrator::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Y{};
        float Distance{};
        float Angle{};
        PDesintegratorParticle Current{};
        PDesintegratorParticle Spark{};
        PDesintegratorParticle Particle{};
        if (FirstParticle == nullptr && RemainingTicks > 18) {
            Y = 0.0f;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
            OriginalLength = Distance;
            if (OriginalLength < 0.0L) {
                OriginalLength = 1.0;
            }
            LengthScale = 1.0;
            while (Y < Distance) {
                Particle = AddParticle();
                Angle = pas::real_divide(Y, Wavelength) * 2.0L * SystemImports::Pi;
                Particle->Position.X = 1.0f;
                Particle->Position.Y = Y;
                Particle->Color = Color;
                Particle->BaseAlpha = System::Trunc(System::Sin(Angle) * 95.0L + 1.6E+2L);
                if (Y < 64.0L) {
                    Particle->Alpha = pas::shr(System::Trunc(static_cast<long double>(Particle->BaseAlpha) * Y), 6);
                } else {
                    Particle->Alpha = Particle->BaseAlpha;
                }
                Particle->Velocity.X = 0.0f;
                Particle->Velocity.Y = 4.0f;
                Particle->State = 1;
                Particle = AddParticle();
                Particle->Position.X = 0.0f;
                Particle->Position.Y = Y;
                Particle->Color = Color;
                Particle->BaseAlpha = System::Trunc(System::Sin(Angle) * 95.0L + 1.6E+2L);
                if (Y < 64.0L) {
                    Particle->Alpha = pas::shr(System::Trunc(static_cast<long double>(Particle->BaseAlpha) * Y), 6);
                } else {
                    Particle->Alpha = Particle->BaseAlpha;
                }
                Particle->Velocity.X = 0.0f;
                Particle->Velocity.Y = 4.0f;
                Particle->State = 1;
                Y = Y + 1.0L;
            }
        } else {
            Distance = OriginalLength;
            LengthScale = pas::real_divide(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)), OriginalLength);
            UpdateHitTestBounds();
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                switch (Current->State) {
                    case 1: {
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        if (Current->Position.Y > Distance) {
                            Spark = AddParticle();
                            pas::store_unaligned<EC_Struct::TPointF>(&Spark->Position, EC_Struct::MakePointF(0.0f, 0.0f));
                            Spark->Color = Current->Color;
                            Spark->Alpha = Current->BaseAlpha;
                            Angle = pas::real_divide(pas::random(12, &System::RandSeed) * SystemImports::Pi, 6.0L);
                            {
                                float cpp_arg = System::Sin(Angle) * 1.0L;
                                float cpp_arg_2 = System::Cos(Angle) * 1.0L;
                                pas::store_unaligned<EC_Struct::TPointF>(&Spark->Velocity, EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
                            }
                            Spark->State = 2;
                            Spark->RemainingTicks = 18;
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) - Distance;
                        }
                        if (Current->Position.Y < 64.0L) {
                            Current->Alpha = pas::shr(System::Trunc(static_cast<long double>(Current->BaseAlpha) * Current->Position.Y), 6);
                        } else {
                            Current->Alpha = Current->BaseAlpha;
                        }
                        if (RemainingTicks < 18) {
                            RemoveParticle(Current);
                        }
                        break;
                    }
                }
            }
        }
        ++PendingSparkSteps;
        --RemainingTicks;
    }

    void TPSWeapon11Desintegrator::AdvanceImpactSparks(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        PDesintegratorParticle Current{};
        double DX{};
        double DY{};
        std::int32_t Minimum{};
        std::int32_t Brightness{};
        std::uint8_t Uniform{};
        std::int32_t TargetX = TargetPoint.X - LocalPosition.X + AbsolutePosition.X;
        std::int32_t TargetY = TargetPoint.Y - LocalPosition.Y + AbsolutePosition.Y;
        PDesintegratorParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            if (Current->State == 2) {
                if (Current->Alpha > 96) {
                    Current->Alpha -= 4;
                }
                Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                X = System::Round(static_cast<long double>(TargetX) + Current->Position.X);
                Y = System::Round(static_cast<long double>(TargetY) + Current->Position.Y);
                DX = 0.0;
                DY = 0.0;
                Minimum = 94;
                Uniform = true;
                if (GlobalsV::HardwareRenderingEnabled) {
                    DX = -0.25;
                    DY = 0.0;
                } else {
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X - 1, Y);
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = -0.25;
                        DY = 0.0;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X - 1, Y - 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = -0.25;
                        DY = -0.25;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X - 1, Y - 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = 0.0;
                        DY = -0.25;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X + 1, Y - 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = 0.25;
                        DY = -0.25;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X + 1, Y);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = 0.25;
                        DY = 0.0;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X + 1, Y + 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = 0.25;
                        DY = 0.25;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X, Y + 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        Minimum = Brightness;
                        DX = 0.0;
                        DY = 0.25;
                    }
                    Brightness = GR_Main::ScreenRenderBuffer->GetBrightness16(X - 1, Y + 1);
                    if (Brightness != Minimum) {
                        Uniform = false;
                    }
                    if (Brightness < Minimum) {
                        DX = -0.25;
                        DY = 0.25;
                    }
                }
                if (Uniform) {
                    DX = 0.0;
                    DY = 0.0;
                }
                Current->Velocity.X = static_cast<long double>(Current->Velocity.X) + DX;
                Current->Velocity.Y = static_cast<long double>(Current->Velocity.Y) + DY;
                --Current->RemainingTicks;
                if (Current->RemainingTicks == 0) {
                    RemoveParticle(Current);
                }
            }
        }
    }

    void TPSWeapon11Desintegrator::Draw(Types::TRect ClipRect) {
        float PX{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, PendingSparkSteps); cpp_range.next(I); ) {
            AdvanceImpactSparks(ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
        PendingSparkSteps = 0;
        std::int32_t TargetX = TargetPoint.X - LocalPosition.X + AbsolutePosition.X;
        std::int32_t TargetY = TargetPoint.Y - LocalPosition.Y + AbsolutePosition.Y;
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PDesintegratorParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State == 2) {
                    X = TargetX + System::Trunc(Particle->Position.X);
                    Y = TargetY + System::Trunc(Particle->Position.Y);
                } else {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * LengthScale;
                    X = AbsolutePosition.X + System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine);
                    Y = AbsolutePosition.Y + System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine);
                }
                GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                if (Particle->State == 2) {
                    X = TargetX + System::Trunc(Particle->Position.X);
                    Y = TargetY + System::Trunc(Particle->Position.Y);
                } else {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * LengthScale;
                    X = AbsolutePosition.X + System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine);
                    Y = AbsolutePosition.Y + System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine);
                }
                if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon11Desintegrator::p_destroy() {
        GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator_Destroy(this);
    }

} // namespace GI_PSWeapon11Desintegrator
