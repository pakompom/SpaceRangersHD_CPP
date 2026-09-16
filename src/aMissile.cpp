#include "layout/aMissile.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aAsteroid.hpp"
#include "types/aEFilm.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aPath.hpp"
#include "types/aShip.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMissile.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"

namespace aMissile {
    void TMissile_Create(TMissile* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextMissileId;
            ++aGalaxy::Galaxy->NextMissileId;
        }
        Self->WeaponId = 0;
        Self->PreviousTarget = nullptr;
        Self->LastTargetDistance = 1.0E+20f;
    }

    void TMissile_Destroy(TMissile* Self) {
        std::int32_t Index{};
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        if (Self->CurrentStar != nullptr) {
            Self->CurrentStar->ClearTargetReferences(Self);
            Index = pas::list_indexof(Self->CurrentStar->Missiles, reinterpret_cast<void*>(Self));
            if (Index >= 0) {
                pas::list_delete(Self->CurrentStar->Missiles, Index);
            }
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Registers the missile in Star, copies weapon data and initializes position, heading and speed.
    void TMissile::InitializeShot(aGalaxy::TStar* Star, aShip::TShip* OwnerShip, aItem::TWeapon* Weapon, pas::Object* Target, std::int32_t ShotIndex) {
        float DY{};
        float DX{};
        pas::list_add(Star->Missiles, reinterpret_cast<void*>(this));
        CurrentStar = Star;
        this->OwnerShip = OwnerShip;
        this->Target = Target;
        WeaponId = Weapon->Id;
        ItemType = static_cast<std::uint8_t>(Weapon->ItemType);
        MinDamage = OwnerShip->GetWeaponMinDamage(Weapon);
        MicroModuleIndex = Weapon->MicroModuleIndex;
        SpecialModuleIndex = Weapon->SpecialModuleIndex;
        MaxDamage = OwnerShip->GetWeaponMaxDamage(Weapon);
        std::int32_t SpeedBonus = this->OwnerShip->GetTotalStatBonus(aConst::bonMissileSpeed);
        std::int32_t SpecialBonus = 0;
        if (SpecialModuleIndex > 0) {
            SpecialBonus = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].StatBonuses[aConst::bonMissileSpeed];
        }
        if (MicroModuleIndex > 0) {
            SpeedBonus += aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses[aConst::bonMissileSpeed];
            if (SpecialBonus < 0) {
                SpecialBonus += System::Round(SpecialBonus * aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses[aConst::bonExtraAkrinPenalty] * 1.0E-4L);
            } else {
                SpecialBonus += System::Round(SpecialBonus * aConst::MicroModuleTemplates[MicroModuleIndex - 1].StatBonuses[aConst::bonExtraAkrinEff] * 1.0E-4L);
            }
        }
        SpeedBonus += SpecialBonus;
        TechLevel = Weapon->TechLevel;
        this->ShotIndex = ShotIndex;
        std::int32_t ShotCount = Weapon->GetShotCount();
        Direction = OwnerShip->MovementDirection;
        Position = OwnerShip->Position;
        if (ShotIndex > 0) {
            Direction = Direction - pas::real_divide(6.0E+1L, ShotCount + 3) * ((ShotIndex + 1) / 2) * (2 * (ShotIndex % 2) - 1);
            DX = System::Sin(aMyFunction::HeadingDegreesToRadians(Direction));
            DY = -System::Cos(aMyFunction::HeadingDegreesToRadians(Direction));
            Position = EC_Struct::MakePointF(Position.X + -DX * 8.0L, Position.Y + -DY * 8.0L);
        }
        MaximumSpeed = pas::real_max<pas::Extended>(static_cast<pas::Extended>(GetWeaponInfo()->MissileMinSpeed / 10), static_cast<long double>(aMyFunction::RemapClamped(TechLevel, 1.0, 8.0, GetWeaponInfo()->MissileMinSpeed, GetWeaponInfo()->MissileMaxSpeed)) + SpeedBonus);
    }

    // Caches Weapon.GetWeaponInfo before the base initializer.
    void TCustomMissile::InitializeShot_2(aGalaxy::TStar* Star, aShip::TShip* OwnerShip, aItem::TWeapon* Weapon, pas::Object* Target, std::int32_t ShotIndex) {
        WeaponInfo = Weapon->GetWeaponInfo();
        InitializeShot(Star, OwnerShip, Weapon, Target, ShotIndex);
    }

    void TMissile::InitializeUnownedShot(aGalaxy::TStar* Star, pas::Object* Target, std::int32_t X, std::int32_t Y, float Direction, std::int32_t MinDamage, std::int32_t MaxDamage, float MaximumSpeed, std::uint8_t ItemType, std::int32_t ModuleIndex, std::int32_t SpecialIndex) {
        pas::list_add(Star->Missiles, reinterpret_cast<void*>(this));
        CurrentStar = Star;
        OwnerShip = nullptr;
        this->Target = Target;
        WeaponId = 0;
        this->ItemType = ItemType;
        MicroModuleIndex = ModuleIndex;
        SpecialModuleIndex = SpecialIndex;
        this->MinDamage = MinDamage;
        this->MaxDamage = MaxDamage;
        this->Direction = Direction;
        Position = EC_Struct::MakePointF(X, Y);
        this->MaximumSpeed = MaximumSpeed;
        TechLevel = 1;
        ShotIndex = 0;
    }

    void TCustomMissile::InitializeUnownedShot_2(aGalaxy::TStar* Star, pas::Object* Target, std::int32_t X, std::int32_t Y, float Direction, std::int32_t MinDamage, std::int32_t MaxDamage, float MaximumSpeed, pas::WideString WeaponName, std::int32_t ModuleIndex, std::int32_t SpecialIndex) {
        WeaponInfo = aGalaxy::Galaxy->RequireCustomWeaponInfo(WeaponName);
        InitializeUnownedShot(Star, Target, X, Y, Direction, MinDamage, MaxDamage, MaximumSpeed, static_cast<std::uint8_t>(WeaponInfo->ItemType), ModuleIndex, SpecialIndex);
    }

    void TMissile::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(Id);
        Buffer->AddDWord(WeaponId);
        Buffer->AddAnsiChar(ItemType);
        Buffer->AddAnsiChar(TechLevel);
        Buffer->AddIntegerValue(MinDamage);
        Buffer->AddIntegerValue(MaxDamage);
        Buffer->AddIntegerValue(MicroModuleIndex);
        if (MicroModuleIndex > 0) {
            Buffer->AddDWord(aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNameHash);
        }
        Buffer->AddIntegerValue(SpecialModuleIndex);
        if (SpecialModuleIndex > 0) {
            Buffer->AddDWord(aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNameHash);
        }
        Buffer->AddSingle(Position.X);
        Buffer->AddSingle(Position.Y);
        Buffer->AddSingle(Direction);
        Buffer->AddSingle(TurnDirection);
        if (CurrentStar == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(CurrentStar->Id);
        }
        if (OwnerShip == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(OwnerShip->Id);
        }
        if (Target == nullptr) {
            Buffer->AddAnsiChar(0);
        } else if (pas::class_cast_if<aShip::TShip*>(Target) != nullptr) {
            Buffer->AddAnsiChar(1);
            Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(Target)->Id);
        } else if (pas::class_cast_if<aItem::TItem*>(Target) != nullptr) {
            Buffer->AddAnsiChar(2);
            Buffer->AddDWord(pas::checked_cast<aItem::TItem*>(Target)->Id);
        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Target) != nullptr) {
            Buffer->AddAnsiChar(3);
            Buffer->AddDWord(pas::checked_cast<aAsteroid::TAsteroid*>(Target)->Id);
        } else if (pas::class_cast_if<TMissile*>(Target) != nullptr) {
            Buffer->AddAnsiChar(4);
            Buffer->AddDWord(pas::checked_cast<TMissile*>(Target)->Id);
        } else {
            Buffer->AddAnsiChar(0);
        }
        Buffer->AddAnsiChar(ShotIndex);
        Buffer->AddIntegerValue(FlightTicks);
        Buffer->AddSingle(SourceHeading);
        Buffer->AddSingle(Speed);
        Buffer->AddSingle(MaximumSpeed);
        if (PreviousTarget == nullptr) {
            Buffer->AddAnsiChar(0);
        } else if (pas::class_cast_if<aShip::TShip*>(PreviousTarget) != nullptr) {
            Buffer->AddAnsiChar(1);
            Buffer->AddDWord(pas::checked_cast<aShip::TShip*>(PreviousTarget)->Id);
        } else if (pas::class_cast_if<aItem::TItem*>(PreviousTarget) != nullptr) {
            Buffer->AddAnsiChar(2);
            Buffer->AddDWord(pas::checked_cast<aItem::TItem*>(PreviousTarget)->Id);
        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(PreviousTarget) != nullptr) {
            Buffer->AddAnsiChar(3);
            Buffer->AddDWord(pas::checked_cast<aAsteroid::TAsteroid*>(PreviousTarget)->Id);
        } else if (pas::class_cast_if<TMissile*>(PreviousTarget) != nullptr) {
            Buffer->AddAnsiChar(4);
            Buffer->AddDWord(pas::checked_cast<TMissile*>(PreviousTarget)->Id);
        } else {
            Buffer->AddAnsiChar(0);
        }
        Buffer->AddSingle(LastTargetPosition.X);
        Buffer->AddSingle(LastTargetPosition.Y);
        Buffer->AddSingle(LastTargetDistance);
    }

    void TCustomMissile::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(WeaponInfo->ConfigName);
        aMissile::TMissile::SaveToBuffer(Buffer);
    }

    void TMissile::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* World) {
        std::int32_t ModuleNumber{};
        // Nested legacy lookup; caller pops the unused static link.
        auto FindLegacyMissileMicroModuleIndex = [&](std::int32_t ConfigNumber) -> std::int32_t {
            std::int32_t I{};
            std::int32_t Result = 0;
            {
                const std::int32_t cpp_last = aConst::MicroModuleTemplates.length() - 1;
                if (0 <= cpp_last) {
                    for (I = 0; I <= cpp_last; ++I) {
                        if (aConst::MicroModuleTemplates[I].ConfigNumber == ConfigNumber) {
                            return I + 1;
                        }
                    }
                }
            }
            return Result;
        };
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Id >= World->NextMissileId) {
            World->NextMissileId = Id + 1;
        }
        if (GlobalsV::LoadedSaveVersion >= 159) {
            WeaponId = EC_Buf::TBufEC_GetUInt32(Buffer);
        }
        ItemType = static_cast<std::uint8_t>(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
        TechLevel = EC_Buf::TBufEC_GetByte(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 100) {
            MinDamage = EC_Buf::TBufEC_GetInt32(Buffer);
            MaxDamage = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            MinDamage = EC_Buf::TBufEC_GetByte(Buffer);
            MaxDamage = EC_Buf::TBufEC_GetByte(Buffer);
        }
        if (GlobalsV::LoadedSaveVersion >= 157) {
            MicroModuleIndex = aItem::ReadSavedMicroModuleIndex(Buffer);
            SpecialModuleIndex = aItem::ReadSavedMicroModuleIndex(Buffer);
        } else if (GlobalsV::LoadedSaveVersion >= 82) {
            MicroModuleIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            SpecialModuleIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 98) {
                if (MicroModuleIndex > 0) {
                    ModuleNumber = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (aConst::MicroModuleTemplates.length() - 1 + 1 < MicroModuleIndex || aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber != ModuleNumber) {
                        MicroModuleIndex = FindLegacyMissileMicroModuleIndex(ModuleNumber);
                    }
                }
                if (SpecialModuleIndex > 0) {
                    ModuleNumber = EC_Buf::TBufEC_GetInt32(Buffer);
                    if (aConst::MicroModuleTemplates.length() - 1 + 1 < SpecialModuleIndex || aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber != ModuleNumber) {
                        SpecialModuleIndex = FindLegacyMissileMicroModuleIndex(ModuleNumber);
                    }
                }
            }
        } else {
            MicroModuleIndex = 0;
            SpecialModuleIndex = 0;
        }
        if (GlobalsV::LoadedSaveVersion < 98) {
            if (MicroModuleIndex != 0) {
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 24) {
                    ++MicroModuleIndex;
                }
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 124) {
                    ++MicroModuleIndex;
                }
                if (aConst::MicroModuleTemplates[MicroModuleIndex - 1].ConfigNumber > 219) {
                    ++MicroModuleIndex;
                }
            }
            if (SpecialModuleIndex != 0) {
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 24) {
                    ++SpecialModuleIndex;
                }
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 124) {
                    ++SpecialModuleIndex;
                }
                if (aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ConfigNumber > 219) {
                    ++SpecialModuleIndex;
                }
            }
        }
        Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
        Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        Direction = EC_Buf::TBufEC_GetSingle(Buffer);
        TurnDirection = EC_Buf::TBufEC_GetSingle(Buffer);
        CurrentStar = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        OwnerShip = reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        SavedTargetKind = EC_Buf::TBufEC_GetByte(Buffer);
        if (SavedTargetKind == 0) {
            Target = nullptr;
        } else {
            Target = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        }
        ShotIndex = EC_Buf::TBufEC_GetByte(Buffer);
        FlightTicks = EC_Buf::TBufEC_GetInt32(Buffer);
        SourceHeading = EC_Buf::TBufEC_GetSingle(Buffer);
        Speed = EC_Buf::TBufEC_GetSingle(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 95) {
            MaximumSpeed = EC_Buf::TBufEC_GetSingle(Buffer);
        } else {
            MaximumSpeed = aMyFunction::RemapClamped(TechLevel, 1.0, 8.0, GetWeaponInfo()->MissileMinSpeed, GetWeaponInfo()->MissileMaxSpeed);
        }
        SavedPreviousTargetKind = EC_Buf::TBufEC_GetByte(Buffer);
        if (SavedPreviousTargetKind == 0) {
            PreviousTarget = nullptr;
        } else {
            PreviousTarget = reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        }
        LastTargetPosition.X = EC_Buf::TBufEC_GetSingle(Buffer);
        LastTargetPosition.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        LastTargetDistance = EC_Buf::TBufEC_GetSingle(Buffer);
    }

    void TCustomMissile::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* World) {
        WeaponInfo = World->RequireCustomWeaponInfo(Buffer->ReadWideString());
        aMissile::TMissile::LoadFromBuffer(Buffer, World);
    }

    void TMissile::ResolveLoadedReferences(aGalaxy::TGalaxy* World) {
        CurrentStar = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(World->IdToStar(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(CurrentStar)))));
        OwnerShip = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(World->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(OwnerShip)), true)));
        if (SavedTargetKind == 1) {
            Target = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(World->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Target)), true)));
        } else if (SavedTargetKind == 2) {
            Target = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(World->IdToItem(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Target)), true)));
        } else if (SavedTargetKind == 3) {
            Target = pas::checked_cast<aAsteroid::TAsteroid*>(static_cast<pas::Object*>(World->IdToAsteroid(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Target)))));
        } else if (SavedTargetKind == 4) {
            Target = pas::checked_cast<TMissile*>(static_cast<pas::Object*>(World->IdToMissile(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Target)))));
        }
        if (SavedPreviousTargetKind == 1) {
            PreviousTarget = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(World->IdToShip(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PreviousTarget)), true)));
        } else if (SavedPreviousTargetKind == 2) {
            PreviousTarget = pas::checked_cast<aItem::TItem*>(static_cast<pas::Object*>(World->IdToItem(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PreviousTarget)), true)));
        } else if (SavedPreviousTargetKind == 3) {
            PreviousTarget = pas::checked_cast<aAsteroid::TAsteroid*>(static_cast<pas::Object*>(World->IdToAsteroid(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PreviousTarget)))));
        } else if (SavedPreviousTargetKind == 4) {
            PreviousTarget = pas::checked_cast<TMissile*>(static_cast<pas::Object*>(World->IdToMissile(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(PreviousTarget)))));
        }
    }

    // Lazily creates and initializes the retained missile scene object.
    SE_Space::TObjectSE* TMissile::GetGraphObject() {
        if (Graphic == nullptr) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Missile"_wref.get(), pas::concat_wide({u"Missile.w", GetGraphSuffix()}), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
            }
            Graphic->SetPosition(Position);
            Graphic->SetAngle(aMyFunction::HeadingDegreesToByte(Direction));
            Graphic->SetAlpha(255);
        }
        return Graphic;
    }

    void TMissile::PrepareTurnMovement(std::int32_t StepIndex, std::uint8_t RecordFilm, std::uint8_t PlayShotSound) {
        float PathLength{};
        float TurnFraction{};
        EC_BlockPar::TBlockParEC* Config{};
        EC_BlockPar::TBlockParEC* Palette{};
        if (RecordFilm) {
            {
                SE_Space::TObjectSE* graphObject = GetGraphObject();
                std::uint32_t id = Id;
                aEFilm::TEFilm* primaryFilm = Globals::PrimaryFilm;
                FilmObject = primaryFilm->AddObject(id, graphObject, 0, 0);
            }
            Globals::PrimaryFilm->SetObjectPosition(StepIndex, FilmObject, Position);
            Globals::PrimaryFilm->SetObjectAngle(StepIndex, FilmObject, aMyFunction::HeadingDegreesToByte(Direction));
            Globals::PrimaryFilm->AttachObject(StepIndex, FilmObject);
            if (PlayShotSound) {
                if (pas::class_cast_if<TCustomMissile*>(this) != nullptr) {
                    Config = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.", GetWeaponInfo()->PrimarySE}));
                } else {
                    Config = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"SE.Weapon.", SysUtils::IntToStr(ItemType - 50)})));
                }
                Palette = Config->FindBlock(u"Palettes"_wref.get());
                if (Palette != nullptr) {
                    Palette = Palette->FindBlock(pas::wide_int_to_str(GetShotVisual()));
                }
                if (Palette != nullptr && Palette->CountParams(u"SoundShot"_wref.get()) > 0) {
                    Globals::PrimaryFilm->PlayObjectSound(StepIndex, FilmObject, Palette->GetParam(u"SoundShot"_wref.get()));
                } else if (Config->CountParams(u"SoundShot"_wref.get()) > 0) {
                    Globals::PrimaryFilm->PlayObjectSound(StepIndex, FilmObject, Config->GetParam(u"SoundShot"_wref.get()));
                } else if (pas::class_cast_if<TCustomMissile*>(this) != nullptr) {
                    Globals::PrimaryFilm->PlayObjectSound(StepIndex, FilmObject, pas::concat_wide({u"Sound.shot", GetWeaponInfo()->ConfigName}));
                } else {
                    Globals::PrimaryFilm->PlayObjectSound(StepIndex, FilmObject, static_cast<pas::WideString>(pas::concat_ansi({"Sound.shot", SysUtils::IntToStr(ItemType - 50)})));
                }
            }
        }
        if (FlightTicks == 0) {
            if (OwnerShip != nullptr) {
                SourceHeading = OwnerShip->MovementDirection;
                Speed = pas::real_max<float>(static_cast<float>(OwnerShip->Speed), MaximumSpeed);
                PathLength = 0.0f;
                if (OwnerShip->MovementPath != nullptr && OwnerShip->MovementPath->NodeCount > 0) {
                    PathLength = OwnerShip->MovementPath->GetLength();
                    TurnFraction = pas::real_divide(StepIndex, CurrentStar->MovementStepCount);
                    if (1.0L - TurnFraction == 0.0L) {
                        PathLength = 0.0f;
                    } else {
                        PathLength = pas::real_divide(PathLength, 1.0L - TurnFraction);
                    }
                    if (aMyFunction::HeadingDifferenceDegrees(OwnerShip->MovementDirection, aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(OwnerShip->MovementPath->ActiveTail->Position.X) - OwnerShip->Position.X, -(static_cast<long double>(OwnerShip->MovementPath->ActiveTail->Position.Y) - OwnerShip->Position.Y)))) < 0.0L) {
                        TurnDirection = -1.0f;
                    } else {
                        TurnDirection = 1.0f;
                    }
                } else {
                    TurnDirection = 1.0f;
                }
                // The initial maximum above is overwritten in the native routine too.
                Speed = PathLength + 1.0E+2L;
                if (Speed < 2.0E+2L) {
                    Speed = 2.0E+2f;
                }
            } else {
                Speed = MaximumSpeed;
                SourceHeading = 0.0f;
                TurnDirection = 0.0f;
            }
            ++FlightTicks;
        }
        OvershootTicks = -1;
    }

    // Returns a hit ship, item or asteroid, or nil when no object was hit.
    pas::Object* TMissile::StepDay(std::int32_t StepIndex, std::uint8_t RecordFilm) {
        pas::Object* Result{};
        double StepScale{};
        double TargetHeading{};
        double Delta{};
        double Separation{};
        EC_Struct::TPointF PreviousPosition{};
        EC_Struct::TPointF TargetPosition{};
        TMissile* OtherMissile{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t VerticalSign{};
        aAsteroid::TAsteroid* Asteroid{};
        aShip::TShip* Ship{};
        std::uint8_t HasTarget{};
        aItem::TItem* Item{};
        float DesiredSpeed{};
        float DistanceSquared{};
        aGalaxy::PMovingDropItemEntry Drop{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* EffectFilm{};
        std::int32_t Stage = 0;
        try {
            Result = nullptr;
            PreviousPosition = Position;
            StepScale = pas::real_divide(2.0E+2L, CurrentStar->MovementStepCount);
            FlightTicks += System::Round(StepScale);
            TargetPosition = EC_Struct::MakePointF(0.0f, 0.0f);
            HasTarget = false;
            DesiredSpeed = MaximumSpeed;
            Stage = 1;
            if (FlightTicks < 200) {
                Stage = 2;
                if (OwnerShip != nullptr) {
                    Stage = 3;
                    Delta = static_cast<long double>(OwnerShip->MovementDirection) - SourceHeading;
                    SourceHeading = OwnerShip->MovementDirection;
                    Separation = ShotIndex * 0.1L + 0.1L;
                    if (Delta > 0.0L) {
                        Delta = Delta - static_cast<long double>(Separation) * StepScale;
                    } else if (Delta < 0.0L) {
                        Delta = Delta + static_cast<long double>(Separation) * StepScale;
                    }
                    if (Delta < -0.5L * StepScale) {
                        Delta = -0.5L * StepScale;
                    } else if (Delta > 0.5L * StepScale) {
                        Delta = 0.5L * StepScale;
                    }
                    Direction = aMyFunction::WrapHeadingDegrees(static_cast<long double>(Direction) + Delta);
                    if (RecordFilm) {
                        Globals::PrimaryFilm->SetObjectAngle(StepIndex, FilmObject, aMyFunction::HeadingDegreesToByte(Direction));
                    }
                    {
                        pas::Extended cpp_left = System::Sin(aMyFunction::HeadingDegreesToRadians(Direction));
                        Position.X = Position.X + cpp_left * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                    }
                    {
                        pas::Extended cpp_left_2 = System::Cos(aMyFunction::HeadingDegreesToRadians(Direction));
                        Position.Y = Position.Y - cpp_left_2 * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                    }
                } else {
                    Stage = 4;
                    {
                        pas::Extended cpp_left_3 = System::Sin(aMyFunction::HeadingDegreesToRadians(Direction));
                        Position.X = Position.X + cpp_left_3 * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                    }
                    {
                        pas::Extended cpp_left_4 = System::Cos(aMyFunction::HeadingDegreesToRadians(Direction));
                        Position.Y = Position.Y - cpp_left_4 * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                    }
                }
            } else {
                Stage = 5;
                RetargetTorpedo();
                Stage = 6;
                if (Target != nullptr) {
                    if (pas::class_cast_if<aShip::TShip*>(Target) != nullptr && reinterpret_cast<aShip::TShip*>(Target)->CurrentStar == CurrentStar && reinterpret_cast<aShip::TShip*>(Target)->InNormalSpace()) {
                        Stage = 7;
                        TargetPosition = reinterpret_cast<aShip::TShip*>(Target)->Position;
                        HasTarget = true;
                    } else if (pas::class_cast_if<aItem::TItem*>(Target) != nullptr) {
                        Stage = 8;
                        TargetPosition = reinterpret_cast<aItem::TItem*>(Target)->Position;
                        HasTarget = true;
                    } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Target) != nullptr) {
                        Stage = 9;
                        TargetPosition = reinterpret_cast<aAsteroid::TAsteroid*>(Target)->Position;
                        HasTarget = true;
                    } else if (pas::class_cast_if<TMissile*>(Target) != nullptr) {
                        Stage = 10;
                        TargetPosition = reinterpret_cast<TMissile*>(Target)->Position;
                        HasTarget = true;
                    } else {
                        Stage = 11;
                        PreviousTarget = Target;
                        Target = nullptr;
                    }
                }
                if (HasTarget) {
                    Stage = 12;
                    VerticalSign = -1;
                    if (FlightTicks > 1 && LastTargetPosition.X == TargetPosition.X && LastTargetPosition.Y == TargetPosition.Y) {
                        Stage = 13;
                        DistanceSquared = aMyFunction::PointDistanceSquared(Position, TargetPosition);
                        if (LastTargetDistance < DistanceSquared && OvershootTicks < 0) {
                            std::int32_t round = System::Round(pas::real_divide(Speed, 1.0E+1L));
                            std::int32_t round_2 = System::Round(pas::real_divide(Speed, 4.0E+1L));
                            OvershootTicks = aMyFunction::RandomIntRange(round_2, round);
                        }
                        LastTargetDistance = DistanceSquared;
                        if (OvershootTicks > 0) {
                            VerticalSign = 1;
                            --OvershootTicks;
                        }
                    }
                    LastTargetPosition = TargetPosition;
                    TargetHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(TargetPosition.X) - Position.X, (static_cast<long double>(TargetPosition.Y) - Position.Y) * VerticalSign));
                    Stage = 14;
                    Delta = aMyFunction::HeadingDifferenceDegrees(Direction, TargetHeading);
                    if (std::fabs(static_cast<pas::Extended>(Delta)) <= 3.0L * StepScale) {
                        Direction = TargetHeading;
                    } else if (Delta < 0.0L) {
                        Direction = aMyFunction::WrapHeadingDegrees(Direction - 3.0L * StepScale);
                    } else {
                        Direction = aMyFunction::WrapHeadingDegrees(Direction + 3.0L * StepScale);
                    }
                    Stage = 14;
                    if (RecordFilm) {
                        Globals::PrimaryFilm->SetObjectAngle(StepIndex, FilmObject, aMyFunction::HeadingDegreesToByte(Direction));
                    }
                }
                Stage = 15;
                if (std::fabs(static_cast<long double>(Speed) - DesiredSpeed) <= 1.0E+1L) {
                    Speed = DesiredSpeed;
                } else if (Speed < DesiredSpeed) {
                    Speed = Speed + 1.0E+1L;
                } else if (Speed > DesiredSpeed) {
                    Speed = Speed - 1.0E+1L;
                }
                {
                    pas::Extended cpp_left_5 = System::Sin(aMyFunction::HeadingDegreesToRadians(Direction));
                    Position.X = Position.X + cpp_left_5 * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                }
                {
                    pas::Extended cpp_left_6 = System::Cos(aMyFunction::HeadingDegreesToRadians(Direction));
                    Position.Y = Position.Y - cpp_left_6 * (pas::real_divide(Speed, 2.0E+2L) * StepScale);
                }
            }
            Stage = 16;
            if (RecordFilm) {
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, FilmObject, Position);
            }
            Stage = 17;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Missiles) - 1); cpp_range.next(I); ) {
                Stage = 18;
                OtherMissile = pas::list_at<TMissile>(CurrentStar->Missiles, I);
                if (OtherMissile != this && (Target == OtherMissile || OtherMissile->Target == this) && aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, OtherMissile->Position, 1.0E+1f)) {
                    Stage = 19;
                    if (RecordFilm) {
                        Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                        EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                        Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Position);
                        Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                        Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                        Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
                        Globals::PrimaryFilm->DetachObject(StepIndex, OtherMissile->FilmObject);
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&OtherMissile->Graphic));
                    }
                    OtherMissile->DestroyQueued = true;
                    DestroyQueued = true;
                    return Result;
                }
            }
            Stage = 20;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_2.next(I); ) {
                Stage = 21;
                Item = pas::list_at<aItem::TItem>(CurrentStar->Items, I);
                Stage = 22;
                if ((GetWeaponInfo()->ShotType != aGalaxyStruct::wstTorpedo || Target == Item) && aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, Item->Position, 1.0E+1f)) {
                    Stage = 23;
                    J = pas::list_count(CurrentStar->MovingDropItems) - 1;
                    while (J >= 0) {
                        Drop = pas::list_at<aGalaxy::TMovingDropItemEntry>(CurrentStar->MovingDropItems, J);
                        if (Drop->Payload == Item) {
                            break;
                        }
                        --J;
                    }
                    if (J < 0) {
                        return Item;
                    }
                }
            }
            Stage = 24;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_3.next(I); ) {
                Stage = 25;
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (static_cast<std::uint8_t>(Ship->IsHullDestroyed() ^ 1) && Ship->InNormalSpace()) {
                    if (TryReturnToOwner(StepIndex, RecordFilm, PreviousPosition, Ship)) {
                        return Result;
                    }
                    if (OwnerShip != Ship && (Target == Ship || OwnerShip == nullptr || OwnerShip->GetRelationLevelToShip(Ship) == aGalaxyStruct::rlHostile)) {
                        Stage = 26;
                        if (aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, Ship->Position, 2.0E+1f)) {
                            return Ship;
                        }
                    }
                }
            }
            Stage = 27;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Asteroids) - 1); cpp_range_4.next(I); ) {
                Asteroid = pas::list_at<aAsteroid::TAsteroid>(CurrentStar->Asteroids, I);
                Stage = 28;
                if (aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, Asteroid->Position, 1.0E+1f)) {
                    return Asteroid;
                }
            }
            Stage = 29;
            if (aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, EC_Struct::MakePointF(0.0f, 0.0f), CurrentStar->Radius * 0.7L)) {
                if (RecordFilm) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Position);
                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
                }
                DestroyQueued = true;
                return Result;
            }
            Stage = 30;
            if (FlightTicks > 1000 && (Target == nullptr || !(FlightTicks * 0.005L * MaximumSpeed < GetWeaponInfo()->MissileRange))) {
                if (RecordFilm) {
                    Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.Asteroid"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                    EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                    Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Position);
                    Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                    Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                    Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
                }
                DestroyQueued = true;
                return Result;
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TMissile.StepDay, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
        return Result;
    }

    std::uint8_t TMissile::TryReturnToOwner(std::int32_t StepIndex, std::uint8_t RecordFilm, EC_Struct::TPointF PreviousPosition, aShip::TShip* Ship) {
        std::int32_t I{};
        std::uint8_t FoundWeapon{};
        aItem::TItem* Item{};
        aItem::TWeapon* Weapon{};
        SE_Space::TObjectSE* Effect{};
        aEFilm::TEFilmObj* EffectFilm{};
        std::uint8_t Result = false;
        if (GetWeaponInfo()->ShotType == aGalaxyStruct::wstTorpedo && OwnerShip != nullptr && Ship == OwnerShip && Target == OwnerShip && aMyFunction::SegmentIntersectsCircle(PreviousPosition, Position, Ship->Position, 2.0E+1f)) {
            FoundWeapon = false;
            {
                const std::int32_t cpp_last = static_cast<std::int32_t>(Ship->WeaponCount);
                if (1 <= cpp_last) {
                    for (I = 1; I <= cpp_last; ++I) {
                        Weapon = Ship->Weapons[I];
                        if (Weapon != nullptr && Weapon->Id == WeaponId) {
                            FoundWeapon = true;
                            if (Weapon->Ammo < Weapon->AmmoCapacity) {
                                ++Weapon->Ammo;
                            }
                            break;
                        }
                    }
                }
            }
            if (!FoundWeapon) {
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Ship->Inventory) - 1); cpp_range.next(I); ) {
                    Item = pas::list_at<aItem::TItem>(Ship->Inventory, I);
                    if (Item->Id == WeaponId) {
                        if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && reinterpret_cast<aItem::TWeapon*>(Item)->Ammo < reinterpret_cast<aItem::TWeapon*>(Item)->AmmoCapacity) {
                            ++reinterpret_cast<aItem::TWeapon*>(Item)->Ammo;
                        }
                        break;
                    }
                }
            }
            if (RecordFilm) {
                Effect = pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, u"Weapon.NoGraph"_wref.get(), ClassesImports::Point(0, 0), 0, -1);
                EffectFilm = Globals::PrimaryFilm->AddObject(0u, Effect, 0, 0);
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, EffectFilm, Position);
                Globals::PrimaryFilm->SetWeaponHit(StepIndex, EffectFilm, 0, 0, false, true);
                Globals::PrimaryFilm->AttachObject(StepIndex, EffectFilm);
                Globals::PrimaryFilm->DetachObject(StepIndex, FilmObject);
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Graphic));
            }
            DestroyQueued = true;
            return true;
        }
        return Result;
    }

    void TMissile::RetargetTorpedo() {
        float BestDistance{};
        float Distance{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        aShip::TShip* BestShip{};
        if (GetWeaponInfo()->ShotType == aGalaxyStruct::wstTorpedo && OwnerShip != nullptr && (Target == nullptr || Target == OwnerShip)) {
            BestDistance = 1.0E+30f;
            BestShip = nullptr;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship != PreviousTarget && Ship->GetRelationLevelToShip(OwnerShip) <= aGalaxyStruct::rlHostile && (!(pas::class_cast_if<aKling::TKling*>(OwnerShip) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(OwnerShip)->IsPlayerCamouflageEffective(Ship) ^ 1)) && (!(pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Ship)->IsPlayerCamouflageEffective(OwnerShip) ^ 1))) {
                    Distance = aMyFunction::PointDistance(Ship->Position, Position);
                    if (Distance <= 7.0E+2L && Distance < BestDistance) {
                        BestDistance = Distance;
                        BestShip = Ship;
                    }
                }
            }
            if (BestShip != nullptr) {
                Target = BestShip;
            } else if (OwnerShip->InNormalSpace() && OwnerShip->CurrentStar == CurrentStar) {
                Target = OwnerShip;
            } else {
                Target = nullptr;
            }
        }
    }

    pas::WideString TMissile::GetDisplayName() {
        pas::WideString Result{};
        Result = aConst::LocalizedText(pas::concat_wide({u"Items.Weapon.Missile.", GetGraphSuffix(), u".Name"}));
        if (SpecialModuleIndex != 0) {
            return pas::concat_wide({Result, u" ", aConst::MicroModuleTemplates[SpecialModuleIndex - 1].Name});
        }
        return Result;
    }

    pas::WideString TMissile::GetInfoText() {
        pas::WideString Result{};
        pas::WideString SpeedText{};
        pas::WideString DamageText{};
        float DamageFactor{};
        Result = pas::concat_wide({aConst::LocalizedText(pas::concat_wide({u"Items.Weapon.Missile.", GetGraphSuffix(), u".Text"})), u"\r\n"});
        if (OwnerShip != nullptr) {
            Result = pas::concat_wide({Result, ([&] {
                pas::WideString name = OwnerShip->GetName();
                pas::WideString localizedText = aConst::LocalizedText(u"Items.Weapon.Missile.TextFrom"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name));
            }()), u"\r\n"});
        }
        if (Target != nullptr && pas::class_cast_if<aAsteroid::TAsteroid*>(Target) != nullptr) {
            Result = pas::concat_wide({Result, ([&] {
                pas::WideString displayName = reinterpret_cast<aAsteroid::TAsteroid*>(Target)->GetDisplayName();
                pas::WideString localizedText_2 = aConst::LocalizedText(u"Items.Weapon.Missile.TextTarget"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName));
            }()), u"\r\n"});
        } else if (Target != nullptr && pas::class_cast_if<aItem::TItem*>(Target) != nullptr) {
            Result = pas::concat_wide({Result, ([&] {
                pas::WideString displayName_2 = reinterpret_cast<aItem::TItem*>(Target)->GetDisplayName();
                pas::WideString localizedText_3 = aConst::LocalizedText(u"Items.Weapon.Missile.TextTarget"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName_2));
            }()), u"\r\n"});
        } else if (Target != nullptr && pas::class_cast_if<aShip::TShip*>(Target) != nullptr) {
            Result = pas::concat_wide({Result, ([&] {
                pas::WideString name_2 = reinterpret_cast<aShip::TShip*>(Target)->GetName();
                pas::WideString localizedText_4 = aConst::LocalizedText(u"Items.Weapon.Missile.TextTarget"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_2));
            }()), u"\r\n"});
        } else if (Target != nullptr && pas::class_cast_if<TMissile*>(Target) != nullptr) {
            Result = pas::concat_wide({Result, ([&] {
                pas::WideString displayName_3 = reinterpret_cast<TMissile*>(Target)->GetDisplayName();
                pas::WideString localizedText_5 = aConst::LocalizedText(u"Items.Weapon.Missile.TextTarget"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_5), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(displayName_3));
            }()), u"\r\n"});
        } else {
            Result = pas::concat_wide({Result, aConst::LocalizedText(u"Items.Weapon.Missile.TextNoTarget"_wref.get()), u"\r\n"});
        }
        if (OwnerShip != nullptr) {
            if (aPlayer::GetPlayer()->HasScannerArtefact(OwnerShip)) {
                if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(OwnerShip) || aPlayer::GetPlayer() == OwnerShip || aPlayer::GetPlayer() == OwnerShip->PartnerShip || OwnerShip->TypeId == aGalaxyStruct::stTranclucator) {
                    SpeedText = pas::wide_int64_to_str(System::Round(Speed));
                    if (OwnerShip->TypeId == aGalaxyStruct::stKling && pas::checked_cast<aKling::TKling*>(OwnerShip)->KlingType == aGalaxyStruct::ktBoss) {
                        DamageFactor = aGalaxy::Galaxy->InterpolateDifficulty(-1, 0.7f, 1.0f, 1.2f, 1.5f) * 2.0L;
                    } else {
                        DamageFactor = 1.0f;
                    }
                    DamageText = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(System::Round(static_cast<long double>(MinDamage) * DamageFactor)), "-", SysUtils::Int64ToStr(System::Round(static_cast<long double>(MaxDamage) * DamageFactor))}));
                } else {
                    SpeedText = u"???"_w;
                    DamageText = u"???"_w;
                }
                Result = pas::concat_wide({Result, aMyFunction::FormatText1(aConst::LocalizedText(u"Items.Weapon.Missile.TextSpeed"_wref.get()), u"<color=255,240,100>"_w, u"<Speed>"_w, SpeedText), u", "});
                return pas::concat_wide({Result, aMyFunction::FormatText1(aConst::LocalizedText(u"Items.Weapon.Missile.TextDamage"_wref.get()), u"<color=255,240,100>"_w, u"<Damage>"_w, DamageText), u"\r\n"});
            }
        }
        return Result;
    }

    std::uint8_t TMissile::CanBeHit(aShip::TShip* Attacker, aItem::TWeapon* UnusedWeapon) {
        std::int32_t Roll{};
        if (Attacker == nullptr) {
            Roll = aMyFunction::NextRandomIntRange(1, 100, aGalaxy::Galaxy->RandomState);
        } else {
            if (pas::class_cast_if<aKling::TKling*>(Attacker) != nullptr && static_cast<aKling::TKling*>(Attacker)->KlingType == 0) {
                return true;
            }
            Roll = aMyFunction::NextRandomIntRange(1, 100, Attacker->RandomState);
        }
        return Roll <= GetWeaponInfo()->MissileChanceToBeHit;
    }

    void TMissile::ClearReferencesTo(pas::Object* Obj) {
        if (OwnerShip == Obj) {
            OwnerShip = nullptr;
        }
        if (Target == Obj) {
            Target = nullptr;
        }
        if (PreviousTarget == Obj) {
            PreviousTarget = nullptr;
        }
    }

    // Special micromodule override unless -1, otherwise the weapon-info default.
    std::int32_t TMissile::GetShotVisual() {
        if (SpecialModuleIndex == 0 || aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ShotVisual == -1) {
            return GetWeaponInfo()->DefaultPalette;
        }
        return aConst::MicroModuleTemplates[SpecialModuleIndex - 1].ShotVisual;
    }

    pas::WideString TMissile::GetGraphSuffix() {
        pas::WideString Result{};
        if (SpecialModuleIndex != 0) {
            Result = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].MissileGraph;
        }
        if (Result == u"") {
            return pas::wide_int_to_str(ItemType - 50 + 1);
        }
        return Result;
    }

    pas::WideString TCustomMissile::GetGraphSuffix() {
        pas::WideString Result{};
        if (SpecialModuleIndex != 0) {
            Result = aConst::MicroModuleTemplates[SpecialModuleIndex - 1].MissileGraph;
        }
        if (Result == u"") {
            return WeaponInfo->ConfigName;
        }
        return Result;
    }

    aConst::PWeaponInfo TMissile::GetWeaponInfo() {
        return &aConst::WeaponInfos[ItemType];
    }

    aConst::PWeaponInfo TCustomMissile::GetWeaponInfo() {
        return WeaponInfo;
    }

    void TMissile::p_destroy() {
        aMissile::TMissile_Destroy(this);
    }

} // namespace aMissile
