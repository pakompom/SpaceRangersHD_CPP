#include "layout/GI_PSWeapon03Lezka.hpp"
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
#include "units/GI_PSWeapon03Lezka.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native Lezka beam and its two configured gradient palettes.
namespace GI_PSWeapon03Lezka {
    pas::DynArray<GI_PSWeapon03Lezka::TLezkaPalette> LezkaPrimaryPalettes{};

    pas::DynArray<GI_PSWeapon03Lezka::TLezkaPalette> LezkaSecondaryPalettes{};

    void LoadLezkaPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        std::int32_t PartIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.2.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        LezkaPrimaryPalettes.set_length(Count);
        LezkaSecondaryPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 2); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))));
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 2); cpp_range_4.next(PartIndex); ) {
                            LezkaPrimaryPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), PartIndex, u","sv));
                        }
                    }
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 2); cpp_range_5.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex + 3)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex + 3)}))));
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 2); cpp_range_6.next(PartIndex); ) {
                            LezkaSecondaryPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), PartIndex, u","sv));
                        }
                    }
                }
            }
        }
    }

    void TPSWeapon03Lezka_Create(TPSWeapon03Lezka* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 3;
        Self->RemainingTicks = 38;
        Self->LifetimeTicks = Self->RemainingTicks;
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->UpdateProjectionBounds();
        Self->PaletteIndex = APaletteIndex;
    }

    void TPSWeapon03Lezka_Destroy(TPSWeapon03Lezka* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon03Lezka::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon03Lezka::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon03Lezka::SetActive(std::uint8_t Enabled) {
        if (Active != Enabled) {
            GI_MessageLoop::TObjectGI::SetActive(Enabled);
        }
    }

    void TPSWeapon03Lezka::UpdateProjectionBounds() {
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
        // Native uses Cosine for this final corner as well.
        D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon03Lezka::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon03Lezka::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PLezkaParticle TPSWeapon03Lezka::AddParticle() {
        PLezkaParticle Particle = static_cast<PLezkaParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TLezkaParticle))));
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

    void TPSWeapon03Lezka::ClearParticles() {
        PLezkaParticle Current{};
        PLezkaParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSWeapon03Lezka::InvalidateRect(Types::TRect Rect) {
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

    // Native empty override.
    void TPSWeapon03Lezka::Invalidate() {
    }

    void TPSWeapon03Lezka::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Y{};
        float Distance{};
        float Phase{};
        PLezkaParticle Particle{};
        PLezkaParticle Current{};
        std::uint8_t UnusedAlpha{};
        if (FirstParticle == nullptr && RemainingTicks >= 2) {
            Y = 0;
            Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
            OriginalLength = Distance;
            if (OriginalLength == 0.0L) {
                OriginalLength = 1.0;
            }
            LengthScale = 1.0;
            UnusedAlpha = 0;
            while (static_cast<long double>(Y) < Distance) {
                Phase = pas::real_divide(Y, Distance);
                Particle = AddParticle();
                {
                    std::int32_t cpp_left = pas::random(HalfWidth * 2, &System::RandSeed);
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(cpp_left - HalfWidth, Y));
                }
                switch (pas::random(3, &System::RandSeed)) {
                    case 0: {
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(LezkaPrimaryPalettes[PaletteIndex]), Phase * 8.0L);
                        break;
                    }
                    case 1: {
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(LezkaSecondaryPalettes[PaletteIndex]), Phase * 8.0L);
                        break;
                    }
                    case 2: {
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(LezkaPrimaryPalettes[PaletteIndex]), Phase * 8.0L);
                        break;
                    }
                }
                Particle->MaximumAlpha = pas::random(250, &System::RandSeed);
                if (Y < 64) {
                    Particle->Alpha = pas::shr(System::Trunc(Particle->MaximumAlpha * Y), 6);
                } else {
                    Particle->Alpha = Particle->MaximumAlpha;
                }
                Particle->AlphaStep = pas::random(10, &System::RandSeed) + 10;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, 3.0f));
                Particle->State = 1 + pas::random(2, &System::RandSeed);
                // Retained native accumulator, although it does not feed a particle field.
                if (UnusedAlpha + 4 < 255) {
                    UnusedAlpha += 4;
                } else {
                    UnusedAlpha = 255;
                }
                ++Y;
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
                        if (Distance + 16.0L < Current->Position.Y) {
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) - Distance;
                            {
                                std::int32_t cpp_left_2 = pas::random(HalfWidth * 2, &System::RandSeed);
                                Current->Position.X = cpp_left_2 - HalfWidth;
                            }
                            pas::store_unaligned<EC_Struct::TPointF>(&Current->Velocity, EC_Struct::MakePointF(0.0f, 3.0f));
                        }
                        if (Current->MaximumAlpha > 254 - Current->AlphaStep) {
                            Current->MaximumAlpha = 255;
                        } else {
                            Current->MaximumAlpha += Current->AlphaStep;
                        }
                        if (Current->Position.Y < 64.0L) {
                            Current->Alpha = pas::shr(System::Trunc(static_cast<long double>(Current->MaximumAlpha) * Current->Position.Y), 6);
                        } else {
                            Current->Alpha = Current->MaximumAlpha;
                        }
                        if (Current->MaximumAlpha == 255) {
                            Current->State = 2;
                        }
                        break;
                    }
                    case 2: {
                        Current->Position.X = static_cast<long double>(Current->Position.X) + Current->Velocity.X;
                        Current->Position.Y = static_cast<long double>(Current->Position.Y) + Current->Velocity.Y;
                        if (Distance + 16.0L < Current->Position.Y) {
                            Current->Position.Y = static_cast<long double>(Current->Position.Y) - Distance;
                            {
                                std::int32_t cpp_left_3 = pas::random(HalfWidth * 2, &System::RandSeed);
                                Current->Position.X = cpp_left_3 - HalfWidth;
                            }
                            pas::store_unaligned<EC_Struct::TPointF>(&Current->Velocity, EC_Struct::MakePointF(0.0f, 3.0f));
                        }
                        if (Current->MaximumAlpha < Current->AlphaStep) {
                            Current->MaximumAlpha = 0;
                        } else {
                            Current->MaximumAlpha -= Current->AlphaStep;
                        }
                        if (Current->Position.Y < 64.0L) {
                            Current->Alpha = pas::shr(System::Trunc(static_cast<long double>(Current->MaximumAlpha) * Current->Position.Y), 6);
                        } else {
                            Current->Alpha = Current->MaximumAlpha;
                        }
                        if (Current->MaximumAlpha == 0) {
                            Current->State = 1;
                        }
                        break;
                    }
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon03Lezka::Draw(Types::TRect ClipRect) {
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
        PLezkaParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                PX = Particle->Position.X;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                GR_DX::QueueDrawPoint(X - 1, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                PX = Particle->Position.X;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                }
                --X;
                if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon03Lezka::p_destroy() {
        GI_PSWeapon03Lezka::TPSWeapon03Lezka_Destroy(this);
    }

} // namespace GI_PSWeapon03Lezka
