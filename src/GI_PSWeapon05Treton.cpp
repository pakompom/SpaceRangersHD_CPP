#include "layout/GI_PSWeapon05Treton.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon05Treton.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_PSWeapon05Treton {
    pas::DynArray<GI_PSWeapon05Treton::TTretonPalette> TretonPalettes{};

    void LoadTretonPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.4.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        TretonPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 1); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        TretonPalettes[Index][ColorIndex] = ([&] {
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

    void TPSWeapon05Treton_Create(TPSWeapon05Treton* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 3;
        Self->RemainingTicks = 60;
        Self->LifetimeTicks = 60;
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->UpdateProjectionBounds();
        Self->SetColors(TretonPalettes[APaletteIndex][0], TretonPalettes[APaletteIndex][1]);
    }

    void TPSWeapon05Treton_Destroy(TPSWeapon05Treton* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon05Treton::SetColors(std::uint16_t FirstColor, std::uint16_t SecondColor) {
        PrimaryColor = FirstColor;
        SecondaryColor = SecondColor;
    }

    void TPSWeapon05Treton::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon05Treton::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon05Treton::UpdateProjectionBounds() {
        std::int32_t DY = -(TargetPoint.Y - LocalPosition.Y);
        if (DY == 0) {
            ++DY;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, DY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        float A = static_cast<long double>(-HalfWidth - 12) * Cosine - static_cast<long double>(-Distance) * Sine;
        float B = static_cast<long double>(HalfWidth + 12) * Cosine - static_cast<long double>(-Distance) * Sine;
        float C = static_cast<long double>(-HalfWidth - 12) * Cosine;
        float D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = static_cast<long double>(-HalfWidth - 12) * Sine + static_cast<long double>(-Distance) * Cosine;
        B = static_cast<long double>(HalfWidth + 12) * Sine + static_cast<long double>(-Distance) * Cosine;
        C = static_cast<long double>(-HalfWidth - 12) * Sine;
        D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon05Treton::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon05Treton::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PTretonParticle TPSWeapon05Treton::AddParticle() {
        PTretonParticle Particle = static_cast<PTretonParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TTretonParticle))));
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

    void TPSWeapon05Treton::ClearParticles() {
        PTretonParticle Current{};
        PTretonParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon05Treton::Invalidate() {
    }

    void TPSWeapon05Treton::InvalidateRect(Types::TRect Rect) {
        Types::TPoint Target{};
        Types::TRect Intersection{};
        MessageLoop->UpdateRects->AddScreenClippedRect(HitTestBounds, Parent->ToAbsolutePoint(LocalPosition), Parent->ToAbsolutePoint(TargetPoint));
        Target = Parent->ToAbsolutePoint(TargetPoint);
        Rect.Left = Target.X - 24;
        Rect.Right = Target.X + 24;
        Rect.Top = Target.Y - 24;
        Rect.Bottom = Target.Y + 24;
        if (EC_Struct::IntersectRects(Intersection, Rect, GR_Main::GameScreenRect)) {
            MessageLoop->QueueUpdateRect(Intersection);
        }
    }

    void TPSWeapon05Treton::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Y{};
        std::int32_t I{};
        float Distance{};
        PTretonParticle Particle{};
        PTretonParticle Current{};
        float DelayScale{};
        if (FirstParticle == nullptr && RemainingTicks == 60) {
            Y = 0;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
            OriginalLength = Distance;
            if (OriginalLength == 0.0L) {
                OriginalLength = 1.0;
            }
            LengthScale = 1.0;
            if (Distance > 3.0E+2L) {
                DelayScale = 2.0E+1f;
            } else {
                DelayScale = pas::real_divide(2.0E+1L * Distance, 3.0E+2L);
            }
            while (static_cast<long double>(Y) < Distance) {
                for (auto cpp_range = pas::for_to<std::int32_t>(-HalfWidth, HalfWidth); cpp_range.next(I); ) {
                    Particle = AddParticle();
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(I, Y + 2 - I));
                    Particle->Color = PrimaryColor;
                    Particle->MaximumAlpha = 255 - pas::idiv(212 * pas::abs(I), HalfWidth);
                    if (Y < 64) {
                        Particle->Alpha = pas::shr(static_cast<std::int32_t>(Particle->MaximumAlpha * System::Trunc(Y)), 6);
                    } else {
                        Particle->Alpha = Particle->MaximumAlpha;
                    }
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, -2.0f));
                    Particle->State = 0;
                    Particle->Countdown = System::Trunc(pas::real_divide(Particle->Position.Y, Distance) * DelayScale);
                }
                for (I = 0; I <= 7; ++I) {
                    Particle = AddParticle();
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, Y + I));
                    Particle->Color = SecondaryColor;
                    Particle->MaximumAlpha = 255 - System::Trunc(System::Sin(pas::real_divide(I, 8.0L) * SystemImports::Pi) * 192.0L);
                    if (Y < 64) {
                        Particle->Alpha = pas::shr(static_cast<std::int32_t>(Particle->MaximumAlpha * System::Trunc(Y)), 6);
                    } else {
                        Particle->Alpha = Particle->MaximumAlpha;
                    }
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, -2.0f));
                    Particle->State = 0;
                    Particle->Countdown = System::Trunc(pas::real_divide(Particle->Position.Y, Distance) * DelayScale);
                }
                Y += 12;
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
                    case 0: {
                        if (Current->Countdown == 0) {
                            Current->Countdown = RemainingTicks + 21 - 60;
                            if (Current->Position.Y < 64.0L) {
                                Current->Alpha = pas::shr(static_cast<std::int32_t>(Current->MaximumAlpha * System::Trunc(Current->Position.Y)), 6);
                            } else {
                                Current->Alpha = Current->MaximumAlpha;
                            }
                            Current->State = 2;
                        } else {
                            --Current->Countdown;
                        }
                        break;
                    }
                    case 1: {
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        if (Current->Position.Y > Distance) {
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) - Distance;
                        }
                        if (Current->Position.Y < 0.0L) {
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) + Distance;
                        }
                        if (Current->Position.Y < 64.0L) {
                            Current->Alpha = pas::shr(static_cast<std::int32_t>(Current->MaximumAlpha * System::Trunc(Current->Position.Y)), 6);
                        } else {
                            Current->Alpha = Current->MaximumAlpha;
                        }
                        break;
                    }
                    case 2: {
                        if (Current->Countdown == 0) {
                            Current->State = 1;
                        } else {
                            --Current->Countdown;
                        }
                        break;
                    }
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon05Treton::Draw(Types::TRect ClipRect) {
        float PX{};
        float PY{};
        std::int32_t X{};
        std::int32_t Y = -(TargetPoint.Y - LocalPosition.Y);
        if (Y == 0) {
            Y = 1;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, Y);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PTretonParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                    X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                    Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                if (Particle->State >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                    X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                    Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon05Treton::p_destroy() {
        GI_PSWeapon05Treton::TPSWeapon05Treton_Destroy(this);
    }

} // namespace GI_PSWeapon05Treton
