#include "layout/GI_PSWeapon07Blaster.hpp"
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
#include "units/GI_PSWeapon07Blaster.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"

// Native blaster particles, curved projectile paths and independent cyclic random pool.
namespace GI_PSWeapon07Blaster {
    pas::DynArray<GI_PSWeapon07Blaster::TBlasterPalette> BlasterPalettes{};

    std::int32_t BlasterRandomIndex = 0;

    pas::Array<std::int32_t, 0, 127> BlasterRandomValues{};

    std::int32_t NextBlasterRandom() {
        BlasterRandomIndex = BlasterRandomIndex + 1 & 0x0000007f;
        return BlasterRandomValues[BlasterRandomIndex];
    }

    void LoadBlasterPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 127); cpp_range.next(Index); ) {
            BlasterRandomValues[Index] = pas::random(100, &System::RandSeed);
        }
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.6.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range_2.next(Index); ) {
            Count = std::max<std::int32_t>(Count,
                    EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        BlasterPalettes.set_length(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 1); cpp_range_4.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(pas::concat_wide({u"Color", pas::wide_int_to_str(ColorIndex)})) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(pas::concat_wide({u"Color",
                                pas::wide_int_to_str(ColorIndex)})));
                        BlasterPalettes[Index][ColorIndex] = ([&] {
                                double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(
                                    EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv));
                                double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(
                                    EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv));
                                double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(
                                    EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u","sv));
                                return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW,
                                    extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                            }());
                    }
                }
            }
        }
    }

    void TPSWeapon07Blaster_Create(TPSWeapon07Blaster* Self, GI_MessageLoop::TObjectGI* Owner,
        std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 4;
        Self->UpdateProjectionBounds();
        Self->RemainingTicks = 60;
        Self->LifetimeTicks = Self->RemainingTicks;
        Self->PrimaryColor = BlasterPalettes[APaletteIndex][0];
        Self->SecondaryColor = BlasterPalettes[APaletteIndex][1];
    }

    void TPSWeapon07Blaster_Destroy(TPSWeapon07Blaster* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon07Blaster::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon07Blaster::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon07Blaster::UpdateProjectionBounds() {
        std::int32_t DY = -(TargetPoint.Y - LocalPosition.Y);
        if (DY == 0) {
            ++DY;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, DY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y -
                LocalPosition.Y));
        float A = static_cast<float>(static_cast<float>(static_cast<double>(-HalfWidth * 2) * Cosine) - static_cast<float>(static_cast<double>(-Distance) * Sine));
        float B = static_cast<float>(static_cast<float>(static_cast<double>(HalfWidth * 2) * Cosine) - static_cast<float>(static_cast<double>(-Distance) * Sine));
        float C = static_cast<float>(static_cast<double>(-HalfWidth * 2) * Cosine);
        float D = static_cast<float>(static_cast<double>(HalfWidth * 2) * Cosine);
        ProjectionBounds.Left = MathImports::Floor(pas::real_min_scalar<float>(
                pas::real_min_scalar<float>(pas::real_min_scalar<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max_scalar<float>(
                pas::real_max_scalar<float>(pas::real_max_scalar<float>(A, B), C), D));
        A = static_cast<float>(static_cast<float>(static_cast<double>(-HalfWidth * 2) * Sine) + static_cast<float>(static_cast<double>(-Distance) * Cosine));
        B = static_cast<float>(static_cast<float>(static_cast<double>(HalfWidth * 2) * Sine) + static_cast<float>(static_cast<double>(-Distance) * Cosine));
        C = static_cast<float>(static_cast<double>(-HalfWidth * 2) * Sine);
        // Native uses Cosine for this final corner as well.
        D = static_cast<float>(static_cast<double>(HalfWidth * 2) * Cosine);
        ProjectionBounds.Top = MathImports::Floor(pas::real_min_scalar<float>(
                pas::real_min_scalar<float>(pas::real_min_scalar<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max_scalar<float>(
                pas::real_max_scalar<float>(pas::real_max_scalar<float>(A, B), C), D));
    }

    void TPSWeapon07Blaster::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X - 32;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y - 32;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X + 32;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y + 32;
    }

    Types::TRect TPSWeapon07Blaster::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PBlasterParticle TPSWeapon07Blaster::AddParticle() {
        PBlasterParticle Particle = static_cast<PBlasterParticle>(
            EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TBlasterParticle))));
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

    void TPSWeapon07Blaster::ClearParticles() {
        PBlasterParticle Current{};
        PBlasterParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon07Blaster::InvalidateRect(Types::TRect Rect) {
        Types::TPoint Target{};
        Types::TRect Intersection{};
        MessageLoop->UpdateRects->AddScreenClippedRect(HitTestBounds, Parent->ToAbsolutePoint(LocalPosition),
            Parent->ToAbsolutePoint(TargetPoint));
        Target = Parent->ToAbsolutePoint(TargetPoint);
        Rect.Left = Target.X - 32;
        Rect.Right = Target.X + 32;
        Rect.Top = Target.Y - 32;
        Rect.Bottom = Target.Y + 32;
        if (EC_Struct::IntersectRects(Intersection, Rect, GR_Main::GameScreenRect)) {
            MessageLoop->QueueUpdateRect(Intersection);
        }
    }

    // Native empty override.
    void TPSWeapon07Blaster::Invalidate() {
    }

    void TPSWeapon07Blaster::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Y{};
        std::int32_t I{};
        std::int32_t J{};
        float Distance{};
        PBlasterParticle Current{};
        PBlasterParticle Spark{};
        PBlasterParticle Particle{};
        float Speed{};
        if (FirstParticle == nullptr && RemainingTicks > 20) {
            Y = 0.0f;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y -
                        LocalPosition.Y));
            Speed = 4.1f;
            if (static_cast<float>(static_cast<double>(Distance) / Speed) > 38.0) {
                Speed = static_cast<float>(Distance / 38.0);
            }
            while (Y < Distance && Y < 2.0) {
                for (J = 0; J <= 1; ++J) {
                    Particle = AddParticle();
                    {
                        std::int32_t cpp_left = pas::random(HalfWidth * 2 + 1, &System::RandSeed);
                        I = cpp_left - HalfWidth;
                    }
                    Particle->BaseX = static_cast<float>((static_cast<float>(static_cast<float>(Y * 2.0) - 1.0)) * (J + 1));
                    Particle->Position.X = I + J;
                    Particle->Position.Y = Y;
                    Particle->Color = SecondaryColor;
                    if (Y < 32.0) {
                        Particle->Alpha = pas::shr(pas::real_trunc(static_cast<float>(255.0 * Y)), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                    Particle = AddParticle();
                    Particle->BaseX = static_cast<float>((static_cast<float>(static_cast<float>(Y * 2.0) - 1.0)) * (J + 1));
                    Particle->Position.X = I + J;
                    Particle->Position.Y = static_cast<float>(Y + 1.0);
                    Particle->Color = SecondaryColor;
                    if (Y < 32.0) {
                        Particle->Alpha = pas::shr(pas::real_trunc(static_cast<float>(255.0 * Y)), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                    Particle = AddParticle();
                    Particle->BaseX = static_cast<float>((static_cast<float>(static_cast<float>(Y * 2.0) - 1.0)) * (J + 1));
                    Particle->Position.X = I + J;
                    Particle->Position.Y = static_cast<float>(Y + 2.0);
                    Particle->Color = SecondaryColor;
                    if (Y < 32.0) {
                        Particle->Alpha = pas::shr(pas::real_trunc(static_cast<float>(255.0 * Y)), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                }
                Y = static_cast<float>(Y + 1.0);
            }
        } else {
            Distance = pas::real_trunc(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y -
                        LocalPosition.Y)));
            UpdateHitTestBounds();
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                switch (Current->State) {
                    case 1: {
                        Current->Position.Y = static_cast<float>(static_cast<double>(Current->Position.Y) + Current->Velocity.Y);
                        if (Current->BaseX >= 0.0) {
                            Current->Position.X = static_cast<float>(static_cast<float>(System::Sin(static_cast<float>(static_cast<float>((static_cast<float>(static_cast<double>(Current->Position.Y) +
                                    Current->Velocity.Y)) * SystemImports::Pi) / Distance)) * 15.0) + Current->BaseX);
                        } else {
                            Current->Position.X = static_cast<float>(static_cast<float>(System::Sin(static_cast<float>(static_cast<float>((static_cast<float>(static_cast<double>(Current->Position.Y) +
                                    Current->Velocity.Y)) * SystemImports::Pi) / Distance)) * -15.0) + Current->BaseX);
                        }
                        if (Current->Position.Y > Distance) {
                            Current->State = 255;
                            RemainingTicks = 20;
                        } else {
                            if (Current->Position.Y < 32.0) {
                                Current->Alpha = pas::shr(pas::real_trunc(static_cast<float>(Current->Position.Y * 255.0)), 5);
                            } else {
                                Current->Alpha = 255;
                            }
                            if (GI_PSWeapon07Blaster::NextBlasterRandom() < 17) {
                                Spark = AddParticle();
                                {
                                    double cpp_right = static_cast<float>(GI_PSWeapon07Blaster::NextBlasterRandom() / 1.0E+2);
                                    Spark->Position.X = static_cast<float>(static_cast<float>(Current->Position.X + cpp_right) - 0.5);
                                }
                                Spark->Position.Y = static_cast<float>(Current->Position.Y - 1.0);
                                Spark->Color = PrimaryColor;
                                Spark->Velocity.X = static_cast<float>((static_cast<float>(static_cast<double>(Spark->Position.X) - Current->Position.X)) *
                                        0.5);
                                Spark->Velocity.Y = static_cast<float>(Current->Velocity.Y * 0.75);
                                Spark->State = 2;
                                Spark->Unknown1E = 30000;
                                {
                                    std::int32_t cpp_right_2 = pas::random(128, &System::RandSeed);
                                    I = Current->Alpha - cpp_right_2;
                                }
                                if (I > 255) {
                                    I = 255;
                                } else if (I < 0) {
                                    I = 0;
                                }
                                Spark->Alpha = I;
                            }
                        }
                        break;
                    }
                    case 2: ++Current->State; break;
                    case 3: {
                        Current->Position.Y = static_cast<float>(static_cast<double>(Current->Position.Y) + Current->Velocity.Y);
                        Current->Position.X = static_cast<float>(static_cast<double>(Current->Position.X) + Current->Velocity.X);
                        Current->Velocity.Y = static_cast<float>(0x1.e666666666665p-1 * Current->Velocity.Y);
                        if (Current->Alpha > 8) {
                            Current->Alpha -= 9;
                        }
                        if (Current->Alpha < 15) {
                            Current->State = 255;
                            Current = nullptr;
                        }
                        if (Current != nullptr && GI_PSWeapon07Blaster::NextBlasterRandom() < 14) {
                            Spark = AddParticle();
                            {
                                double cpp_right_3 = static_cast<float>(GI_PSWeapon07Blaster::NextBlasterRandom() / 4.0E+2);
                                Spark->Position.X = static_cast<float>(static_cast<float>(Current->Position.X + cpp_right_3) - 0.125);
                            }
                            Spark->Position.Y = static_cast<float>(Current->Position.Y - 1.0);
                            Spark->Color = Current->Color;
                            Spark->Velocity.X = static_cast<float>(static_cast<float>(static_cast<double>(Spark->Position.X) - Current->Position.X) +
                                    Current->Velocity.X);
                            Spark->Velocity.Y = static_cast<float>(0x1.6666666666665p-1 * Current->Velocity.Y);
                            Spark->State = 2;
                            Spark->Unknown1E = 30000;
                            {
                                std::int32_t cpp_right_4 = pas::random(60, &System::RandSeed);
                                I = Current->Alpha + cpp_right_4 - 32;
                            }
                            if (I > 255) {
                                I = 255;
                            } else if (I < 0) {
                                I = 0;
                            }
                            Spark->Alpha = I;
                        }
                        break;
                    }
                    case 4: {
                        Current->Position.Y = static_cast<float>(static_cast<double>(Current->Position.Y) + Current->Velocity.Y);
                        Current->Position.X = static_cast<float>(static_cast<double>(Current->Position.X) + Current->Velocity.X);
                        Current->Velocity.Y = static_cast<float>(0x1.e666666666665p-1 * Current->Velocity.Y);
                        Current->Velocity.X = static_cast<float>(0x1.e666666666665p-1 * Current->Velocity.X);
                        if (Current->Alpha < 246) {
                            Current->Alpha += 10;
                        }
                        if (Current->Alpha > 245) {
                            Current->State = 5;
                        }
                        break;
                    }
                    case 5: {
                        Current->Position.Y = static_cast<float>(static_cast<double>(Current->Position.Y) + Current->Velocity.Y);
                        Current->Position.X = static_cast<float>(static_cast<double>(Current->Position.X) + Current->Velocity.X);
                        Current->Velocity.Y = static_cast<float>(0x1.e666666666665p-1 * Current->Velocity.Y);
                        Current->Velocity.X = static_cast<float>(0x1.e666666666665p-1 * Current->Velocity.X);
                        if (Current->Alpha > 25) {
                            Current->Alpha -= 26;
                        }
                        if (Current->Alpha < 26) {
                            Current->State = 255;
                        }
                        break;
                    }
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon07Blaster::Draw(Types::TRect ClipRect) {
        double PX{};
        double PY{};
        std::int32_t X{};
        std::int32_t Y = -(TargetPoint.Y - LocalPosition.Y);
        if (Y == 0) {
            ++Y;
        }
        double Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, Y);
        double Sine = System::Sin(Angle);
        double Cosine = System::Cos(Angle);
        PBlasterParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State != 255) {
                    PX = Particle->Position.X;
                    PY = -Particle->Position.Y;
                    X = pas::real_round(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(PX) * Cosine) - static_cast<float>(static_cast<double>(PY) * Sine)) +
                            AbsolutePosition.X));
                    Y = pas::real_round(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(PX) * Sine) + static_cast<float>(static_cast<double>(PY) * Cosine)) +
                            AbsolutePosition.Y));
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                    GR_DX::QueueDrawPoint(X - 1, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                if (Particle->State != 255) {
                    PX = Particle->Position.X;
                    PY = -Particle->Position.Y;
                    X = pas::real_round(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(PX) * Cosine) - static_cast<float>(static_cast<double>(PY) * Sine)) +
                            AbsolutePosition.X));
                    Y = pas::real_round(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(PX) * Sine) + static_cast<float>(static_cast<double>(PY) * Cosine)) +
                            AbsolutePosition.Y));
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                    --X;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon07Blaster::p_destroy() {
        GI_PSWeapon07Blaster::TPSWeapon07Blaster_Destroy(this);
    }

} // namespace GI_PSWeapon07Blaster
