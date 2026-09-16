#include "layout/GI_PSWeapon01Laser.hpp"
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
#include "units/GI_PSWeapon01Laser.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_PSWeapon01Laser {
    pas::DynArray<GI_PSWeapon01Laser::TBeamLaserPalette> BeamLaserPalettes{};

    pas::DynArray<float> BeamLaserWidths{};

    pas::DynArray<std::int32_t> BeamLaserDurations{};

    void LoadBeamLaserPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        std::int32_t PartIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.0.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        BeamLaserPalettes.set_length(Count);
        BeamLaserWidths.set_length(Count);
        BeamLaserDurations.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 2); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 2); cpp_range_4.next(PartIndex); ) {
                            BeamLaserPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, PartIndex, u","_wref.get()));
                        }
                    }
                }
                if (PaletteBlock->CountParams(u"Width"_wref.get()) > 0) {
                    BeamLaserWidths[Index] = EC_Str::ExtractDecimalToSingleW(PaletteBlock->GetParam(u"Width"_wref.get()));
                } else {
                    BeamLaserWidths[Index] = 2.0f;
                }
                if (PaletteBlock->CountParams(u"Time"_wref.get()) > 0) {
                    BeamLaserDurations[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"Time"_wref.get()));
                } else {
                    BeamLaserDurations[Index] = 40;
                }
            }
        }
    }

    void TPSWeapon01Laser_Create(TPSWeapon01Laser* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = BeamLaserWidths[APaletteIndex];
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->RemainingTicks = BeamLaserDurations[APaletteIndex];
        Self->UpdateProjectionBounds();
        Self->PaletteIndex = APaletteIndex;
    }

    void TPSWeapon01Laser_Destroy(TPSWeapon01Laser* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon01Laser::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon01Laser::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon01Laser::UpdateProjectionBounds() {
        std::int32_t DY = -(TargetPoint.Y - LocalPosition.Y);
        if (DY == 0) {
            ++DY;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, DY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        float A = (-HalfWidth - 12.0L) * Cosine - static_cast<long double>(-Distance) * Sine;
        float B = (HalfWidth + 12.0L) * Cosine - static_cast<long double>(-Distance) * Sine;
        float C = (-HalfWidth - 12.0L) * Cosine;
        float D = (HalfWidth + 12.0L) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = (-HalfWidth - 12.0L) * Sine + static_cast<long double>(-Distance) * Cosine;
        B = (HalfWidth + 12.0L) * Sine + static_cast<long double>(-Distance) * Cosine;
        C = (-HalfWidth - 12.0L) * Sine;
        D = (HalfWidth + 12.0L) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon01Laser::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon01Laser::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PBeamLaserParticle TPSWeapon01Laser::AddParticle() {
        PBeamLaserParticle Particle = static_cast<PBeamLaserParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TBeamLaserParticle))));
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

    void TPSWeapon01Laser::RemoveParticle(PBeamLaserParticle Particle) {
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

    void TPSWeapon01Laser::ClearParticles() {
        PBeamLaserParticle Current{};
        PBeamLaserParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon01Laser::InvalidateRect(Types::TRect Rect) {
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

    void TPSWeapon01Laser::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        std::int32_t Power{};
        float Distance{};
        float Angle{};
        PBeamLaserParticle Particle{};
        PBeamLaserParticle Current{};
        PBeamLaserParticle Spark{};
        std::uint8_t Alpha{};
        Invalidate();
        std::int32_t FadeThreshold = BeamLaserDurations[PaletteIndex] * 3 / 5;
        if (FirstParticle == nullptr && RemainingTicks >= FadeThreshold) {
            I = 0;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
            OriginalLength = Distance;
            if (OriginalLength == 0.0L) {
                OriginalLength = 1.0;
            }
            LengthScale = 1.0;
            Angle = 0.0f;
            Alpha = 0;
            while (static_cast<long double>(I) < Distance) {
                Particle = AddParticle();
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(System::Sin(pas::real_divide(SystemImports::Pi * Angle, 1.8E+2L)) * (HalfWidth - 0.0L), I));
                Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(BeamLaserPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 2.0L);
                Particle->Alpha = Alpha;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, 1.0E+1f));
                Particle->State = 1;
                Particle = AddParticle();
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(System::Sin(pas::real_divide((Angle + 9.0E+1L) * SystemImports::Pi, 1.8E+2L)) * (HalfWidth - 0.0L), I));
                Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(BeamLaserPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 2.0L);
                Particle->Alpha = Alpha;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, 1.0E+1f));
                Particle->State = 1;
                Particle = AddParticle();
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(System::Sin(pas::real_divide((Angle + 1.8E+2L) * SystemImports::Pi, 1.8E+2L)) * (HalfWidth - 0.0L), I));
                Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(BeamLaserPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 2.0L);
                Particle->Alpha = Alpha;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, 1.0E+1f));
                Particle->State = 1;
                if (Alpha + 4 < 255) {
                    Alpha += 4;
                } else {
                    Alpha = 255;
                }
                ++I;
                Angle = Angle + 1.0E+1L;
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
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        if (Current->Alpha + 4 < 255) {
                            Current->Alpha += 4;
                        } else {
                            Current->Alpha = 255;
                        }
                        if (Current->Position.Y > Distance) {
                            for (I = 1; I <= 1; ++I) {
                                Spark = AddParticle();
                                Spark->Position.X = Current->Position.X;
                                Spark->Position.Y = Current->Position.Y;
                                Spark->Color = Current->Color;
                                Angle = pas::real_divide(pas::random(16, &System::RandSeed), 8.0L) * SystemImports::Pi;
                                Power = pas::random(50, &System::RandSeed);
                                Spark->Velocity.X = pas::real_divide(System::Sin(Angle) * (Power + 50), 5.0E+1L);
                                Spark->Velocity.Y = pas::real_divide(System::Cos(Angle) * (Power + 50), 5.0E+1L);
                                Spark->State = 2;
                                {
                                    std::int32_t cpp_right = pas::random(100, &System::RandSeed);
                                    Power = pas::shr(static_cast<std::int32_t>(Current->Alpha), 1) - cpp_right;
                                }
                                if (Power < 0) {
                                    Power = 0;
                                }
                                Spark->Alpha = Power;
                            }
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) - Distance;
                            Current->Alpha = 0;
                        }
                        if (RemainingTicks < FadeThreshold) {
                            RemoveParticle(Current);
                        }
                        break;
                    }
                    case 2: {
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
                        if (Current->Alpha < 246) {
                            Current->Alpha += 16;
                        }
                        if (Current->Alpha > 245) {
                            Current->State = 3;
                        }
                        break;
                    }
                    case 3: {
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
                        if (Current->Alpha > 25) {
                            Current->Alpha -= 26;
                        }
                        if (Current->Alpha < 26) {
                            RemoveParticle(Current);
                        }
                        break;
                    }
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon01Laser::Draw(Types::TRect ClipRect) {
        float PX{};
        float PY{};
        std::int32_t X{};
        std::int32_t Y = -(TargetPoint.Y - LocalPosition.Y);
        if (Y == 0) {
            ++Y;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, Y);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PBeamLaserParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                PX = static_cast<long double>(Particle->Position.X) * LengthScale;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                PX = static_cast<long double>(Particle->Position.X) * LengthScale;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon01Laser::p_destroy() {
        GI_PSWeapon01Laser::TPSWeapon01Laser_Destroy(this);
    }

} // namespace GI_PSWeapon01Laser
