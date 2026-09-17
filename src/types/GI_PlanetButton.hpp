#pragma once
#include "runtime_support.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Planet {
    struct TPlanetGI;

} // namespace GI_Planet

namespace GI_PlanetButton {
    struct TPlanetButtonGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanetButtonGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TPlanetButtonGI, GI_Panel::TPanelGI, "TPlanetButtonGI", 332)
        void p_destroy() override;
        // The native implementation is empty; it does not reset panel or child state.
        void Clear() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        std::uint8_t cpp_padding[4];
        GI_Planet::TPlanetGI* NormalPlanet;
        GI_Planet::TPlanetGI* HoverPlanet;
        GI_Label::TLabelGI* TextLabel;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_PlanetButton
