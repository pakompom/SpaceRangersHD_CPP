#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_RadioGroup {
    struct TRadioGroupGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRadioGroupGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TRadioGroupGI, GI_MessageLoop::TObjectGI, "TRadioGroupGI", 296)
        void p_destroy() override;
        // Empty implementation.
        void Clear() override;
        void SetConfigPath(const pas::WideString& Path) override;
        void SetSize(Types::TPoint Size) override;
        void RefreshItemImages();
        void ClearSelection();
        void SelectItem(pas::WideString Name);
        void ItemClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t MouseState, Types::TPoint Point);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        GI_MessageLoop::TObjectNotifyEventGI SelectionChangedCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_RadioGroup
