#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace fChameleon {
    struct TfChameleon;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfChameleon : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfChameleon, GI_MessageLoop::TMessageLoopGI, "TfChameleon", 244)
        void OnOpen() override;
        void AddChoice(std::int32_t Index, std::int32_t X, std::int32_t Y, pas::WideString Text, std::uint8_t Selected, std::uint8_t Disabled);
        void ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        static void ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        static void ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void MoveChoice(std::int32_t Delta);
        void AcceptClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessCallbackTimers() override;
        std::uint8_t ChameleonActive;
        std::uint8_t VisualType;
        std::uint8_t cpp_padding[2];
        pas::Array<std::int32_t, 0, 2> Charges;
        // One-based: disable, Blazer, Keller, Terron.
        std::int32_t Choice;
        // Disabled choices have nil entries.
        pas::Array<GI_Image::TImageGI*, 1, 4> ChoiceImages;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fChameleon
