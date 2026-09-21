#include "layout/SE_Weapon.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_PDTurretWeapon.hpp"
#include "units/GI_PSEyes.hpp"
#include "units/GI_PSMissileHit.hpp"
#include "units/GI_PSWeapon01Laser.hpp"
#include "units/GI_PSWeapon02FragCannon.hpp"
#include "units/GI_PSWeapon03Lezka.hpp"
#include "units/GI_PSWeapon05Treton.hpp"
#include "units/GI_PSWeapon06Phaser.hpp"
#include "units/GI_PSWeapon07Blaster.hpp"
#include "units/GI_PSWeapon08ECutter.hpp"
#include "units/GI_PSWeapon09MResonator.hpp"
#include "units/GI_PSWeapon10AVision.hpp"
#include "units/GI_PSWeapon11Desintegrator.hpp"
#include "units/GI_PSWeapon12Turbogravir.hpp"
#include "units/GI_PSWeapon13IMHO.hpp"
#include "units/GI_PSWeapon14Vertix.hpp"
#include "units/GI_PSWeapon16Esodafer.hpp"
#include "units/GI_PSWeapon17Kafacitor.hpp"
#include "units/GI_RadialEffect.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Weapon {
    // The owner-color helper's original unit ownership is unresolved.
    void InitializeWeaponVisualResources() {
        GI_PSWeapon01Laser::LoadBeamLaserPalettes();
        GI_PSWeapon02FragCannon::LoadFragCannonPalettes();
        GI_PSWeapon03Lezka::LoadLezkaPalettes();
        GI_PSWeapon05Treton::LoadTretonPalettes();
        GI_PSWeapon06Phaser::LoadPhaserPalettes();
        GI_PSWeapon07Blaster::LoadBlasterPalettes();
        GI_PSWeapon08ECutter::LoadECutterPalettes();
        GI_PSWeapon09MResonator::LoadMResonatorPalettes();
        GI_PSWeapon10AVision::LoadAVisionPalettes();
        GI_PSWeapon11Desintegrator::LoadDesintegratorPalettes();
        GI_PSWeapon12Turbogravir::LoadTurbogravirPalettes();
        GI_PSWeapon13IMHO::LoadIMHOPalettes();
        GI_PSWeapon14Vertix::LoadWeapon14AnimationPaths();
        GI_PSWeapon16Esodafer::LoadEsodaferPalettes();
        GI_PSWeapon17Kafacitor::LoadKafacitorPalettes();
        GI_PSEyes::LoadEyesPalettes();
        GI_PSMissileHit::LoadMissileHitAnimationPaths();
        GI_RadialEffect::LoadRadiationPalettes();
    }

    // Stores visual/variant and appends them to GraphKey. Position is passed through the base constructor.
    void TWeaponSE_Create(TWeaponSE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition, std::int32_t ShotVisual, std::int32_t Variant) {
        Self->ShotVisual = ShotVisual;
        Self->HitVariant = Variant;
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
        if (Variant >= 0) {
            Self->GraphKey = pas::concat_wide({Self->GraphKey, u",", pas::wide_int_to_str(ShotVisual), u",", pas::wide_int_to_str(Variant)});
        } else if (ShotVisual > 0) {
            Self->GraphKey = pas::concat_wide({Self->GraphKey, u",", pas::wide_int_to_str(ShotVisual)});
        }
    }

    void TWeaponSE_Destroy(TWeaponSE* Self) {
        if (Self->SourceObject != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->SourceObject));
        }
        if (Self->TargetObject != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->TargetObject));
        }
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TWeaponSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        float Distance{};
        float Angle{};
        std::int32_t I{};
        EC_BlockPar::TBlockParEC* WeaponConfig{};
        EC_BlockPar::TBlockParEC* VisualConfig{};
        pas::WideString Key{};
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        std::uint8_t HasDestruction = false;
        ImmediateDestruction = false;
        ProjectileFinished = false;
        WeaponConfig = GR_Main::GameDataConfig->GetBlock(u"Weapon"sv);
        Key = EC_Str::ExtractDelimitedPartW(pas::view(GraphKey), 0, u","sv);
        I = EC_Str::ExtractDigitsToIntW(pas::view(Key));
        if (ShotVisual == 0 && EC_Str::CountDelimitedPartsW(pas::view(GraphKey), u","sv) > 1 || pas::in_set<3, 3, 14, 14, 17, 17>(I)) {
            if (EC_Str::CountDelimitedPartsW(pas::view(GraphKey), u","sv) > 1) {
                ShotVisual = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(GraphKey), 1, u","sv)));
            }
            VisualConfig = nullptr;
            if (pas::in_set<3, 3, 14, 14, 17, 17>(I)) {
                VisualConfig = GR_Main::GameDataConfig->FindBlockByPath(u"SE.Weapon.Eyes"_wref.get());
            }
            if (VisualConfig == nullptr) {
                VisualConfig = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.", Key}));
            }
            LoadTemplate(VisualConfig);
        }
        std::uint8_t UseRandomHit = true;
        if (Key == u"Weapon.Star") {
            // Native allocates this child and then clears the retained projectile reference.
            Projectile = pas::construct_call<GI_PSWeapon12Turbogravir::TPSBlueWhirlGI>(GI_PSWeapon12Turbogravir::TPSBlueWhirlGI_Create, Space->MapPanel);
            Projectile = nullptr;
            UseRandomHit = false;
        } else if (Key == u"Weapon.NoGraph") {
            Projectile = nullptr;
            UseRandomHit = false;
        } else if (Key == u"Weapon.Kamikaze") {
            Projectile = nullptr;
            UseRandomHit = false;
        } else if (Key == u"Weapon.Asteroid") {
            Projectile = nullptr;
            UseRandomHit = false;
            SourceAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            SourceAnimation->SetImagePath(u"Bm.Asteroid.Des"_wref.get());
            {
                WindowsSdk::TPoint contentSize = SourceAnimation->GetContentSize();
                GI_GAI::TgaiGI* sourceAnimation = SourceAnimation;
                sourceAnimation->SetSize(contentSize);
            }
            SourceAnimation->SetOrigin(EC_Struct::HalfPoint(SourceAnimation->ClientSize));
            SourceAnimation->SetDepthByName(u"Weapon"_wref.get());
            SourceAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(SourceAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* sourceAnimation_2 = SourceAnimation;
                sourceAnimation_2->LoadFrameSequenceFromText(cpp_arg);
            }
            SourceAnimation->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            SourceAnimation->StopAutoPlayback();
            SourceAnimationInterval = 3;
            ImmediateDestruction = true;
            HasDestruction = true;
        } else if (Key == u"Weapon.MissileHit") {
            Projectile = nullptr;
            UseRandomHit = false;
            SourceAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            SourceAnimation->SetImagePath(GI_PSMissileHit::MissileHitAnimationPaths[ShotVisual][0]);
            {
                WindowsSdk::TPoint contentSize_2 = SourceAnimation->GetContentSize();
                GI_GAI::TgaiGI* sourceAnimation_3 = SourceAnimation;
                sourceAnimation_3->SetSize(contentSize_2);
            }
            SourceAnimation->SetOrigin(EC_Struct::HalfPoint(SourceAnimation->ClientSize));
            SourceAnimation->SetDepthByName(u"Weapon"_wref.get());
            SourceAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(SourceAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* sourceAnimation_4 = SourceAnimation;
                sourceAnimation_4->LoadFrameSequenceFromText(cpp_arg_2);
            }
            SourceAnimation->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            SourceAnimation->StopAutoPlayback();
            SourceAnimationInterval = 3;
            ImmediateDestruction = true;
            HasDestruction = true;
        } else if (Key == u"Weapon.Shock") {
            Projectile = nullptr;
            UseRandomHit = false;
            TargetAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            TargetAnimation->SetImagePath(u"Bm.Weapon.W17"_wref.get());
            {
                WindowsSdk::TPoint contentSize_3 = TargetAnimation->GetContentSize();
                GI_GAI::TgaiGI* targetAnimation = TargetAnimation;
                targetAnimation->SetSize(contentSize_3);
            }
            TargetAnimation->SetOrigin(EC_Struct::HalfPoint(TargetAnimation->ClientSize));
            TargetAnimation->SetDepthByName(u"Weapon"_wref.get());
            TargetAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg_3 = static_cast<pas::WideString>(pas::concat_ansi({"[40,0-", SysUtils::IntToStr(TargetAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* targetAnimation_2 = TargetAnimation;
                targetAnimation_2->LoadFrameSequenceFromText(cpp_arg_3);
            }
            TargetAnimation->StopAutoPlayback();
            TargetAnimationInterval = 2;
        } else if (Key == u"Weapon.Nine") {
            Projectile = pas::construct_call<GI_PSWeapon09MResonator::TPSWeapon09BranchGI>(GI_PSWeapon09MResonator::TPSWeapon09BranchGI_Create, Space->MapPanel, ShotVisual);
        } else if (Key == u"Weapon.PDTurret") {
            Projectile = pas::construct_call<GI_PDTurretWeapon::TPSPDWeaponGI>(GI_PDTurretWeapon::TPSPDWeaponGI_Create, Space->MapPanel);
        } else if (Key == u"Weapon.RadialEffect") {
            Projectile = pas::construct_call<GI_RadialEffect::TPSRadEffectGI>(GI_RadialEffect::TPSRadEffectGI_Create, Space->MapPanel, ShotVisual);
            UseRandomHit = false;
        } else if (Key == u"Weapon.AuraEffect") {
            Projectile = nullptr;
            UseRandomHit = false;
            TargetAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            TargetAnimation->SetImagePath(GI_RadialEffect::AuraAnimationPaths[ShotVisual]);
            {
                WindowsSdk::TPoint contentSize_4 = TargetAnimation->GetContentSize();
                GI_GAI::TgaiGI* targetAnimation_3 = TargetAnimation;
                targetAnimation_3->SetSize(contentSize_4);
            }
            TargetAnimation->SetOrigin(EC_Struct::HalfPoint(TargetAnimation->ClientSize));
            TargetAnimation->SetDepthByName(u"Weapon"_wref.get());
            TargetAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg_4 = static_cast<pas::WideString>(pas::concat_ansi({"[40,0-", SysUtils::IntToStr(TargetAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* targetAnimation_4 = TargetAnimation;
                targetAnimation_4->LoadFrameSequenceFromText(cpp_arg_4);
            }
            TargetAnimation->StopAutoPlayback();
            TargetAnimationInterval = 2;
        } else if (I == 0) {
            Projectile = pas::construct_call<GI_PSWeapon01Laser::TPSWeapon01Laser>(GI_PSWeapon01Laser::TPSWeapon01Laser_Create, Space->MapPanel, ShotVisual);
        } else if (I == 1) {
            Projectile = pas::construct_call<GI_PSWeapon02FragCannon::TPSWeapon02FragCannon>(GI_PSWeapon02FragCannon::TPSWeapon02FragCannon_Create, Space->MapPanel, ShotVisual);
        } else if (I == 2) {
            Projectile = pas::construct_call<GI_PSWeapon03Lezka::TPSWeapon03Lezka>(GI_PSWeapon03Lezka::TPSWeapon03Lezka_Create, Space->MapPanel, ShotVisual);
        } else if (I == 4) {
            Projectile = pas::construct_call<GI_PSWeapon05Treton::TPSWeapon05Treton>(GI_PSWeapon05Treton::TPSWeapon05Treton_Create, Space->MapPanel, ShotVisual);
        } else if (I == 5) {
            Projectile = pas::construct_call<GI_PSWeapon06Phaser::TPSWeapon06Phaser>(GI_PSWeapon06Phaser::TPSWeapon06Phaser_Create, Space->MapPanel, ShotVisual);
        } else if (I == 6) {
            Projectile = pas::construct_call<GI_PSWeapon07Blaster::TPSWeapon07Blaster>(GI_PSWeapon07Blaster::TPSWeapon07Blaster_Create, Space->MapPanel, ShotVisual);
        } else if (I == 7) {
            Projectile = pas::construct_call<GI_PSWeapon08ECutter::TPSWeapon08ECutter>(GI_PSWeapon08ECutter::TPSWeapon08ECutter_Create, Space->MapPanel, ShotVisual);
        } else if (I == 8) {
            Projectile = pas::construct_call<GI_PSWeapon09MResonator::TPSWeapon09MResonator>(GI_PSWeapon09MResonator::TPSWeapon09MResonator_Create, Space->MapPanel, ShotVisual);
            UseRandomHit = false;
        } else if (I == 9) {
            Projectile = pas::construct_call<GI_PSWeapon10AVision::TPSWeapon10AVision>(GI_PSWeapon10AVision::TPSWeapon10AVision_Create, Space->MapPanel, ShotVisual);
            UseRandomHit = false;
            TargetAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            TargetAnimation->SetImagePath(GI_PSWeapon10AVision::AVisionAnimationPaths[ShotVisual][0]);
            {
                WindowsSdk::TPoint contentSize_5 = TargetAnimation->GetContentSize();
                GI_GAI::TgaiGI* targetAnimation_5 = TargetAnimation;
                targetAnimation_5->SetSize(contentSize_5);
            }
            TargetAnimation->SetOrigin(EC_Struct::HalfPoint(TargetAnimation->ClientSize));
            TargetAnimation->SetDepthByName(u"Weapon"_wref.get());
            TargetAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg_5 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(TargetAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* targetAnimation_6 = TargetAnimation;
                targetAnimation_6->LoadFrameSequenceFromText(cpp_arg_5);
            }
            TargetAnimation->StopAutoPlayback();
            TargetAnimationInterval = 2;
        } else if (I == 10) {
            Projectile = pas::construct_call<GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator>(GI_PSWeapon11Desintegrator::TPSWeapon11Desintegrator_Create, Space->MapPanel, ShotVisual);
        } else if (I == 11) {
            Projectile = pas::construct_call<GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir>(GI_PSWeapon12Turbogravir::TPSWeapon12Turbogravir_Create, Space->MapPanel, ShotVisual);
        } else if (I == 12) {
            Projectile = pas::construct_call<GI_PSWeapon13IMHO::TPSWeapon13IMHO>(GI_PSWeapon13IMHO::TPSWeapon13IMHO_Create, Space->MapPanel, ShotVisual);
        } else if (I == 13) {
            Projectile = nullptr;
            UseRandomHit = false;
            SourceAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            SourceAnimation->SetImagePath(GI_PSWeapon14Vertix::Weapon14AnimationPaths[ShotVisual][0]);
            {
                WindowsSdk::TPoint contentSize_6 = SourceAnimation->GetContentSize();
                GI_GAI::TgaiGI* sourceAnimation_5 = SourceAnimation;
                sourceAnimation_5->SetSize(contentSize_6);
            }
            SourceAnimation->SetOrigin(EC_Struct::HalfPoint(SourceAnimation->ClientSize));
            SourceAnimation->SetDepthByName(u"Weapon"_wref.get());
            SourceAnimation->SetPositionModeW(true);
            SourceAnimation->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            SourceAnimation->SequenceIndex = 0;
            SourceAnimation->UpdateAutoGeometry();
            SourceAnimation->StopAutoPlayback();
            SourceAnimationInterval = 1;
        } else if (I == 15) {
            Projectile = pas::construct_call<GI_PSWeapon16Esodafer::TPSWeapon16Esodafer>(GI_PSWeapon16Esodafer::TPSWeapon16Esodafer_Create, Space->MapPanel, ShotVisual);
        } else if (I == 16) {
            Projectile = pas::construct_call<GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor>(GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor_Create, Space->MapPanel, ShotVisual);
            TargetAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            TargetAnimation->SetImagePath(GI_PSWeapon17Kafacitor::KafacitorAnimationPaths[ShotVisual][0]);
            {
                WindowsSdk::TPoint contentSize_7 = TargetAnimation->GetContentSize();
                GI_GAI::TgaiGI* targetAnimation_7 = TargetAnimation;
                targetAnimation_7->SetSize(contentSize_7);
            }
            TargetAnimation->SetOrigin(EC_Struct::HalfPoint(TargetAnimation->ClientSize));
            TargetAnimation->SetDepthByName(u"Weapon"_wref.get());
            TargetAnimation->SetPositionModeW(true);
            {
                const pas::WideString& cpp_arg_6 = static_cast<pas::WideString>(pas::concat_ansi({"[40,0-", SysUtils::IntToStr(TargetAnimation->GetMainImageFrameCount() - 1), "]"}));
                GI_GAI::TgaiGI* targetAnimation_8 = TargetAnimation;
                targetAnimation_8->LoadFrameSequenceFromText(cpp_arg_6);
            }
            TargetAnimation->StopAutoPlayback();
            TargetAnimationInterval = 2;
        } else {
            Projectile = pas::construct_call<GI_PSEyes::TPSEyesGI>(GI_PSEyes::TPSEyesGI_Create, Space->MapPanel, ShotVisual);
        }
        if (HitVariant < 0) {
            if (EC_Str::CountDelimitedPartsW(pas::view(GraphKey), u","sv) > 2) {
                HitVariant = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(GraphKey), 2, u","sv)));
            } else if (UseRandomHit) {
                HitVariant = pas::random(EC_Str::ExtractDigitsToIntW(pas::view(WeaponConfig->GetParam(u"HitCount"sv))), &System::RandSeed) + 1;
            }
        }
        if (HitVariant > 0) {
            HitEffect = pas::construct_call<TWeaponEffect>(TWeaponEffect_Create, HitVariant, Space->MapPanel);
        }
        if (DepthExpression != u"") {
            if (TargetAnimation != nullptr) {
                TargetAnimation->SetDepthByName(DepthExpression);
            }
            if (SourceAnimation != nullptr) {
                SourceAnimation->SetDepthByName(DepthExpression);
            }
        }
        if (Projectile != nullptr) {
            if (DepthExpression != u"") {
                Projectile->SetDepthByName(DepthExpression);
            } else {
                Projectile->SetDepthByName(u"Weapon"_wref.get());
            }
            Projectile->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            Projectile->SetTargetPoint(EC_Struct::TruncatePointF(GetTargetPoint()));
            Projectile->SetPositionModeW(true);
        }
        DamageLabelPoint = GetTargetPoint();
        DamageLabelPoint.X = DamageLabelPoint.X - 3.0E+1L;
        DamageLabelPoint.Y = DamageLabelPoint.Y - 3.0E+1L;
        if (HitColor != 0) {
            DamageLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Space->MapPanel);
            DamageLabel->SetFontName(GlobalsV::NormalFontName);
            DamageLabel->SetDepthByName(u"HitPoint"_wref.get());
            DamageLabel->SetPosition(EC_Struct::TruncatePointF(DamageLabelPoint));
            if (HitDamage >= 0) {
                DamageLabel->SetText(pas::wide_int_to_str(HitDamage));
            } else {
                DamageLabel->SetText(static_cast<pas::WideString>(pas::concat_ansi({"+", SysUtils::IntToStr(-HitDamage)})));
            }
            DamageLabel->SetWordWrapEnabled(false);
            DamageLabel->SetTextAlignX(GI_Main::taxAuto);
            DamageLabel->SetTextAlignY(GI_Main::tayAuto);
            DamageLabel->SetPositionModeW(true);
            DamageLabel->SetMouseViewUpdates(true);
            DamageLabel->SetTextColor(HitColor);
        }
        if (TargetDestroyed) {
            if (DestructionEffect == 0) {
                if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(TargetObject) != nullptr || TargetObject->Size.X > 128 || TargetObject->Size.Y > 128) {
                    DestructionEffect = 4;
                }
            }
            if (DestructionEffect == 7) {
                if (TargetObject != nullptr) {
                    TargetObject->DetachFromSpace();
                }
            } else if (DestructionEffect == 6) {
                DestructionDetachStep = 10;
                if (TargetObject != nullptr) {
                    DestructionAlpha = TargetObject->GetAlpha();
                    DestructionAlphaStep = pas::real_divide(0.0L - DestructionAlpha, 11.0L);
                } else {
                    DestructionAlpha = 255.0f;
                    DestructionAlphaStep = 0.0f;
                }
            } else if (DestructionEffect == 0) {
                DestructionAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                DestructionAnimation->SetActive(false);
                {
                    const pas::WideString& cpp_arg_7 = static_cast<pas::WideString>(pas::concat_ansi({"Bm.Weapon.Expl", SysUtils::IntToStr(aMyFunction::RandomIntRange(0, 1))}));
                    GI_GAI::TgaiGI* destructionAnimation = DestructionAnimation;
                    destructionAnimation->SetImagePath(cpp_arg_7);
                }
                {
                    WindowsSdk::TPoint contentSize_8 = DestructionAnimation->GetContentSize();
                    GI_GAI::TgaiGI* destructionAnimation_2 = DestructionAnimation;
                    destructionAnimation_2->SetSize(contentSize_8);
                }
                DestructionAnimation->SetOrigin(EC_Struct::HalfPoint(DestructionAnimation->ClientSize));
                DestructionAnimation->SetDepthByName(u"Weapon"_wref.get());
                DestructionAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_8 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(DestructionAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* destructionAnimation_3 = DestructionAnimation;
                    destructionAnimation_3->LoadFrameSequenceFromText(cpp_arg_8);
                }
                DestructionAnimation->StopAutoPlayback();
                DestructionFrameInterval = 2;
                DestructionDetachStep = DestructionAnimation->SequenceFrameCount / 3;
                if (TargetObject != nullptr) {
                    DestructionAlpha = TargetObject->GetAlpha();
                    {
                        pas::Extended cpp_right = DestructionAnimation->GetMainImageFrameCount() / 2 - 1;
                        DestructionAlphaStep = pas::real_divide(0.0L - DestructionAlpha, cpp_right);
                    }
                } else {
                    DestructionAlpha = 255.0f;
                    DestructionAlphaStep = 0.0f;
                }
            } else if (DestructionEffect == 1) {
                DestructionAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                DestructionAnimation->SetActive(false);
                DestructionAnimation->SetImagePath(u"Bm.Weapon.Bomb"_wref.get());
                {
                    WindowsSdk::TPoint contentSize_9 = DestructionAnimation->GetContentSize();
                    GI_GAI::TgaiGI* destructionAnimation_4 = DestructionAnimation;
                    destructionAnimation_4->SetSize(contentSize_9);
                }
                DestructionAnimation->SetOrigin(EC_Struct::HalfPoint(DestructionAnimation->ClientSize));
                DestructionAnimation->SetDepthByName(u"Weapon"_wref.get());
                DestructionAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_9 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(DestructionAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* destructionAnimation_5 = DestructionAnimation;
                    destructionAnimation_5->LoadFrameSequenceFromText(cpp_arg_9);
                }
                DestructionAnimation->StopAutoPlayback();
                DestructionFrameInterval = 1;
                DestructionDetachStep = DestructionAnimation->SequenceFrameCount / 3;
            } else if (DestructionEffect == 4) {
                DestructionAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                DestructionAnimation->SetActive(false);
                DestructionAnimation->SetImagePath(u"Bm.Weapon.Expl0"_wref.get());
                {
                    WindowsSdk::TPoint contentSize_10 = DestructionAnimation->GetContentSize();
                    GI_GAI::TgaiGI* destructionAnimation_6 = DestructionAnimation;
                    destructionAnimation_6->SetSize(contentSize_10);
                }
                DestructionAnimation->SetOrigin(EC_Struct::HalfPoint(DestructionAnimation->ClientSize));
                DestructionAnimation->SetDepthByName(u"Weapon"_wref.get());
                DestructionAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_10 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(DestructionAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* destructionAnimation_7 = DestructionAnimation;
                    destructionAnimation_7->LoadFrameSequenceFromText(cpp_arg_10);
                }
                DestructionAnimation->StopAutoPlayback();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 3); cpp_range.next(I); ) {
                    ExtraDestructionAnimations[I] = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                    ExtraDestructionAnimations[I]->SetActive(false);
                    ExtraDestructionAnimations[I]->SetImagePath(u"Bm.Weapon.Expl0"_wref.get());
                    {
                        WindowsSdk::TPoint contentSize_11 = ExtraDestructionAnimations[I]->GetContentSize();
                        GI_GAI::TgaiGI* cpp_arg_11 = ExtraDestructionAnimations[I];
                        cpp_arg_11->SetSize(contentSize_11);
                    }
                    ExtraDestructionAnimations[I]->SetOrigin(EC_Struct::HalfPoint(ExtraDestructionAnimations[I]->ClientSize));
                    ExtraDestructionAnimations[I]->SetDepthByName(u"Weapon"_wref.get());
                    ExtraDestructionAnimations[I]->SetPositionModeW(true);
                    {
                        const pas::WideString& cpp_arg_12 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(ExtraDestructionAnimations[I]->GetMainImageFrameCount() - 1), "]"}));
                        GI_GAI::TgaiGI* cpp_arg_13 = ExtraDestructionAnimations[I];
                        cpp_arg_13->LoadFrameSequenceFromText(cpp_arg_12);
                    }
                    ExtraDestructionAnimations[I]->StopAutoPlayback();
                    Distance = aMyFunction::RandomIntRange(50, 100);
                    Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::RandomIntRange(0, 360));
                    ExtraDestructionAnimations[I]->UserValue = System::Round(System::Sin(Angle) * Distance);
                    ExtraDestructionAnimations[I]->UserIndex = System::Round(System::Cos(Angle) * -Distance);
                }
                DestructionFrameInterval = 2;
                if (TargetObject != nullptr) {
                    DestructionAlpha = TargetObject->GetAlpha();
                    {
                        pas::Extended cpp_right_2 = DestructionAnimation->GetMainImageFrameCount() / 2 - 1;
                        DestructionAlphaStep = pas::real_divide(0.0L - DestructionAlpha, cpp_right_2);
                    }
                } else {
                    DestructionAlpha = 255.0f;
                    DestructionAlphaStep = 0.0f;
                }
            } else if (DestructionEffect == 5) {
                TargetAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                TargetAnimation->SetImagePath(u"Bm.Weapon.Kamikaze"_wref.get());
                {
                    WindowsSdk::TPoint contentSize_12 = TargetAnimation->GetContentSize();
                    GI_GAI::TgaiGI* targetAnimation_9 = TargetAnimation;
                    targetAnimation_9->SetSize(contentSize_12);
                }
                TargetAnimation->SetOrigin(EC_Struct::HalfPoint(TargetAnimation->ClientSize));
                TargetAnimation->SetDepthByName(u"Weapon"_wref.get());
                TargetAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_14 = static_cast<pas::WideString>(pas::concat_ansi({"[1,0-", SysUtils::IntToStr(TargetAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* targetAnimation_10 = TargetAnimation;
                    targetAnimation_10->LoadFrameSequenceFromText(cpp_arg_14);
                }
                TargetAnimation->StopAutoPlayback();
                TargetAnimationInterval = 3;
                if (Globals::FilmSoundEffectsEnabled) {
                    if (GlobalsV::SoundInSpaceEnabled) {
                        if (Space->ContainsMapPoint(GetTargetPoint())) {
                            GR_Main::SoundManager->PlaySound(HitSoundPath);
                        }
                    }
                }
            } else if (DestructionEffect == 2) {
                DestructionAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                DestructionAnimation->SetActive(false);
                DestructionAnimation->SetImagePath(u"Bm.Asteroid.Des"_wref.get());
                {
                    WindowsSdk::TPoint contentSize_13 = DestructionAnimation->GetContentSize();
                    GI_GAI::TgaiGI* destructionAnimation_8 = DestructionAnimation;
                    destructionAnimation_8->SetSize(contentSize_13);
                }
                DestructionAnimation->SetOrigin(EC_Struct::HalfPoint(DestructionAnimation->ClientSize));
                DestructionAnimation->SetDepthByName(u"Weapon"_wref.get());
                DestructionAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_15 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(DestructionAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* destructionAnimation_9 = DestructionAnimation;
                    destructionAnimation_9->LoadFrameSequenceFromText(cpp_arg_15);
                }
                DestructionAnimation->StopAutoPlayback();
                DestructionFrameInterval = 4;
                DestructionDetachStep = DestructionAnimation->SequenceFrameCount / 3;
            } else if (DestructionEffect == 3) {
                DestructionAnimation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                DestructionAnimation->SetActive(false);
                DestructionAnimation->SetImagePath(u"Bm.Asteroid.Des"_wref.get());
                {
                    WindowsSdk::TPoint contentSize_14 = DestructionAnimation->GetContentSize();
                    GI_GAI::TgaiGI* destructionAnimation_10 = DestructionAnimation;
                    destructionAnimation_10->SetSize(contentSize_14);
                }
                DestructionAnimation->SetOrigin(EC_Struct::HalfPoint(DestructionAnimation->ClientSize));
                DestructionAnimation->SetDepthByName(u"Weapon"_wref.get());
                DestructionAnimation->SetPositionModeW(true);
                {
                    const pas::WideString& cpp_arg_16 = static_cast<pas::WideString>(pas::concat_ansi({"[50,0-", SysUtils::IntToStr(DestructionAnimation->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* destructionAnimation_11 = DestructionAnimation;
                    destructionAnimation_11->LoadFrameSequenceFromText(cpp_arg_16);
                }
                DestructionAnimation->StopAutoPlayback();
                DestructionFrameInterval = 4;
                DestructionDetachStep = DestructionAnimation->SequenceFrameCount / 5;
            }
            HasDestruction = true;
            if (ImmediateDestruction) {
                DestructionAnimation->SetActive(true);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 5); cpp_range_2.next(I); ) {
                    if (ExtraDestructionAnimations[I] != nullptr) {
                        ExtraDestructionAnimations[I]->SetActive(true);
                    }
                }
            }
        }
        if (HasDestruction) {
            if (ImmediateDestruction) {
                if (Globals::FilmSoundEffectsEnabled) {
                    if (GlobalsV::SoundInSpaceEnabled) {
                        if (Space->ContainsMapPoint(GetTargetPoint())) {
                            GR_Main::SoundManager->PlaySound(HitSoundPath);
                        }
                    }
                }
            }
        }
        if (PlayShotSound && Globals::FilmSoundEffectsEnabled && GlobalsV::SoundInSpaceEnabled) {
            if (Projectile != nullptr) {
                if (Space->ContainsMapPoint(EC_Struct::PointToPointF(Projectile->LocalPosition))) {
                    GR_Main::SoundManager->PlaySound(ShotSoundPath);
                }
            } else if (SourceAnimation != nullptr) {
                if (Space->ContainsMapPoint(EC_Struct::PointToPointF(SourceAnimation->LocalPosition))) {
                    GR_Main::SoundManager->PlaySound(ShotSoundPath);
                }
            }
        }
        if (HitEffect != nullptr) {
            HitEffect->Owner = Space->MapPanel;
            HitEffect->SetSourcePoint(GetSourcePoint());
            HitEffect->SetTargetPoint(GetTargetPoint());
            HitEffect->DepthExpression = u"Weapon"_w;
            HitEffect->Started = false;
        }
        StepIndex = 0;
    }

    void TWeaponSE::DetachFromSpace() {
        std::int32_t Index{};
        if (HitEffect != nullptr) {
            pas::free(HitEffect);
            HitEffect = nullptr;
        }
        if (SourceAnimation != nullptr) {
            pas::free(SourceAnimation);
            SourceAnimation = nullptr;
        }
        if (TargetAnimation != nullptr) {
            pas::free(TargetAnimation);
            TargetAnimation = nullptr;
        }
        if (Projectile != nullptr) {
            Projectile->Invalidate();
            pas::free(Projectile);
            Projectile = nullptr;
        }
        if (DamageLabel != nullptr) {
            DamageLabel->Invalidate();
            pas::free(DamageLabel);
            DamageLabel = nullptr;
        }
        if (DestructionAnimation != nullptr) {
            pas::free(DestructionAnimation);
            DestructionAnimation = nullptr;
        }
        for (Index = 0; Index <= 5; ++Index) {
            if (ExtraDestructionAnimations[Index] != nullptr) {
                pas::free(ExtraDestructionAnimations[Index]);
                ExtraDestructionAnimations[Index] = nullptr;
            }
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TWeaponSE::SetHit(std::int32_t Color, std::int32_t Damage, std::uint8_t Destroyed, std::uint8_t PlaySound) {
        HitColor = Color;
        HitDamage = Damage;
        TargetDestroyed = Destroyed;
        PlayShotSound = PlaySound;
    }

    // Retains both scene references.
    void TWeaponSE::SetEndpoints(SE_Space::TObjectSE* Source, SE_Space::TObjectSE* Target) {
        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&SourceObject), Source);
        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&TargetObject), Target);
    }

    EC_Struct::TPointF TWeaponSE::GetTargetPoint() {
        EC_Struct::TPointF Result{};
        if (TargetObject == nullptr) {
            Result = Position;
        } else if (SE_Ship2::TShip2SE* ship2SE = pas::class_cast_if<SE_Ship2::TShip2SE*>(TargetObject)) {
            std::uint8_t angle = ship2SE->GetAngle();
            SE_Ship2::TShip2SE* cpp_arg = ship2SE;
            Result = cpp_arg->GetTargetPoint(angle, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(TargetObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(SourceObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(this)) >> 2);
        } else {
            Result = TargetObject->Position;
        }
        return Result;
    }

    EC_Struct::TPointF TWeaponSE::GetSourcePoint() {
        EC_Struct::TPointF Result{};
        if (SourceObject == nullptr) {
            Result = Position;
        } else if (SE_Ship2::TShip2SE* ship2SE = pas::class_cast_if<SE_Ship2::TShip2SE*>(SourceObject)) {
            Result = ship2SE->GetWeaponPortPoint(SourceObject->GetAngle(), static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(TargetObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(SourceObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(this)) >> 2);
        } else if (SE_Ruins::TRuinsSE* ruinsSE = pas::class_cast_if<SE_Ruins::TRuinsSE*>(SourceObject)) {
            Result = ruinsSE->GetWeaponPortPoint(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(TargetObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(SourceObject)) + static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(this)) >> 2);
        } else {
            Result = SourceObject->Position;
        }
        return Result;
    }

    void TWeaponSE::Advance() {
        std::int32_t Index{};
        if (!IsAttachedToSpace()) {
            return;
        }
        if (HitEffect != nullptr && Projectile != nullptr) {
            if (!HitEffect->Started) {
                if (static_cast<std::uint8_t>(ProjectileFinished ^ 1) && HitEffect->LeftTime >= Projectile->RemainingTicks && HitEffect->BeforeEnd) {
                    HitEffect->Started = true;
                    HitEffect->Start();
                } else if (static_cast<std::uint8_t>(ProjectileFinished ^ 1) && static_cast<std::uint8_t>(HitEffect->BeforeEnd ^ 1) && Projectile->GetElapsedTicks() >= HitEffect->LeftTime) {
                    HitEffect->Started = true;
                    HitEffect->Start();
                }
            } else {
                HitEffect->SetSourcePoint(GetSourcePoint());
                HitEffect->SetTargetPoint(GetTargetPoint());
                if (HitEffect->IsFinished()) {
                    if (DestructionAnimation == nullptr) {
                        DetachFromSpace();
                        return;
                    }
                } else {
                    HitEffect->Advance();
                }
            }
        }
        if (Projectile != nullptr) {
            if (!ProjectileFinished) {
                Projectile->Advance(nullptr, 0);
            }
            if (static_cast<std::uint8_t>(ProjectileFinished ^ 1) && Projectile->IsFinished()) {
                ProjectileFinished = true;
                if (Projectile != nullptr) {
                    Projectile->SetActive(false);
                }
                if (DestructionAnimation != nullptr) {
                    if (!DestructionAnimation->Active) {
                        DestructionAnimation->SetActive(true);
                        for (Index = 0; Index <= 5; ++Index) {
                            if (ExtraDestructionAnimations[Index] != nullptr) {
                                ExtraDestructionAnimations[Index]->SetActive(true);
                            }
                        }
                        if (Globals::FilmSoundEffectsEnabled) {
                            if (GlobalsV::SoundInSpaceEnabled) {
                                if (Space->ContainsMapPoint(GetTargetPoint())) {
                                    GR_Main::SoundManager->PlaySound(HitSoundPath);
                                }
                            }
                        }
                    }
                } else if (HitEffect == nullptr && TargetAnimation == nullptr) {
                    DetachFromSpace();
                }
            }
        } else if (SourceAnimation == nullptr) {
            if (StepIndex >= 50 && static_cast<std::uint8_t>(ProjectileFinished ^ 1)) {
                ProjectileFinished = true;
                if (DestructionAnimation != nullptr) {
                    if (!DestructionAnimation->Active) {
                        DestructionAnimation->SetActive(true);
                        for (Index = 0; Index <= 5; ++Index) {
                            if (ExtraDestructionAnimations[Index] != nullptr) {
                                ExtraDestructionAnimations[Index]->SetActive(true);
                            }
                        }
                        if (Globals::FilmSoundEffectsEnabled) {
                            if (GlobalsV::SoundInSpaceEnabled) {
                                if (Space->ContainsMapPoint(GetTargetPoint())) {
                                    GR_Main::SoundManager->PlaySound(HitSoundPath);
                                }
                            }
                        }
                    }
                } else if (HitEffect == nullptr && TargetAnimation == nullptr) {
                    DetachFromSpace();
                }
            }
        } else if (SourceAnimation != nullptr) {
            if (SourceAnimation->SequenceFrame == SourceAnimation->SequenceFrameCount - 1) {
                pas::free(SourceAnimation);
                SourceAnimation = nullptr;
                ProjectileFinished = true;
                if (DestructionAnimation != nullptr) {
                    if (!DestructionAnimation->Active) {
                        DestructionAnimation->SetActive(true);
                        for (Index = 0; Index <= 5; ++Index) {
                            if (ExtraDestructionAnimations[Index] != nullptr) {
                                ExtraDestructionAnimations[Index]->SetActive(true);
                            }
                        }
                        if (Globals::FilmSoundEffectsEnabled) {
                            if (GlobalsV::SoundInSpaceEnabled) {
                                if (Space->ContainsMapPoint(GetTargetPoint())) {
                                    GR_Main::SoundManager->PlaySound(HitSoundPath);
                                }
                            }
                        }
                    }
                } else if (HitEffect == nullptr && TargetAnimation == nullptr) {
                    DetachFromSpace();
                }
            } else if (pas::imod(StepIndex, SourceAnimationInterval) == 0) {
                SourceAnimation->SetSequenceFrame(SourceAnimation->SequenceFrame + 1);
                SourceAnimation->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            }
        }
        if (TargetAnimation != nullptr) {
            if (TargetAnimation->SequenceFrame == TargetAnimation->SequenceFrameCount - 1) {
                pas::free(TargetAnimation);
                TargetAnimation = nullptr;
                if (DestructionAnimation == nullptr) {
                    DetachFromSpace();
                }
            } else {
                if (pas::imod(StepIndex, TargetAnimationInterval) == 0) {
                    TargetAnimation->SetSequenceFrame(TargetAnimation->SequenceFrame + 1);
                }
                TargetAnimation->SetPosition(EC_Struct::TruncatePointF(GetTargetPoint()));
            }
        }
        if (DestructionEffect == 6) {
            if (DestructionDetachStep > 0) {
                --DestructionDetachStep;
                if (DestructionDetachStep == 0) {
                    if (TargetObject != nullptr) {
                        TargetObject->DetachFromSpace();
                    }
                }
            }
        }
        if (DestructionAnimation != nullptr && DestructionAnimation->Active == true) {
            if (DestructionAnimation->SequenceFrame == DestructionAnimation->SequenceFrameCount - 1) {
                pas::free(DestructionAnimation);
                DestructionAnimation = nullptr;
                for (Index = 0; Index <= 5; ++Index) {
                    if (ExtraDestructionAnimations[Index] != nullptr) {
                        pas::free(ExtraDestructionAnimations[Index]);
                        ExtraDestructionAnimations[Index] = nullptr;
                    }
                }
                if (TargetAnimation == nullptr && ProjectileFinished) {
                    DetachFromSpace();
                }
            } else {
                if (pas::imod(StepIndex, DestructionFrameInterval) == 0) {
                    DestructionAnimation->SetSequenceFrame(DestructionAnimation->SequenceFrame + 1);
                    for (Index = 0; Index <= 5; ++Index) {
                        if (ExtraDestructionAnimations[Index] != nullptr) {
                            ExtraDestructionAnimations[Index]->SetSequenceFrame(ExtraDestructionAnimations[Index]->SequenceFrame + 1);
                        }
                    }
                    if (DestructionAnimation->SequenceFrame == DestructionDetachStep && TargetObject != nullptr) {
                        TargetObject->DetachFromSpace();
                    }
                }
                DestructionAnimation->SetPosition(EC_Struct::TruncatePointF(GetTargetPoint()));
                for (Index = 0; Index <= 5; ++Index) {
                    if (ExtraDestructionAnimations[Index] != nullptr) {
                        ExtraDestructionAnimations[Index]->SetPosition(ClassesImports::Point(DestructionAnimation->LocalPosition.X + ExtraDestructionAnimations[Index]->UserValue, DestructionAnimation->LocalPosition.Y + ExtraDestructionAnimations[Index]->UserIndex));
                    }
                }
                if (DestructionAlphaStep != 0.0L) {
                    DestructionAlpha = static_cast<long double>(DestructionAlpha) + DestructionAlphaStep;
                    if (DestructionAlpha < 0.0L) {
                        DestructionAlpha = 0.0f;
                    } else if (DestructionAlpha > 255.0L) {
                        DestructionAlpha = 255.0f;
                    }
                    TargetObject->SetAlpha(System::Round(DestructionAlpha));
                }
            }
        }
        if (Projectile != nullptr) {
            Projectile->SetPosition(EC_Struct::TruncatePointF(GetSourcePoint()));
            Projectile->SetTargetPoint(EC_Struct::TruncatePointF(GetTargetPoint()));
        }
        if (DamageLabel != nullptr) {
            DamageLabelPoint.X = DamageLabelPoint.X - 1.0L;
            DamageLabelPoint.Y = DamageLabelPoint.Y - 1.0L;
            DamageLabel->SetPosition(EC_Struct::RoundPointF(DamageLabelPoint));
        }
        ++StepIndex;
    }

    void TWeaponSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        EC_BlockPar::TBlockParEC* Palettes{};
        auto GetWeaponTemplateParam = [&](pas::WideString Name) -> pas::WideString {
            if (PaletteBlock != nullptr && PaletteBlock->CountParams(Name) > 0) {
                return PaletteBlock->GetParam(pas::view(Name));
            } else if (Block->CountParams(Name) > 0) {
                return Block->GetParam(pas::view(Name));
            } else {
                return pas::WideString();
            }
        };
        SE_Space::TObjectSE::LoadTemplate(Block);
        PaletteBlock = nullptr;
        if (Block->CountBlocks(u"Palettes"_wref.get()) > 0) {
            Palettes = Block->GetBlock(u"Palettes"sv);
            if (Palettes->CountBlocks(pas::wide_int_to_str(ShotVisual)) > 0) {
                PaletteBlock = Palettes->GetBlock(pas::view(pas::wide_int_to_str(ShotVisual)));
            }
        }
        ShotSoundPath = GetWeaponTemplateParam(u"SoundShot"_w);
        HitSoundPath = GetWeaponTemplateParam(u"SoundExpl"_w);
        if (PaletteBlock != nullptr && PaletteBlock->CountParams(u"PosZ"_wref.get()) > 0) {
            DepthExpression = PaletteBlock->GetParam(u"PosZ"sv);
        } else if (Block->CountParams(u"PosZ"_wref.get()) > 0) {
            DepthExpression = Block->GetParam(u"PosZ"sv);
        }
    }

    void TWeaponEffect_Create(TWeaponEffect* Self, std::int32_t AEffectIndex, GI_MessageLoop::TObjectGI* AOwner) {
        EC_BlockPar::TBlockParEC* Block{};
        Self->Started = true;
        Self->EffectIndex = AEffectIndex;
        Self->Owner = AOwner;
        Self->Direction = 0.0f;
        Self->AnimationInterval = 1;
        Self->AnimationCountdown = 0;
        Block = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"Weapon.", SysUtils::IntToStr(Self->EffectIndex)})));
        if (Block->CountParams(u"LeftTime"_wref.get()) > 0) {
            Self->LeftTime = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"LeftTime"sv)));
        } else {
            Self->LeftTime = 10;
        }
        if (Block->CountParams(u"BeforeEnd"_wref.get()) > 0) {
            Self->BeforeEnd = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"BeforeEnd"sv)));
        } else {
            Self->BeforeEnd = true;
        }
    }

    void TWeaponEffect_Destroy(TWeaponEffect* Self) {
        Self->Clear();
    }

    void TWeaponEffect::Clear() {
        while (FirstItem != nullptr) {
            RemoveItem(FirstItem);
        }
    }

    PWeaponEffectItem TWeaponEffect::AddItem() {
        PWeaponEffectItem Item{};
        pas::new_value(Item);
        Item->Next = nullptr;
        Item->Prev = LastItem;
        if (FirstItem == nullptr) {
            FirstItem = Item;
        } else {
            LastItem->Next = Item;
        }
        LastItem = Item;
        Item->Image = nullptr;
        pas::store_unaligned<EC_Struct::TPointF>(&Item->Position, EC_Struct::MakePointF(0.0f, 0.0f));
        Item->Angle = 0.0f;
        Item->Lifetime = 1;
        Item->Speed = 0.0f;
        Item->Acceleration = 0.0f;
        return Item;
    }

    void TWeaponEffect::RemoveItem(PWeaponEffectItem Item) {
        if (Item == nullptr) {
            return;
        }
        if (Item->Image != nullptr) {
            pas::free(Item->Image);
            Item->Image = nullptr;
        }
        if (Item->Next != nullptr) {
            Item->Next->Prev = Item->Prev;
        }
        if (Item->Prev != nullptr) {
            Item->Prev->Next = Item->Next;
        }
        if (LastItem == Item) {
            LastItem = Item->Prev;
        }
        if (FirstItem == Item) {
            FirstItem = Item->Next;
        }
        pas::dispose(Item);
    }

    void TWeaponEffect::AddTargetEffect(std::int32_t Index) {
        EC_BlockPar::TBlockParEC* Block{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"Weapon.", SysUtils::IntToStr(EffectIndex), ".D:", SysUtils::IntToStr(Index)})));
        PWeaponEffectItem Item = AddItem();
        Item->AtTarget = true;
        pas::store_unaligned<EC_Struct::TPointF>(&Item->Position, EC_Struct::MakePointF(0.0f, EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"StartPos"sv))));
        Item->Angle = pas::real_divide(EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Angle"sv))), 1.8E+2L) * SystemImports::Pi;
        Item->Speed = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Speed"sv));
        Item->Acceleration = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Accel"sv));
        if (Block->CountParams(u"AutoAnim"_wref.get()) > 0) {
            Item->AutoAnimation = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"AutoAnim"sv)));
        } else {
            Item->AutoAnimation = true;
        }
        if (Block->CountParams(u"LoopAnim"_wref.get()) > 0) {
            Item->LoopAnimation = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"LoopAnim"sv)));
        } else {
            Item->LoopAnimation = true;
        }
        if (Block->CountParams(u"SkipTime"_wref.get()) > 0) {
            Item->SkipTime = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"SkipTime"sv)));
        } else {
            Item->SkipTime = 0;
        }
        Item->Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        Item->Image->SetImagePath(Block->GetParam(u"Image"sv));
        Item->Image->SequenceIndex = 0;
        Item->Image->UpdateAutoGeometry();
        {
            WindowsSdk::TPoint contentSize = Item->Image->GetContentSize();
            GI_GAI::TgaiGI* image = Item->Image;
            image->SetSize(contentSize);
        }
        Item->Image->SetOrigin(EC_Struct::HalfPoint(Item->Image->ClientSize));
        Item->Image->SetDepthByName(DepthExpression);
        Item->Image->SetPosition(EC_Struct::TruncatePointF(aMyFunction::OffsetPointByRadiusAngle(TargetPoint, Item->Position.Y, Item->Angle)));
        Item->Image->SetPositionModeW(true);
        Item->Image->SetSequenceFrame(0);
        if (Item->SkipTime == 0) {
            Item->Image->RestartPlayback();
        } else {
            Item->Image->SetActive(false);
        }
        if (Block->CountParams(u"LifeTime"_wref.get()) > 0) {
            Item->Lifetime = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"LifeTime"sv)));
        } else {
            Item->Lifetime = Item->Image->SequenceFrameCount * AnimationInterval;
        }
        Item->Image->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
        if (Item->AutoAnimation && static_cast<std::uint8_t>(Item->LoopAnimation ^ 1)) {
            Item->Image->CycleCompleteCallback = pas::bind_method<&TWeaponEffect::AnimationComplete>(this);
        }
    }

    void TWeaponEffect::AddSourceEffect(std::int32_t Index) {
        EC_BlockPar::TBlockParEC* Block{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"Weapon.", SysUtils::IntToStr(EffectIndex), ".S:", SysUtils::IntToStr(Index)})));
        PWeaponEffectItem Item = AddItem();
        Item->AtTarget = false;
        pas::store_unaligned<EC_Struct::TPointF>(&Item->Position, EC_Struct::MakePointF(0.0f, EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"StartPos"sv))));
        Item->Angle = pas::real_divide(EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Angle"sv))), 1.8E+2L) * SystemImports::Pi;
        Item->Speed = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Speed"sv));
        Item->Acceleration = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Accel"sv));
        if (Block->CountParams(u"AutoAnim"_wref.get()) > 0) {
            Item->AutoAnimation = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"AutoAnim"sv)));
        } else {
            Item->AutoAnimation = true;
        }
        if (Block->CountParams(u"LoopAnim"_wref.get()) > 0) {
            Item->LoopAnimation = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"LoopAnim"sv)));
        } else {
            Item->LoopAnimation = true;
        }
        if (Block->CountParams(u"SkipTime"_wref.get()) > 0) {
            Item->SkipTime = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"SkipTime"sv)));
        } else {
            Item->SkipTime = 0;
        }
        Item->Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        Item->Image->SetImagePath(Block->GetParam(u"Image"sv));
        Item->Image->SequenceIndex = 0;
        Item->Image->UpdateAutoGeometry();
        {
            WindowsSdk::TPoint contentSize = Item->Image->GetContentSize();
            GI_GAI::TgaiGI* image = Item->Image;
            image->SetSize(contentSize);
        }
        Item->Image->SetOrigin(EC_Struct::HalfPoint(Item->Image->ClientSize));
        Item->Image->SetDepthByName(DepthExpression);
        Item->Image->SetPosition(EC_Struct::TruncatePointF(aMyFunction::OffsetPointByRadiusAngle(SourcePoint, Item->Position.Y, Item->Angle)));
        Item->Image->SetPositionModeW(true);
        Item->Image->SetSequenceFrame(0);
        if (Item->SkipTime == 0) {
            Item->Image->RestartPlayback();
        } else {
            Item->Image->SetActive(false);
        }
        if (Block->CountParams(u"LifeTime"_wref.get()) > 0) {
            Item->Lifetime = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"LifeTime"sv)));
        } else {
            Item->Lifetime = Item->Image->SequenceFrameCount * AnimationInterval;
        }
        Item->Image->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
        if (Item->AutoAnimation && static_cast<std::uint8_t>(Item->LoopAnimation ^ 1)) {
            Item->Image->CycleCompleteCallback = pas::bind_method<&TWeaponEffect::AnimationComplete>(this);
        }
    }

    void TWeaponEffect::Start() {
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"Weapon.", SysUtils::IntToStr(EffectIndex)})));
        if (Block->CountParams(u"AnimTakt"_wref.get()) > 0) {
            AnimationInterval = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"AnimTakt"sv)));
        } else {
            AnimationInterval = 1;
        }
        AnimationCountdown = AnimationInterval;
        std::int32_t Count = Block->CountBlocks(u"D"_wref.get());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            AddTargetEffect(Index);
        }
        Count = Block->CountBlocks(u"S"_wref.get());
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            AddSourceEffect(Index);
        }
    }

    void TWeaponEffect::Advance() {
        PWeaponEffectItem Previous{};
        // Preserve the native shared animation countdown and the per-item angle - 90 update.
        PWeaponEffectItem Item = FirstItem;
        while (Item != nullptr) {
            if (Item->SkipTime == 0) {
                --Item->Lifetime;
                Item->Position.Y = static_cast<long double>(Item->Position.Y) + Item->Speed;
                Item->Speed = static_cast<long double>(Item->Speed) + Item->Acceleration;
            }
            if (Item->Image != nullptr) {
                if (Item->AtTarget) {
                    Item->Image->SetPosition(EC_Struct::TruncatePointF(aMyFunction::OffsetPointByRadiusAngle(TargetPoint, Item->Position.Y, Item->Angle - 9.0E+1L)));
                } else {
                    Item->Image->SetPosition(EC_Struct::TruncatePointF(aMyFunction::OffsetPointByRadiusAngle(SourcePoint, Item->Position.Y, Item->Angle - 9.0E+1L)));
                }
                if (Item->SkipTime == 0) {
                    --AnimationCountdown;
                    if (AnimationCountdown == 0) {
                        if (!Item->AutoAnimation) {
                            if (Item->Image->SequenceFrame == Item->Image->SequenceFrameCount - 1) {
                                if (Item->LoopAnimation) {
                                    Item->Image->SetSequenceFrame(0);
                                } else {
                                    Item->Lifetime = 0;
                                }
                            } else {
                                Item->Image->SetSequenceFrame(Item->Image->SequenceFrame + 1);
                            }
                        }
                        AnimationCountdown = AnimationInterval;
                    }
                } else {
                    --Item->SkipTime;
                    if (Item->SkipTime == 0) {
                        Item->Image->SetActive(true);
                        Item->Image->RestartPlayback();
                    }
                }
            }
            Previous = Item;
            Item = Item->Next;
            if (Previous->Lifetime == 0) {
                RemoveItem(Previous);
            }
        }
    }

    void TWeaponEffect::AnimationComplete(GI_MessageLoop::TObjectGI* Sender) {
        PWeaponEffectItem Item = reinterpret_cast<PWeaponEffectItem>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        RemoveItem(Item);
    }

    std::uint8_t TWeaponEffect::IsFinished() {
        return FirstItem == nullptr;
    }

    void TWeaponEffect::SetSourcePoint(EC_Struct::TPointF Point) {
        SourcePoint = Point;
        float X = static_cast<long double>(TargetPoint.X) - SourcePoint.X;
        float Y = static_cast<long double>(TargetPoint.Y) - SourcePoint.Y;
        if (std::fabs(static_cast<pas::Extended>(X)) < 1.0L) {
            Direction = Math::ArcTan2(Y, 1.0L) + pas::constant(2.0L * SystemImports::Pi);
        } else {
            Direction = Math::ArcTan2(Y, X) + pas::constant(2.0L * SystemImports::Pi);
        }
    }

    void TWeaponEffect::SetTargetPoint(EC_Struct::TPointF Point) {
        TargetPoint = Point;
        float X = static_cast<long double>(TargetPoint.X) - SourcePoint.X;
        float Y = static_cast<long double>(TargetPoint.Y) - SourcePoint.Y;
        if (std::fabs(static_cast<pas::Extended>(X)) < 1.0L) {
            Direction = Math::ArcTan2(Y, 1.0L) + pas::constant(2.0L * SystemImports::Pi);
        } else {
            Direction = Math::ArcTan2(Y, X) + pas::constant(2.0L * SystemImports::Pi);
        }
    }

    void TWeaponEffect::p_destroy() {
        SE_Weapon::TWeaponEffect_Destroy(this);
    }

    void TWeaponSE::p_destroy() {
        SE_Weapon::TWeaponSE_Destroy(this);
    }

} // namespace SE_Weapon
