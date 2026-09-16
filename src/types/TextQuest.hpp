#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/TextQuestInterface.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EventClass {
    struct TEvent;

} // namespace EventClass

namespace LocationClass {
    struct TLocation;

} // namespace LocationClass

namespace ParameterClass {
    struct TParameter;

} // namespace ParameterClass

namespace PathClass {
    struct TPath;

} // namespace PathClass

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace TextQuest {
    struct TTextQuest;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextQuest : EC_Struct::TObjectEx {
        PAS_CLASS_META(TTextQuest, EC_Struct::TObjectEx, "TTextQuest", 128)
        void p_destroy() override;
        void Reset();
        std::int32_t GetLocationCount();
        std::int32_t GetPathCount();
        LocationClass::TLocation* GetLocation(std::int32_t Index);
        PathClass::TPath* GetPath(std::int32_t Index);
        std::int32_t GetParameterCount();
        ParameterClass::TParameter* GetParameter(std::int32_t Index);
        void LoadFromReader(EC_Buf::TBufEC* Reader, std::uint8_t HeaderOnly);
        void ResetEventIndices();
        std::int32_t FindLocationIndex(std::int32_t LocationId);
        std::int32_t FindPathIndex(std::int32_t PathId);
        void BuildLegacySequences();
        void FreeLegacySequences();
        void InferLegacyVisitLimits();
        pas::WideString ExpandText(pas::WideString Text, std::uint8_t Colorize);
        std::uint8_t CheckCriticalParameters();
        void Start(std::int32_t Money, std::uint8_t PreserveExternalParameters);
        void EnterLocation(std::int32_t LocationId);
        void FollowPath(std::int32_t PathId);
        void ShowEvent(EventClass::TEvent* Event);
        void ShowOutcome();
        void ShowParameters();
        pas::List* Locations;
        pas::List* Paths;
        std::int32_t FormatVersion;
        std::int32_t MajorVersion;
        std::int32_t MinorVersion;
        TextFieldClass::TTextField* ChangeLogText;
        std::int32_t EditorScreenWidth;
        std::int32_t EditorScreenHeight;
        std::int32_t EditorGridWidth;
        std::int32_t EditorGridHeight;
        std::int32_t Difficulty;
        std::uint8_t CompleteOnFinish;
        std::uint8_t IssuerRaceMask;
        std::uint8_t TargetOwnerMask;
        std::uint8_t PlayerCareerMask;
        std::uint8_t PlayerRaceMask;
        std::uint8_t cpp_padding[3];
        std::int32_t SuccessRelationDelta;
        std::int32_t DefaultTraversalLimit;
        TextFieldClass::TTextField* QuestDescriptionText;
        TextFieldClass::TTextField* QuestSuccessGovMessageText;
        TextFieldClass::TTextField* ToStarText;
        TextFieldClass::TTextField* ToPlanetText;
        TextFieldClass::TTextField* DateText;
        TextFieldClass::TTextField* MoneyText;
        TextFieldClass::TTextField* FromPlanetText;
        TextFieldClass::TTextField* FromStarText;
        TextFieldClass::TTextField* RangerText;
        TextQuestInterface::TTextQuestInterface* PlayerInterface;
        std::uint8_t TextShown;
        std::uint8_t cpp_padding_2[3];
        EventClass::TEvent* DisplayedEvent;
        EventClass::TEvent* OutcomeEvent;
        TextQuestInterface::TQuestOutcome Outcome;
        pas::List* Parameters;
        pas::WideString LastEventSource;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace TextQuest
