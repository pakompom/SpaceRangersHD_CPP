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

    // Owns PScriptABRequest records.
    extern pas::List* QueuedArcadeBattles;

    // Owns PScriptPBRequest records.
    extern pas::List* QueuedPlanetaryBattles;

    // // Owns PQueuedTextQuest records.
    extern pas::List* QueuedTextQuests;

    // Owns PScriptVDRequest records.
    extern pas::List* QueuedVideos;

    extern aScript::TScript* ResumingScript;

    extern aScript::TScript* StagedArcadeShipScript;

    // Direct stores in TryShowQueuedArcadeBattle establish ownership.
    extern std::uint8_t ScriptArcadeReturnScreenId;

    extern std::uint8_t ScriptTakeoffRequested;

    // Owns ships waiting for an arcade request.
    extern aMyFunction::TObjectList* StagedArcadeShips;

    extern std::uint8_t ScriptEndTurnRequested;

    extern EC_Expression::TVarArrayEC* ScriptFunctionScope;

    extern EC_Expression::TCodeProcessEC* ScriptProcess;

    // Record entries are freed explicitly before clearing.
    extern aMyFunction::TObjectList* ScriptDialogOverrides;

    extern aScript::TLibraryCache* ScriptLibraryCache;

    // Named artifact action code via TArtefact.GetActionCode.
    extern aScript::TScriptCache* ArtefactScriptCache;

    // Item-type artifact action code; item types 8 and 9 use the named artifact cache instead.
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

    // Moves the first request belonging to ResumingScript to its queue's front.
    std::uint8_t TryDispatchResumingScriptRequest();

    std::uint8_t DispatchPendingScriptRequests();

    void StartScriptRequestThread();

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GABStatus changed by the script.
    void CompleteQueuedArcadeBattle(std::int32_t Status);

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GQuestStatus changed by the script.
    void CompleteQueuedTextQuest(TScriptQuestStatus Status);

    // Requires a nonempty queue. Clears ResumingScript when no requests remain; preserves GRobotStatus changed by the script.
    void CompleteQueuedPlanetaryBattle(std::int32_t Status);

    // Requires a nonempty queue when a player exists. Clears ResumingScript when no requests remain; preserves GVideoStatus changed by the script.
    void CompleteQueuedVideo(std::int32_t Status);

    void LogScriptStepCount(std::int32_t ExpressionCount);

    void InitializeScriptEngine();

    void FinalizeScriptEngine();

    void RunGlobalScriptsForContext(aGalaxy::TStar* Star, std::int32_t RunFrom);

    std::uint8_t TryStartScriptByName(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, pas::WideString Name);

    std::uint8_t TryStartScriptInstanceFromTemplate(aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet, std::int32_t TemplateIndex);

    // Replaces and frees Script on success; retains it on failure. Requires an existing Galaxy.Scripts entry.
    std::uint8_t TryRestartScript(TScript* Script, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* AnchorPlanet);

    void CompileScriptTemplateCondition(std::int32_t TemplateIndex);

    std::uint8_t IsStarProtectedByScript(aGalaxy::TStar* Star);

    std::uint8_t ScriptDefinitionBit(std::uint32_t Value, std::int32_t BitIndex);

    aGalaxyStruct::TOwnerMask DecodeScriptRaceMask(std::uint32_t Value);

    // Bit 9 also selects the player's current owner ID.
    aGalaxyStruct::TOwnerMask DecodeScriptOwnerMask(std::uint32_t Value);

    TScriptEconomyMask DecodeScriptEconomyMask(std::uint32_t Value);

    TScriptGovernmentMask DecodeScriptGovernmentMask(std::uint32_t Value);

    TScriptShipTypeMask DecodeScriptShipTypeMask(std::uint32_t Value);

    aGalaxy::TDominatorSeriesMask DecodeScriptDominatorMask(std::uint32_t Value, std::uint8_t KlingType);

    // Values outside 0..7 become owner 6.
    std::uint8_t DecodeScriptItemOwner(std::int32_t Value);

    // Values outside 0..4 become hostile.
    aGalaxyStruct::TRelationLevel DecodeScriptRelationLevel(std::int32_t Value);

    // DominatorMasks requires eight entries indexed by TKlingType. StationNames is a comma-separated filter when ship-type bit 8 is set.
    std::uint8_t ScriptShipMatchesType(aShip::TShip* Ship, TScriptShipTypeMask ShipTypeMask, pas::WideString StationNames, pas::OpenArray<aGalaxy::TDominatorSeriesMask> DominatorMasks);

    // Caller owns the list; ship references are borrowed.
    pas::List* CollectScriptCandidateShips(aGalaxy::TStar* Star);

    // Returns the first match or nil; leaves Candidates unchanged.
    aShip::TShip* FindScriptGroupCandidate(pas::List* Candidates, TScriptGroup* Group);

    // The player may have multiple bindings; ordinary ships have one.
    TScriptShip* GetScriptShipBindingForContext(aShip::TShip* Ship, TScript* Script);

    void ClearScriptDialogRules();

    // Nil Scope temporarily clears CurrentScript. Compiled code is freed after execution.
    void ExecuteScriptText(pas::WideString SourceText, EC_Expression::TVarArrayEC* Scope);

    // Caller owns the returned code.
    EC_Expression::TCodeEC* CompileScriptText(pas::WideString SourceText);

    void RunScriptCode(pas::WideString ContextName, EC_Expression::TCodeEC* Code, EC_Expression::TCodeEC* ParentCode);

    TScriptCacheUnit* GetCachedActionCode(TScriptCache*& Cache, pas::WideString Name, EC_BlockPar::TBlockParEC* Config);

    // Runs with CurrentScript nil. VirtualKey=0 leaves KEY and KEYMOD unchanged.
    void ExecuteGameplayUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t VirtualKey);

    // When CurrentScript is nil, only Snapshot.Script is written.
    void ScriptSnap(TScriptContextSnapshot& Snapshot);

    void ScriptUnSnap(TScriptContextSnapshot Snapshot);

    // Returns ScriptItemActParam, initially zero.
    std::int32_t RunItemUseCode(aItem::TItem* Item, aShip::TShip* Ship);

    // Uses artifact or useless-item configuration code. Object slots can carry event-specific integers.
    std::int32_t RunItemConfigActionCode(aItem::TItem* Item, std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);

    // Returns the event parameter after script changes. Object slots can carry event-specific integers.
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

    // EntryCode runs before CurShip/EndState refresh; StateCode sees the new context.
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

    void TScript_RunShipState(TScript* Self, TScriptShip* Binding);

    void TScript_RunTurnCode(TScript* Self);

    void TScript_RunAuxiliaryCode(TScript* Self);

    void TScript_CallDialog(TScript* Self, std::int32_t Index);

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
