#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W14 {
    struct TabW14;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW14 : ab_Object::TabObject {
        PAS_CLASS_META(TabW14, ab_Object::TabObject, "TabW14", 192)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle);
        void Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::int32_t Phase;
        std::int32_t ExpireTick;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W14
