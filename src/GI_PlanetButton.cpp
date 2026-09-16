#include "layout/GI_PlanetButton.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_Planet.hpp"
#include "units/GI_PlanetButton.hpp"

namespace GI_PlanetButton {
    void TPlanetButtonGI_Create(TPlanetButtonGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->NormalPlanet = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Self);
        Self->NormalPlanet->SetDepth(2.0);
        Self->HoverPlanet = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Self);
        Self->HoverPlanet->SetDepth(2.0);
        Self->HoverPlanet->SetActive(false);
        Self->TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Self);
        Self->TextLabel->SetDepth(1.0);
    }

    void TPlanetButtonGI_Destroy(TPlanetButtonGI* Self) {
        pas::free(Self->NormalPlanet);
        pas::free(Self->HoverPlanet);
        pas::free(Self->TextLabel);
        GI_Panel::TPanelGI_Destroy(Self);
    }

    // The native implementation is empty; it does not reset panel or child state.
    void TPlanetButtonGI::Clear() {
    }

    void TPlanetButtonGI::OnMouseEnter() {
        GI_Panel::TPanelGI::OnMouseEnter();
        NormalPlanet->SetActive(false);
        HoverPlanet->SetActive(true);
    }

    void TPlanetButtonGI::OnMouseLeave() {
        GI_Panel::TPanelGI::OnMouseLeave();
        NormalPlanet->SetActive(true);
        HoverPlanet->SetActive(false);
    }

    void TPlanetButtonGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        DispatchNamedEvent(1, Point.X, Point.Y);
    }

    void TPlanetButtonGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_Panel::TPanelGI::LoadFromConfigPath(Path);
    }

    void TPlanetButtonGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        TextLabel->SetActive(false);
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        {
            const pas::WideString& param = Block->GetParam(u"PN_ImageLight"_wref.get());
            const pas::WideString& param_2 = Block->GetParam(u"PN_Image"_wref.get());
            const pas::WideString& param_3 = Block->GetParam(u"PN_Mask"_wref.get());
            NormalPlanet->SetImage(param_3, param_2, param);
        }
        SetSize(NormalPlanet->ClientSize);
        {
            const pas::WideString& param_4 = Block->GetParam(u"PA_ImageLight"_wref.get());
            const pas::WideString& param_5 = Block->GetParam(u"PA_Image"_wref.get());
            const pas::WideString& param_6 = Block->GetParam(u"PN_Mask"_wref.get());
            HoverPlanet->SetImage(param_6, param_5, param_4);
        }
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            TextLabel->SetFontName(Block->GetParam(u"Font"_wref.get()));
        }
        if (Block->CountParams(u"Text"_wref.get()) > 0) {
            TextLabel->SetText(Block->GetParam(u"Text"_wref.get()));
            TextLabel->SetActive(true);
            TextLabel->SetSize(ClientSize);
        }
    }

    void TPlanetButtonGI::p_destroy() {
        GI_PlanetButton::TPlanetButtonGI_Destroy(this);
    }

} // namespace GI_PlanetButton
