#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aGroup {
    struct TGroupRouteOrder;

    struct TGroup;

    #pragma pack(push, 1)
    struct TGroupRouteOrder {
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        pas::Object* Target;
        EC_Struct::TPointF Destination;
        std::uint8_t WaitMode;
        std::uint8_t cpp_padding_2[3];
        std::int32_t WaitUntilTurn;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGroup : EC_Struct::TObjectEx {
        PAS_CLASS_META(TGroup, EC_Struct::TObjectEx, "TGroup", 32)
        void p_destroy() override;
        void Save(EC_Buf::TBufEC* Buffer);
        void Load(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        void AddShip(aShip::TShip* Ship);
        void NextDay();
        void Disband();
        std::uint8_t SelectLiberationTarget();
        std::uint8_t BuildLiberationOrders();
        std::uint8_t AreShipsAssembled();
        void AdvanceRouteForShips();
        pas::WideString GetShipGreeting(void* Ship);
        aGalaxy::TStar* FindCentralMemberStar();
        std::int32_t CreatedTurn;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        pas::List* Ships;
        pas::DynArray<TGroupRouteOrder> Route;
        aGalaxy::TStar* TargetStar;
        aGalaxy::TStar* AssemblyStar;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aGroup
