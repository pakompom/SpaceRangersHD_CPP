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
        std::uint32_t Color28;
        std::uint32_t Color2C;
        std::uint32_t Color30;
        std::uint32_t Color34;
        std::int32_t AppearanceIndex;
        pas::WideString MapPath;
        std::int32_t BoundaryKind;
        std::int32_t PortalSlotCount;
        double Danger;
        double ApproachDanger;
        double RouteCost;
        pas::List* Objects;
        std::uint8_t ImageActive;
        std::uint8_t cpp_padding_2[3];
        GI_GAI::TgaiGI* Image;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PabSpaceLink = TabSpaceLink*;

    #pragma pack(push, 1)
    struct TabSpaceLink {
        PabSpaceLink Prev;
        PabSpaceLink Next;
        TabSpace* First;
        TabSpace* Last;
        std::int32_t ExitIndex;
        pas::Array<Types::TPoint, 0, 10> Points;
    };
    #pragma pack(pop)

} // namespace ab_Space
