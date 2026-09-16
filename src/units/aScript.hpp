#pragma once
#include "types/aGalaxy.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aScript.hpp"
#include "types/aShip.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Expression {
    struct TCodeEC;

    struct TCodeProcessEC;

    struct TVarArrayEC;

} // namespace EC_Expression

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aScript {
    extern aScript::TScript* CurrentScript;

    extern pas::List* QueuedArcadeBattles;

    extern pas::List* QueuedPlanetaryBattles;

    extern pas::List* QueuedTextQuests;

    extern pas::List* QueuedVideos;

    extern aScript::TScript* ResumingScript;

    extern aScript::TScript* StagedArcadeShipScript;

    extern std::uint8_t ScriptArcadeReturnScreenId;

    extern std::uint8_t ScriptTakeoffRequested;

    extern aMyFunction::TObjectList* StagedArcadeShips;

    extern std::uint8_t ScriptEndTurnRequested;

    extern EC_Expression::TVarArrayEC* ScriptFunctionScope;

    extern EC_Expression::TCodeProcessEC* ScriptProcess;

    extern aMyFunction::TObjectList* ScriptDialogOverrides;

    extern aScript::TLibraryCache* ScriptLibraryCache;

    extern aScript::TScriptCache* ArtefactScriptCache;

    extern aScript::TScriptCache* ArtefactKindScriptCache;

    extern aScript::TScriptCache* UselessItemScriptCache;

    extern aScript::TScriptCache* CustomShipInfoScriptCache;

    extern aScript::TScriptGICache* GameplayUiScriptCache;

    extern aScript::TScriptState* CurrentScriptState;

    extern aMyFunction::TObjectList* ScriptDialogInjections;

    extern aMyFunction::TObjectList* ScriptDialogBlocks;

    extern aScript::TScriptThread* ScriptRequestThread;

    void LogScriptCallHistory();

    void ClearPendingScriptRequests();

    std::uint8_t HasPendingScriptRequests();

    std::uint8_t TryShowQueuedArcadeBattle();

    std::uint8_t TryShowQueuedTextQuest();

    std::uint8_t TryRunQueuedPlanetaryBattle();

    std::uint8_t TryShowQueuedVideo();

    std::uint8_t TryDispatchScriptTakeoff();

    std::uint8_t TryDispatchScriptEndTurn();

    std::uint8_t TryDispatchResumingScriptRequest();

    std::uint8_t DispatchPendingScriptRequests();

    void StartScriptRequestThread();

    void CompleteQueuedArcadeBattle(std::int32_t Status);

    void CompleteQueuedTextQuest(TScriptQuestStatus Status);

    void CompleteQueuedPlanetaryBattle(std::int32_t Status);

    void CompleteQueuedVideo(std::int32_t Status);

    void LogScriptStepCount(std::int32_t ExpressionCount);

    void InitializeScriptEngine();

    void FinalizeScriptEngine();

    void RunGlobalScriptsForContext(aGalaxy::TStar* Star, std::int32_t RunFrom);

    std::uint8_t TryStartScriptByName(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, pas::WideString Name);

    std::uint8_t TryStartScriptInstanceFromTemplate(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, std::int32_t TemplateIndex);

    std::uint8_t TryRestartScript(TScript* Script, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet);

    void CompileScriptTemplateCondition(std::int32_t TemplateIndex);

    std::uint8_t IsStarProtectedByScript(aGalaxy::TStar* Star);

    std::uint8_t ScriptDefinitionBit(std::uint32_t Value, std::int32_t BitIndex);

    aGalaxyStruct::TOwnerMask DecodeScriptRaceMask(std::uint32_t Value);

    aGalaxyStruct::TOwnerMask DecodeScriptOwnerMask(std::uint32_t Value);

    TScriptEconomyMask DecodeScriptEconomyMask(std::uint32_t Value);

    TScriptGovernmentMask DecodeScriptGovernmentMask(std::uint32_t Value);

    TScriptShipTypeMask DecodeScriptShipTypeMask(std::uint32_t Value);

    aGalaxy::TDominatorSeriesMask DecodeScriptDominatorMask(std::uint32_t Value, std::uint8_t KlingType);

    std::uint8_t DecodeScriptItemOwner(std::int32_t Value);

    aGalaxyStruct::TRelationLevel DecodeScriptRelationLevel(std::int32_t Value);

    std::uint8_t ScriptShipMatchesType(aShip::TShip* Ship, TScriptShipTypeMask ShipTypeMask, pas::WideString StationNames, pas::OpenArray<aGalaxy::TDominatorSeriesMask> DominatorMasks);

    pas::List* CollectScriptCandidateShips(aGalaxy::TStar* Star);

    aShip::TShip* FindScriptGroupCandidate(pas::List* Candidates, TScriptGroup* Group);

    TScriptShip* GetScriptShipBindingForContext(aShip::TShip* Ship, TScript* Script);

    void ClearScriptDialogRules();

    void ExecuteScriptText(pas::WideString SourceText, EC_Expression::TVarArrayEC* Scope);

    EC_Expression::TCodeEC* CompileScriptText(pas::WideString SourceText);

    void RunScriptCode(pas::WideString ContextName, EC_Expression::TCodeEC* Code, EC_Expression::TCodeEC* ParentCode);

    TScriptCacheUnit* GetCachedActionCode(TScriptCache*& Cache, pas::WideString Name, EC_BlockPar::TBlockParEC* Config);

    void ExecuteGameplayUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t VirtualKey);

    void ScriptSnap(TScriptContextSnapshot& Snapshot);

    void ScriptUnSnap(TScriptContextSnapshot Snapshot);

    std::int32_t RunItemUseCode(aItem::TItem* Item, aShip::TShip* Ship);

    std::int32_t RunItemConfigActionCode(aItem::TItem* Item, std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);

    std::int32_t RunCustomShipInfoActionCode(aShip::PCustomShipInfo Info, std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);

    pas::WideString GetScriptContextDescription();

    void TScriptThread_Create(TScriptThread* Self);

    void TScriptThread_Execute(TScriptThread* Self);

    void TScriptStar_Create(TScriptStar* Self);

    void TScriptStar_Destroy(TScriptStar* Self);

    void TScriptConstellation_Create(TScriptConstellation* Self);

    void TScriptConstellation_Destroy(TScriptConstellation* Self);

    void TScriptShip_Create(TScriptShip* Self);

    void TScriptShip_Destroy(TScriptShip* Self);

    void TScriptPlace_Create(TScriptPlace* Self);

    void TScriptPlace_Destroy(TScriptPlace* Self);

    void TScriptItem_Create(TScriptItem* Self);

    void TScriptItem_Destroy(TScriptItem* Self);

    void TScriptGroup_Create(TScriptGroup* Self);

    void TScriptGroup_Destroy(TScriptGroup* Self);

    void TScriptState_Create(TScriptState* Self);

    void TScriptState_Destroy(TScriptState* Self);

    void TScriptDialog_Create(TScriptDialog* Self);

    void TScriptDialog_Destroy(TScriptDialog* Self);

    void TScriptDialogMsg_Create(TScriptDialogMsg* Self);

    void TScriptDialogMsg_Destroy(TScriptDialogMsg* Self);

    void TScriptDialogAnswer_Create(TScriptDialogAnswer* Self);

    void TScriptDialogAnswer_Destroy(TScriptDialogAnswer* Self);

    void TScript_Create(TScript* Self);

    void TScript_Destroy(TScript* Self);

    void TLibraryHandler_Create(TLibraryHandler* Self, pas::WideString LibraryName, std::uint32_t ModuleHandle, EC_BlockPar::TBlockParEC* DefinitionBlock);

    void TLibraryHandler_Destroy(TLibraryHandler* Self);

    void TLibraryCache_Create(TLibraryCache* Self);

    void TLibraryCache_Destroy(TLibraryCache* Self);

    void TScriptCache_Create(TScriptCache* Self);

    void TScriptGICache_Create(TScriptGICache* Self);

    void TScriptCache_Destroy(TScriptCache* Self);

    void TScriptGICache_Destroy(TScriptGICache* Self);

    void TScriptCacheUnit_Create(TScriptCacheUnit* Self);

    void TScriptCacheUnit_Destroy(TScriptCacheUnit* Self);

    void TScriptGICacheUnit_Create(TScriptGICacheUnit* Self);

    void TScriptGICacheUnit_Destroy(TScriptGICacheUnit* Self);

} // namespace aScript
