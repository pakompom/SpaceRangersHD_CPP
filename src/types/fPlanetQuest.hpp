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
        static pas::WideString GetTextBeforeDelimiter(const pas::WideString& Text, char16_t Delimiter);
        static pas::WideString GetTextAfterComma(const pas::WideString& Text, char16_t IgnoredDelimiter);
        static pas::WideString GetQuestContentHash(std::int32_t QuestId);
        void LoadQuestById(std::int32_t QuestId);
        void LoadQuestByName(const pas::WideString& Name);
        void StartLoadedQuest();
        void ClearChoices();
        void AddChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback);
        void AddDisabledChoice(pas::WideString Text, std::int32_t Value, TQuestChoiceEvent Callback);
        static GI_MessageLoop::TObjectGI* CreateChoiceInlineObject(GI_Label::TLabelGI* Sender, EC_CacheFont::PFontObjectEC Item);
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void DisabledChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void DisabledChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        static void DisabledChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        void FinishChoiceLayout();
        void AnimateTextPage(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ClearParameterPanel();
        void AppendParameterText(pas::WideString Text);
        void LayoutParameterPanel();
        void SetQuestText(const pas::WideString& Text);
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        pas::WideString GetTextColorTag(std::int32_t StyleIndex);
        std::uint32_t GetTextColor(std::int32_t StyleIndex);
        std::uint32_t GetDisabledTextColor(std::int32_t StyleIndex);
        void ApplyStyle();
        void SelectPageMode(GI_MessageLoop::TObjectGI* Sender);
        void SelectStyle(GI_MessageLoop::TObjectGI* Sender);
        void ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible);
        void SetQuestPicture(pas::WideString Name);
        void RequestLoadGame(GI_MessageLoop::TObjectGI* Sender);
        void QuestKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void SelectMusic() override;
        pas::WideString ExpandTemplateText(pas::WideString Text);
        static void IgnoreChoice(std::int32_t Value);
        void ContinueToLocation(std::int32_t LocationId);
        void ContinueAlongPath(std::int32_t PathId);
        void ContinueToOutcome(std::int32_t Value);
        void CompleteQuestSuccess(std::int32_t Value);
        void CompleteQuestFailure(std::int32_t Value);
        void CompleteQuestDeath(std::int32_t Value);
        void ApplyLegacyPictureOverrides();
        void ExportMoneyToPlayer();
        void ImportMoneyFromPlayer();
        void ExportExternalParameters();
        void ImportExternalParameters();
        pas::WideString ExpandExternalText(pas::WideString Text);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        pas::WideString QuestName;
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
        WindowsSdk::TPoint ParameterPanelOrigin;
        std::uint8_t cpp_padding_2[2];
        std::int32_t QuestId;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextQuestPlayerInterface : TextQuestInterface::TTextQuestInterface {
        PAS_CLASS_META(TTextQuestPlayerInterface, TextQuestInterface::TTextQuestInterface, "TTextQuestPlayerInterface", 4)
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
        TQuestChoiceEvent Callback;
        std::int32_t Value;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPlanetQuest
