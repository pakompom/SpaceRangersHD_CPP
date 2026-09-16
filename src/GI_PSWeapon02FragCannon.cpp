#include "layout/GI_PSWeapon02FragCannon.hpp"
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
#include "units/GI_PSWeapon02FragCannon.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace GI_PSWeapon02FragCannon {
    pas::DynArray<GI_PSWeapon02FragCannon::TFragCannonPalette> FragCannonPalettes{};

    std::int32_t FragCannonRandomIndex = 0;

    pas::Array<std::int32_t, 0, 127> FragCannonRandomValues{};

    std::int32_t NextFragCannonRandom() {
        FragCannonRandomIndex = FragCannonRandomIndex + 1 & 0x0000007f;
        return FragCannonRandomValues[FragCannonRandomIndex];
    }

    void LoadFragCannonPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 127); cpp_range.next(Index); ) {
            FragCannonRandomValues[Index] = pas::random(100, &System::RandSeed);
        }
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.1.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range_2.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        FragCannonPalettes.set_length(Count);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 1); cpp_range_4.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        FragCannonPalettes[Index][ColorIndex] = ([&] {
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

    void TPSWeapon02FragCannon_Create(TPSWeapon02FragCannon* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 4;
        Self->UpdateProjectionBounds();
        Self->RemainingTicks = 60;
        Self->LifetimeTicks = Self->RemainingTicks;
        Self->PrimaryColor = FragCannonPalettes[APaletteIndex][0];
        Self->SecondaryColor = FragCannonPalettes[APaletteIndex][1];
    }

    void TPSWeapon02FragCannon_Destroy(TPSWeapon02FragCannon* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon02FragCannon::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon02FragCannon::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon02FragCannon::UpdateProjectionBounds() {
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

    void TPSWeapon02FragCannon::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X - 32;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y - 32;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X + 32;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y + 32;
    }

    Types::TRect TPSWeapon02FragCannon::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PFragCannonParticle TPSWeapon02FragCannon::AddParticle() {
        PFragCannonParticle Particle = static_cast<PFragCannonParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TFragCannonParticle))));
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

    void TPSWeapon02FragCannon::ClearParticles() {
        PFragCannonParticle Current{};
        PFragCannonParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon02FragCannon::InvalidateRect(Types::TRect Rect) {
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

    void TPSWeapon02FragCannon::Invalidate() {
    }

    void TPSWeapon02FragCannon::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Y{};
        std::int32_t I{};
        std::int32_t Power{};
        std::int32_t J{};
        float Distance{};
        PFragCannonParticle Current{};
        PFragCannonParticle Spark{};
        PFragCannonParticle Particle{};
        float Speed{};
        if (FirstParticle == nullptr && RemainingTicks > 20) {
            Y = 0.0f;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
            Speed = 4.1f;
            if (pas::real_divide(Distance, Speed) > 38.0L) {
                Speed = pas::real_divide(Distance, 38.0L);
            }
            while (Y < Distance && Y < 128.0L) {
                for (J = 0; J <= 0; ++J) {
                    Particle = AddParticle();
                    {
                        std::int32_t cpp_right = aMyFunction::RandomIntRange(0, 1) * 2 - 1;
                        I = HalfWidth * cpp_right;
                    }
                    Particle->Position.X = I + J;
                    Particle->Position.Y = Y;
                    Particle->Color = PrimaryColor;
                    if (Y < 32.0L) {
                        Particle->Alpha = pas::shr(System::Trunc(255.0L * Y), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                    Particle = AddParticle();
                    Particle->Position.X = I + J;
                    Particle->Position.Y = Y + 1.0L;
                    Particle->Color = PrimaryColor;
                    if (Y < 32.0L) {
                        Particle->Alpha = pas::shr(System::Trunc(255.0L * Y), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                    Particle = AddParticle();
                    Particle->Position.X = I + J;
                    Particle->Position.Y = Y + 2.0L;
                    Particle->Color = PrimaryColor;
                    if (Y < 32.0L) {
                        Particle->Alpha = pas::shr(System::Trunc(255.0L * Y), 5);
                    }
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = Speed;
                    Particle->State = 1;
                    Particle->Unknown1E = 30000;
                }
                Y = Y + 16.0L;
            }
        } else {
            Distance = System::Trunc(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)));
            UpdateHitTestBounds();
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                switch (Current->State) {
                    case 1: {
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        if (Current->Position.Y > Distance) {
                            for (I = 0; I <= 11; ++I) {
                                Spark = AddParticle();
                                Spark->Position.X = Current->Position.X;
                                Spark->Position.Y = Current->Position.Y;
                                Spark->Color = SecondaryColor;
                                Y = pas::real_divide(pas::random(16, &System::RandSeed), 8.0L) * SystemImports::Pi;
                                Power = pas::random(50, &System::RandSeed);
                                Spark->Velocity.X = pas::real_divide(System::Sin(Y) * (Power + 50), 5.0E+1L);
                                Spark->Velocity.Y = pas::real_divide(System::Cos(Y) * (Power + 50), 5.0E+1L);
                                Spark->State = 4;
                                {
                                    std::int32_t cpp_right_2 = GI_PSWeapon02FragCannon::NextFragCannonRandom();
                                    Power = pas::shr(static_cast<std::int32_t>(Current->Alpha), 1) - cpp_right_2;
                                }
                                if (Power < 0) {
                                    Power = 0;
                                }
                                Spark->Alpha = Power;
                            }
                            Current->State = 255;
                            RemainingTicks = 20;
                        } else {
                            if (Current->Position.Y < 32.0L) {
                                Current->Alpha = pas::shr(System::Trunc(Current->Position.Y * 255.0L), 5);
                            } else {
                                Current->Alpha = 255;
                            }
                            if (GI_PSWeapon02FragCannon::NextFragCannonRandom() < 5) {
                                Spark = AddParticle();
                                {
                                    pas::Extended cpp_right_3 = pas::real_divide(GI_PSWeapon02FragCannon::NextFragCannonRandom(), 1.0E+2L);
                                    Spark->Position.X = Current->Position.X + cpp_right_3 - 0.5L;
                                }
                                Spark->Position.Y = Current->Position.Y - 1.0L;
                                Spark->Color = Current->Color;
                                Spark->Velocity.X = static_cast<long double>(Spark->Position.X) - Current->Position.X;
                                Spark->Velocity.Y = Current->Velocity.Y * 0.75L;
                                Spark->State = 2;
                                Spark->Unknown1E = 30000;
                                {
                                    std::int32_t cpp_right_4 = pas::random(128, &System::RandSeed);
                                    I = Current->Alpha - cpp_right_4;
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
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        if (Current->Alpha > 8) {
                            Current->Alpha -= 9;
                        }
                        if (Current->Alpha < 15) {
                            Current->State = 255;
                            Current = nullptr;
                        }
                        if (Current != nullptr && GI_PSWeapon02FragCannon::NextFragCannonRandom() < 14) {
                            Spark = AddParticle();
                            {
                                pas::Extended cpp_right_5 = pas::real_divide(GI_PSWeapon02FragCannon::NextFragCannonRandom(), 4.0E+2L);
                                Spark->Position.X = Current->Position.X + cpp_right_5 - 0.125L;
                            }
                            Spark->Position.Y = Current->Position.Y - 1.0L;
                            Spark->Color = Current->Color;
                            Spark->Velocity.X = static_cast<long double>(Spark->Position.X) - Current->Position.X + Current->Velocity.X;
                            Spark->Velocity.Y = 0.7L * Current->Velocity.Y;
                            Spark->State = 2;
                            Spark->Unknown1E = 30000;
                            {
                                std::int32_t cpp_right_6 = pas::random(60, &System::RandSeed);
                                I = Current->Alpha + cpp_right_6 - 32;
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
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
                        if (Current->Alpha < 246) {
                            Current->Alpha += 10;
                        }
                        if (Current->Alpha > 245) {
                            Current->State = 5;
                        }
                        break;
                    }
                    case 5: {
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
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

    void TPSWeapon02FragCannon::Draw(Types::TRect ClipRect) {
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
        PFragCannonParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State != 255) {
                    PX = Particle->Position.X;
                    PY = -Particle->Position.Y;
                    X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                    Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
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
                    X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                    Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
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

    void TPSWeapon02FragCannon::p_destroy() {
        GI_PSWeapon02FragCannon::TPSWeapon02FragCannon_Destroy(this);
    }

} // namespace GI_PSWeapon02FragCannon
