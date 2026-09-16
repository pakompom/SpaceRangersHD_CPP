#include "layout/GI_PSWeapon09MResonator.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon09MResonator.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

// Native MResonator and branch controls, particle layout and palette/resource loader.
namespace GI_PSWeapon09MResonator {
    pas::DynArray<GI_PSWeapon09MResonator::TMResonatorPalette> MResonatorPalettes{};

    pas::DynArray<GI_PSWeapon09MResonator::TGAISet> MResonatorAnimationPaths{};

    void LoadMResonatorPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.8.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        MResonatorPalettes.set_length(Count);
        MResonatorAnimationPaths.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 0); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        MResonatorPalettes[Index][ColorIndex] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                }
                if (PaletteBlock->CountParams(u"GAI"_wref.get()) > 0) {
                    MResonatorAnimationPaths[Index][0] = PaletteBlock->GetParam(u"GAI"_wref.get());
                }
            }
        }
    }

    void TPSWeapon09BranchGI_Create(TPSWeapon09BranchGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 90;
        Self->Unknown130 = 0;
        Self->Unknown144 = 20;
        Self->ParticleColor = MResonatorPalettes[APaletteIndex][0];
    }

    void TPSWeapon09BranchGI_Destroy(TPSWeapon09BranchGI* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Native empty override.
    void TPSWeapon09BranchGI::Invalidate() {
    }

    void TPSWeapon09BranchGI::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon09BranchGI::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon09BranchGI::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon09BranchGI::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon09BranchGI::GrowParticles() {
        ParticleCapacity += 100;
        Particles = static_cast<PMResonatorParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
    }

    PMResonatorParticle TPSWeapon09BranchGI::AddParticle() {
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PMResonatorParticle Result = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
        ++ParticleCount;
        return Result;
    }

    void TPSWeapon09BranchGI::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PMResonatorParticle Particle{};
        std::int32_t J{};
        std::int32_t I{};
        std::int32_t K{};
        std::int32_t Delay{};
        float PY{};
        float PX{};
        float Speed{};
        float Angle{};
        if (RemainingTicks == 60) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            if (OriginalLength < 1.0L) {
                OriginalLength = 1.0f;
            }
            for (I = 1; I <= 5; ++I) {
                PX = aMyFunction::RandomIntRange(-16, 16);
                PY = aMyFunction::RandomIntRange(-16, 16);
                Delay = aMyFunction::RandomIntRange(5, 6);
                for (J = 0; J <= 2; ++J) {
                    for (K = 0; K <= 7; ++K) {
                        Particle = AddParticle();
                        Particle->Kind = 1;
                        switch (K) {
                            case 0: {
                                Particle->Position.X = static_cast<long double>(J) + PX;
                                Particle->Position.Y = static_cast<long double>(J) + PY;
                                break;
                            }
                            case 1: {
                                Particle->Position.X = static_cast<long double>(J) + PX;
                                Particle->Position.Y = static_cast<long double>(PY) - J;
                                break;
                            }
                            case 2: {
                                Particle->Position.X = static_cast<long double>(PX) - J;
                                Particle->Position.Y = static_cast<long double>(J) + PY;
                                break;
                            }
                            case 3: {
                                Particle->Position.X = static_cast<long double>(PX) - J;
                                Particle->Position.Y = static_cast<long double>(PY) - J;
                                break;
                            }
                            case 4: {
                                Particle->Position.X = static_cast<long double>(J) + PX + 1.0L;
                                Particle->Position.Y = static_cast<long double>(J) + PY;
                                break;
                            }
                            case 5: {
                                Particle->Position.X = static_cast<long double>(J) + PX + 1.0L;
                                Particle->Position.Y = static_cast<long double>(PY) - J;
                                break;
                            }
                            case 6: {
                                Particle->Position.X = static_cast<long double>(PX) - J + 1.0L;
                                Particle->Position.Y = static_cast<long double>(J) + PY;
                                break;
                            }
                            case 7: {
                                Particle->Position.X = static_cast<long double>(PX) - J + 1.0L;
                                Particle->Position.Y = static_cast<long double>(PY) - J;
                                break;
                            }
                        }
                        Particle->Color = ParticleColor;
                        Particle->Alpha = 0;
                        Particle->MovementDelay = 6;
                        Particle->DelayTicks = Delay;
                        Particle->Unknown1A = 0;
                        Particle->Unknown1B = 1;
                    }
                }
            }
        }
        Particle = Particles;
        J = ParticleCount;
        I = 0;
        UpdateHitTestBounds();
        if (90 - RemainingTicks > 30) {
            while (J > 0) {
                if (Particle->Kind == 1) {
                    --Particle->DelayTicks;
                    if (Particle->DelayTicks < 5) {
                        Particle->Alpha += 50;
                    }
                    --Particle->MovementDelay;
                    if (Particle->MovementDelay == 0) {
                        Particle->Kind = 2;
                        Particle->Velocity.X = pas::real_divide(-Particle->Position.X, 5.0E+1L);
                        Particle->Velocity.Y = pas::real_divide(OriginalLength - 24.0L, 3.0E+1L);
                    }
                } else if (Particle->Kind == 2) {
                    Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                    Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                    if (OriginalLength - 12.0L < Particle->Position.Y) {
                        Speed = pas::real_divide(aMyFunction::RandomIntRange(6, 24), 1.0E+1L);
                        Angle = pas::real_divide(aMyFunction::RandomIntRange(0, 360), 1.8E+2L) * SystemImports::Pi;
                        Particle->Velocity.X = System::Cos(Angle) * Speed;
                        Particle->Velocity.Y = System::Sin(Angle) * Speed;
                        Particle->Kind = 3;
                        Particle->DelayTicks = 6;
                    }
                } else if (Particle->Kind == 3) {
                    Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                    Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                    Particle->Velocity.X = 0.99L * Particle->Velocity.X;
                    Particle->Velocity.Y = 0.99L * Particle->Velocity.Y;
                    if (Particle->DelayTicks > 0) {
                        --Particle->DelayTicks;
                    } else if (Particle->Alpha > 11) {
                        Particle->Alpha -= 10;
                    }
                }
                ++I;
                Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particles, I * static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
                --J;
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon09BranchGI::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        if (OriginalLength == 0.0L) {
            OriginalLength = 1.0f;
        }
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PMResonatorParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon09MResonator_Create(TPSWeapon09MResonator* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 90;
        Self->Unknown130 = 0;
        Self->Animation = nullptr;
        Self->AnimationPosition = EC_Struct::MakePointF(Self->LocalPosition.X, Self->LocalPosition.Y);
        Self->AnimationVelocity = EC_Struct::MakePointF(0.0f, 0.0f);
        Self->Unknown158 = 20;
        Self->AnimationPath = MResonatorAnimationPaths[APaletteIndex][0];
    }

    void TPSWeapon09MResonator_Destroy(TPSWeapon09MResonator* Self) {
        if (Self->Animation != nullptr) {
            pas::free(Self->Animation);
            Self->Animation = nullptr;
        }
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Native empty override.
    void TPSWeapon09MResonator::Invalidate() {
    }

    void TPSWeapon09MResonator::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon09MResonator::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon09MResonator::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon09MResonator::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon09MResonator::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Angle{};
        float PX{};
        float PY{};
        float Sine{};
        float Cosine{};
        float Scale{};
        if (RemainingTicks == 90) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            if (OriginalLength < 1.0L) {
                OriginalLength = 1.0f;
            }
            if (Animation != nullptr) {
                pas::free(Animation);
            }
            Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Parent);
            Animation->SetImagePath(AnimationPath);
            {
                WindowsSdk::TPoint contentSize = Animation->GetContentSize();
                GI_GAI::TgaiGI* animation = Animation;
                animation->SetSize(contentSize);
            }
            Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
            Animation->SetDepthByName(u"Weapon"_wref.get());
            Animation->SetPosition(TargetPoint);
            Animation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(Animation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* animation_2 = Animation;
                animation_2->LoadFrameSequenceFromText(cpp_arg);
            }
            Animation->SetSequenceFrame(0);
            Animation->StopAutoPlayback();
            AnimationVelocity = EC_Struct::MakePointF(0.0f, pas::real_divide(OriginalLength - 24.0L, 3.0E+1L));
            AnimationPosition = EC_Struct::MakePointF(0.0f, 24.0L - AnimationVelocity.Y);
        }
        PMResonatorParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        std::int32_t I = 0;
        while (Count > 0) {
            // Native dormant particle branch still evaluates Kind before advancing.
            static_cast<void>(Particle->Kind == 1);
            ++I;
            Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particles, I * static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
            --Count;
        }
        if (Animation != nullptr) {
            if (AnimationPosition.Y < OriginalLength) {
                AnimationPosition.Y = static_cast<long double>(AnimationPosition.Y) + AnimationVelocity.Y;
                AnimationPosition.X = static_cast<long double>(AnimationPosition.X) + AnimationVelocity.X;
                Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
                PY = -(TargetPoint.Y - LocalPosition.Y);
                if (PY == 0.0L) {
                    PY = 1.0f;
                }
                Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
                Sine = System::Sin(Angle);
                Cosine = System::Cos(Angle);
                PX = AnimationPosition.X;
                PY = static_cast<long double>(AnimationPosition.Y) * Scale;
                {
                    std::int32_t cpp_arg_2 = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + LocalPosition.Y;
                    std::int32_t cpp_arg_3 = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + LocalPosition.X;
                    Animation->SetPosition(ClassesImports::Point(cpp_arg_3, cpp_arg_2));
                }
            } else if (Animation->SequenceFrame == Animation->SequenceFrameCount - 1) {
                pas::free(Animation);
                Animation = nullptr;
            } else {
                Animation->SetSequenceFrame(Animation->SequenceFrame + 1);
                Animation->SetPosition(TargetPoint);
            }
        }
        if (RemainingTicks > 0) {
            --RemainingTicks;
        }
        if (RemainingTicks == 0 && Animation != nullptr) {
            pas::free(Animation);
            Animation = nullptr;
        }
    }

    void TPSWeapon09MResonator::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        if (OriginalLength == 0.0L) {
            OriginalLength = 1.0f;
        }
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PMResonatorParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PMResonatorParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TMResonatorParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon09MResonator::p_destroy() {
        GI_PSWeapon09MResonator::TPSWeapon09MResonator_Destroy(this);
    }

    void TPSWeapon09BranchGI::p_destroy() {
        GI_PSWeapon09MResonator::TPSWeapon09BranchGI_Destroy(this);
    }

} // namespace GI_PSWeapon09MResonator
