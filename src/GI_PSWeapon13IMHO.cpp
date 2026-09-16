#include "layout/GI_PSWeapon13IMHO.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon13IMHO.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native TPSWeapon13IMHO and methods; includes its configuration palette loader.
namespace GI_PSWeapon13IMHO {
    pas::DynArray<GI_PSWeapon13IMHO::TIMHOPalette> IMHOPalettes{};

    void LoadIMHOPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.12.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        IMHOPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 7); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        IMHOPalettes[Index][ColorIndex] = ([&] {
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

    void TPSWeapon13IMHO_Create(TPSWeapon13IMHO* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 55;
        Self->LifetimeTicks = 55;
        Self->Colors[0] = IMHOPalettes[APaletteIndex][0];
        Self->Colors[1] = IMHOPalettes[APaletteIndex][1];
        Self->Colors[2] = IMHOPalettes[APaletteIndex][2];
        Self->Colors[3] = IMHOPalettes[APaletteIndex][3];
        Self->Colors[4] = IMHOPalettes[APaletteIndex][4];
        Self->Colors[5] = IMHOPalettes[APaletteIndex][5];
        Self->Colors[6] = IMHOPalettes[APaletteIndex][6];
        Self->Colors[7] = IMHOPalettes[APaletteIndex][7];
    }

    void TPSWeapon13IMHO_Destroy(TPSWeapon13IMHO* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Native empty override.
    void TPSWeapon13IMHO::Invalidate() {
    }

    void TPSWeapon13IMHO::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon13IMHO::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon13IMHO::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon13IMHO::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon13IMHO::GrowParticles() {
        ParticleCapacity += 100;
        Particles = static_cast<PIMHOParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TIMHOParticle))));
    }

    PIMHOParticle TPSWeapon13IMHO::AddParticle() {
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PIMHOParticle Result = static_cast<PIMHOParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TIMHOParticle))));
        ++ParticleCount;
        return Result;
    }

    void TPSWeapon13IMHO::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PIMHOParticle Particle{};
        std::int32_t J{};
        std::int32_t I{};
        std::int32_t K{};
        std::int32_t L{};
        std::int32_t M{};
        float Speed{};
        float PX{};
        float PY{};
        if (RemainingTicks == 55) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            Speed = pas::real_divide(OriginalLength + 4.0E+1L, 55.0L);
            for (I = 0; I <= 1; ++I) {
                for (J = 0; J <= 3; ++J) {
                    K = -10;
                    while (K < 10) {
                        {
                            pas::Extended cpp_left = pas::real_divide(J * 6 * 10, OriginalLength) + 4.0L;
                            PX = cpp_left * pas::real_divide(K, 1.0E+1L);
                        }
                        {
                            pas::Extended cpp_right = System::Sin(pas::real_divide(K * 3, 1.0E+1L) * SystemImports::Pi) * 1.5L;
                            PY = System::Sqrt(256 - pas::sqr(K)) + J * 10 - 4.0E+1L + I + cpp_right;
                        }
                        for (L = -1; L <= 1; ++L) {
                            for (M = -1; M <= 1; ++M) {
                                Particle = AddParticle();
                                Particle->Kind = 2;
                                Particle->Position.X = static_cast<long double>(L) + PX;
                                Particle->Position.Y = static_cast<long double>(M) + PY;
                                Particle->Color = Colors[pas::abs(K) * 8 / 11];
                                Particle->Alpha = 0;
                                Particle->Velocity.X = pas::real_divide(pas::real_divide(6.0L * Particle->Position.X, 4.0L), 55.0L);
                                Particle->Velocity.Y = Speed;
                                Particle->Unknown1A = 0;
                            }
                        }
                        K += 10;
                    }
                }
            }
        }
        UpdateHitTestBounds();
        I = 0;
        Particle = Particles;
        J = ParticleCount;
        while (J > 0) {
            if (Particle->Kind == 2) {
                Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                if (Particle->Position.Y >= OriginalLength) {
                    Particle->Kind = 0;
                }
                if (Particle->Position.Y > 0.0L) {
                    if (Particle->Alpha < 231) {
                        Particle->Alpha += 24;
                    } else {
                        Particle->Alpha = 255;
                    }
                }
            }
            ++I;
            Particle = static_cast<PIMHOParticle>(EC_Mem::AddPointerOffset(Particles, I * static_cast<std::int32_t>(sizeof(TIMHOParticle))));
            --J;
        }
        if (RemainingTicks > 0) {
            --RemainingTicks;
        }
    }

    void TPSWeapon13IMHO::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        if (OriginalLength == 0.0L) {
            Advance(nullptr, 0);
        }
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PIMHOParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 2) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PIMHOParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TIMHOParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(nullptr);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 2) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PIMHOParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TIMHOParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon13IMHO::p_destroy() {
        GI_PSWeapon13IMHO::TPSWeapon13IMHO_Destroy(this);
    }

} // namespace GI_PSWeapon13IMHO
