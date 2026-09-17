#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W01 {
    struct TabW01;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW01 : ab_Object::TabObject {
        PAS_CLASS_META(TabW01, ab_Object::TabObject, "TabW01", 236)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset);
        void Advance() override;
        void UpdateVisuals() override;
        // Constructor sets 500; no consumer in this family.
        float ValueB0;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::uint8_t Exploding;
        std::uint8_t cpp_padding[3];
        float TrailDistance;
        std::uint8_t cpp_padding_2[4];
        EC_Struct::TVector3D LastTrailPosition;
        std::int32_t ExpireTick;
        float TurnSpeed;
        pas::List* TrailImages;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W01
