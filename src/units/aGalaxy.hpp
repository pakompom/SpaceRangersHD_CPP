#pragma once
#include "types/aGalaxy.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    // Reset by galaxy construction; meaning unresolved.
    extern std::int32_t ReservedMessageCounter;

    // Reset by ship messages; incremented by Galaxy.NextDay and saved with galaxy state.
    extern std::uint32_t TurnsSinceLastShipMessage;

    // Startup size-check marker: positive means mismatch; nonpositive is accepted.
    extern std::int32_t ModuleSizeIntegrityStatus;

    // 0 unchecked, 1 accepted, 2 mismatch.
    extern std::uint8_t ModuleCrcIntegrityStatus;

    // Cleared on mismatch while saving star 1; no native readers.
    extern std::int32_t ModuleCrcFailureValue;

    extern aGalaxy::TGalaxy* Galaxy;

    extern aGalaxy::TStar* PlayerStar;

    // Incremented when a ship dialogue passes the shared message-delay threshold; only count <= 1 is accepted.
    extern std::uint8_t PlayerDialogueRequestCount;

    // Borrowed TShip entries.
    extern pas::List* WingmenPendingLeadershipPenalty;

    // Native initial camera-step limit.
    extern std::int32_t CameraSpeed;

    // Native initial camera-step limit.
    extern std::int32_t FastCameraSpeed;

    // May prepare movement or start black-hole entry; false without a player.
    std::uint8_t ShouldContinuePlayerTravel();

    // Uses distance divided by Speed + 1; zero for interrupted travel or unsupported orders.
    float EstimatePlayerTravelTurns();

    // Object markup embeds the native pointer, not an ID. Ships/planets/loose items must be in the player's star. Suppress returns empty; otherwise supported objects require a player.
    pas::WideString GetLocalObjectLink(pas::Object* Obj, std::uint8_t Suppress);

    double GameTurnToDateTime(std::int32_t Turn);

    pas::WideString FormatGameTurnDate(std::int32_t Turn);

    void TGalaxy_Create(TGalaxy* Self);

    void TGalaxy_Destroy(TGalaxy* Self);

    void TGalaxy_LoadFromBuffer(TGalaxy* Self, EC_Buf::TBufEC* Buffer);

    void THole_Create(THole* Self);

    void THole_Destroy(THole* Self);

    void TCustomSystemInfo_Create(TCustomSystemInfo* Self);

    void TCustomSystemInfo_Destroy(TCustomSystemInfo* Self);

    void TStar_Create(TStar* Self);

    void TStar_Destroy(TStar* Self);

    void TConstellation_Create(TConstellation* Self);

    void TConstellation_Destroy(TConstellation* Self);

    // Constant arguments preserve evaluation order; computed arguments stay at their call sites.
    void CreateFilmEffect(const pas::WideString& GraphKey, std::int32_t ShotVisual, SE_Space::TObjectSE*& Effect, aEFilm::TEFilmObj*& EffectFilm);

    void TInterfaceStateOverride_Create(TInterfaceStateOverride* Self);

    // Restores the original control value when the control still exists.
    void TInterfaceStateOverride_Destroy(TInterfaceStateOverride* Self);

    void TInterfaceTextOverride_Create(TInterfaceTextOverride* Self);

    // Restores the original control value when the control still exists.
    void TInterfaceTextOverride_Destroy(TInterfaceTextOverride* Self);

    void TInterfaceImageOverride_Create(TInterfaceImageOverride* Self);

    // Restores the original value when the control still exists; an empty original style is not restored.
    void TInterfaceImageOverride_Destroy(TInterfaceImageOverride* Self);

    void TInterfacePosOverride_Create(TInterfacePosOverride* Self);

    // Restores the original control value when the control still exists.
    void TInterfacePosOverride_Destroy(TInterfacePosOverride* Self);

    void TInterfaceSizeOverride_Create(TInterfaceSizeOverride* Self);

    // Restores the original control value when the control still exists.
    void TInterfaceSizeOverride_Destroy(TInterfaceSizeOverride* Self);

    void TStoredItem_CreateEmpty(TStoredItem* Self);

    void TStoredItem_Create(TStoredItem* Self, pas::WideString Name, pas::Object* Item);

    void TStoredItem_Destroy(TStoredItem* Self);

} // namespace aGalaxy
