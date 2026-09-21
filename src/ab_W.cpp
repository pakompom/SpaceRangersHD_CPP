#include "layout/ab_W.hpp"
#include "types/EC_Cache.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_Sound.hpp"
#include "types/aConst.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/System.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_W.hpp"
#include "units/ab_W01.hpp"
#include "units/ab_W02.hpp"
#include "units/ab_W03.hpp"
#include "units/ab_W04.hpp"
#include "units/ab_W05.hpp"
#include "units/ab_W06.hpp"
#include "units/ab_W07.hpp"
#include "units/ab_W08.hpp"
#include "units/ab_W09.hpp"
#include "units/ab_W10.hpp"
#include "units/ab_W11.hpp"
#include "units/ab_W12.hpp"
#include "units/ab_W13.hpp"
#include "units/ab_W14.hpp"
#include "units/ab_W15.hpp"
#include "units/ab_W16.hpp"
#include "units/ab_W17.hpp"
#include "units/ab_W18.hpp"

// Shared arcade weapon records and dispatch:.
// Inferred ownership; original unit boundary remains unresolved.
namespace ab_W {
    void ab_Weapon_InitializeFromInfo(PabWeapon Weapon, aConst::PWeaponInfo Info) {
        ab_W::ab_Weapon_Initialize(Weapon, Info->ArcadeWeaponType);
    }

    void ab_Weapon_Initialize(PabWeapon Weapon, std::uint8_t ItemType) {
        Weapon->ItemType = ItemType;
        if (ItemType == 50) {
            Weapon->Kind = 0;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 3;
            Weapon->AmmoCost = 100;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 8;
            Weapon->Damage = 4;
            Weapon->Range = 6.0E+2;
        } else if (ItemType == 51) {
            Weapon->Kind = 1;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 100;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 5;
            Weapon->Damage = 5;
            Weapon->Range = 4.0E+2;
        } else if (ItemType == 52) {
            Weapon->Kind = 2;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 200;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 30;
            Weapon->Damage = 12;
            Weapon->Range = 9.0E+2;
        } else if (ItemType == 53) {
            Weapon->Kind = 3;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 200;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 30;
            Weapon->Damage = 15;
            Weapon->Range = 5.0E+2;
        } else if (ItemType == 54) {
            Weapon->Kind = 4;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 300;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 30;
            Weapon->Damage = 1400;
            Weapon->Range = 5.0E+2;
        } else if (ItemType == 55) {
            Weapon->Kind = 5;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 25;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 2;
            Weapon->Damage = 2;
            Weapon->Range = 3.0E+2;
        } else if (ItemType == 56) {
            Weapon->Kind = 6;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 1;
            Weapon->AmmoCost = 100;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 10;
            Weapon->Damage = 15;
            Weapon->Range = 7.0E+2;
        } else if (ItemType == 57) {
            Weapon->Kind = 7;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 300;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 10;
            Weapon->Damage = 50;
            Weapon->Range = 5.0E+2;
        } else if (ItemType == 58) {
            Weapon->Kind = 8;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 1;
            Weapon->AmmoCost = 250;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 20;
            Weapon->Damage = 35;
            Weapon->Range = 9.0E+2;
        } else if (ItemType == 59) {
            Weapon->Kind = 9;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 500;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 30;
            Weapon->Damage = 80;
            Weapon->Range = 9.0E+2;
        } else if (ItemType == 60) {
            Weapon->Kind = 10;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 3;
            Weapon->AmmoCost = 500;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 30;
            Weapon->Damage = 80;
            Weapon->Range = 1.1E+3;
        } else if (ItemType == 61) {
            Weapon->Kind = 11;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 500;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 50;
            Weapon->Damage = 200;
            Weapon->Range = 5.0E+2;
        } else if (ItemType == 62) {
            Weapon->Kind = 12;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 400;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 40;
            Weapon->Damage = 80;
            Weapon->Range = 4.5E+2;
        } else if (ItemType == 63) {
            Weapon->Kind = 13;
            Weapon->Ammo = 10000;
            Weapon->MaxAmmo = 10000;
            Weapon->RechargePerTick = 10;
            Weapon->AmmoCost = 7000;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 50;
            Weapon->Damage = 80;
            Weapon->Range = 2.0E+3;
        } else if (ItemType == 64) {
            Weapon->Kind = 14;
            Weapon->Ammo = 10000;
            Weapon->MaxAmmo = 10000;
            Weapon->RechargePerTick = 10;
            Weapon->AmmoCost = 9000;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 60;
            Weapon->Damage = 100;
            Weapon->Range = 1.0E+3;
        } else if (ItemType == 65) {
            Weapon->Kind = 15;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 1;
            Weapon->AmmoCost = 250;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 20;
            Weapon->Damage = 30;
            Weapon->Range = 9.0E+2;
        } else if (ItemType == 66) {
            Weapon->Kind = 16;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 150;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 10;
            Weapon->Damage = 14;
            Weapon->Range = 9.0E+2;
        } else if (ItemType == 67) {
            Weapon->Kind = 17;
            Weapon->Ammo = 1000;
            Weapon->MaxAmmo = 1000;
            Weapon->RechargePerTick = 2;
            Weapon->AmmoCost = 200;
            Weapon->LastFireTick = 0;
            Weapon->FireIntervalTicks = 20;
            Weapon->Damage = 20;
            Weapon->Range = 1.5E+2;
        }
    }

