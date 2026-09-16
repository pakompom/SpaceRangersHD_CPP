#include "layout/fMods.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aModsInfo.hpp"
#include "units/aMyFunction.hpp"
#include "units/fListBox.hpp"
#include "units/fMods.hpp"

namespace fMods {
    std::int32_t AlignModRowHeight(std::int32_t Height, std::int32_t Step);

    void AddModRow(aModsInfo::TModInfo* Info, std::int32_t Tab, TfModsManager* Self, std::int32_t& ButtonWidth);

    std::uint8_t CollectModDependencies(aModsInfo::TModInfo* Info, TfModsManager* Self, EC_BlockPar::TBlockParEC*& EnableIndices, EC_BlockPar::TBlockParEC*& KnownNames, std::int32_t& VariantIndex, std::uint8_t& AskBeforeDependency, pas::List*& Choices);

    fMods::TfModsManager* ModsManagerScreen = nullptr;

    std::uint32_t ModTabColor{};

    std::uint32_t ModTabDownColor{};

    std::uint32_t ModSelectedColor{};

    std::uint32_t ModWarningColor{};

    std::uint32_t ModErrorColor{};

    std::int32_t ShowModsManager(GI_MessageLoop::TMessageLoopGI* Parent) {
        std::int32_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        if ((aModsInfo::ModInfos == nullptr || pas::list_count(aModsInfo::ModInfos) <= 0) && ModsManagerScreen != nullptr) {
            pas::free(ModsManagerScreen);
            ModsManagerScreen = nullptr;
        }
        aModsInfo::InitializeModInfos();
        if (pas::list_count(aModsInfo::ModInfos) <= 0) {
            GI_MessageBox::ShowMessageBoxGI(Parent, aConst::LocalizedText(u"FormMods.NoMods"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04, 0, 0, 0);
            return 0;
        }
        Parent->RootUiObject->NativeHook50();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(true, 0);
        if (ModsManagerScreen == nullptr) {
            ModsManagerScreen = pas::construct_call<TfModsManager>(GI_MessageLoop::TMessageLoopGI_Create);
            ModsManagerScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"ModsManager"_wref.get(), true);
            ModsManagerScreen->InitializeLayout();
        }
        ModsManagerScreen->ParentLoop = Parent;
        Parent->ChildLoop = ModsManagerScreen;
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Parent->ChildLoop = nullptr;
                ModsManagerScreen->ParentLoop = nullptr;
            };
            Result = ModsManagerScreen->Run();
            Parent->InvalidateViewport();
        }
        Parent->RestoreCursorState(&State);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->NativeHook48();
        return Result;
    }

    void TfModsManager_Destroy(TfModsManager* Self) {
        Self->TabButtons.set_length(0);
        Self->TabPanels.set_length(0);
        Self->TabHeights.set_length(0);
        Self->SelectedCounts.set_length(0);
        Self->WarningCounts.set_length(0);
        Self->ErrorCounts.set_length(0);
        Self->InvalidSelections.set_length(0);
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfModsManager::InitializeLayout() {
        std::int32_t ButtonWidth{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t GroupIndex{};
        std::int32_t Weight{};
        std::int32_t CandidateWeight{};
        std::int32_t BestWeight{};
        std::int32_t BestIndex{};
        pas::WideString GroupName{};
        pas::WideString SectionName{};
        GI_PanelScrollBar::TPanelScrollBarGI* ScrollPanel{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Dependency{};
        pas::List* Groups{};
        pas::List* Group{};
        EC_BlockPar::TBlockParEC* GroupIndices{};
        EC_BlockPar::TBlockParEC* Block{};
        auto AddModSectionTitle = [&](pas::WideString Text, std::int32_t Tab) -> void {
            GI_Image::TImageGI* Image{};
            if (this->TabHeights[Tab] != 0) {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this->TabPanels[Tab]);
                Image->SetImagePath(u"GI,Bm.FormOptions2.2Line"_w);
                Image->SetPosition(ClassesImports::Point(0, this->TabHeights[Tab]));
                Image->SetSize(([&] {
                    std::int32_t cpp_arg = Image->GetContentSize().Y + 2;
                    std::int32_t x = this->TabPanels[Tab]->ClientSize.X;
                    return ClassesImports::Point(x, cpp_arg);
                }()));
                Image->SetImageKindX(GI_Main::ikxLeftFill);
                this->TabHeights[Tab] += Image->ClientSize.Y;
            } else {
                this->TabHeights[Tab] += 10;
            }
            GI_Label::TLabelGI* LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, this->TabPanels[Tab]);
            LabelControl->SetFontName(GlobalsV::BigFontName);
            LabelControl->SetPositionModeW(false);
            LabelControl->SetWordWrapEnabled(false);
            LabelControl->SetPosition(ClassesImports::Point(0, this->TabHeights[Tab]));
            LabelControl->SetSize(ClassesImports::Point(this->TabPanels[Tab]->ClientSize.X, 1));
            LabelControl->SetTextAlignX(GI_Main::taxLeft);
            LabelControl->SetTextAlignY(GI_Main::tayAuto);
            LabelControl->SetTextColor(ModTabColor);
            LabelControl->SetText(Text);
            LabelControl->SetTextAlignY(GI_Main::tayTop);
            LabelControl->SetSize(ClassesImports::Point(LabelControl->ClientSize.X, LabelControl->ClientSize.Y + 1));
            this->TabHeights[Tab] = this->TabHeights[Tab] + fMods::AlignModRowHeight(LabelControl->ClientSize.Y, 10) + 2;
        };
        auto AddModRows = [&](pas::List* List, std::int32_t Tab) -> void {
            std::int32_t I{};
            aModsInfo::TModInfo* Info{};
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(List) - 1); cpp_range.next(I); ) {
                Info = pas::list_at<aModsInfo::TModInfo>(List, I);
                fMods::AddModRow(Info, Tab, this, ButtonWidth);
            }
        };
        auto ConfigureModTab = [&](pas::WideString Text, std::int32_t Tab) -> void {
            GI_GraphButton::TGraphButtonGI* cpp_with = this->TabButtons[Tab];
            cpp_with->HelpText = Text;
            cpp_with->UpCallback = pas::bind_method<&TfModsManager::TabClick>(this);
            cpp_with->DownCallback = pas::bind_method<&TfModsManager::TabClick>(this);
            cpp_with->SetActive(true);
        };
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        ModTabColor = GR_Main::GetStyleColorGI(u"Mods.ColorNormal"_w, 88, 229, 255);
        ModTabDownColor = GR_Main::GetStyleColorGI(u"Mods.ColorSelected"_w, 0, 0, 0);
        ModSelectedColor = GR_Main::GetStyleColorGI(u"Mods.ColorActive"_w, 0, 255, 0);
        ModWarningColor = GR_Main::GetStyleColorGI(u"Mods.ColorProblems"_w, 255, 150, 50);
        ModErrorColor = GR_Main::GetStyleColorGI(u"Mods.ColorCriticalProblems"_w, 255, 0, 0);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            ScrollPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(MainPanel->FindByNameRecursive(u"PanelSet"_wref.get()));
            {
                std::int32_t cpp_arg = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(MainPanel->FindByNameRecursive(u"ButGroup0"_wref.get()))->CaptionLabel->GetLineHeight() * 2;
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = ScrollPanel->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(cpp_arg);
            }
            ScrollPanel->VerticalScrollBar->SetLargeChange(ScrollPanel->ClientSize.Y);
            ScrollPanel->VerticalScrollBar->SetPageSize(ScrollPanel->ClientSize.Y);
        }
        {
            GI_MessageLoop::TObjectGI* cpp_with_2 = ScrollPanel->Parent;
            cpp_with_2->SetPosition(ClassesImports::Point(cpp_with_2->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_2->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Cancel"_wref.get()))->UpCallback = pas::bind_method<&TfModsManager::CloseClick>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"Confirm"_wref.get()))->UpCallback = pas::bind_method<&TfModsManager::ApplyClick>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButReset"_wref.get()))->UpCallback = pas::bind_method<&TfModsManager::ClearSelectionClick>(this);
        TabButtons.set_length(1);
        TabCount = 0;
        GI_GraphButton::TGraphButtonGI* Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButGroup0"_wref.get()));
        do {
            TabButtons[TabCount] = Button;
            ++TabCount;
            TabButtons.set_length(TabCount + 1);
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(FindControlByPath(pas::concat_wide({u"ButGroup", EC_Str::IntToWideString(TabCount)})));
        } while (!(Button == nullptr));
        TabPanels.set_length(TabCount);
        TabHeights.set_length(TabCount);
        SelectedCounts.set_length(TabCount);
        WarningCounts.set_length(TabCount);
        ErrorCounts.set_length(TabCount);
        InvalidSelections.set_length(TabCount);
        NeedsValidation = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range.next(I); ) {
            TabHeights[I] = 0;
            TabPanels[I] = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, ScrollPanel);
            {
                GI_Panel::TPanelGI* cpp_with_3 = TabPanels[I];
                cpp_with_3->SetSize(ClassesImports::Point(ScrollPanel->ClientSize.X, 0));
                cpp_with_3->SetPosition(ClassesImports::Point(0, 0));
                cpp_with_3->SetDepth(-1.0E+2);
                cpp_with_3->SetPositionModeW(true);
                cpp_with_3->UserValue = I;
            }
            SelectedCounts[I] = 0;
            WarningCounts[I] = 0;
            ErrorCounts[I] = 0;
        }
        pas::List* Missing = pas::make_object<pas::List>();
        pas::List* Anonymous = pas::make_object<pas::List>();
        pas::List* NoSection = pas::make_object<pas::List>();
        Groups = pas::make_object<pas::List>();
        pas::List* Weights = pas::make_object<pas::List>();
        GroupIndices = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_2.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if (Info->MissingFolder) {
                pas::list_add(Missing, reinterpret_cast<void*>(Info));
            } else if (Info->Name == u"" && Info->Section == u"") {
                pas::list_add(Anonymous, reinterpret_cast<void*>(Info));
            } else if (Info->Section == u"") {
                pas::list_add(NoSection, reinterpret_cast<void*>(Info));
            } else if (GroupIndices->CountParams(Info->Section) > 0) {
                pas::list_add(pas::list_at<pas::List>(Groups, EC_Str::ExtractDigitsToIntW(GroupIndices->GetParam(Info->Section))), reinterpret_cast<void*>(Info));
            } else {
                Group = pas::make_object<pas::List>();
                pas::list_add(Group, reinterpret_cast<void*>(Info));
                GroupIndices->AddParam(Info->Section, EC_Str::IntToWideString(pas::list_count(Groups)));
                pas::list_add(Groups, reinterpret_cast<void*>(Group));
            }
        }
        EC_BlockPar::TBlockParEC* Dependents = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_3.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if (!Info->MissingFolder) {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range_4.next(J); ) {
                    Dependency = Info->Dependencies[J];
                    if (Dependency != nullptr) {
                        if (Dependents->CountBlocks(Dependency->Name) > 0) {
                            Block = Dependents->GetBlock(Dependency->Name);
                        } else {
                            Block = Dependents->AddChildBlock(Dependency->Name);
                        }
                        if (Info->Section != u"") {
                            SectionName = Info->Section;
                        } else {
                            SectionName = u"{"_w;
                        }
                        if (Block->CountParams(SectionName) > 0) {
                            Block->SetOrAddParam(SectionName, EC_Str::IntToWideString(EC_Str::ExtractDigitsToIntW(Block->GetParam(SectionName)) + 1));
                        } else {
                            Block->AddParam(SectionName, u"1"_wref.get());
                        }
                    }
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_5.next(I); ) {
            pas::list_add(Weights, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(pas::list_count(pas::list_at<pas::List>(Groups, I))))));
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_6.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if (Info->MissingFolder || Info->Name == u"" || Dependents->CountBlocks(Info->Name) <= 0) {
                continue;
            }
            if (!Info->DuplicateName) {
                if (Info->Section != u"") {
                    Block = Dependents->GetBlockByPath(Info->Name);
                    Weight = 0;
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_7.next(J); ) {
                        if (Block->GetParamName(J) != Info->Section) {
                            Weight += EC_Str::ExtractDigitsToIntW(Block->GetParamValue(J));
                        }
                    }
                    J = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParam(Info->Section));
                    pas::list_put(Weights, J, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Weights, J))) + Weight))));
                }
                Dependents->DeleteChildBlock(Info->Name);
            } else {
                Block = Dependents->GetBlockByPath(Info->Name);
                if (Info->Section != u"") {
                    GroupName = Info->Section;
                } else {
                    GroupName = u"{"_w;
                }
                if (Block->CountParams(GroupName) > 0) {
                    Block->DeleteParam(GroupName);
                    if (Block->GetParamCount() <= 0) {
                        Dependents->DeleteChildBlock(Info->Name);
                    }
                } else if (Block->CountBlocks(GroupName) <= 0) {
                    Block->AddBlockByPath(GroupName);
                }
            }
        }
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Dependents->GetBlockCount() - 1); cpp_range_8.next(I); ) {
            Block = Dependents->GetBlockByIndex(I);
            if (Block->GetBlockCount() > 0) {
                Weight = 0;
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Block->GetParamCount() - 1); cpp_range_9.next(J); ) {
                    Weight += EC_Str::ExtractDigitsToIntW(Block->GetParamValue(J));
                }
                Weight = pas::idiv(Weight - 1, Block->GetBlockCount()) + 1;
                for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Block->GetBlockCount() - 1); cpp_range_10.next(J); ) {
                    GroupName = Block->GetBlockNameByIndex(J);
                    if (GroupName != u"{") {
                        GroupIndex = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParam(GroupName));
                        pas::list_put(Weights, GroupIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Weights, GroupIndex))) + Weight))));
                    }
                }
            }
        }
        std::int32_t GroupCount = pas::list_count(Groups);
        Group = pas::make_object<pas::List>();
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_11.next(I); ) {
            if (static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Weights, I))) <= 1 && pas::list_count(pas::list_at<pas::List>(Groups, I)) == 1) {
                pas::list_put(Weights, I, nullptr);
                --GroupCount;
                pas::list_add(Group, pas::list_get(pas::list_at<pas::List>(Groups, I), 0));
            }
        }
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(NoSection) - 1); cpp_range_12.next(I); ) {
            pas::list_add(Group, pas::list_get(NoSection, I));
        }
        for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(Anonymous) - 1); cpp_range_13.next(I); ) {
            pas::list_add(Group, pas::list_get(Anonymous, I));
        }
        if (pas::list_count(Group) > 0) {
            GroupIndices->AddParam(u"{}"_wref.get(), EC_Str::IntToWideString(pas::list_count(Groups)));
            pas::list_add(Groups, reinterpret_cast<void*>(Group));
            pas::list_add(Weights, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(1))));
            ++GroupCount;
        } else {
            pas::free(Group);
        }
        for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range_14.next(J); ) {
            TabButtons[J]->SetActive(false);
        }
        std::int32_t AvailableTabs = TabCount;
        if (pas::list_count(Missing) > 0) {
            if (TabCount <= GroupCount + 1) {
                I = TabCount - 1;
            } else {
                I = GroupCount;
            }
            --AvailableTabs;
            AddModRows(Missing, I);
            {
                pas::WideString localizedText = aConst::LocalizedText(u"FormMods.GroupNameForMissing"_wref.get());
                std::int32_t i = I;
                ConfigureModTab(std::move(localizedText), i);
            }
        }
        std::int32_t SeparateGroups = AvailableTabs - (AvailableTabs < GroupCount);
        if (SeparateGroups > GroupCount) {
            SeparateGroups = GroupCount;
        }
        for (auto cpp_range_15 = pas::for_to<std::int32_t>(0, SeparateGroups - 1); cpp_range_15.next(J); ) {
            BestWeight = 0;
            BestIndex = -1;
            for (auto cpp_range_16 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_16.next(I); ) {
                GroupIndex = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParamValue(I));
                CandidateWeight = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Weights, GroupIndex)));
                if (CandidateWeight > BestWeight) {
                    BestWeight = CandidateWeight;
                    BestIndex = I;
                }
            }
            GroupIndex = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParamValue(BestIndex));
            pas::list_put(Weights, GroupIndex, nullptr);
            Group = pas::list_at<pas::List>(Groups, GroupIndex);
            GroupName = GroupIndices->GetParamName(BestIndex);
            if (GroupName == u"{}") {
                GroupName = aConst::LocalizedText(u"FormMods.GroupNameForMisc"_wref.get());
            }
            AddModSectionTitle(GroupName, J);
            AddModRows(Group, J);
            ConfigureModTab(GroupName, J);
        }
        if (SeparateGroups < GroupCount) {
            for (auto cpp_range_17 = pas::for_to<std::int32_t>(SeparateGroups, GroupCount - 1); cpp_range_17.next(J); ) {
                BestWeight = 0;
                BestIndex = -1;
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_18.next(I); ) {
                    GroupIndex = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParamValue(I));
                    CandidateWeight = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Weights, GroupIndex)));
                    if (CandidateWeight > BestWeight) {
                        BestWeight = CandidateWeight;
                        BestIndex = I;
                    }
                }
                GroupIndex = EC_Str::ExtractDigitsToIntW(GroupIndices->GetParamValue(BestIndex));
                pas::list_put(Weights, GroupIndex, nullptr);
                Group = pas::list_at<pas::List>(Groups, GroupIndex);
                GroupName = GroupIndices->GetParamName(BestIndex);
                if (GroupName == u"{}") {
                    GroupName = aConst::LocalizedText(u"FormMods.GroupNameForMisc"_wref.get());
                }
                AddModSectionTitle(GroupName, AvailableTabs - 1);
                AddModRows(Group, AvailableTabs - 1);
            }
            ConfigureModTab(aConst::LocalizedText(u"FormMods.GroupNameForOther"_wref.get()), AvailableTabs - 1);
        }
        for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range_19.next(I); ) {
            GI_Panel::TPanelGI* cpp_with_4 = TabPanels[I];
            cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, TabHeights[I]));
        }
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfModsManager::KeyDown>(this);
        pas::free(Missing);
        pas::free(Anonymous);
        pas::free(NoSection);
        for (auto cpp_range_20 = pas::for_to<std::int32_t>(0, pas::list_count(Groups) - 1); cpp_range_20.next(I); ) {
            pas::free(pas::list_at<pas::Object>(Groups, I));
        }
        pas::free(Groups);
        pas::free(Weights);
        pas::free(GroupIndices);
    }

    std::int32_t AlignModRowHeight(std::int32_t Height, std::int32_t Step) {
        return System::Round(pas::real_divide(Height, Step) + 0.501L) * Step;
    }

    void AddModRow(aModsInfo::TModInfo* Info, std::int32_t Tab, TfModsManager* Self, std::int32_t& ButtonWidth) {
        GI_Image::TImageGI* Line{};
        if (Self->TabHeights[Tab] != 0) {
            Line = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self->TabPanels[Tab]);
            Line->SetImagePath(u"GI,Bm.FormOptions2.2Line"_w);
            Line->SetPosition(ClassesImports::Point(0, Self->TabHeights[Tab]));
            Line->SetSize(([&] {
                std::int32_t cpp_arg = Line->GetContentSize().Y + 2;
                std::int32_t x = Self->TabPanels[Tab]->ClientSize.X;
                return ClassesImports::Point(x, cpp_arg);
            }()));
            Line->SetImageKindX(GI_Main::ikxLeftFill);
            Self->TabHeights[Tab] += Line->ClientSize.Y;
        } else {
            Self->TabHeights[Tab] += 10;
        }
        GI_Image::TImageGI* Switch = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self->TabPanels[Tab]);
        Switch->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Info));
        Switch->SetImagePath(u"GI,Bm.FormOptions2.2SwitchN"_w);
        Switch->UserIndex = Info->Selected;
        Switch->SetSize(Switch->GetContentSize());
        ButtonWidth = Switch->ClientSize.X;
        Switch->SetPosition(ClassesImports::Point(Self->TabPanels[Tab]->ClientSize.X - ButtonWidth - 0, Self->TabHeights[Tab] + 10));
        Switch->SetImageKindY(GI_Main::ikyCenter);
        Switch->LeftButtonDownCallback = pas::bind_method<&TfModsManager::SwitchMouseDown>(Self);
        Switch->MouseEnterCallback = pas::bind_static_method<&TfModsManager::SwitchMouseEnter>(Self);
        Switch->MouseLeaveCallback = pas::bind_static_method<&TfModsManager::SwitchMouseLeave>(Self);
        Info->SwitchImage = Switch;
        GI_GraphButton::TGraphButtonGI* InfoButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Self->TabPanels[Tab]);
        InfoButton->SetImageNormalPath(u"GI,Bm.MsgPlayer.2UserN"_wref.get());
        InfoButton->SetImageNormalActivePath(u"GI,Bm.MsgPlayer.2UserA"_wref.get());
        InfoButton->SetImageDownPath(u"GI,Bm.MsgPlayer.2UserD"_wref.get());
        InfoButton->SetSize(InfoButton->GetMaxStateImageSize());
        ButtonWidth += InfoButton->ClientSize.X;
        InfoButton->SetPosition(ClassesImports::Point(Self->TabPanels[Tab]->ClientSize.X - ButtonWidth - 0, Self->TabHeights[Tab] + 9));
        InfoButton->UpCallback = pas::bind_method<&TfModsManager::ShowInfoClick>(Self);
        InfoButton->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Info));
        Switch->UserData = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Self->TabPanels[Tab])));
        {
            GI_GraphButton::TGraphButtonGI* cpp_with = reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Switch->UserData)));
            cpp_with->SetImageNormalPath(u"GI,Bm.MsgPlayer.2ShipMinusN"_wref.get());
            cpp_with->SetImageNormalActivePath(u"GI,Bm.MsgPlayer.2ShipMinusA"_wref.get());
            cpp_with->SetImageDownPath(u"GI,Bm.MsgPlayer.2ShipMinusD"_wref.get());
            cpp_with->SetSize(cpp_with->GetMaxStateImageSize());
            ButtonWidth += cpp_with->ClientSize.X;
            cpp_with->SetPosition(ClassesImports::Point(Self->TabPanels[Tab]->ClientSize.X - ButtonWidth + 2, Self->TabHeights[Tab] + 9));
            cpp_with->UpCallback = pas::bind_method<&TfModsManager::ShowProblemsClick>(Self);
            cpp_with->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Info));
        }
        Switch->UserState = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Self->TabPanels[Tab])));
        {
            GI_Label::TLabelGI* cpp_with_2 = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Switch->UserState)));
            cpp_with_2->SetFontName(GlobalsV::NormalFontName);
            cpp_with_2->SetPositionModeW(false);
            cpp_with_2->SetWordWrapEnabled(true);
            cpp_with_2->SetPosition(ClassesImports::Point(0, Self->TabHeights[Tab]));
            cpp_with_2->SetSize(ClassesImports::Point(Self->TabPanels[Tab]->ClientSize.X - ButtonWidth, 1));
            cpp_with_2->SetTextAlignX(GI_Main::taxLeft);
            cpp_with_2->SetTextAlignY(GI_Main::tayAuto);
            cpp_with_2->SetText(Info->GetDisplayName());
            cpp_with_2->SetTextAlignY(GI_Main::tayTop);
            cpp_with_2->SetSize(ClassesImports::Point(cpp_with_2->ClientSize.X, cpp_with_2->ClientSize.Y + 1));
            Self->TabHeights[Tab] = Self->TabHeights[Tab] + fMods::AlignModRowHeight(cpp_with_2->ClientSize.Y, 20) + 2;
        }
        {
            GI_Label::TLabelGI* cpp_with_3 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Self->TabPanels[Tab]);
            cpp_with_3->SetFontName(GlobalsV::NormalFontName);
            cpp_with_3->SetPositionModeW(false);
            cpp_with_3->SetWordWrapEnabled(true);
            cpp_with_3->SetPosition(ClassesImports::Point(0, Self->TabHeights[Tab]));
            cpp_with_3->SetSize(ClassesImports::Point(Self->TabPanels[Tab]->ClientSize.X - ButtonWidth, 1));
            cpp_with_3->SetTextAlignX(GI_Main::taxLeft);
            cpp_with_3->SetTextAlignY(GI_Main::tayAuto);
            cpp_with_3->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(205, 205, 205));
            if (Info->SmallDescription != u"") {
                cpp_with_3->SetText(Info->SmallDescription);
            } else if (Info->FullDescription != u"") {
                cpp_with_3->SetText(Info->FullDescription);
            } else {
                cpp_with_3->SetText(aConst::LocalizedText(u"FormMods.NoDescription"_wref.get()));
            }
            cpp_with_3->SetTextAlignY(GI_Main::tayTop);
            cpp_with_3->SetSize(ClassesImports::Point(cpp_with_3->ClientSize.X, cpp_with_3->ClientSize.Y + 1));
            Self->TabHeights[Tab] = Self->TabHeights[Tab] + fMods::AlignModRowHeight(cpp_with_3->ClientSize.Y, 20) + 5;
        }
        TfModsManager::UpdateModSwitch(Switch);
    }

    void TfModsManager::OnOpen() {
        pas::WideString FileName{};
        std::int32_t I{};
        std::int32_t Tab{};
        aModsInfo::TModInfo* Info{};
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range.next(I); ) {
            SelectedCounts[I] = 0;
            WarningCounts[I] = 0;
            ErrorCounts[I] = 0;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_2.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            Info->SwitchImage->UserIndex = Info->Selected;
            if (Info->ConflictCount == 0 && Info->DependencyCount == 0) {
                TfModsManager::UpdateModSwitch(Info->SwitchImage);
            }
            Tab = Info->SwitchImage->Parent->UserValue;
            if (Info->Selected) {
                ++SelectedCounts[Tab];
            }
            if (Info->DuplicateName || Info->Misplaced || Info->UnsupportedLanguage && Info->Selected) {
                ++WarningCounts[Tab];
            }
            if (Info->MissingFolder || Info->MissingDependency) {
                if (Info->Selected) {
                    ++ErrorCounts[Tab];
                } else {
                    ++WarningCounts[Tab];
                }
            }
        }
        ValidateSelection();
        SelectedTab = -1;
        TabClick(TabButtons[0]);
        if (GR_Main::UserSettingsConfig->CountParams(u"WeWarnedUserAboutMods"_wref.get()) == 0 || static_cast<std::uint8_t>(GI_Main::ParseEnabledNameGI(EC_Str::TrimWideString(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"WeWarnedUserAboutMods"_wref.get()))) ^ 1)) {
            GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormMods.WarningAchievements"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
            if (GR_Main::UserSettingsConfig->CountParams(u"WeWarnedUserAboutMods"_wref.get()) == 0) {
                GR_Main::UserSettingsConfig->AddParam(u"WeWarnedUserAboutMods"_wref.get(), u"True"_wref.get());
            } else {
                GR_Main::UserSettingsConfig->SetOrAddParam(u"WeWarnedUserAboutMods"_wref.get(), u"True"_wref.get());
            }
            FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"CFG.TXT"});
            GR_Main::UserSettingsConfig->SaveTextFile(FileName.pchar(), true, false);
        }
    }

    void TfModsManager::TabClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index = EC_Str::ExtractDigitsToIntW(Sender->ControlName);
        if (SelectedTab == Index) {
            UpdateTabDisplay();
        } else {
            SelectedTab = Index;
            UpdateTabDisplay();
            {
                GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"_wref.get()));
                PanelSet->SetScrollOffset(ClassesImports::Point(0, 0));
                PanelSet->UpdateScrollRanges();
                PanelSet->SetVerticalScrollbarEnabled(TabHeights[SelectedTab] > PanelSet->ClientSize.Y);
            }
        }
    }

    void TfModsManager::UpdateTabDisplay() {
        std::int32_t I{};
        if (NeedsValidation) {
            ValidateSelection();
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range.next(I); ) {
            GI_GraphButton::TGraphButtonGI* cpp_with = TabButtons[I];
            if (cpp_with->Active) {
                if (SelectedCounts[I] == 0) {
                    cpp_with->SetCaption(cpp_with->HelpText);
                } else {
                    cpp_with->SetCaption(pas::concat_wide({cpp_with->HelpText, u" (", EC_Str::IntToWideString(SelectedCounts[I]), u")"}));
                }
                cpp_with->SetDown(I == SelectedTab);
                if (ErrorCounts[I] > 0 || InvalidSelections[I]) {
                    cpp_with->SetCaptionColor(ModErrorColor);
                } else if (WarningCounts[I] > 0) {
                    cpp_with->SetCaptionColor(ModErrorColor);
                } else if (SelectedCounts[I] > 0) {
                    cpp_with->SetCaptionColor(ModSelectedColor);
                } else if (cpp_with->Down) {
                    cpp_with->SetCaptionColor(ModTabDownColor);
                    cpp_with->CaptionColors[0] = ModTabColor;
                    cpp_with->CaptionColors[1] = ModTabColor;
                } else {
                    cpp_with->SetCaptionColor(ModTabColor);
                    cpp_with->CaptionColors[2] = ModTabDownColor;
                    cpp_with->CaptionColors[3] = ModTabDownColor;
                }
                TabPanels[I]->SetActive(I == SelectedTab);
            }
        }
    }

    void TfModsManager::ValidateSelection() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t VariantIndex{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Related{};
        GI_Image::TImageGI* Switch{};
        GI_Image::TImageGI* RelatedSwitch{};
        std::uint8_t Invalid{};
        std::uint8_t Found{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, TabCount - 1); cpp_range.next(I); ) {
            InvalidSelections[I] = false;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_2.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if (Info->ConflictCount != 0 || Info->DependencyCount != 0) {
                Switch = Info->SwitchImage;
                TfModsManager::UpdateModSwitch(Switch);
                if (Switch->UserIndex != 0) {
                    if (!InvalidSelections[Switch->Parent->UserValue]) {
                        Invalid = false;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Info->ConflictCount - 1); cpp_range_3.next(J); ) {
                            VariantIndex = 0;
                            Related = Info->Conflicts[J];
                            if (Related != nullptr) {
                                while (true) {
                                    RelatedSwitch = Related->SwitchImage;
                                    if (RelatedSwitch->UserIndex == 1) {
                                        Invalid = true;
                                        break;
                                    }
                                    if (!Related->DuplicateName) {
                                        break;
                                    }
                                    ++VariantIndex;
                                    Related = Info->GetConflict(J, VariantIndex);
                                    if (Related == nullptr) {
                                        break;
                                    }
                                }
                            }
                            if (Invalid) {
                                break;
                            }
                        }
                        if (!Invalid) {
                            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range_4.next(J); ) {
                                Found = false;
                                VariantIndex = 0;
                                Related = Info->Dependencies[J];
                                if (Related != nullptr) {
                                    while (true) {
                                        RelatedSwitch = Related->SwitchImage;
                                        if (RelatedSwitch->UserIndex == 1) {
                                            Found = true;
                                            break;
                                        }
                                        if (!Related->DuplicateName) {
                                            break;
                                        }
                                        ++VariantIndex;
                                        Related = Info->GetDependency(J, VariantIndex);
                                        if (Related == nullptr) {
                                            break;
                                        }
                                    }
                                }
                                if (!Found) {
                                    Invalid = true;
                                    break;
                                }
                            }
                        }
                        if (Invalid) {
                            InvalidSelections[Switch->Parent->UserValue] = true;
                            reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Switch->UserState)))->SetTextColor(ModErrorColor);
                            reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Switch->UserData)))->SetActive(Invalid);
                        }
                    }
                }
            }
        }
        NeedsValidation = false;
    }

    void TfModsManager::ClearSelectionClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
            SetModSelected(pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I)->SwitchImage, false);
        }
        UpdateTabDisplay();
    }

    void TfModsManager::SelectAll() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Related{};
        aModsInfo::TModInfo* Conflict{};
        GI_Image::TImageGI* Switch{};
        std::uint8_t Changed{};
        std::uint8_t Invalid{};
        std::uint8_t Found{};
        do {
            Changed = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
                Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                if (static_cast<std::uint8_t>(Info->MissingFolder ^ 1) && static_cast<std::uint8_t>(Info->MissingDependency ^ 1)) {
                    Switch = Info->SwitchImage;
                    if (Switch->UserIndex != 1 && Switch->Parent->UserValue == SelectedTab) {
                        Invalid = false;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Info->ConflictCount - 1); cpp_range_2.next(J); ) {
                            Related = Info->Conflicts[J];
                            K = 0;
                            if (Related != nullptr) {
                                while (true) {
                                    if (Related->SwitchImage->UserIndex == 1) {
                                        Invalid = true;
                                        break;
                                    }
                                    if (!Related->DuplicateName) {
                                        break;
                                    }
                                    ++K;
                                    Related = Info->GetConflict(J, K);
                                    if (Related == nullptr) {
                                        break;
                                    }
                                }
                            }
                        }
                        if (!Invalid) {
                            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range_3.next(J); ) {
                                Related = Info->Dependencies[J];
                                Found = false;
                                K = 0;
                                if (Related != nullptr) {
                                    while (true) {
                                        if (Related->SwitchImage->UserIndex == 1) {
                                            Found = true;
                                            break;
                                        }
                                        if (!Related->DuplicateName) {
                                            break;
                                        }
                                        ++K;
                                        Related = Info->GetDependency(J, K);
                                        if (Related == nullptr) {
                                            break;
                                        }
                                    }
                                }
                                if (!Found) {
                                    Invalid = true;
                                    break;
                                }
                            }
                            if (!Invalid) {
                                if (Info->ReferencedAsConflict) {
                                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_4.next(J); ) {
                                        Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, J);
                                        if (Related->SwitchImage->UserIndex != 0) {
                                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Related->ConflictCount - 1); cpp_range_5.next(K); ) {
                                                Conflict = Related->Conflicts[K];
                                                if (Conflict != nullptr) {
                                                    if (Conflict->Name == Info->Name) {
                                                        Invalid = true;
                                                        break;
                                                    }
                                                }
                                            }
                                            if (Invalid) {
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (!Invalid) {
                                    Changed = true;
                                    SetModSelected(Switch, true);
                                }
                            }
                        }
                    }
                }
            }
        } while (Changed);
        UpdateTabDisplay();
    }

    void TfModsManager::DeselectAll() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t VariantIndex{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Related{};
        aModsInfo::TModInfo* Dependency{};
        GI_Image::TImageGI* Switch{};
        std::uint8_t Changed{};
        std::uint8_t Required{};
        std::uint8_t Found{};
        std::uint8_t UniqueName{};
        do {
            Changed = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
                Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                Switch = Info->SwitchImage;
                if (Switch->UserIndex != 0 && Switch->Parent->UserValue == SelectedTab) {
                    UniqueName = static_cast<std::uint8_t>(Info->DuplicateName ^ 1);
                    Required = false;
                    if (Info->ReferencedAsDependency) {
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_2.next(J); ) {
                            Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, J);
                            if (Related->SwitchImage->UserIndex != 0) {
                                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Related->DependencyCount - 1); cpp_range_3.next(K); ) {
                                    Dependency = Related->Dependencies[K];
                                    if (Dependency != nullptr) {
                                        if (UniqueName) {
                                            Required = Dependency == Info;
                                            if (Required) {
                                                break;
                                            }
                                        } else if (Dependency->Name == Info->Name) {
                                            Found = false;
                                            VariantIndex = 0;
                                            while (Dependency != nullptr) {
                                                if (Dependency != Info && Dependency->SwitchImage->UserIndex == 1) {
                                                    Found = true;
                                                    break;
                                                }
                                                ++VariantIndex;
                                                Dependency = Related->GetDependency(K, VariantIndex);
                                            }
                                            if (!Found) {
                                                Required = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (Required) {
                                    break;
                                }
                            }
                        }
                    }
                    if (!Required) {
                        Changed = true;
                        SetModSelected(Switch, false);
                    }
                }
            }
        } while (Changed);
        UpdateTabDisplay();
    }

    void TfModsManager::CloseClick(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode == 0) {
            RequestClose(2);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfModsManager::ApplyClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t J{};
        aModsInfo::TModInfo* Info{};
        pas::WideString Folders{};
        std::uint8_t Ordered{};
        std::uint8_t Unchanged = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if ((Info->SwitchImage->UserIndex == 1) != Info->Selected) {
                Unchanged = false;
                break;
            }
        }
        if (Unchanged) {
            Ordered = true;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aModsInfo::SelectedModInfos) - 1); cpp_range_2.next(I); ) {
                std::uint32_t cpp_left = pas::list_at<aModsInfo::TModInfo>(aModsInfo::SelectedModInfos, I)->Priority;
                if (cpp_left < pas::list_at<aModsInfo::TModInfo>(aModsInfo::SelectedModInfos, I - 1)->Priority) {
                    Ordered = false;
                    break;
                }
            }
            if (Ordered || ([&] {
                const pas::WideString& localizedText = aConst::LocalizedText(u"FormMods.QueryWrongOrderFix"_wref.get());
                GI_MessageLoop::TMessageLoopGI* innermostScreenLoop = Globals::GetInnermostScreenLoop();
                return GI_MessageBox::ShowMessageBoxGI(innermostScreenLoop, localizedText, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgWarning, 0, 0, 0);
            }()) != GI_MessageBox::mbgResultOK) {
                if (ExitCode == 0) {
                    RequestClose(2);
                } else {
                    RequestClose(ExitCode);
                }
                return;
            }
        }
        pas::List* List = pas::make_object<pas::List>();
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_3.next(I); ) {
            Info = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
            if (Info->SwitchImage->UserIndex == 1) {
                pas::list_add(List, reinterpret_cast<void*>(Info));
            }
        }
        Folders = pas::WideString();
        if (pas::list_count(List) > 0) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(List) - 1); cpp_range_4.next(I); ) {
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(List) - 1 - I); cpp_range_5.next(J); ) {
                    std::uint32_t cpp_left_2 = pas::list_at<aModsInfo::TModInfo>(List, J)->Priority;
                    if (cpp_left_2 > pas::list_at<aModsInfo::TModInfo>(List, J + 1)->Priority) {
                        Info = pas::list_at<aModsInfo::TModInfo>(List, J);
                        pas::list_put(List, J, pas::list_get(List, J + 1));
                        pas::list_put(List, J + 1, reinterpret_cast<void*>(Info));
                    }
                }
            }
            Folders = pas::list_at<aModsInfo::TModInfo>(List, 0)->Folder;
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, pas::list_count(List) - 1); cpp_range_6.next(I); ) {
                Folders = pas::concat_wide({Folders, u", ", pas::list_at<aModsInfo::TModInfo>(List, I)->Folder});
            }
        }
        pas::free(List);
        EC_BlockPar::TBlockParEC* Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        Block->AddParam(u"CurrentMod"_wref.get(), Folders);
        Block->SaveTextFile(pas::literal_pointer(u"Mods\\ModCFG.txt"), true, false);
        pas::free(Block);
        Globals::ReloadModsRequested = true;
        if (ExitCode == 0) {
            RequestClose(2);
        } else {
            RequestClose(ExitCode);
        }
    }

    void TfModsManager::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_RETURN) {
            ApplyClick(nullptr);
        } else if (Key == WindowsSdk::VK_ESCAPE) {
            CloseClick(nullptr);
        } else if (Key == 'A' && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            SelectAll();
        } else if (Key == 'Z' && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            DeselectAll();
        }
    }

    void TfModsManager::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* PanelSet = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelSet"_wref.get()));
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position - PanelSet->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            PanelSet->VerticalScrollBar->SetPosition_2(PanelSet->VerticalScrollBar->Position + PanelSet->VerticalScrollBar->SmallChange);
        }
    }

    void TfModsManager::SwitchMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserIndex != 1) {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchA"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    void TfModsManager::SwitchMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserIndex != 1) {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(Sender);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"SwitchN"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
    }

    void TfModsManager::SwitchMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        EC_BlockPar::TBlockParEC* EnableIndices{};
        std::int32_t VariantIndex{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Index{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Related{};
        aModsInfo::TModInfo* Dependency{};
        pas::WideString Value{};
        pas::WideString Text{};
        pas::WideString Temp{};
        EC_BlockPar::TBlockParEC* DisableIndices{};
        std::uint8_t PassChanged{};
        std::uint8_t Alternative{};
        GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
        Info = reinterpret_cast<aModsInfo::TModInfo*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        std::uint8_t Changed = false;
        pas::List* Choices = nullptr;
        DisableIndices = nullptr;
        EnableIndices = nullptr;
        EC_BlockPar::TBlockParEC* DisableNames = nullptr;
        EC_BlockPar::TBlockParEC* KnownNames = nullptr;
        std::uint8_t AskBeforeDependency = true;
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                if (Sender->UserIndex == 1) {
                    if (!Info->ReferencedAsDependency) {
                        SetModSelected(Sender, false);
                        Changed = true;
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    DisableIndices = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    DisableNames = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    DisableIndices->AddParam(Info->IndexText, u""_wref.get());
                    DisableNames->AddParam(Info->Name, u""_wref.get());
                    do {
                        PassChanged = false;
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
                            Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                            if (Related->SwitchImage->UserIndex != 0) {
                                if (Related->DependencyCount != 0) {
                                    if (DisableIndices->CountParams(EC_Str::IntToWideString(I)) <= 0) {
                                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Related->DependencyCount - 1); cpp_range_2.next(J); ) {
                                            Dependency = Related->Dependencies[J];
                                            if (Dependency != nullptr && DisableNames->CountParams(Dependency->Name) > 0) {
                                                Alternative = false;
                                                if (Dependency->DuplicateName) {
                                                    VariantIndex = 0;
                                                    while (Dependency != nullptr) {
                                                        if (Dependency->SwitchImage->UserIndex == 1 && DisableIndices->CountParams(Dependency->IndexText) <= 0) {
                                                            Alternative = true;
                                                            break;
                                                        }
                                                        ++VariantIndex;
                                                        Dependency = Related->GetDependency(J, VariantIndex);
                                                    }
                                                }
                                                if (!Alternative) {
                                                    PassChanged = true;
                                                    DisableIndices->AddParam(EC_Str::IntToWideString(I), u""_wref.get());
                                                    if (DisableNames->CountParams(Related->Name) <= 0) {
                                                        DisableNames->AddParam(Related->Name, u""_wref.get());
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } while (PassChanged);
                    if (DisableIndices->GetParamCount() == 1) {
                        SetModSelected(Sender, false);
                        Changed = true;
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    Value = pas::WideString();
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, DisableIndices->GetParamCount() - 1); cpp_range_3.next(I); ) {
                        Text = DisableIndices->GetParamName(I);
                        if (Text != Info->IndexText) {
                            Index = EC_Str::ExtractDigitsToIntW(Text);
                            if (Value == u"") {
                                Value = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name;
                            } else {
                                Value = pas::concat_wide({Value, u", ", pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name});
                            }
                        }
                    }
                    Text = aConst::LocalizedText(u"FormMods.QueryTurnOffWithExtra"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<ModName>"_w, pas::concat_wide({u"<color=255,240,100>", Info->Name, u"</color>"}), pas::WideString());
                    aMyFunction::ReplaceTextToken(Text, u"<ModsList>"_w, pas::concat_wide({u"<color=255,240,100>", Value, u"</color>"}), pas::WideString());
                    if (GI_MessageBox::ShowMessageBoxGI(Globals::GetInnermostScreenLoop(), Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, DisableIndices->GetParamCount() - 1); cpp_range_4.next(I); ) {
                        Index = EC_Str::ExtractDigitsToIntW(DisableIndices->GetParamName(I));
                        SetModSelected(pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->SwitchImage, false);
                    }
                    Changed = true;
                } else {
                    if (static_cast<std::uint8_t>(Info->ReferencedAsConflict ^ 1) && Info->ConflictCount == 0 && Info->DependencyCount == 0) {
                        SetModSelected(Sender, true);
                        Changed = true;
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    EnableIndices = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    KnownNames = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    DisableIndices = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    DisableNames = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    KnownNames->AddParam(Info->Name, u""_wref.get());
                    if (!fMods::CollectModDependencies(Info, this, EnableIndices, KnownNames, VariantIndex, AskBeforeDependency, Choices)) {
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, EnableIndices->GetParamCount() - 1); cpp_range_5.next(I); ) {
                        Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, EC_Str::ExtractDigitsToIntW(EnableIndices->GetParamName(I)));
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Related->ConflictCount - 1); cpp_range_6.next(J); ) {
                            Dependency = Related->Conflicts[J];
                            if (Dependency != nullptr && DisableNames->CountParams(Dependency->Name) <= 0) {
                                if (KnownNames->CountParams(Dependency->Name) > 0) {
                                    Text = aConst::LocalizedText(u"FormMods.ErrorInvalidConfiguration"_wref.get());
                                    aMyFunction::ReplaceTextToken(Text, u"<ModName>"_w, Dependency->Name, pas::WideString());
                                    GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                                    cpp_flow = pas::FinallyFlow::Return;
                                    goto cpp_cleanup;
                                }
                                DisableNames->AddParam(Dependency->Name, u""_wref.get());
                                VariantIndex = 0;
                                while (Dependency != nullptr) {
                                    if (Dependency->SwitchImage->UserIndex == 1) {
                                        DisableIndices->AddParam(Dependency->IndexText, u""_wref.get());
                                    }
                                    if (!Dependency->DuplicateName) {
                                        break;
                                    }
                                    ++VariantIndex;
                                    Dependency = Related->GetConflict(J, VariantIndex);
                                }
                            }
                        }
                    }
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_7.next(I); ) {
                        Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                        if (Related->ConflictCount != 0) {
                            Text = EC_Str::IntToWideString(I);
                            if ((Related->SwitchImage->UserIndex != 0 || EnableIndices->CountParams(Text) > 0) && DisableIndices->CountParams(Text) <= 0) {
                                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Related->ConflictCount - 1); cpp_range_8.next(J); ) {
                                    Dependency = Related->Conflicts[J];
                                    if (Dependency != nullptr) {
                                        if (KnownNames->CountParams(Dependency->Name) > 0) {
                                            if (EnableIndices->CountParams(Text) > 0) {
                                                Text = aConst::LocalizedText(u"FormMods.ErrorInvalidConfiguration"_wref.get());
                                                aMyFunction::ReplaceTextToken(Text, u"<ModName>"_w, Related->Name, pas::WideString());
                                                GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                                            }
                                            DisableIndices->AddParam(Text, u""_wref.get());
                                            if (DisableNames->CountParams(Related->Name) <= 0) {
                                                DisableNames->AddParam(Related->Name, u""_wref.get());
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    do {
                        PassChanged = false;
                        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range_9.next(I); ) {
                            Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                            if (Related->DependencyCount != 0) {
                                Value = EC_Str::IntToWideString(I);
                                if ((Related->SwitchImage->UserIndex != 0 || EnableIndices->CountParams(Value) > 0) && DisableIndices->CountParams(Value) <= 0) {
                                    for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Related->DependencyCount - 1); cpp_range_10.next(J); ) {
                                        Dependency = Related->Dependencies[J];
                                        if (Dependency != nullptr && DisableNames->CountParams(Dependency->Name) > 0) {
                                            Alternative = false;
                                            if (Dependency->DuplicateName) {
                                                VariantIndex = 0;
                                                while (Dependency != nullptr) {
                                                    if (Dependency->SwitchImage->UserIndex == 1 && DisableIndices->CountParams(Dependency->IndexText) <= 0) {
                                                        Alternative = true;
                                                        break;
                                                    }
                                                    ++VariantIndex;
                                                    Dependency = Related->GetDependency(J, VariantIndex);
                                                }
                                            }
                                            if (!Alternative) {
                                                PassChanged = true;
                                                Temp = EC_Str::IntToWideString(I);
                                                if (EnableIndices->CountParams(Temp) > 0) {
                                                    Text = aConst::LocalizedText(u"FormMods.ErrorInvalidConfiguration"_wref.get());
                                                    if (Related != Info) {
                                                        aMyFunction::ReplaceTextToken(Text, u"<ModName>"_w, Related->Name, pas::WideString());
                                                    } else {
                                                        aMyFunction::ReplaceTextToken(Text, u"<ModName>"_w, Info->Folder, pas::WideString());
                                                    }
                                                    GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                                                    cpp_flow = pas::FinallyFlow::Return;
                                                    goto cpp_cleanup;
                                                }
                                                DisableIndices->AddParam(Temp, u""_wref.get());
                                                if (DisableNames->CountParams(Related->Name) <= 0) {
                                                    DisableNames->AddParam(Related->Name, u""_wref.get());
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } while (PassChanged);
                    if (DisableIndices->GetParamCount() == 0 && EnableIndices->GetParamCount() == 1) {
                        SetModSelected(Sender, true);
                        Changed = true;
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    Value = pas::WideString();
                    for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, DisableIndices->GetParamCount() - 1); cpp_range_11.next(I); ) {
                        Index = EC_Str::ExtractDigitsToIntW(DisableIndices->GetParamName(I));
                        if (Value == u"") {
                            Value = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name;
                        } else {
                            Value = pas::concat_wide({Value, u", ", pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name});
                        }
                    }
                    Text = pas::WideString();
                    for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, EnableIndices->GetParamCount() - 1); cpp_range_12.next(I); ) {
                        Temp = EnableIndices->GetParamName(I);
                        if (Temp != Info->IndexText) {
                            Index = EC_Str::ExtractDigitsToIntW(Temp);
                            if (Text == u"") {
                                Text = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name;
                            } else {
                                Text = pas::concat_wide({Text, u", ", pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->Name});
                            }
                        }
                    }
                    if (Value != u"") {
                        Temp = aConst::LocalizedText(u"FormMods.QueryTurnOnWithExtra2"_wref.get());
                        aMyFunction::ReplaceTextToken(Temp, u"<ModsList>"_w, pas::concat_wide({u"<color=255,240,100>", Value, u"</color>"}), pas::WideString());
                    } else {
                        Temp = pas::WideString();
                    }
                    if (Text != u"") {
                        Value = aConst::LocalizedText(u"FormMods.QueryTurnOnWithExtra1"_wref.get());
                        aMyFunction::ReplaceTextToken(Value, u"<ModsList>"_w, pas::concat_wide({u"<color=255,240,100>", Text, u"</color>"}), pas::WideString());
                    } else {
                        Value = pas::WideString();
                    }
                    if (Temp != u"" && Value != u"") {
                        Value = pas::concat_wide({Value, u"\r\n", Temp});
                    } else {
                        Value = pas::concat_wide({Value, Temp});
                    }
                    Temp = pas::concat_wide({aConst::LocalizedText(u"FormMods.QueryTurnOnWithExtra0"_wref.get()), u"\r\n", Value, u"\r\n", aConst::LocalizedText(u"FormMods.QueryTurnOnWithExtra3"_wref.get())});
                    if (GI_MessageBox::ShowMessageBoxGI(Globals::GetInnermostScreenLoop(), Temp, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    Changed = true;
                    for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, DisableIndices->GetParamCount() - 1); cpp_range_13.next(I); ) {
                        Index = EC_Str::ExtractDigitsToIntW(DisableIndices->GetParamName(I));
                        SetModSelected(pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->SwitchImage, false);
                    }
                    for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, EnableIndices->GetParamCount() - 1); cpp_range_14.next(I); ) {
                        Index = EC_Str::ExtractDigitsToIntW(EnableIndices->GetParamName(I));
                        SetModSelected(pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, Index)->SwitchImage, true);
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (Choices != nullptr) {
                pas::free(Choices);
            }
            if (DisableIndices != nullptr) {
                pas::free(DisableIndices);
            }
            if (EnableIndices != nullptr) {
                pas::free(EnableIndices);
            }
            if (DisableNames != nullptr) {
                pas::free(DisableNames);
            }
            if (KnownNames != nullptr) {
                pas::free(KnownNames);
            }
            if (Changed) {
                UpdateTabDisplay();
            }
            TfModsManager::SwitchMouseLeave(Sender);
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
    }

    std::uint8_t CollectModDependencies(aModsInfo::TModInfo* Info, TfModsManager* Self, EC_BlockPar::TBlockParEC*& EnableIndices, EC_BlockPar::TBlockParEC*& KnownNames, std::int32_t& VariantIndex, std::uint8_t& AskBeforeDependency, pas::List*& Choices) {
        pas::WideString Name{};
        pas::WideString Caption{};
        aModsInfo::TModInfo* Related{};
        std::int32_t PartIndex{};
        std::int32_t I{};
        std::int32_t SelectedIndex{};
        System::PWideString Choice{};
        std::uint8_t Found{};
        std::uint8_t Result = false;
        EnableIndices->AddParam(Info->IndexText, u""_wref.get());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range.next(PartIndex); ) {
            Related = Info->Dependencies[PartIndex];
            if (Related == nullptr) {
                Name = aConst::LocalizedText(u"FormMods.ErrorNoDependency"_wref.get());
                aMyFunction::ReplaceTextToken(Name, u"<ModName>"_w, pas::concat_wide({u"<color=255,240,100>", EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Info->DependencyNames, PartIndex, u","_wref.get())), u"</color>"}), pas::WideString());
                GI_MessageBox::ShowMessageBoxGI(Self, Name, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                return Result;
            }
            Name = Related->Name;
            if (KnownNames->CountParams(Name) > 0) {
                continue;
            }
            KnownNames->AddParam(Name, u""_wref.get());
            if (Related->SwitchImage->UserIndex != 1) {
                if (!Related->DuplicateName) {
                    if (!fMods::CollectModDependencies(Related, Self, EnableIndices, KnownNames, VariantIndex, AskBeforeDependency, Choices)) {
                        return Result;
                    }
                } else {
                    Found = false;
                    VariantIndex = 1;
                    Related = Info->GetDependency(PartIndex, VariantIndex);
                    while (Related != nullptr) {
                        if (Related->SwitchImage->UserIndex == 1) {
                            Found = true;
                            break;
                        }
                        ++VariantIndex;
                        Related = Info->GetDependency(PartIndex, VariantIndex);
                    }
                    if (!Found) {
                        if (AskBeforeDependency) {
                            AskBeforeDependency = false;
                            if (([&] {
                                const pas::WideString& localizedText = aConst::LocalizedText(u"FormMods.QuerySelectDependencyFirst"_wref.get());
                                GI_MessageLoop::TMessageLoopGI* innermostScreenLoop = Globals::GetInnermostScreenLoop();
                                return GI_MessageBox::ShowMessageBoxGI(innermostScreenLoop, localizedText, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
                            }()) != GI_MessageBox::mbgResultOK) {
                                return Result;
                            }
                        }
                        if (Choices == nullptr) {
                            Choices = pas::make_object<pas::List>();
                        }
                        I = 0;
                        Related = Info->Dependencies[PartIndex];
                        while (Related != nullptr) {
                            pas::new_value(Choice);
                            *Choice = Related->Folder;
                            pas::list_add(Choices, static_cast<void*>(Choice));
                            ++I;
                            Related = Info->GetDependency(PartIndex, I);
                        }
                        Caption = aConst::LocalizedText(u"FormMods.QuerySelectDependency"_wref.get());
                        aMyFunction::ReplaceTextToken(Caption, u"<ModName>"_w, Name, pas::WideString());
                        if (fListBox::ShowListDialog(Globals::GetInnermostScreenLoop(), SelectedIndex, Caption, Choices, 0, 0) != 1) {
                            SelectedIndex = -1;
                        }
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Choices) - 1); cpp_range_2.next(I); ) {
                            pas::dispose(pas::list_get(Choices, I));
                        }
                        pas::list_clear(Choices);
                        if (SelectedIndex < 0) {
                            return Result;
                        }
                        if (!fMods::CollectModDependencies(Info->GetDependency(PartIndex, SelectedIndex), Self, EnableIndices, KnownNames, VariantIndex, AskBeforeDependency, Choices)) {
                            return Result;
                        }
                    }
                }
            }
        }
        return true;
    }

    void TfModsManager::SetModSelected(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Value) {
        if ((Sender->UserIndex == 1) == Value) {
            return;
        }
        std::int32_t Tab = Sender->Parent->UserValue;
        aModsInfo::TModInfo* Info = reinterpret_cast<aModsInfo::TModInfo*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        std::uint8_t LanguageWarning = Info->UnsupportedLanguage && static_cast<std::uint8_t>(Info->DuplicateName ^ 1) && static_cast<std::uint8_t>(Info->Misplaced ^ 1);
        if (Value) {
            Sender->UserIndex = 1;
            ++SelectedCounts[Tab];
            if (Info->MissingFolder || Info->MissingDependency) {
                ++ErrorCounts[Tab];
                --WarningCounts[Tab];
            } else if (LanguageWarning) {
                ++WarningCounts[Tab];
            }
        } else {
            Sender->UserIndex = 0;
            --SelectedCounts[Tab];
            if (Info->MissingFolder || Info->MissingDependency) {
                --ErrorCounts[Tab];
                ++WarningCounts[Tab];
            } else if (LanguageWarning) {
                --WarningCounts[Tab];
            }
        }
        TfModsManager::UpdateModSwitch(Sender);
        if (Info->ConflictCount > 0 || Info->DependencyCount > 0 || Info->ReferencedAsConflict || Info->ReferencedAsDependency) {
            NeedsValidation = true;
        }
    }

    void TfModsManager::UpdateModSwitch(GI_MessageLoop::TObjectGI* Sender) {
        aModsInfo::TModInfo* Info = reinterpret_cast<aModsInfo::TModInfo*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        std::uint8_t Selected = Sender->UserIndex == 1;
        std::uint8_t Warning = Info->DuplicateName || Info->Misplaced || Info->UnsupportedLanguage && Selected || Info->MissingFolder || Info->MissingDependency;
        if (Selected) {
            pas::checked_cast<GI_Image::TImageGI*>(Sender)->SetImagePath(u"GI,Bm.FormOptions2.2SwitchD"_w);
        } else {
            pas::checked_cast<GI_Image::TImageGI*>(Sender)->SetImagePath(u"GI,Bm.FormOptions2.2SwitchN"_w);
        }
        reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserData)))->SetActive(Warning || Info->UnsupportedLanguage);
        {
            GI_Label::TLabelGI* cpp_with = reinterpret_cast<GI_Label::TLabelGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserState)));
            if ((Info->MissingFolder || Info->MissingDependency) && Selected) {
                cpp_with->SetTextColor(ModErrorColor);
            } else if (Warning) {
                cpp_with->SetTextColor(ModWarningColor);
            } else if (Selected) {
                cpp_with->SetTextColor(ModSelectedColor);
            } else {
                cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 234, 118));
            }
        }
    }

    void TfModsManager::ShowInfoClick(GI_MessageLoop::TObjectGI* Sender) {
        aModsInfo::TModInfo* Info{};
        pas::WideString Body{};
        pas::WideString Text{};
        Info = reinterpret_cast<aModsInfo::TModInfo*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        Body = aConst::LocalizedText(u"FormMods.InfoName"_wref.get());
        aMyFunction::ReplaceTextToken(Body, u"<Name>"_w, pas::concat_wide({u"<color=255,240,100>", Info->GetDisplayName(), u"</color>"}), pas::WideString());
        Body = pas::concat_wide({Body, u"\r\n", u" ", u"\r\n"});
        if (Info->Author == u"") {
            Text = aConst::LocalizedText(u"FormMods.InfoAuthorUnknown"_wref.get());
        } else if (EC_Str::CountDelimitedPartsW(Info->Author, u","_wref.get()) > 1) {
            Text = aConst::LocalizedText(u"FormMods.InfoAuthors"_wref.get());
        } else {
            Text = aConst::LocalizedText(u"FormMods.InfoAuthor"_wref.get());
        }
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, pas::concat_wide({u"<color=255,240,100>", Info->Author, u"</color>"}), pas::WideString());
        Body = pas::concat_wide({Body, Text, u"\r\n", u" ", u"\r\n"});
        if (Info->FullDescription == u"") {
            Body = pas::concat_wide({Body, aConst::LocalizedText(u"FormMods.NoDescription"_wref.get()), u"\r\n", u" ", u"\r\n"});
        } else {
            Body = pas::concat_wide({Body, Info->FullDescription, u"\r\n", u" ", u"\r\n"});
        }
        if (Info->DependencyNames != u"") {
            Text = aConst::LocalizedText(u"FormMods.InfoDependencies"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Mods>"_w, pas::concat_wide({u"<color=255,240,100>", Info->DependencyNames, u"</color>"}), pas::WideString());
            Body = pas::concat_wide({Body, Text, u"\r\n", u" ", u"\r\n"});
        }
        if (Info->ConflictNames != u"") {
            Text = aConst::LocalizedText(u"FormMods.InfoConflicts"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Mods>"_w, pas::concat_wide({u"<color=255,240,100>", Info->ConflictNames, u"</color>"}), pas::WideString());
            Body = pas::concat_wide({Body, Text, u"\r\n", u" ", u"\r\n"});
        }
        Text = aConst::LocalizedText(u"FormMods.InfoPath"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Path>"_w, pas::concat_wide({u"<color=255,240,100>", Info->Folder, u"</color>"}), pas::WideString());
        Body = pas::concat_wide({Body, Text});
        GI_MessageBox::ShowMessageBoxGI(this, Body, GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04 | GI_MessageBox::mbgLeftAlign, 0, 0, 0);
    }

    void TfModsManager::ShowProblemsClick(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t VariantIndex{};
        aModsInfo::TModInfo* Info{};
        aModsInfo::TModInfo* Related{};
        pas::WideString Body{};
        pas::WideString Value{};
        pas::WideString Text{};
        std::uint8_t Found{};
        std::uint32_t Options{};
        Info = reinterpret_cast<aModsInfo::TModInfo*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        std::uint8_t Selected = Info->SwitchImage->UserIndex == 1;
        std::uint8_t Critical = false;
        Body = aConst::LocalizedText(u"FormMods.ProblemsInfoHeader"_wref.get());
        aMyFunction::ReplaceTextToken(Body, u"<Name>"_w, pas::concat_wide({u"<color=255,240,100>", Info->GetDisplayName(), u"</color>"}), pas::WideString());
        Body = pas::concat_wide({Body, u"\r\n", u" ", u"\r\n"});
        if (Info->MissingFolder) {
            Critical = true;
            Body = pas::concat_wide({Body, aConst::LocalizedText(u"FormMods.ProblemsInfoMissing"_wref.get()), u"\r\n", u" ", u"\r\n"});
        } else {
            if (Info->UnsupportedLanguage) {
                Body = pas::concat_wide({Body, aConst::LocalizedText(u"FormMods.ProblemsInfoForeign"_wref.get()), u"\r\n", u" ", u"\r\n"});
            }
            if (Info->Misplaced) {
                Body = pas::concat_wide({Body, aConst::LocalizedText(u"FormMods.ProblemsInfoMisplaced"_wref.get()), u"\r\n", u" ", u"\r\n"});
            }
            if (Info->DuplicateName) {
                Value = pas::WideString();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aModsInfo::ModInfos) - 1); cpp_range.next(I); ) {
                    Related = pas::list_at<aModsInfo::TModInfo>(aModsInfo::ModInfos, I);
                    if (Related->DuplicateName && Related->Name == Info->Name && Related != Info) {
                        if (Value == u"") {
                            Value = Related->Folder;
                        } else {
                            Value = pas::concat_wide({Value, u", ", Related->Folder});
                        }
                    }
                }
                if (Info->ReferencedAsConflict || Info->ReferencedAsDependency) {
                    Text = aConst::LocalizedText(u"FormMods.ProblemsInfoSharedName"_wref.get());
                    Critical = true;
                } else {
                    Text = aConst::LocalizedText(u"FormMods.ProblemsInfoSharedName2"_wref.get());
                }
                aMyFunction::ReplaceTextToken(Text, u"<Mods>"_w, pas::concat_wide({u"<color=255,240,100>", Value, u"</color>"}), pas::WideString());
                Body = pas::concat_wide({Body, Text, u"\r\n", u" ", u"\r\n"});
            }
            if (Selected && Info->ConflictCount > 0) {
                Text = pas::WideString();
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Info->ConflictCount - 1); cpp_range_2.next(J); ) {
                    VariantIndex = 0;
                    Related = Info->Conflicts[J];
                    while (Related != nullptr) {
                        if (Related->SwitchImage->UserIndex == 1) {
                            if (Text == u"") {
                                Text = Related->Folder;
                            } else {
                                Text = pas::concat_wide({Text, u", ", Related->Folder});
                            }
                        }
                        if (!Related->DuplicateName) {
                            break;
                        }
                        ++VariantIndex;
                        Related = Info->GetConflict(J, VariantIndex);
                    }
                }
                if (Text != u"") {
                    Value = aConst::LocalizedText(u"FormMods.ProblemsInfoConflicts"_wref.get());
                    aMyFunction::ReplaceTextToken(Value, u"<Mods>"_w, pas::concat_wide({u"<color=255,240,100>", Text, u"</color>"}), pas::WideString());
                    Body = pas::concat_wide({Body, Value, u"\r\n", u" ", u"\r\n"});
                    Critical = true;
                }
            }
            if (Info->DependencyCount > 0) {
                Text = pas::WideString();
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Info->DependencyCount - 1); cpp_range_3.next(J); ) {
                    Related = Info->Dependencies[J];
                    if (Related == nullptr) {
                        Value = aConst::LocalizedText(u"FormMods.ProblemsInfoDependencies2"_wref.get());
                        aMyFunction::ReplaceTextToken(Value, u"<Mod>"_w, pas::concat_wide({u"<color=255,240,100>", EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Info->DependencyNames, J, u","_wref.get())), u"</color>"}), pas::WideString());
                        Body = pas::concat_wide({Body, Value, u"\r\n", u" ", u"\r\n"});
                        Critical = Selected;
                    } else if (Selected) {
                        Found = false;
                        VariantIndex = 0;
                        while (Related != nullptr) {
                            if (Related->SwitchImage->UserIndex == 1) {
                                Found = true;
                                break;
                            }
                            if (!Related->DuplicateName) {
                                break;
                            }
                            ++VariantIndex;
                            Related = Info->GetDependency(J, VariantIndex);
                        }
                        if (!Found) {
                            if (Text == u"") {
                                Text = Info->Dependencies[J]->Name;
                            } else {
                                Text = pas::concat_wide({Text, u", ", Info->Dependencies[J]->Name});
                            }
                        }
                    }
                }
                if (Text != u"") {
                    Value = aConst::LocalizedText(u"FormMods.ProblemsInfoDependencies"_wref.get());
                    aMyFunction::ReplaceTextToken(Value, u"<Mods>"_w, pas::concat_wide({u"<color=255,240,100>", Text, u"</color>"}), pas::WideString());
                    Body = pas::concat_wide({Body, Value, u"\r\n", u" ", u"\r\n"});
                    Critical = true;
                }
            }
        }
        if (Critical) {
            Options = 0x00000020u;
        } else {
            Options = 0x00000008u;
        }
        GI_MessageBox::ShowMessageBoxGI(this, Body, Options | (GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04), 0, 0, 0);
    }

    void TfModsManager::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

    void TfModsManager::p_destroy() {
        fMods::TfModsManager_Destroy(this);
    }

} // namespace fMods
