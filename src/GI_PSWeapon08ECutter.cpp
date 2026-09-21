#include "layout/GI_PSWeapon08ECutter.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon08ECutter.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native TPSWeapon08ECutter and methods; includes its configuration palette loader.
namespace GI_PSWeapon08ECutter {
    pas::DynArray<GI_PSWeapon08ECutter::TECutterPalette> ECutterPalettes{};

    void LoadECutterPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.7.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        ECutterPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 7); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))));
                        ECutterPalettes[Index][ColorIndex] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u","sv));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                }
            }
        }
    }

    void TPSWeapon08ECutter_Create(TPSWeapon08ECutter* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 55;
        Self->LifetimeTicks = 55;
        Self->Colors[0] = ECutterPalettes[APaletteIndex][0];
        Self->Colors[1] = ECutterPalettes[APaletteIndex][1];
        Self->Colors[2] = ECutterPalettes[APaletteIndex][2];
        Self->Colors[3] = ECutterPalettes[APaletteIndex][3];
        Self->Colors[4] = ECutterPalettes[APaletteIndex][4];
        Self->Colors[5] = ECutterPalettes[APaletteIndex][5];
        Self->Colors[6] = ECutterPalettes[APaletteIndex][6];
        Self->Colors[7] = ECutterPalettes[APaletteIndex][7];
    }

    void TPSWeapon08ECutter_Destroy(TPSWeapon08ECutter* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Native empty override.
    void TPSWeapon08ECutter::Invalidate() {
    }

    void TPSWeapon08ECutter::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon08ECutter::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon08ECutter::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon08ECutter::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon08ECutter::GrowParticles() {
        ParticleCapacity += 100;
        Particles = static_cast<PECutterParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TECutterParticle))));
    }

    PECutterParticle TPSWeapon08ECutter::AddParticle() {
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PECutterParticle Result = static_cast<PECutterParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TECutterParticle))));
        ++ParticleCount;
        return Result;
    }

    void TPSWeapon08ECutter::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PECutterParticle Particle{};
        std::int32_t J{};
        std::int32_t I{};
        std::int32_t K{};
        float Speed{};
        if (RemainingTicks == 55) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            Speed = pas::real_divide(OriginalLength + 128.0L, 55.0L);
            for (I = 0; I <= 1; ++I) {
                for (J = 0; J <= 15; ++J) {
                    for (K = -6; K <= 6; ++K) {
                        Particle = AddParticle();
                        Particle->Kind = 2;
                        {
                            pas::Extended cpp_left = pas::real_divide(J * 3 * 8, OriginalLength) + 3.0L;
                            Particle->Position.X = cpp_left * pas::real_divide(K, 6.0L);
                        }
                        Particle->Position.Y = System::Sqrt(144 - pas::sqr(K)) + J * 8 - 128.0L + I;
                        Particle->Color = Colors[pas::abs(K) * 8 / 7];
                        Particle->Alpha = 0;
                        Particle->Velocity.X = pas::real_divide(pas::real_divide(3.0L * Particle->Position.X, 3.0L), 55.0L);
                        Particle->Velocity.Y = Speed;
                        Particle->Unknown1A = 0;
                    }
                }
            }
        }
        I = 0;
        Particle = Particles;
        J = ParticleCount;
        UpdateHitTestBounds();
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
            Particle = static_cast<PECutterParticle>(EC_Mem::AddPointerOffset(Particles, I * static_cast<std::int32_t>(sizeof(TECutterParticle))));
            --J;
        }
        if (RemainingTicks > 0) {
            --RemainingTicks;
        }
    }

    void TPSWeapon08ECutter::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PECutterParticle Particle = Particles;
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
                Particle = static_cast<PECutterParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TECutterParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
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
                Particle = static_cast<PECutterParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TECutterParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon08ECutter::p_destroy() {
        GI_PSWeapon08ECutter::TPSWeapon08ECutter_Destroy(this);
    }

} // namespace GI_PSWeapon08ECutter
