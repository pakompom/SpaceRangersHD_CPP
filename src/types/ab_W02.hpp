#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W02 {
    struct TabW02;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW02 : ab_Object::TabObject {
        PAS_CLASS_META(TabW02, ab_Object::TabObject, "TabW02", 196)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset);
        void Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::int32_t Phase;
        std::int32_t ExpireTick;
        std::int32_t ArmTick;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W02
