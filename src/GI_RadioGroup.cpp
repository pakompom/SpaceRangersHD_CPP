#include "layout/GI_RadioGroup.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_RadioGroup.hpp"
#include "units/GI_TransImage.hpp"
#include "units/SysUtils.hpp"

namespace GI_RadioGroup {
    void TRadioGroupGI_Create(TRadioGroupGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

    void TRadioGroupGI_Destroy(TRadioGroupGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TRadioGroupGI::Clear() {
    }

    void TRadioGroupGI::SetConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::SetConfigPath(Path);
        RefreshItemImages();
        Invalidate();
    }

    void TRadioGroupGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
    }

    void TRadioGroupGI::AddItem(pas::WideString Name, Types::TPoint Position) {
        GI_TransImage::TTransImageGI* Image = pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, this);
        Image->SetPosition(Position);
        Image->SetName(Name);
        Image->UserValue = 0;
        Image->SetActive(true);
        Image->LeftButtonDownCallback = pas::bind_method<&TRadioGroupGI::ItemClick>(this);
        Image = pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, this);
        Image->SetPosition(Position);
        Image->SetName(Name);
        Image->UserValue = 1;
        Image->SetActive(false);
        Image->LeftButtonDownCallback = pas::bind_method<&TRadioGroupGI::ItemClick>(this);
        RefreshItemImages();
    }

    void TRadioGroupGI::RefreshItemImages() {
        GI_MessageLoop::TObjectGI* Item = FirstChild;
        while (Item != nullptr) {
            if (Item->UserValue == 0) {
                GI_TransImage::TTransImageGI* cpp_arg = pas::checked_cast<GI_TransImage::TTransImageGI*>(Item);
                const pas::WideString& cpp_arg_2 = pas::concat_wide({ConfigPath, u".IUnchecked"});
                cpp_arg->SetImagePath(cpp_arg_2);
            } else {
                GI_TransImage::TTransImageGI* cpp_arg_3 = pas::checked_cast<GI_TransImage::TTransImageGI*>(Item);
                const pas::WideString& cpp_arg_4 = pas::concat_wide({ConfigPath, u".IChecked"});
                cpp_arg_3->SetImagePath(cpp_arg_4);
            }
            Item = Item->NextSibling;
        }
    }

    void TRadioGroupGI::ClearSelection() {
        GI_MessageLoop::TObjectGI* Item = FirstChild;
        while (Item != nullptr) {
            if (Item->UserValue == 0) {
                Item->SetActive(true);
            } else {
                Item->SetActive(false);
            }
            Item = Item->NextSibling;
        }
    }

    void TRadioGroupGI::SelectItem(pas::WideString Name) {
        ClearSelection();
        GI_MessageLoop::TObjectGI* Item = FirstChild;
        while (Item != nullptr) {
            if (Item->ControlName == Name) {
                if (Item->UserValue == 0) {
                    Item->SetActive(false);
                } else {
                    Item->SetActive(true);
                }
            } else if (Item->UserValue == 0) {
                Item->SetActive(true);
            } else {
                Item->SetActive(false);
            }
            Item = Item->NextSibling;
        }
    }

    void TRadioGroupGI::ItemClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t MouseState, Types::TPoint Point) {
        SelectItem(Sender->ControlName);
        if (pas::assigned(SelectionChangedCallback)) {
            SelectionChangedCallback(this);
        }
    }

    void TRadioGroupGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
    }

    void TRadioGroupGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        EC_BlockPar::TBlockParEC* Items{};
        std::int32_t Index{};
        std::int32_t Count{};
        pas::WideString Text{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        if (Block->CountBlocks(u"RadioButton"_wref.get()) > 0) {
            Items = Block->GetBlock(u"RadioButton"_wref.get());
            Count = Items->GetParamCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                Text = Items->GetParamValue(Index);
                {
                    Types::TPoint point = ([&] {
                        std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
                        std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
                        return ClassesImports::Point(strToInt_2, strToInt);
                    }());
                    pas::WideString paramName = Items->GetParamName(Index);
                    AddItem(std::move(paramName), point);
                }
            }
        }
        if (Block->CountParams(u"Checked"_wref.get()) > 0) {
            SelectItem(EC_Str::TrimWideString(Block->GetParam(u"Checked"_wref.get())));
        }
    }

    void TRadioGroupGI::p_destroy() {
        GI_RadioGroup::TRadioGroupGI_Destroy(this);
    }

} // namespace GI_RadioGroup
