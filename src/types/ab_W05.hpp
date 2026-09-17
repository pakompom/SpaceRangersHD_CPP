#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W05 {
    struct TabW05;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW05 : ab_Object::TabObject {
        PAS_CLASS_META(TabW05, ab_Object::TabObject, "TabW05", 228)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset);
        void virtual_TabObject_Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::uint8_t Exploding;
        std::uint8_t cpp_padding[3];
        float TrailDistance;
        EC_Struct::TVector3D LastTrailPosition;
        std::int32_t ExpireTick;
        float TurnDelta;
        pas::List* TrailImages;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W05
