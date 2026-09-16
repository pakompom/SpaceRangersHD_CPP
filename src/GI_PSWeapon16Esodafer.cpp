#include "layout/GI_PSWeapon16Esodafer.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon16Esodafer.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native Esodafer projectile, reusable particles and branching impact sparks.
namespace GI_PSWeapon16Esodafer {
    pas::DynArray<GI_PSWeapon16Esodafer::TEsodaferPalette> EsodaferPalettes{};

    void LoadEsodaferPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.15.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        EsodaferPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 2); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        EsodaferPalettes[Index][ColorIndex] = ([&] {
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

    void TPSWeapon16Esodafer_Create(TPSWeapon16Esodafer* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 50;
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->RemainingTicks = 160;
        Self->UpdateProjectionBounds();
        Self->SetColors(EsodaferPalettes[APaletteIndex][0], EsodaferPalettes[APaletteIndex][1], EsodaferPalettes[APaletteIndex][2]);
    }

    void TPSWeapon16Esodafer_Destroy(TPSWeapon16Esodafer* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon16Esodafer::SetColors(std::uint16_t Primary, std::uint16_t Secondary, std::uint16_t Tertiary) {
        Colors[0] = Primary;
        Colors[1] = Secondary;
        Colors[2] = Tertiary;
    }

    void TPSWeapon16Esodafer::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon16Esodafer::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon16Esodafer::UpdateProjectionBounds() {
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        if (Distance == 0.0L) {
            Distance = 1.0f;
        }
        float Cosine = pas::real_divide(-(TargetPoint.Y - LocalPosition.Y), Distance);
        float Sine = pas::real_divide(TargetPoint.X - LocalPosition.X, Distance);
        float A = static_cast<long double>(-HalfWidth - 12) * Cosine - (-Distance - 1.0E+2L) * Sine;
        float B = static_cast<long double>(HalfWidth + 12) * Cosine - (-Distance - 1.0E+2L) * Sine;
        float C = static_cast<long double>(-HalfWidth - 12) * Cosine;
        float D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = static_cast<long double>(-HalfWidth - 12) * Sine + (-Distance - 1.0E+2L) * Cosine;
        B = static_cast<long double>(HalfWidth + 12) * Sine + (-Distance - 1.0E+2L) * Cosine;
        C = static_cast<long double>(-HalfWidth - 12) * Sine;
        // Native uses Cosine for this final corner as well.
        D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon16Esodafer::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon16Esodafer::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PEsodaferParticle TPSWeapon16Esodafer::AddParticle() {
        PEsodaferParticle Particle = static_cast<PEsodaferParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TEsodaferParticle))));
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

    void TPSWeapon16Esodafer::ClearParticles() {
        PEsodaferParticle Current{};
        PEsodaferParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon16Esodafer::InvalidateRect(Types::TRect Rect) {
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

    void TPSWeapon16Esodafer::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        float Distance{};
        float Angle{};
        float Speed{};
        PEsodaferParticle Particle{};
        PEsodaferParticle Current{};
        PEsodaferParticle Spark{};
        auto AcquireEsodaferParticle = [&]() -> PEsodaferParticle {
            PEsodaferParticle Candidate = this->FirstParticle;
            while (Candidate != nullptr && Candidate->State != 255) {
                Candidate = Candidate->Next;
            }
            if (Candidate == nullptr) {
                Candidate = AddParticle();
            }
            return Candidate;
        };
        Invalidate();
        if (FirstParticle == nullptr && RemainingTicks >= 24) {
            Distance = System::Round(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)));
            OriginalLength = pas::real_max<float>(7.0E+1f, Distance);
            Particle = AddParticle();
            pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, 0.0f));
            Particle->Color = Colors[1];
            Particle->Alpha = 0;
            Speed = pas::real_divide(6.0L * Distance, RemainingTicks);
            pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, Speed));
            Particle->State = 1;
            LengthScale = 1.0;
            for (I = 1; I <= 16; ++I) {
                Particle = AddParticle();
                {
                    float cpp_arg = System::Cos(I * 3.14L * 0.125L) * 2.0L;
                    float cpp_arg_2 = System::Sin(I * 3.14L * 0.125L) * 2.0L - 1.0E+1L;
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
                }
                Particle->Color = Colors[1];
                Particle->Alpha = 0;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, Speed));
                Particle->State = 2;
            }
            for (I = 1; I <= 16; ++I) {
                Particle = AddParticle();
                {
                    float cpp_arg_3 = System::Cos(I * 3.14L * 0.125L) * 1.5L;
                    float cpp_arg_4 = System::Sin(I * 3.14L * 0.125L) * 1.5L - 2.0E+1L;
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4));
                }
                Particle->Color = Colors[1];
                Particle->Alpha = 0;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(Particle->Position.X * 0.5L, Speed));
                Particle->State = 2;
            }
            for (I = 1; I <= 16; ++I) {
                Particle = AddParticle();
                {
                    float cpp_arg_5 = System::Cos(I * 3.14L * 0.125L) * 1.0L;
                    float cpp_arg_6 = System::Sin(I * 3.14L * 0.125L) * 1.0L - 3.0E+1L;
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(cpp_arg_5, cpp_arg_6));
                }
                Particle->Color = Colors[1];
                Particle->Alpha = 0;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(Particle->Position.X * 1.0L, Speed));
                Particle->State = 2;
            }
        } else {
            Distance = OriginalLength;
            LengthScale = pas::real_divide(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)), OriginalLength);
            UpdateHitTestBounds();
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                if (Current->State == 1) {
                    Spark = AcquireEsodaferParticle();
                    pas::store_unaligned<EC_Struct::TPointF>(&Spark->Position, Current->Position);
                    Spark->Color = Colors[pas::random(3, &System::RandSeed)];
                    Spark->Alpha = Current->Alpha;
                    Speed = pas::real_divide(0.06L * Distance, RemainingTicks);
                    Angle = pas::random(360, &System::RandSeed) * 0.01745329252L;
                    {
                        float cpp_arg_7 = System::Cos(Angle) * Speed + Current->Velocity.X;
                        float cpp_arg_8 = System::Sin(Angle) * Speed + Current->Velocity.Y;
                        pas::store_unaligned<EC_Struct::TPointF>(&Spark->Velocity, EC_Struct::MakePointF(cpp_arg_7, cpp_arg_8));
                    }
                    Spark->State = 2;
                } else if (Current->State == 2 && pas::random(101, &System::RandSeed) < 3 || Current->State == 3 && pas::random(101, &System::RandSeed) < 10) {
                    if (Current->Alpha > 100) {
                        Current->Alpha -= 70;
                        Current->Velocity.X = 0.9L * Current->Velocity.X;
                        Current->Velocity.Y = 0.9L * Current->Velocity.Y;
                        for (auto cpp_range = pas::for_to<std::int32_t>(1, Current->State * 4 + 1); cpp_range.next(I); ) {
                            Spark = AcquireEsodaferParticle();
                            pas::store_unaligned<EC_Struct::TPointF>(&Spark->Position, Current->Position);
                            Spark->Color = Colors[pas::random(3, &System::RandSeed)];
                            Spark->Alpha = Current->Alpha;
                            if (Current->State == 3) {
                                Speed = pas::real_divide(2.0E+1L, RemainingTicks);
                            } else {
                                Speed = pas::real_divide(0.04L * Distance, RemainingTicks);
                            }
                            Angle = pas::random(360, &System::RandSeed) * 0.01745329252L;
                            {
                                float cpp_arg_9 = System::Cos(Angle) * Speed + Current->Velocity.X;
                                float cpp_arg_10 = System::Sin(Angle) * Speed + Current->Velocity.Y;
                                pas::store_unaligned<EC_Struct::TPointF>(&Spark->Velocity, EC_Struct::MakePointF(cpp_arg_9, cpp_arg_10));
                            }
                            Spark->State = Current->State;
                        }
                    } else {
                        Current->State = 255;
                    }
                }
                if (Current->State == 2 && FirstParticle->State == 1) {
                    if (Current->Alpha >= 3) {
                        Current->Alpha -= 3;
                    }
                    Current->Velocity.X = (static_cast<long double>(Current->Velocity.X) - FirstParticle->Velocity.X) * 0.8L + FirstParticle->Velocity.X;
                    Current->Velocity.Y = (static_cast<long double>(Current->Velocity.Y) - FirstParticle->Velocity.Y) * 0.8L + FirstParticle->Velocity.Y;
                }
                if (RemainingTicks > 150 && pas::in_set<1, 2>(Current->State)) {
                    Current->Alpha = std::min<std::int32_t>(255, Current->Alpha + 30);
                }
                if (RemainingTicks < 40 && pas::in_set<2, 3>(Current->State) && Current->Alpha >= 5) {
                    Current->Alpha -= 5;
                }
                if (Current->Alpha < 100 && RemainingTicks < 150) {
                    Current->State = 255;
                }
                Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                if (Current->Position.Y >= Distance && pas::in_set<1, 2>(Current->State)) {
                    Current->Position.Y = (static_cast<long double>(Current->Position.Y) - Distance) * 0.3L + Distance;
                    Speed = pas::real_divide(4.0E+1L, RemainingTicks);
                    Angle = pas::random(360, &System::RandSeed) * 0.01745329252L;
                    if (Current->State == 2) {
                        float cpp_arg_11 = System::Cos(Angle) * Speed;
                        float cpp_arg_12 = System::Sin(Angle) * Speed;
                        pas::store_unaligned<EC_Struct::TPointF>(&Current->Velocity, EC_Struct::MakePointF(cpp_arg_11, cpp_arg_12));
                    } else {
                        pas::store_unaligned<EC_Struct::TPointF>(&Current->Velocity, EC_Struct::MakePointF(0.0f, 0.0f));
                    }
                    Current->State = 3;
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon16Esodafer::Draw(Types::TRect ClipRect) {
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
        PEsodaferParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State != 255) {
                    PX = static_cast<long double>(Particle->Position.X) * LengthScale;
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
                if (Particle->State != 255) {
                    PX = static_cast<long double>(Particle->Position.X) * LengthScale;
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

    void TPSWeapon16Esodafer::p_destroy() {
        GI_PSWeapon16Esodafer::TPSWeapon16Esodafer_Destroy(this);
    }

} // namespace GI_PSWeapon16Esodafer
