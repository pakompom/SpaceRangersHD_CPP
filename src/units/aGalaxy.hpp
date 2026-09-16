#pragma once
#include "types/aGalaxy.hpp"

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilm;

    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    extern std::int32_t ReservedMessageCounter;

    extern std::uint32_t TurnsSinceLastShipMessage;

    extern std::int32_t ModuleSizeIntegrityStatus;

    extern std::uint8_t ModuleCrcIntegrityStatus;

    extern std::int32_t ModuleCrcFailureValue;

    extern aGalaxy::TGalaxy* Galaxy;

    extern aGalaxy::TStar* PlayerStar;

    extern std::uint8_t PlayerDialogueRequestCount;

    extern pas::List* WingmenPendingLeadershipPenalty;

    extern std::int32_t CameraSpeed;

    extern std::int32_t FastCameraSpeed;

    std::uint8_t ShouldContinuePlayerTravel();

    float EstimatePlayerTravelTurns();

    pas::WideString GetLocalObjectLink(pas::Object* Obj, std::uint8_t Suppress);

    double GameTurnToDateTime(std::int32_t Turn);

    pas::WideString FormatGameTurnDate(std::int32_t Turn);

    void TGalaxy_Create(TGalaxy* Self);

    void TGalaxy_Destroy(TGalaxy* Self);

    void THole_Create(THole* Self);

    void THole_Destroy(THole* Self);

    void TCustomSystemInfo_Create(TCustomSystemInfo* Self);

    void TCustomSystemInfo_Destroy(TCustomSystemInfo* Self);

    void TStar_Create(TStar* Self);

    void TStar_Destroy(TStar* Self);

    void TConstellation_Create(TConstellation* Self);

    void TConstellation_Destroy(TConstellation* Self);

    aEFilm::TEFilm* CurrentFilm();

    void CreateFilmEffect(const pas::WideString& GraphKey, std::int32_t ShotVisual, SE_Space::TObjectSE*& Effect, aEFilm::TEFilmObj*& EffectFilm);

    void TInterfaceStateOverride_Create(TInterfaceStateOverride* Self);

    void TInterfaceStateOverride_Destroy(TInterfaceStateOverride* Self);

    void TInterfaceTextOverride_Create(TInterfaceTextOverride* Self);

    void TInterfaceTextOverride_Destroy(TInterfaceTextOverride* Self);

    void TInterfaceImageOverride_Create(TInterfaceImageOverride* Self);

    void TInterfaceImageOverride_Destroy(TInterfaceImageOverride* Self);

    void TInterfacePosOverride_Create(TInterfacePosOverride* Self);

    void TInterfacePosOverride_Destroy(TInterfacePosOverride* Self);

    void TInterfaceSizeOverride_Create(TInterfaceSizeOverride* Self);

    void TInterfaceSizeOverride_Destroy(TInterfaceSizeOverride* Self);

    void TStoredItem_CreateEmpty(TStoredItem* Self);

    void TStoredItem_Create(TStoredItem* Self, pas::WideString Name, pas::Object* Item);

    void TStoredItem_Destroy(TStoredItem* Self);

} // namespace aGalaxy
