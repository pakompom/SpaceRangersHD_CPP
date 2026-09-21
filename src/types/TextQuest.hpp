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

    using TQuestRaceSet = pas::Set<0, 6>;

    using TQuestPlayerCareerSet = pas::Set<0, 2>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextQuest : EC_Struct::TObjectEx {
        PAS_CLASS_META(TTextQuest, EC_Struct::TObjectEx, "TTextQuest", 128)
        void p_destroy() override;
        // Retains PlayerInterface and the owned text/event containers.
        void Reset();
        std::int32_t GetLocationCount();
        std::int32_t GetPathCount();
        // Index is one-based.
        LocationClass::TLocation* GetLocation(std::int32_t Index);
        // Index is one-based.
        PathClass::TPath* GetPath(std::int32_t Index);
        std::int32_t GetParameterCount();
        // Index is one-based; the list includes an extra trailing parameter.
        ParameterClass::TParameter* GetParameter(std::int32_t Index);
        // HeaderOnly still loads parameters and quest text.
        void LoadFromReader(EC_Buf::TBufEC* Reader, std::uint8_t HeaderOnly);
        void ResetEventIndices();
        // Returns the last matching one-based index; displays a message and returns zero when absent.
        std::int32_t FindLocationIndex(std::int32_t LocationId);
        // Returns the last matching one-based index; displays a message and returns zero when absent.
        std::int32_t FindPathIndex(std::int32_t PathId);
        // Used before quest version 1111111126.
        void BuildLegacySequences();
        void FreeLegacySequences();
        // Skips success and death locations; ordinary failure locations still participate.
        void InferLegacyVisitLimits();
        // Supports {formula}, [pN], [dN], and [dN:formula], including parameter-name aliases. Recursive display expansion has no cycle guard.
        pas::WideString ExpandText(pas::WideString Text, std::uint8_t Colorize);
        // Outcome precedence: death, failure, success.
        std::uint8_t CheckCriticalParameters();
        // External names begin with ext_; negative Money uses the initial range. Requires PlayerInterface.
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
        TQuestRaceSet IssuerRaces;
        // Empty inherits IssuerRaces for placement; offers require matching owners.
        TQuestRaceSet TargetRaces;
        TQuestPlayerCareerSet PlayerCareers;
        TQuestRaceSet PlayerRaces;
        std::uint8_t cpp_padding[3];
        std::int32_t SuccessRelationDelta;
        // // Serialized editor default; not applied by this runtime.
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
        // // Diagnostic location/path label; Reset preserves it.
        pas::WideString LastEventSource;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Quest-header race bits; the destination also supports uninhabited planets.
    enum TQuestRace : std::uint8_t {
        qrMaloc = 0,
        qrPeleng = 1,
        qrHuman = 2,
        qrFeyan = 3,
        qrGaal = 4,
        qrUninhabited = 6,
    };

    enum TQuestPlayerCareer : std::uint8_t {
        qpcTrader = 0,
        qpcPirate = 1,
        qpcWarrior = 2,
    };

} // namespace TextQuest
