#include "layout/GI_PSWeapon12Turbogravir.hpp"
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
#include "units/GI_PSWeapon12Turbogravir.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native Turbogravir dual strands and Blue Whirl, including dormant particle states.
namespace GI_PSWeapon12Turbogravir {
    pas::DynArray<GI_PSWeapon12Turbogravir::TTurbogravirPalette> TurbogravirPrimaryPalettes{};

    pas::DynArray<GI_PSWeapon12Turbogravir::TTurbogravirPalette> TurbogravirSecondaryPalettes{};

    void LoadTurbogravirPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        std::int32_t PartIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.11.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        TurbogravirPrimaryPalettes.set_length(Count);
        TurbogravirSecondaryPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 2); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))));
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 2); cpp_range_4.next(PartIndex); ) {
                            TurbogravirPrimaryPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), PartIndex, u","sv));
                        }
                    }
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 2); cpp_range_5.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex + 3)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex + 3)}))));
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 2); cpp_range_6.next(PartIndex); ) {
                            TurbogravirSecondaryPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), PartIndex, u","sv));
                        }
                    }
                }
            }
        }
    }

    void TPSWeapon12Turbogravir_Create(TPSWeapon12Turbogravir* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 3;
        Self->Wavelength = 64;
        Self->HalfWavelength = pas::shr(Self->Wavelength, 1);
        Self->OriginalLength = 1.0;
        Self->LengthScale = 1.0;
        Self->PhaseMask = Self->Wavelength - 1;
        Self->BuildWaveTables();
        Self->UpdateProjectionBounds();
        Self->EnabledStrands = 3;
        Self->PaletteIndex = APaletteIndex;
    }

    void TPSWeapon12Turbogravir_Destroy(TPSWeapon12Turbogravir* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon12Turbogravir::BuildWaveTables() {
        std::int32_t I{};
        float Angle{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PhaseMask); cpp_range.next(I); ) {
            Angle = pas::real_divide(I, Wavelength) * 2.0L * SystemImports::Pi;
            OffsetTable[I] = System::Trunc(System::Sin(Angle) * HalfWidth);
            AlphaTable[I] = System::Trunc((System::Cos(Angle) + 1.5L) * 1.0E+2L);
        }
    }

    void TPSWeapon12Turbogravir::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon12Turbogravir::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon12Turbogravir::UpdateProjectionBounds() {
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
        // Native uses Cosine for this final corner as well.
        D = static_cast<long double>(HalfWidth * 2) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon12Turbogravir::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon12Turbogravir::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PTurbogravirParticle TPSWeapon12Turbogravir::AddParticle() {
        PTurbogravirParticle Particle = static_cast<PTurbogravirParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TTurbogravirParticle))));
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

    void TPSWeapon12Turbogravir::ClearParticles() {
        PTurbogravirParticle Current{};
        PTurbogravirParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    // Native empty override.
    void TPSWeapon12Turbogravir::Invalidate() {
    }

    void TPSWeapon12Turbogravir::InvalidateRect(Types::TRect Rect) {
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

    void TPSWeapon12Turbogravir::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        std::int32_t Distance{};
        PTurbogravirParticle Current{};
        PTurbogravirParticle Particle{};
        std::int32_t X{};
        if (FirstParticle == nullptr && RemainingTicks > 24) {
            I = 0;
            Distance = System::Trunc(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)));
            OriginalLength = Distance;
            if (OriginalLength == 0.0L) {
                OriginalLength = 1.0;
            }
            LengthScale = 1.0;
            while (I < Distance) {
                if (pas::shr(I, 4) % 2 == 0) {
                    if ((EnabledStrands & 1) != 0) {
                        X = OffsetTable[I & PhaseMask];
                        Particle = AddParticle();
                        Particle->Position.X = X;
                        Particle->Position.Y = I;
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(TurbogravirPrimaryPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 5.0L);
                        if (I < 64) {
                            Particle->Alpha = pas::shr(I * AlphaTable[I & PhaseMask], 6);
                        }
                        Particle->Velocity.X = 0;
                        Particle->Velocity.Y = 2;
                        Particle->State = 1;
                        Particle->Unknown2E = 30000;
                        Particle = AddParticle();
                        Particle->Position.X = X;
                        Particle->Position.Y = I + 1;
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(TurbogravirPrimaryPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 5.0L);
                        if (I < 64) {
                            Particle->Alpha = pas::shr(I * AlphaTable[I & PhaseMask], 6);
                        }
                        Particle->Velocity.X = 0;
                        Particle->Velocity.Y = 2;
                        Particle->State = 1;
                        Particle->Unknown2E = 30000;
                    }
                    if ((EnabledStrands & 2) != 0) {
                        X = OffsetTable[I + HalfWavelength & PhaseMask];
                        Particle = AddParticle();
                        Particle->Position.X = X;
                        Particle->Position.Y = I;
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(TurbogravirSecondaryPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 5.0L);
                        Particle->Alpha = AlphaTable[I + HalfWavelength & PhaseMask];
                        if (I < 64) {
                            Particle->Alpha = pas::shr(I * AlphaTable[I + HalfWavelength & PhaseMask], 6);
                        }
                        Particle->Velocity.X = 0;
                        Particle->Velocity.Y = 2;
                        Particle->State = 2;
                        Particle->Unknown2E = 30000;
                        Particle = AddParticle();
                        Particle->Position.X = X;
                        Particle->Position.Y = I + 1;
                        Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(TurbogravirSecondaryPalettes[PaletteIndex]), pas::real_divide(I, Distance) * 5.0L);
                        Particle->Alpha = AlphaTable[I + HalfWavelength & PhaseMask];
                        if (I < 64) {
                            Particle->Alpha = pas::shr(I * AlphaTable[I + HalfWavelength & PhaseMask], 6);
                        }
                        Particle->Velocity.X = 0;
                        Particle->Velocity.Y = 2;
                        Particle->State = 2;
                        Particle->Unknown2E = 30000;
                    }
                }
                I += 2;
            }
        } else {
            Distance = System::Round(OriginalLength);
            LengthScale = pas::real_divide(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)), OriginalLength);
            UpdateHitTestBounds();
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                switch (Current->State) {
                    case 1: {
                        Current->Position.Y += Current->Velocity.Y;
                        if (Current->Position.Y > Distance) {
                            Current->Position.Y -= Distance;
                            Current->Alpha = 0;
                        }
                        Current->Position.X = OffsetTable[Current->Position.Y & PhaseMask];
                        if (Current->Position.Y < 64) {
                            Current->Alpha = pas::shr(Current->Position.Y * AlphaTable[Current->Position.Y & PhaseMask], 6);
                        } else {
                            Current->Alpha = AlphaTable[Current->Position.Y & PhaseMask];
                        }
                        break;
                    }
                    case 2: {
                        Current->Position.Y += Current->Velocity.Y;
                        if (Current->Position.Y > Distance) {
                            Current->Position.Y -= Distance;
                            Current->Alpha = 0;
                        }
                        Current->Position.X = OffsetTable[Current->Position.Y + HalfWavelength & PhaseMask];
                        if (Current->Position.Y < 64) {
                            Current->Alpha = pas::shr(Current->Position.Y * AlphaTable[Current->Position.Y + HalfWavelength & PhaseMask], 6);
                        } else {
                            Current->Alpha = AlphaTable[Current->Position.Y + HalfWavelength & PhaseMask];
                        }
                        break;
                    }
                    case 3: {
                        Current->Position.Y += Current->Velocity.Y;
                        I = OffsetTable[Current->Position.Y + HalfWavelength & PhaseMask];
                        if (I < 0) {
                            Current->Position.X = -pas::shr(Current->Radius * -I, 5);
                        } else {
                            Current->Position.X = pas::shr(Current->Radius * I, 5);
                        }
                        if (Current->Alpha > 1) {
                            --Current->Alpha;
                        }
                        Current->Radius += 2;
                        if (Current->Radius > 63) {
                            Current->State = 255;
                        }
                        break;
                    }
                    case 4: {
                        Current->Position.Y += Current->Velocity.Y;
                        I = OffsetTable[Current->Position.Y & PhaseMask];
                        if (I < 0) {
                            Current->Position.X = -pas::shr(Current->Radius * -I, 5);
                        } else {
                            Current->Position.X = pas::shr(Current->Radius * I, 5);
                        }
                        if (Current->Alpha > 1) {
                            --Current->Alpha;
                        }
                        Current->Radius += 2;
                        if (Current->Radius > 63) {
                            Current->State = 255;
                        }
                        break;
                    }
                    case 5: {
                        Current->FloatPosition.Y = static_cast<long double>(Current->FloatPosition.Y) + Current->FloatVelocity.Y;
                        Current->FloatPosition.X = static_cast<long double>(Current->FloatPosition.X) + Current->FloatVelocity.X;
                        Current->Position.X = System::Trunc(Current->FloatPosition.X);
                        Current->Position.Y = System::Trunc(Current->FloatPosition.Y);
                        Current->FloatVelocity.Y = 0.95L * Current->FloatVelocity.Y;
                        Current->FloatVelocity.X = 0.95L * Current->FloatVelocity.X;
                        if (Current->Alpha < 246) {
                            Current->Alpha += 16;
                        }
                        if (Current->Alpha > 245) {
                            Current->State = 6;
                        }
                        break;
                    }
                    case 6: {
                        Current->FloatPosition.Y = static_cast<long double>(Current->FloatPosition.Y) + Current->FloatVelocity.Y;
                        Current->FloatPosition.X = static_cast<long double>(Current->FloatPosition.X) + Current->FloatVelocity.X;
                        Current->Position.X = System::Trunc(Current->FloatPosition.X);
                        Current->Position.Y = System::Trunc(Current->FloatPosition.Y);
                        Current->FloatVelocity.Y = 0.95L * Current->FloatVelocity.Y;
                        Current->FloatVelocity.X = 0.95L * Current->FloatVelocity.X;
                        if (Current->Alpha > 25) {
                            Current->Alpha -= 20;
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

    void TPSWeapon12Turbogravir::Draw(Types::TRect ClipRect) {
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
        PTurbogravirParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                if (Particle->State != 255) {
                    PX = static_cast<long double>(Particle->Position.X) * LengthScale;
                    PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                    X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                    Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                    GR_DX::QueueDrawPoint(X - 1, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(nullptr);
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
                    --X;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = Particle->Next;
            }
        }
    }

    // $2B4..$2B7 is inherited alignment padding, not an additional field.
    void TPSBlueWhirlGI_Create(TPSBlueWhirlGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir_Create(Self, Owner, 0);
        Self->EnabledStrands = 1;
    }

    void TPSWeapon12Turbogravir::p_destroy() {
        GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir_Destroy(this);
    }

} // namespace GI_PSWeapon12Turbogravir
