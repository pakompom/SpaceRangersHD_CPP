#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/EC_Thread.hpp"
#include "types/aGalaxy.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Ether {
    struct TEther;

} // namespace EC_Ether

namespace EC_Expression {
    struct TCodeEC;

    struct TVarArrayEC;

    struct TVarEC;

} // namespace EC_Expression

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

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
    struct TDialogBlock;

    struct TDialogInject;

    struct TDialogOverride;

    struct TScriptABRequest;

    struct TScriptPBRequest;

    struct TScriptTQRequest;

    struct TScriptVDRequest;

} // namespace aScript

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aScript {
    struct TScriptStar;

    struct TScriptConstellation;

    struct TScriptShip;

    struct TScriptPlace;

    struct TScriptItem;

    struct TScriptGroup;

    struct TScriptState;

    struct TScriptDialog;

    struct TScriptDialogMsg;

    struct TScriptDialogAnswer;

    struct TScript;

    struct TScriptThread;

    struct TScriptCacheUnit;

    struct TScriptCache;

    struct TScriptGICacheUnit;

    struct TScriptGICache;

    struct TLibraryHandler;

    struct TLibraryCache;

    struct TScriptStarConstraint;

    struct TScriptGroupRelation;

    struct TScriptPlanet;

    struct TScriptShipOtb;

    struct TScriptContextSnapshot;

    #pragma pack(push, 1)
    struct TScriptGroupRelation {
        std::int32_t Group1;
        std::int32_t Group2;
        std::int32_t Relation1To2;
        std::int32_t Relation2To1;
        float MinCombatBalance;
        float MaxCombatBalance;
    };
    #pragma pack(pop)

    using PScriptPlanetBinding = TScriptPlanet*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScript : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScript, EC_Struct::TObjectEx, "TScript", 96)
        void p_destroy() override;
        void Clear();
        void PublishShipContext(TScriptShip* Binding);
        void PublishCurrentShip(aShip::TShip* Ship);
        TScriptStar* GetStar(pas::WideString Name);
        PScriptPlanetBinding GetPlanetBinding(pas::WideString Name);
        TScriptItem* GetItem(pas::WideString Name);
        void RunShipState(TScriptShip* Binding);
        void RunTurnCode();
        void RunAuxiliaryCode();
        void CallDialog(std::int32_t Index);
        void CallDialogByVariable(pas::WideString Name);
        void CallDialogMessage(std::int32_t Index);
        void BuildDialogAnswer(std::int32_t Index);
        void ExecuteDialogAnswer(std::int32_t Index);
        void BindShip(std::int32_t GroupIndex, aShip::TShip* Ship);
        void UnbindShip(aShip::TShip* Ship);
        void ClearShipBindings();
        void ChangeState(TScriptShip* Binding, std::int32_t StateIndex);
        void SetGroupRelation(std::int32_t SourceGroup, std::int32_t TargetGroup, aGalaxyStruct::TRelationLevel Level);
        void SetPlanetRelation(std::int32_t GroupIndex, aPlanet::TPlanet* Planet, aGalaxyStruct::TRelationLevel Level);
        std::uint8_t TryBindStars(std::int32_t StarIndex);
        std::uint8_t LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* FirstPlanet, std::uint8_t CreateObjects);
        std::uint8_t LoadFromFile(pas::WideString FileName, aGalaxy::TStar* AnchorStar, aPlanet::TPlanet* FirstPlanet, std::uint8_t CreateObjects);
        void SaveState(EC_Buf::TBufEC* Buffer);
        void LoadState(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);
        void BindImportedFunctions();
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy);
        std::int32_t ClassId;
        pas::WideString ScriptFileName;
        pas::List* Constellations;
        pas::List* Stars;
        pas::List* Places;
        pas::List* Items;
        pas::List* Groups;
        pas::List* Ships;
        pas::List* States;
        pas::List* Dialogs;
        pas::List* DialogMessages;
        pas::List* DialogAnswers;
        EC_Expression::TCodeEC* InitCode;
        EC_Expression::TCodeEC* TurnCode;
        EC_Expression::TCodeEC* AuxiliaryCode;
        EC_Ether::TEther* Ether;
        aShip::TShip* CurrentShip;
        std::int32_t CurrentDialog;
        std::int32_t CurrentAnswer;
        std::uint8_t SkipGreeting;
        std::uint8_t cpp_padding[3];
        pas::DynArray<TScriptGroupRelation> GroupRelations;
        aPlanet::TPlanet* AnchorPlanet;
        EC_Str::TStringsEC* EtherIds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TScriptTQRequest {
        pas::WideString Name;
        pas::WideString SuccessCaption;
        pas::WideString FailureCaption;
        TScript* Script;
    };
    #pragma pack(pop)

    using PQueuedTextQuest = TScriptTQRequest*;

    #pragma pack(push, 1)
    struct TScriptABRequest {
        pas::WideString MapName;
        aMyFunction::TObjectList* Ships;
        std::int32_t BackgroundId;
        pas::WideString BackgroundMapName;
        TScript* Script;
    };
    #pragma pack(pop)

    using PScriptABRequest = TScriptABRequest*;

    #pragma pack(push, 1)
    struct TScriptPBRequest {
        pas::WideString MapName;
        pas::WideString StartText;
        pas::WideString SuccessText;
        pas::WideString FailureText;
        pas::WideString PlaceText;
        TScript* Script;
    };
    #pragma pack(pop)

    using PScriptPBRequest = TScriptPBRequest*;

    #pragma pack(push, 1)
    struct TScriptVDRequest {
        pas::WideString Video;
        pas::WideString Soundtrack;
        TScript* Script;
    };
    #pragma pack(pop)

    using PScriptVDRequest = TScriptVDRequest*;

    using TScriptActionTypeSet = pas::Set<0, 61>;

    using TScriptStepTypeSet = pas::Set<0, 11>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptItem : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptItem, EC_Struct::TObjectEx, "TScriptItem", 100)
        void p_destroy() override;
        void CompileActionCode();
        std::int32_t RunActionCode(std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);
        pas::WideString FormatDataText(pas::WideString Text, pas::WideString ColorTag);
        pas::WideString Name;
        pas::WideString LocationVarName;
        std::int32_t DefinitionKind;
        std::int32_t DefinitionType;
        std::int32_t Weight;
        std::int32_t Level;
        std::int32_t DefinitionValue1C;
        std::uint8_t OwnerId;
        std::uint8_t cpp_padding[3];
        pas::WideString ConfigName;
        aItem::TItem* Item;
        std::uint8_t CanSell;
        std::uint8_t cpp_padding_2[3];
        pas::Array<std::int32_t, 1, 3> Data;
        pas::WideString TextData1;
        pas::WideString TextData2;
        pas::WideString TextData3;
        pas::WideString OnUseText;
        pas::WideString OnActionText;
        EC_Expression::TCodeEC* ActionCode;
        TScriptActionTypeSet ActionTypeMask;
        TScriptStepTypeSet StepTypeMask;
        std::uint8_t ActionCodeInitialized;
        std::uint8_t cpp_padding_3[1];
        TScript* Script;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TScriptContextSnapshot {
        TScript* Script;
        aShip::TShip* CurrentShip;
        std::uint8_t EndState;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptCache : pas::Object {
        PAS_CLASS_META(TScriptCache, pas::Object, "TScriptCache", 8)
        void p_destroy() override;
        TScriptCacheUnit* GetOrCompile(pas::WideString Name, EC_BlockPar::TBlockParEC* Config);
        aMyFunction::TObjectList* Entries;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLibraryCache : pas::Object {
        PAS_CLASS_META(TLibraryCache, pas::Object, "TLibraryCache", 8)
        void p_destroy() override;
        TLibraryHandler* GetLib(pas::WideString Name);
        void InitFunction(EC_Expression::TVarEC* Cell);
        aMyFunction::TObjectList* Libraries;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptGICache : pas::Object {
        PAS_CLASS_META(TScriptGICache, pas::Object, "TScriptGICache", 8)
        void p_destroy() override;
        TScriptGICacheUnit* GetOrCompile(EC_BlockPar::TBlockParEC* Block);
        aMyFunction::TObjectList* Entries;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLibraryHandler : pas::Object {
        PAS_CLASS_META(TLibraryHandler, pas::Object, "TLibraryHandler", 16)
        void p_destroy() override;
        void InitFunction(EC_Expression::TVarEC* Cell);
        void InitAllFunctions(EC_Expression::TVarArrayEC* Scope);
        pas::WideString LibraryName;
        std::uint32_t ModuleHandle;
        EC_BlockPar::TBlockParEC* DefinitionBlock;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptShip : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptShip, EC_Struct::TObjectEx, "TScriptShip", 44)
        void p_destroy() override;
        TScriptGroup* GetGroup();
        std::int32_t RunActionCode(std::uint8_t ActionType, aShip::TShip* Ship, pas::Object* Object1, pas::Object* Object2, std::int32_t Param);
        TScript* Script;
        std::int32_t GroupIndex;
        aShip::TShip* Ship;
        pas::Array<std::uint32_t, 0, 3> Data;
        TScriptState* State;
        pas::WideString StateText;
        std::uint8_t EndState;
        std::uint8_t Hit;
        std::uint8_t HitPlayer;
        std::uint8_t cpp_padding[1];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptState : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptState, EC_Struct::TObjectEx, "TScriptState", 76)
        void p_destroy() override;
        pas::WideString Name;
        std::int32_t StateKind;
        pas::WideString TargetVarName;
        std::uint32_t TargetValue;
        pas::DynArray<pas::WideString> EnemyGroupNames;
        pas::DynArray<std::int32_t> EnemyGroupIndices;
        pas::WideString PickupItemVarName;
        TScriptItem* PickupItem;
        std::uint8_t PickUpNearbyItems;
        std::uint8_t cpp_padding[3];
        pas::WideString AuxiliaryText;
        EC_Expression::TCodeEC* AuxiliaryCode;
        pas::WideString OnActionText;
        EC_Expression::TCodeEC* ActionCode;
        TScriptActionTypeSet ActionTypeMask;
        TScriptStepTypeSet StepTypeMask;
        std::uint8_t cpp_padding_2[2];
        EC_Expression::TCodeEC* EntryCode;
        EC_Expression::TCodeEC* StateCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptThread : EC_Thread::TThreadEC {
        PAS_CLASS_META(TScriptThread, EC_Thread::TThreadEC, "TScriptThread", 44)
        void virtual_TThreadEC_Execute() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptCacheUnit : pas::Object {
        PAS_CLASS_META(TScriptCacheUnit, pas::Object, "TScriptCacheUnit", 28)
        void p_destroy() override;
        void Initialize(pas::WideString Name, pas::WideString SourceText, pas::WideString ActionTypes, pas::WideString StepTypes);
        pas::WideString Name;
        pas::WideString SourceText;
        EC_Expression::TCodeEC* Code;
        TScriptActionTypeSet ActionTypeMask;
        TScriptStepTypeSet StepTypeMask;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptPlace : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptPlace, EC_Struct::TObjectEx, "TScriptPlace", 52)
        void p_destroy() override;
        EC_Struct::TPointF GetPoint();
        EC_Struct::TPointF GetRandomPoint(std::uint32_t Seed);
        std::uint8_t ShipInPlace(aShip::TShip* Ship);
        TScript* Script;
        pas::WideString Name;
        pas::WideString OriginVarName;
        aGalaxy::TStar* OriginStar;
        std::int32_t PlaceKind;
        float AngleOffset;
        float DistanceScale;
        std::int32_t Radius;
        pas::WideString TargetVarName;
        std::uint32_t TargetValue;
        pas::WideString TargetVarName2;
        EC_Expression::TVarEC* TargetValue2;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TScriptStarConstraint {
        TScriptStar* OtherStar;
        std::int32_t MinDistance;
        std::int32_t MaxDistance;
        std::uint8_t RequireBlackHole;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using TScriptEconomyMask = pas::Set<0, 7>;

    using TScriptGovernmentMask = pas::Set<0, 7>;

    #pragma pack(push, 1)
    struct TScriptPlanet {
        pas::WideString Name;
        aGalaxyStruct::TOwnerMask RaceMask;
        aGalaxyStruct::TOwnerMask OwnerMask;
        TScriptEconomyMask EconomyMask;
        TScriptGovernmentMask GovernmentMask;
        std::int32_t MinOrbitPercent;
        std::int32_t MaxOrbitPercent;
        pas::WideString DefinitionText;
        aPlanet::TPlanet* Planet;
    };
    #pragma pack(pop)

    using TScriptShipTypeMask = pas::Set<0, 15>;

    using TScriptDominatorMasks = pas::Array<aGalaxy::TDominatorSeriesMask, 0, 7>;

    #pragma pack(push, 1)
    struct TScriptShipOtb {
        std::int32_t Count;
        aGalaxyStruct::TOwnerMask OwnerMask;
        TScriptShipTypeMask ShipTypeMask;
        std::uint8_t PlayerOnly;
        std::int32_t MinSpeed;
        std::int32_t MaxSpeed;
        std::int32_t WeaponRequirement;
        std::int32_t MinCargoHookLevel;
        std::int32_t MinFreeCargoSpace;
        std::int32_t MinTraderStatus;
        std::int32_t MaxTraderStatus;
        std::int32_t MinWarriorStatus;
        std::int32_t MaxWarriorStatus;
        std::int32_t MinPirateStatus;
        std::int32_t MaxPirateStatus;
        float MinStrength;
        float MaxStrength;
        pas::WideString StationNames;
        TScriptDominatorMasks DominatorMasks;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptStar : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptStar, EC_Struct::TObjectEx, "TScriptStar", 32)
        void p_destroy() override;
        pas::WideString Name;
        std::int32_t ConstellationIndex;
        std::uint8_t RejectHostilePresence;
        std::uint8_t ProtectStar;
        std::uint8_t cpp_padding[2];
        pas::DynArray<TScriptStarConstraint> Constraints;
        pas::DynArray<TScriptPlanet> Planets;
        pas::DynArray<TScriptShipOtb> ShipRequirements;
        aGalaxy::TStar* Star;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptConstellation : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptConstellation, EC_Struct::TObjectEx, "TScriptConstellation", 8)
        void p_destroy() override;
        aGalaxy::TConstellation* Constellation;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptGroup : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptGroup, EC_Struct::TObjectEx, "TScriptGroup", 112)
        void p_destroy() override;
        pas::WideString Name;
        pas::WideString PlanetVarName;
        aPlanet::TPlanet* Planet;
        std::int32_t InitialStateIndex;
        aGalaxyStruct::TOwnerMask OwnerMask;
        TScriptShipTypeMask ShipTypeMask;
        std::uint8_t cpp_padding[1];
        std::int32_t MinCount;
        std::int32_t MaxCount;
        std::int32_t MinSpeed;
        std::int32_t MaxSpeed;
        std::int32_t WeaponRequirement;
        std::int32_t MinCargoHookLevel;
        std::int32_t MinFreeCargoSpace;
        std::uint8_t IncludePlayer;
        std::uint8_t cpp_padding_2[3];
        pas::WideString StationNames;
        TScriptDominatorMasks DominatorMasks;
        float MinStrength;
        float MaxStrength;
        std::int32_t MinTraderStatus;
        std::int32_t MaxTraderStatus;
        std::int32_t MinWarriorStatus;
        std::int32_t MaxWarriorStatus;
        std::int32_t MinPirateStatus;
        std::int32_t MaxPirateStatus;
        std::int32_t MaxDistanceFromPlanet;
        pas::WideString DefinitionText;
        pas::List* Ships;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptDialog : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptDialog, EC_Struct::TObjectEx, "TScriptDialog", 12)
        void p_destroy() override;
        pas::WideString Name;
        EC_Expression::TCodeEC* Code;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptDialogMsg : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptDialogMsg, EC_Struct::TObjectEx, "TScriptDialogMsg", 12)
        void p_destroy() override;
        pas::WideString Name;
        EC_Expression::TCodeEC* Code;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptDialogAnswer : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptDialogAnswer, EC_Struct::TObjectEx, "TScriptDialogAnswer", 16)
        void p_destroy() override;
        pas::WideString Name;
        EC_Expression::TCodeEC* AnswerCode;
        EC_Expression::TCodeEC* ActionCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PScriptShipRequirement = TScriptShipOtb*;

    #pragma pack(push, 1)
    struct TDialogBlock {
        pas::WideString Text;
        TScript* Script;
        std::uint8_t Mode;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using PScriptDialogBlock = TDialogBlock*;

    #pragma pack(push, 1)
    struct TDialogInject {
        TScript* Script;
        pas::WideString DialogName;
        pas::WideString Text;
        pas::WideString Answer;
        std::int32_t Priority;
        std::uint32_t AnswerData;
        std::uint8_t ReplaceGreeting;
        std::uint8_t cpp_padding[3];
        pas::WideString ActionCode;
        TScript* ActionScript;
    };
    #pragma pack(pop)

    using PScriptDialogInjection = TDialogInject*;

    #pragma pack(push, 1)
    struct TDialogOverride {
        pas::WideString DialogName;
        std::int32_t Priority;
        TScript* Script;
        std::uint32_t AnswerData;
    };
    #pragma pack(pop)

    using PScriptDialogOverride = TDialogOverride*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptGICacheUnit : pas::Object {
        PAS_CLASS_META(TScriptGICacheUnit, pas::Object, "TScriptGICacheUnit", 16)
        void p_destroy() override;
        void Initialize(EC_BlockPar::TBlockParEC* Block, pas::WideString SourceText);
        EC_BlockPar::TBlockParEC* Block;
        pas::WideString SourceText;
        EC_Expression::TCodeEC* Code;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TScriptQuestStatus : std::uint32_t {
        sqsNone = 0,
        sqsQueued = 1,
        sqsSuccess = 2,
        sqsFailure = 3,
    };

    inline constexpr std::int32_t spkPolar = 0;

    inline constexpr std::int32_t spkPlanetPosition = 1;

    inline constexpr std::int32_t spkDockedPlanet = 2;

    inline constexpr std::int32_t spkStarDirection = 3;

    inline constexpr std::int32_t spkScriptItem = 4;

    inline constexpr std::int32_t spkGroupCentroid = 5;

    inline constexpr std::int32_t spkCoordinates = 6;

    inline constexpr std::int32_t sskIdle = 0;

    inline constexpr std::int32_t sskMoveToPlace = 1;

    inline constexpr std::int32_t sskFollowGroup = 2;

    inline constexpr std::int32_t sskJumpToStar = 3;

    inline constexpr std::int32_t sskLandOnPlanet = 4;

    inline constexpr std::int32_t sskNormalAI = 5;

} // namespace aScript
