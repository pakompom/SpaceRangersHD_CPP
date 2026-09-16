#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W04 {
    struct TabW04;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW04 : ab_Object::TabObject {
        PAS_CLASS_META(TabW04, ab_Object::TabObject, "TabW04", 200)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset);
        void LaunchChild(TabW04* Parent, float Angle);
        void Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::int32_t Phase;
        std::int32_t ExpireTick;
        std::int32_t AimTick;
        std::uint8_t cpp_padding[4];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W04
