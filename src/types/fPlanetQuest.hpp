#pragma once
#include "runtime_support.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/TextQuestInterface.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace TextQuest {
    struct TTextQuest;

} // namespace TextQuest

namespace fPlanetQuest {
    struct TfQuestA;

    struct TfPlanetQuest;

    struct TTextQuestPlayerInterface;

    using TQuestChoiceEvent = pas::Method<void(std::int32_t)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPlanetQuest : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfPlanetQuest, GI_MessageLoop::TMessageLoopGI, "TfPlanetQuest", 276)
        static pas::WideString GetTextBeforeDelimiter(const std::u16string_view& Text, char16_t Delimiter);
        // A nonempty string without a comma is read past its end.
        static pas::WideString GetTextAfterComma(const std::u16string_view& Text, char16_t IgnoredDelimiter);
        // Hexadecimal complement of the quest buffer's CRC32.
        static pas::WideString GetQuestContentHash(std::int32_t QuestId);
        void LoadQuestById(std::int32_t QuestId);
        void LoadQuestByName(const pas::WideString& Name);
        // Applies PQI overrides only through quest version 1111111124.
        void StartLoadedQuest();
        void ClearChoices();
        void AddChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback);
        // Value and callback are ignored.
        void AddDisabledChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback);
        static GI_MessageLoop::TObjectGI* CreateChoiceInlineObject(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item);
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void DisabledChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void DisabledChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void DisabledChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        // Only exact deltas of +120 and -120 are handled.
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void FinishChoiceLayout();
        void AnimateTextPage(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ClearParameterPanel();
        void AppendParameterText(pas::WideString Text);
        void LayoutParameterPanel();
        // <fix> sections use the fixed-width font.
        void SetQuestText(const pas::WideString& Text);
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        // StyleIndex is zero-based.
        pas::WideString GetTextColorTag(std::int32_t StyleIndex);
        std::uint32_t GetTextColor(std::int32_t StyleIndex);
        std::uint32_t GetDisabledTextColor(std::int32_t StyleIndex);
        void ApplyStyle();
        void SelectPageMode(GI_MessageLoop::TObjectGI* Sender);
        void SelectStyle(GI_MessageLoop::TObjectGI* Sender);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible);
        // Suppresses repeated picture names.
        void SetQuestPicture(pas::WideString Name);
        void RequestLoadGame(GI_MessageLoop::TObjectGI* Sender);
        void QuestKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void SelectMusic() override;
        pas::WideString ExpandTemplateText(pas::WideString Text);
        // Empty callback.
        static void IgnoreChoice(std::int32_t Value);
        void ContinueToLocation(std::int32_t LocationId);
        void ContinueAlongPath(std::int32_t PathId);
        // Value is unused.
        void ContinueToOutcome(std::int32_t Value);
        // Queued script quests report status 2.
        void CompleteQuestSuccess(std::int32_t Value);
        // Queued script quests report status 3.
        void CompleteQuestFailure(std::int32_t Value);
        void CompleteQuestDeath(std::int32_t Value);
        // PQI keys are quest,L|P|PAR,indices; picture names lose the Bm.PQI. prefix.
        void ApplyLegacyPictureOverrides();
        // Uses the first enabled money parameter; writes the player's clamped balance back.
        void ExportMoneyToPlayer();
        // Uses the first enabled money parameter.
        void ImportMoneyFromPlayer();
        // ext_name maps to GQuestVarExt_name; queued-script scope takes precedence over global scope.
        void ExportExternalParameters();
        // Writes clamped parameter values back to the script variables.
        void ImportExternalParameters();
        // <txt_name> uses global GQuestVarExt_name; queued-script scope is ignored.
        pas::WideString ExpandExternalText(pas::WideString Text);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        pas::WideString QuestName;
        // // Owned until OnClose.
        TextQuest::TTextQuest* Quest;
        pas::WideString CurrentDate;
        std::int32_t DaysElapsed;
        pas::WideString CurrentPicture;
        std::uint32_t MoneyLimitComplement;
        pas::WideString CurrentText;
        std::int32_t NextChoiceTop;
        std::int32_t ParameterPanelWidth;
        std::int32_t ParameterPanelHeight;
        GI_MessageLoop::PCallbackTimerGI PageAnimationTimer;
        std::int32_t PreviousStyleIndex;
        std::int32_t ChoiceCount;
        std::uint8_t cpp_padding[2];
        // // Unaligned in the native layout.
        WindowsSdk::TPoint ParameterPanelOrigin;
        std::uint8_t cpp_padding_2[2];
        // // -1 for a named quest.
        std::int32_t QuestId;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Methods use the active planet-quest screen; the interface has no instance state.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextQuestPlayerInterface : TextQuestInterface::TTextQuestInterface {
        PAS_CLASS_META(TTextQuestPlayerInterface, TextQuestInterface::TTextQuestInterface, "TTextQuestPlayerInterface", 4)
        // Suppresses repeated text after template expansion.
        void ShowText(pas::WideString Text) override;
        void ShowPicture(pas::WideString Name) override;
        void PlayMusic(pas::WideString Name) override;
        void PlaySound(pas::WideString Name) override;
        void ShowParameters(pas::WideString Text) override;
        void AddContinueAction() override;
        void AddSuccessAction() override;
        void AddDeathAction() override;
        void AddFailureAction() override;
        void AddPathAction(pas::WideString Text, std::int32_t PathId) override;
        void AddDisabledPath(pas::WideString Text) override;
        void AddPathContinueAction(std::int32_t PathId) override;
        void AddLocationContinueAction(std::int32_t LocationId) override;
        void AdvanceDays(std::int32_t Days) override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfQuestA : pas::Object {
        PAS_CLASS_META(TfQuestA, pas::Object, "TfQuestA", 20)
        void p_destroy() override;
        std::uint8_t cpp_padding[4];
        // No recovered method uses.
        TQuestChoiceEvent Callback;
        std::int32_t Value;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPlanetQuest
