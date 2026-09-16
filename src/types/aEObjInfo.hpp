#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/aConst.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aEObjInfo {
    struct TEObjInfo;

    struct TEOTCustomStarInfo;

    struct TEOTPlanet;

    struct TEOTShip;

    struct TEOTItem;

    struct TEOTAsteroid;

    struct TEOTMissile;

    // Native record RTTI.
    struct TEOTPlanet {
        std::uint32_t Id;
        pas::WideString Name;
        std::uint8_t OwnerId;
        std::uint8_t RaceId;
        std::uint8_t cpp_padding[2];
        std::int32_t Population;
        aGalaxyStruct::TPlanetEconomy Economy;
        aGalaxyStruct::TPlanetGovernment Government;
        aGalaxyStruct::TRelationLevel Relation;
        std::uint8_t cpp_padding_2[1];
        std::int32_t UnexploredWater;
        std::int32_t UnexploredLand;
        std::int32_t UnexploredHills;
        pas::WideString TreasureHint;
        pas::WideString Faction;
    };

    // Native record RTTI.
    struct TEOTShip {
        std::uint32_t Id;
        pas::WideString Name;
        pas::WideString FullName;
        std::uint8_t OwnerId;
        aGalaxyStruct::TDominatorSeries DominatorSeries;
        std::uint8_t cpp_padding[2];
        pas::WideString TypeName;
        std::int32_t Speed;
        std::int32_t HullCapacity;
        std::int32_t HullPoints;
        double HullFragility;
        // Live snapshot only; absent from the serialized format.
        std::uint8_t OutsideNormalSpace;
        std::uint8_t ScannerResolved;
        std::uint8_t cpp_padding_2[2];
        pas::WideString DefenseText;
        pas::WideString DamageText;
        std::int32_t RepairPoints;
        aGalaxyStruct::TRelationLevel Relation;
        std::uint8_t cpp_padding_3[3];
        std::int32_t WinChance;
        pas::WideString PortraitImage;
        std::int32_t CombatStatusCount;
        pas::WideString CombatStatusText;
        pas::WideString Faction;
    };

    // Native record RTTI.
    struct TEOTItem {
        std::uint32_t Id;
        pas::WideString Name;
        pas::WideString ImagePath;
        aConst::TItemType ItemType;
        std::uint8_t cpp_padding[3];
        pas::WideString InfoText;
        std::int32_t Weight;
        std::int32_t Cost;
        std::uint8_t OwnerId;
        std::uint8_t cpp_padding_2[3];
        double ConditionPercent;
        double Fragility;
        aGalaxyStruct::TDominatorSeries DominatorSeries;
        std::uint8_t cpp_padding_3[3];
        pas::WideString Faction;
    };

    // Native record RTTI.
    struct TEOTAsteroid {
        std::uint32_t Id;
        pas::WideString Name;
        pas::WideString InfoText;
    };

    // Native record RTTI.
    struct TEOTMissile {
        std::uint32_t Id;
        pas::WideString Name;
        pas::WideString InfoText;
    };

    // Native record RTTI.
    struct TEOTCustomStarInfo {
        pas::WideString Name;
        pas::WideString ImagePath;
        pas::WideString Text;
        std::int32_t Distance;
    };

    using PEPlanetInfo = TEOTPlanet*;

    using PEShipInfo = TEOTShip*;

    using PEItemInfo = TEOTItem*;

    using PEAsteroidInfo = TEOTAsteroid*;

    using PEMissileInfo = TEOTMissile*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEObjInfo : EC_Struct::TObjectEx {
        PAS_CLASS_META(TEObjInfo, EC_Struct::TObjectEx, "TEObjInfo", 36)
        void p_destroy() override;
        void Clear();
        void LoadFromStar(aGalaxy::TStar* Star);
        // Borrowed pointer into the snapshot array.
        PEPlanetInfo FindPlanet(std::uint32_t ObjectId);
        // Borrowed pointer into the snapshot array.
        PEShipInfo FindShip(std::uint32_t ObjectId);
        // Borrowed pointer into the snapshot array.
        PEItemInfo FindItem(std::uint32_t ObjectId);
        // Borrowed pointer into the snapshot array.
        PEAsteroidInfo FindAsteroid(std::uint32_t ObjectId);
        // Borrowed pointer into the snapshot array.
        PEMissileInfo FindMissile(std::uint32_t ObjectId);
        // Appends the current snapshot; doubles are serialized as singles.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Legacy versions omit scanner, repair, faction or custom-system fields.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, std::int32_t Version);
        pas::WideString StarName;
        std::int32_t StarRadius;
        pas::DynArray<TEOTPlanet> Planets;
        pas::DynArray<TEOTShip> Ships;
        pas::DynArray<TEOTItem> Items;
        pas::DynArray<TEOTAsteroid> Asteroids;
        pas::DynArray<TEOTMissile> Missiles;
        pas::DynArray<TEOTCustomStarInfo> CustomSystemInfos;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PECustomSystemInfo = TEOTCustomStarInfo*;

} // namespace aEObjInfo