    void ab_Weapon_Fire(PabWeapon Weapon, ab_Object::TabObject* Owner, float DamageScale) {
        ab_W01::TabW01* W01{};
        ab_W02::TabW02* W02{};
        ab_W03::TabW03* W03{};
        ab_W04::TabW04* W04{};
        ab_W05::TabW05* W05{};
        ab_W06::TabW06* W06{};
        ab_W07::TabW07* W07{};
        ab_W08::TabW08* W08{};
        ab_W09::TabW09* W09{};
        ab_W10::TabW10* W10{};
        ab_W11::TabW11* W11{};
        ab_W12::TabW12* W12{};
        ab_W13::TabW13* W13{};
        ab_W14::TabW14* W14{};
        ab_W15::TabW15* W15{};
        ab_W16::TabW16* W16{};
        ab_W17::TabW17* W17{};
        ab_W18::TabW18* W18{};
        float Angle{};
        std::int32_t Index{};
        auto ConfigureObjectSound = [&](ab_Object::TabObject* Obj, std::int32_t Kind) -> void {
            if (Globals::ArcadeWeaponLoopTicks[Kind] >= 0) {
                Obj->SoundDelay = Globals::ArcadeWeaponLoopTicks[Kind];
                Obj->SoundPath = Globals::ArcadeWeaponLoopSounds[Kind];
                Obj->SoundGroup = Kind + 9000;
            }
        };
        if (Globals::ArcadeWeaponFirstSounds[Weapon->Kind] != u"") {
            if (ab_Global::IsDepthBeforeSphereHorizon(Owner->GetProjectedPosition().Z)) {
                GR_Main::SoundManager->PlaySound(Globals::ArcadeWeaponFirstSounds[Weapon->Kind]);
            }
        }
        if (Weapon->Kind == 0) {
            W01 = pas::construct_call<ab_W01::TabW01>(ab_W01::TabW01_Create);
            ab_Object::ab_Object_Add(W01);
            W01->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W01, 0);
        } else if (Weapon->Kind == 1) {
            W02 = pas::construct_call<ab_W02::TabW02>(ab_W02::TabW02_Create);
            ab_Object::ab_Object_Add(W02);
            W02->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W02, 1);
        } else if (Weapon->Kind == 2) {
            W03 = pas::construct_call<ab_W03::TabW03>(ab_W03::TabW03_Create);
            ab_Object::ab_Object_Add(W03);
            W03->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W03, 2);
        } else if (Weapon->Kind == 3) {
            W04 = pas::construct_call<ab_W04::TabW04>(ab_W04::TabW04_Create);
            ab_Object::ab_Object_Add(W04);
            W04->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W04, 3);
        } else if (Weapon->Kind == 4) {
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), -15.0f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), -1.0E+1f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), -5.0f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), 0.0f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), 5.0f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), 1.0E+1f);
            ConfigureObjectSound(W05, 4);
            W05 = pas::construct_call<ab_W05::TabW05>(ab_W05::TabW05_Create);
            ab_Object::ab_Object_Add(W05);
            W05->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 7.0L)), 15.0f);
            ConfigureObjectSound(W05, 4);
        } else if (Weapon->Kind == 5) {
            W06 = pas::construct_call<ab_W06::TabW06>(ab_W06::TabW06_Create);
            ab_Object::ab_Object_Add(W06);
            W06->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W06, 5);
        } else if (Weapon->Kind == 6) {
            W07 = pas::construct_call<ab_W07::TabW07>(ab_W07::TabW07_Create);
            ab_Object::ab_Object_Add(W07);
            W07->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W07, 6);
        } else if (Weapon->Kind == 7) {
            W08 = pas::construct_call<ab_W08::TabW08>(ab_W08::TabW08_Create);
            ab_Object::ab_Object_Add(W08);
            W08->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f, 0, nullptr);
            ConfigureObjectSound(W08, 7);
        } else if (Weapon->Kind == 8) {
            W09 = pas::construct_call<ab_W09::TabW09>(ab_W09::TabW09_Create);
            ab_Object::ab_Object_Add(W09);
            W09->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale));
            ConfigureObjectSound(W09, 8);
        } else if (Weapon->Kind == 9) {
            W10 = pas::construct_call<ab_W10::TabW10>(ab_W10::TabW10_Create);
            ab_Object::ab_Object_Add(W10);
            W10->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W10, 9);
        } else if (Weapon->Kind == 10) {
            W11 = pas::construct_call<ab_W11::TabW11>(ab_W11::TabW11_Create);
            ab_Object::ab_Object_Add(W11);
            W11->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W11, 10);
        } else if (Weapon->Kind == 11) {
            W12 = pas::construct_call<ab_W12::TabW12>(ab_W12::TabW12_Create);
            ab_Object::ab_Object_Add(W12);
            W12->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 2.0L)), 0.0f);
            ConfigureObjectSound(W12, 11);
            W12 = pas::construct_call<ab_W12::TabW12>(ab_W12::TabW12_Create);
            ab_Object::ab_Object_Add(W12);
            W12->Launch(Owner, System::Round(pas::real_divide(static_cast<long double>(Weapon->Damage) * DamageScale, 2.0L)), 1.8E+2f);
            ConfigureObjectSound(W12, 11);
        } else if (Weapon->Kind == 12) {
            W13 = pas::construct_call<ab_W13::TabW13>(ab_W13::TabW13_Create);
            ab_Object::ab_Object_Add(W13);
            W13->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f, 0, nullptr);
            ConfigureObjectSound(W13, 12);
            W13 = pas::construct_call<ab_W13::TabW13>(ab_W13::TabW13_Create);
            ab_Object::ab_Object_Add(W13);
            W13->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 4.0E+1f, 0, nullptr);
            ConfigureObjectSound(W13, 12);
            W13 = pas::construct_call<ab_W13::TabW13>(ab_W13::TabW13_Create);
            ab_Object::ab_Object_Add(W13);
            W13->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 3.2E+2f, 0, nullptr);
            ConfigureObjectSound(W13, 12);
        } else if (Weapon->Kind == 13) {
            Angle = 0.0f;
            Index = 0;
            while (Angle < 3.6E+2L) {
                W14 = pas::construct_call<ab_W14::TabW14>(ab_W14::TabW14_Create);
                ab_Object::ab_Object_Add(W14);
                W14->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), Angle);
                if ((Index & 1) == 0) {
                    ConfigureObjectSound(W14, 13);
                }
                Angle = Angle + 15.0L;
                ++Index;
            }
        } else if (Weapon->Kind == 14) {
            Angle = 0.0f;
            while (Angle < 3.6E+2L) {
                W15 = pas::construct_call<ab_W15::TabW15>(ab_W15::TabW15_Create);
                ab_Object::ab_Object_Add(W15);
                W15->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), Angle);
                ConfigureObjectSound(W15, 14);
                Angle = Angle + 45.0L;
            }
        } else if (Weapon->Kind == 15) {
            W16 = pas::construct_call<ab_W16::TabW16>(ab_W16::TabW16_Create);
            ab_Object::ab_Object_Add(W16);
            W16->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale));
            ConfigureObjectSound(W16, 8);
        } else if (Weapon->Kind == 16) {
            W17 = pas::construct_call<ab_W17::TabW17>(ab_W17::TabW17_Create);
            ab_Object::ab_Object_Add(W17);
            W17->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale));
            ConfigureObjectSound(W17, 2);
        } else if (Weapon->Kind == 17) {
            W18 = pas::construct_call<ab_W18::TabW18>(ab_W18::TabW18_Create);
            ab_Object::ab_Object_Add(W18);
            W18->Launch(Owner, System::Round(static_cast<long double>(Weapon->Damage) * DamageScale), 0.0f);
            ConfigureObjectSound(W18, 3);
        }
    }

    void ab_Weapon_QueueImageLoad(PabWeapon Weapon, pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        switch (Weapon->Kind) {
            case 0: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w01b_s"_wref.get());
                break;
            }
            case 1: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w02_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w02_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w02a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w02a_s"_wref.get());
                break;
            }
            case 2: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w03_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w03_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w03a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w03a_s"_wref.get());
                break;
            }
            case 3: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04b_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04c_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w04c_s"_wref.get());
                break;
            }
            case 4: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w05b_s"_wref.get());
                break;
            }
            case 5: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w06_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w06_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w06a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w06a_s"_wref.get());
                break;
            }
            case 6: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w07b_s"_wref.get());
                break;
            }
            case 7: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w08b_s"_wref.get());
                break;
            }
            case 8: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09b_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09c_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w09c_s"_wref.get());
                break;
            }
            case 9: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w10b_s"_wref.get());
                break;
            }
            case 10: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w11b_s"_wref.get());
                break;
            }
            case 11: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w12b_s"_wref.get());
                break;
            }
            case 12: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w13b_s"_wref.get());
                break;
            }
            case 13: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w14_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w14_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w14a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w14a_s"_wref.get());
                break;
            }
            case 14: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w15_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w15_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w15a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w15a_s"_wref.get());
                break;
            }
            case 15: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16b_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16c_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w16c_s"_wref.get());
                break;
            }
            case 16: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17a_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17b_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17b_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17c_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w17c_s"_wref.get());
                break;
            }
            case 17: {
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w18_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w18_s"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w18a_f"_wref.get());
                GR_Main::GlobalCache->QueueNamedLoadIfMissing(PendingLoads, u"GAI"sv, u"Bm.AB.w18a_s"_wref.get());
                break;
            }
        }
    }

} // namespace ab_W
