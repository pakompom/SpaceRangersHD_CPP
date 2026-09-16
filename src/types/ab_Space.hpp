#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace ab_Space {
    struct TabSpaceLink;

    struct TabSpace;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabSpace : EC_Struct::TObjectEx {
        PAS_CLASS_META(TabSpace, EC_Struct::TObjectEx, "TabSpace", 108)
        void p_destroy() override;
        void ClearVisuals();
        // Empty native update hook.
        static void UpdateVisuals();
        void CreateImage();
        void ClearImage();
        void Update();
        void ClearObjects();
        void PopulateObjects();
        void PopulateHoleEncounter();
        void PopulateScriptedEncounter();
        void PopulateKellerEncounter();
        void UpdateApproachDanger();
        // Native instance receiver is unused; visits the complete graph.
        static void PruneApproachDanger();
        pas::WideString GetDangerText();
        void RecountLinks();
        TabSpace* Prev;
        TabSpace* Next;
        Types::TPoint GridPosition;
        Types::TPoint MapPosition;
        std::uint8_t cpp_padding[4];
        std::int32_t IncomingCount;
        std::int32_t OutgoingCount;
        // Map rendering color; precise role pending.
        std::uint32_t Color28;
        std::uint32_t Color2C;
        std::uint32_t Color30;
        std::uint32_t Color34;
        // Six difficulty/visual variants, each with six palette entries.
        std::int32_t AppearanceIndex;
        // Arena resource path.
        pas::WideString MapPath;
        // 1 for the synthetic start/end nodes.
        std::int32_t BoundaryKind;
        // Used by exit-index shuffling; initializer still under review.
        std::int32_t PortalSlotCount;
        // Local encounter difficulty, used for danger text, visuals and rewards.
        double Danger;
        // Minimum accumulated predecessor danger, followed by graph pruning.
        double ApproachDanger;
        // Temporary reverse-search cost for the selected route.
        double RouteCost;
        // Owned objects associated with this space.
        pas::List* Objects;
        std::uint8_t ImageActive;
        std::uint8_t cpp_padding_2[3];
        GI_GAI::TgaiGI* Image;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PabSpaceLink = TabSpaceLink*;

    // Allocation size verified; remaining fields unresolved.
    #pragma pack(push, 1)
    struct TabSpaceLink {
        PabSpaceLink Prev;
        PabSpaceLink Next;
        TabSpace* First;
        TabSpace* Last;
        std::int32_t ExitIndex;
        // Arrow outline and halo geometry.
        pas::Array<Types::TPoint, 0, 10> Points;
    };
    #pragma pack(pop)

} // namespace ab_Space
