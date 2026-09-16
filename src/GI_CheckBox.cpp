#include "layout/GI_CheckBox.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_CheckBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_TransImage.hpp"
#include "units/GR_Main.hpp"

namespace GI_CheckBox {
    void TCheckBoxGI_Create(TCheckBoxGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->CheckedImage = pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, Self);
        Self->CheckedImage->SetImageKindX(GI_Main::ikxCenter);
        Self->CheckedImage->SetImageKindY(GI_Main::ikyCenter);
        Self->CheckedImage->SetActive(false);
        Self->UncheckedImage = pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, Self);
        Self->UncheckedImage->SetImageKindX(GI_Main::ikxCenter);
        Self->UncheckedImage->SetImageKindY(GI_Main::ikyCenter);
        Self->UncheckedImage->SetActive(true);
        Self->Checked = false;
    }

    void TCheckBoxGI_Destroy(TCheckBoxGI* Self) {
        pas::free(Self->CheckedImage);
        pas::free(Self->UncheckedImage);
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TCheckBoxGI::Clear() {
        Checked = false;
    }

    void TCheckBoxGI::SetConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::SetConfigPath(Path);
        RefreshStateImages();
        Invalidate();
    }

    void TCheckBoxGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        CheckedImage->SetPosition(ClassesImports::Point(Size.X / 2 - CheckedImage->ClientSize.X / 2, Size.Y / 2 - CheckedImage->ClientSize.Y / 2));
        UncheckedImage->SetPosition(ClassesImports::Point(Size.X / 2 - UncheckedImage->ClientSize.X / 2, Size.Y / 2 - UncheckedImage->ClientSize.Y / 2));
    }

    void TCheckBoxGI::RefreshStateImages() {
        CheckedImage->SetImagePath(pas::concat_wide({ConfigPath, u".IChecked"}));
        {
            WindowsSdk::TPoint contentSize = CheckedImage->GetContentSize();
            GI_MessageLoop::TObjectGI* checkedImage = CheckedImage;
            checkedImage->SetSize(contentSize);
        }
        UncheckedImage->SetImagePath(pas::concat_wide({ConfigPath, u".IUnchecked"}));
        {
            WindowsSdk::TPoint contentSize_2 = UncheckedImage->GetContentSize();
            GI_MessageLoop::TObjectGI* uncheckedImage = UncheckedImage;
            uncheckedImage->SetSize(contentSize_2);
        }
        CheckedImage->SetPosition(ClassesImports::Point(ClientSize.X / 2 - CheckedImage->ClientSize.X / 2, ClientSize.Y / 2 - CheckedImage->ClientSize.Y / 2));
        UncheckedImage->SetPosition(ClassesImports::Point(ClientSize.X / 2 - UncheckedImage->ClientSize.X / 2, ClientSize.Y / 2 - UncheckedImage->ClientSize.Y / 2));
    }

    void TCheckBoxGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (Checked == true) {
            Checked = false;
            CheckedImage->SetActive(false);
            UncheckedImage->SetActive(true);
        } else {
            Checked = true;
            CheckedImage->SetActive(true);
            UncheckedImage->SetActive(false);
        }
        if (pas::assigned(ChangedCallback)) {
            ChangedCallback(this);
        }
    }

    void TCheckBoxGI::LoadFromConfigPath(const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Checked"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"Checked"_wref.get())) == u"True") {
                Checked = true;
            } else {
                Checked = false;
            }
        }
    }

    void TCheckBoxGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        if (Block->CountParams(u"Checked"_wref.get()) > 0) {
            if (EC_Str::TrimWideString(Block->GetParam(u"Checked"_wref.get())) == u"True") {
                Checked = true;
            } else {
                Checked = false;
            }
        }
        RefreshStateImages();
    }

    void TCheckBoxGI::p_destroy() {
        GI_CheckBox::TCheckBoxGI_Destroy(this);
    }

} // namespace GI_CheckBox
