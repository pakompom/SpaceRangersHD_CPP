#pragma once
#include "runtime_support.hpp"

namespace ab_W {
    struct TabWeapon;

    #pragma pack(push, 1)
    struct TabWeapon {
        // Zero-based arcade weapon type.
        std::uint8_t Kind;
        // Corresponding campaign item type (50..67).
        std::uint8_t ItemType;
        std::uint8_t cpp_padding[2];
        // Campaign equipment slot index and secondary-fire flag; copied from TEquipment.AssignedSlotData.
        std::uint32_t SlotData;
        std::int32_t Ammo;
        std::int32_t MaxAmmo;
        std::int32_t RechargePerTick;
        std::int32_t AmmoCost;
        std::int32_t LastFireTick;
        std::int32_t FireIntervalTicks;
        std::int32_t Damage;
        std::uint8_t cpp_padding_2[4];
        double Range;
    };
    #pragma pack(pop)

    using PabWeapon = TabWeapon*;

} // namespace ab_W
