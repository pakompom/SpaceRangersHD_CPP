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
        // Same numeric orders as TShipOrder.
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        // Serialized as an object ID until ResolveLoadedReferences.
        pas::Object* Target;
        EC_Struct::TPointF Destination;
        // 0: arrival, 2: group assembly, 3: WaitUntilTurn.
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
        // Ships initially contains serialized IDs, pending reference resolution. Does not clear existing entries.
        void Load(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        // Rebinds ship and route target IDs after Load.
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        // Appends Ship, assigns LiberationGroup and resets its order index.
        void AddShip(aShip::TShip* Ship);
        // May remove and free Self when empty or older than 150 days.
        void NextDay();
        // Detaches member ships, removes Self from Galaxy.LiberationGroups, and frees Self.
        void Disband();
        // Chooses TargetStar and a Coalition AssemblyStar within 28 parsecs. Failure disbands and frees Self.
        std::uint8_t SelectLiberationTarget();
        // Builds staging, landing and attack orders and publishes news. May disband and free Self when no suitable staging planet exists.
        std::uint8_t BuildLiberationOrders();
        // Requires a nonempty member list; same route index, no land/jump order, and within 300 units.
        std::uint8_t AreShipsAssembled();
        // Advances members in reverse order, detaching completed members.
        void AdvanceRouteForShips();
        pas::WideString GetShipGreeting(void* Ship);
        // Chooses a member's current star minimizing summed rounded distances to all galaxy planets.
        aGalaxy::TStar* FindCentralMemberStar();
        std::int32_t CreatedTurn;
        std::uint32_t GenerationSeed;
        std::uint32_t RandomState;
        // Borrowed TShip entries.
        pas::List* Ships;
        // Owned route; references in each order are borrowed.
        pas::DynArray<TGroupRouteOrder> Route;
        // Destination of the liberation attack.
        aGalaxy::TStar* TargetStar;
        // Nearby Coalition staging system.
        aGalaxy::TStar* AssemblyStar;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aGroup
