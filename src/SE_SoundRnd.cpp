#include "layout/SE_SoundRnd.hpp"
#include "types/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SE_SoundRnd.hpp"
#include "units/aMyFunction.hpp"

// Native class metadata and dynamic-array RTTI identify SE_SoundRnd.
namespace SE_SoundRnd {
    SE_SoundRnd::TSoundRndSE* FirstRandomSound = nullptr;

    SE_SoundRnd::TSoundRndSE* LastRandomSound = nullptr;

    TSoundRndSE* CreateRandomSound() {
        TSoundRndSE* Sound = pas::construct_call<TSoundRndSE>(TSoundRndSE_Create);
        if (LastRandomSound != nullptr) {
            LastRandomSound->Next = Sound;
        }
        Sound->Prev = LastRandomSound;
        Sound->Next = nullptr;
        LastRandomSound = Sound;
        if (FirstRandomSound == nullptr) {
            FirstRandomSound = Sound;
        }
        return Sound;
    }

    // Unlinks and frees a nonnil registered sound.
    void FreeRandomSound(TSoundRndSE* Sound) {
        if (Sound->Prev != nullptr) {
            Sound->Prev->Next = Sound->Next;
        }
        if (Sound->Next != nullptr) {
            Sound->Next->Prev = Sound->Prev;
        }
        if (LastRandomSound == Sound) {
            LastRandomSound = Sound->Prev;
        }
        if (FirstRandomSound == Sound) {
            FirstRandomSound = Sound->Next;
        }
        pas::free(Sound);
    }

    void FreeAllRandomSounds() {
        while (!(FirstRandomSound == nullptr)) {
            SE_SoundRnd::FreeRandomSound(LastRandomSound);
        }
    }

    // Creates and loads an uncached name, then selects a weighted group; -1 for zero total weight.
    TSoundRndSE* FindRandomSound(pas::WideString Name, std::int32_t& GroupIndex) {
        std::int32_t Index{};
        TSoundRndSE* Sound = FirstRandomSound;
        while (Sound != nullptr) {
            if (Sound->Name == Name) {
                break;
            }
            Sound = Sound->Next;
        }
        if (Sound == nullptr) {
            Sound = SE_SoundRnd::CreateRandomSound();
            Sound->Name = Name;
            Sound->LoadFromBlock(GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.Sound.Rnd.", Name})));
        }
        if (Sound->TotalGroupWeight < 1) {
            GroupIndex = -1;
        } else {
            GroupIndex = aMyFunction::RandomIntRange(0, Sound->TotalGroupWeight - 1);
            Index = 0;
            while (true) {
                GroupIndex -= Sound->Groups[Index].Weight;
                if (GroupIndex < 0) {
                    GroupIndex = Index;
                    break;
                }
                ++Index;
            }
        }
        return Sound;
    }

    void TSoundRndSE_Create(TSoundRndSE* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TSoundRndSE_Destroy(TSoundRndSE* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TSoundRndSE::Clear() {
        std::int32_t Index{};
        {
            const std::int32_t cpp_last = Groups.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    Groups[Index].SoundWeights = nullptr;
                    Groups[Index].SoundNames = nullptr;
                }
            }
        }
        Groups = nullptr;
    }

    void TSoundRndSE::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Index{};
        std::int32_t ParamIndex{};
        std::int32_t ParamCount{};
        std::int32_t SoundIndex{};
        std::int32_t SoundCount{};
        EC_BlockPar::TBlockParEC* GroupBlock{};
        pas::WideString Text{};
        TotalGroupWeight = 0;
        std::int32_t Count = Block->GetBlockCount();
        Groups.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            GroupBlock = Block->GetBlockByIndex(Index);
            Groups[Index].Weight = EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index));
            TotalGroupWeight += Groups[Index].Weight;
            Groups[Index].Group = EC_Str::ExtractDigitsToIntW(GroupBlock->GetParam(u"Group"_wref.get()));
            Text = GroupBlock->GetParam(u"NextTime"_wref.get());
            Groups[Index].NextTimeMin = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 0, u"-"_wref.get()));
            Groups[Index].NextTimeMax = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 1, u"-"_wref.get()));
            SoundCount = 0;
            ParamCount = GroupBlock->GetParamCount();
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ParamCount - 1); cpp_range_2.next(ParamIndex); ) {
                if (EC_Str::IsIntegerTextW(GroupBlock->GetParamName(ParamIndex))) {
                    ++SoundCount;
                }
            }
            Groups[Index].TotalSoundWeight = 0;
            Groups[Index].SoundNames.set_length(SoundCount);
            Groups[Index].SoundWeights.set_length(SoundCount);
            SoundIndex = 0;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ParamCount - 1); cpp_range_3.next(ParamIndex); ) {
                Text = GroupBlock->GetParamName(ParamIndex);
                if (EC_Str::IsIntegerTextW(Text)) {
                    Groups[Index].SoundWeights[SoundIndex] = EC_Str::ExtractDigitsToIntW(Text);
                    Groups[Index].SoundNames[SoundIndex] = GroupBlock->GetParamValue(ParamIndex);
                    Groups[Index].TotalSoundWeight += Groups[Index].SoundWeights[SoundIndex];
                    ++SoundIndex;
                }
            }
        }
    }

    pas::WideString TSoundRndSE::SelectSound(std::int32_t GroupIndex) {
        std::int32_t Index{};
        std::int32_t Weight{};
        if (Groups[GroupIndex].TotalSoundWeight >= 1) {
            Weight = aMyFunction::RandomIntRange(0, Groups[GroupIndex].TotalSoundWeight - 1);
            {
                const std::int32_t cpp_last = Groups[GroupIndex].SoundNames.length() - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        Weight -= Groups[GroupIndex].SoundWeights[Index];
                        if (Weight < 0) {
                            return Groups[GroupIndex].SoundNames[Index];
                        }
                    }
                }
            }
        }
        return pas::WideString();
    }

    void TSoundRndSE::p_destroy() {
        SE_SoundRnd::TSoundRndSE_Destroy(this);
    }

} // namespace SE_SoundRnd
