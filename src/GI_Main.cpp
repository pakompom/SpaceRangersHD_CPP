#include "types/BreakMessageGIException.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_AImage.hpp"
#include "units/GI_AlphaImage.hpp"
#include "units/GI_CheckBox.hpp"
#include "units/GI_Circle.hpp"
#include "units/GI_CountBar.hpp"
#include "units/GI_Door.hpp"
#include "units/GI_Edit.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GAIFile.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Grid.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_InfiniteImage.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Line.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MultiImage.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_PanelScrollBar.hpp"
#include "units/GI_Planet.hpp"
#include "units/GI_PlanetButton.hpp"
#include "units/GI_PolyLine.hpp"
#include "units/GI_RadioGroup.hpp"
#include "units/GI_RotateImage.hpp"
#include "units/GI_RotateImage2.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/GI_RotateImageGAI.hpp"
#include "units/GI_SBPath.hpp"
#include "units/GI_ScrollBar.hpp"
#include "units/GI_ShrLight.hpp"
#include "units/GI_SimpleButton.hpp"
#include "units/GI_SimpleImage.hpp"
#include "units/GI_SpaceCircle.hpp"
#include "units/GI_SpaceImg.hpp"
#include "units/GI_StarField.hpp"
#include "units/GI_StarFieldImg.hpp"
#include "units/GI_StarFieldM.hpp"
#include "units/GI_StatusBar.hpp"
#include "units/GI_TextButton.hpp"
#include "units/GI_TransImage.hpp"
#include "units/GI_Window.hpp"
#include "units/GI_XviD.hpp"
#include "units/GI_Zone.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_Main {
    void BreakUiMessage() {
        GR_Main::SuppressExceptionLogCopy = true;
        pas::raise(pas::make_exception<BreakMessageGIException::EBreakMessageGI>("No error"_a));
    }

    GI_MessageLoop::TObjectGI* CreateControlByName(pas::WideString Name, GI_MessageLoop::TObjectGI* Owner) {
        if (Name == u"Panel") {
            return pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Owner);
        } else if (Name == u"PanelScrollBar") {
            return pas::construct_call<GI_PanelScrollBar::TPanelScrollBarGI>(GI_PanelScrollBar::TPanelScrollBarGI_Create, Owner);
        } else if (Name == u"Window") {
            return pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, Owner);
        } else if (Name == u"SimpleImage") {
            return pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, Owner);
        } else if (Name == u"TransImage") {
            return pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, Owner);
        } else if (Name == u"AlphaImage") {
            return pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Owner);
        } else if (Name == u"RotateImage") {
            return pas::construct_call<GI_RotateImage::TRotateImageGI>(GI_RotateImage::TRotateImageGI_Create, Owner);
        } else if (Name == u"RotateImage2") {
            return pas::construct_call<GI_RotateImage2::TRotateImage2GI>(GI_RotateImage2::TRotateImage2GI_Create, Owner);
        } else if (Name == u"RotateImage5") {
            return pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, Owner);
        } else if (Name == u"RotateImageGAI") {
            return pas::construct_call<GI_RotateImageGAI::TRotateImageGaiGI>(GI_RotateImageGAI::TRotateImageGaiGI_Create, Owner);
        } else if (Name == u"Image") {
            return pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        } else if (Name == u"InfiniteImage") {
            return pas::construct_call<GI_InfiniteImage::TInfiniteImageGI>(GI_InfiniteImage::TInfiniteImageGI_Create, Owner);
        } else if (Name == u"AImage") {
            return pas::construct_call<GI_AImage::TAImageGI>(GI_AImage::TAImageGI_Create, Owner);
        } else if (Name == u"GI") {
            return pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, Owner);
        } else if (Name == u"GAI") {
            return pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        } else if (Name == u"GAIFile") {
            return pas::construct_call<GI_GAIFile::TGAIFileGI>(GI_GAIFile::TGAIFileGI_Create, Owner);
        } else if (Name == u"MultiImage") {
            return pas::construct_call<GI_MultiImage::TMultiImageGI>(GI_MultiImage::TMultiImageGI_Create, Owner);
        } else if (Name == u"Door") {
            return pas::construct_call<GI_Door::TDoorGI>(GI_Door::TDoorGI_Create, Owner);
        } else if (Name == u"SimpleButton") {
            return pas::construct_call<GI_SimpleButton::TSimpleButtonGI>(GI_SimpleButton::TSimpleButtonGI_Create, Owner);
        } else if (Name == u"TextButton") {
            return pas::construct_call<GI_TextButton::TTextButtonGI>(GI_TextButton::TTextButtonGI_Create, Owner);
        } else if (Name == u"GraphButton") {
            return pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Owner);
        } else if (Name == u"Zone") {
            return pas::construct_call<GI_Zone::TZoneGI>(GI_Zone::TZoneGI_Create, Owner);
        } else if (Name == u"Label") {
            return pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
        } else if (Name == u"Edit") {
            return pas::construct_call<GI_Edit::TEditGI>(GI_Edit::TEditGI_Create, Owner);
        } else if (Name == u"ScrollBar") {
            return pas::construct_call<GI_ScrollBar::TScrollBarGI>(GI_ScrollBar::TScrollBarGI_Create, Owner);
        } else if (Name == u"CountBar") {
            return pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, Owner);
        } else if (Name == u"SBPath") {
            return pas::construct_call<GI_SBPath::TSBPathGI>(GI_SBPath::TSBPathGI_Create, Owner);
        } else if (Name == u"StatusBar") {
            return pas::construct_call<GI_StatusBar::TStatusBarGI>(GI_StatusBar::TStatusBarGI_Create, Owner);
        } else if (Name == u"Planet") {
            return pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Owner);
        } else if (Name == u"PlanetButton") {
            return pas::construct_call<GI_PlanetButton::TPlanetButtonGI>(GI_PlanetButton::TPlanetButtonGI_Create, Owner);
        } else if (Name == u"CheckBox") {
            return pas::construct_call<GI_CheckBox::TCheckBoxGI>(GI_CheckBox::TCheckBoxGI_Create, Owner);
        } else if (Name == u"RadioGroup") {
            return pas::construct_call<GI_RadioGroup::TRadioGroupGI>(GI_RadioGroup::TRadioGroupGI_Create, Owner);
        } else if (Name == u"Grid") {
            return pas::construct_call<GI_Grid::TGridGI>(GI_Grid::TGridGI_Create, Owner);
        } else if (Name == u"Line") {
            return pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, Owner);
        } else if (Name == u"Circle") {
            return pas::construct_call<GI_Circle::TCircleGI>(GI_Circle::TCircleGI_Create, Owner);
        } else if (Name == u"Frame") {
            return pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, Owner);
        } else if (Name == u"ShrLight") {
            return pas::construct_call<GI_ShrLight::TShrLightGI>(GI_ShrLight::TShrLightGI_Create, Owner);
        } else if (Name == u"GraphBuf") {
            return pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
        } else if (Name == u"StarField") {
            return pas::construct_call<GI_StarField::TStarFieldGI>(GI_StarField::TStarFieldGI_Create, Owner);
        } else if (Name == u"StarFieldM") {
            return pas::construct_call<GI_StarFieldM::TStarFieldMGI>(GI_StarFieldM::TStarFieldMGI_Create, Owner);
        } else if (Name == u"StarFieldImg") {
            return pas::construct_call<GI_StarFieldImg::TStarFieldImgGI>(GI_StarFieldImg::TStarFieldImgGI_Create, Owner);
        } else if (Name == u"SpaceCircle") {
            return pas::construct_call<GI_SpaceCircle::TSpaceCircleGI>(GI_SpaceCircle::TSpaceCircleGI_Create, Owner);
        } else if (Name == u"SpaceImg") {
            return pas::construct_call<GI_SpaceImg::TSpaceImgGI>(GI_SpaceImg::TSpaceImgGI_Create, Owner);
        } else if (Name == u"PolyLine") {
            return pas::construct_call<GI_PolyLine::TPolyLineGI>(GI_PolyLine::TPolyLineGI_Create, Owner);
        } else if (Name == u"XviD") {
            return pas::construct_call<GI_XviD::TxvidGI>(GI_XviD::TxvidGI_Create, Owner);
        } else {
            return nullptr;
        }
    }

    TImageKindXGI ParseImageKindXName(pas::WideString Name) {
        if (Name == u"LeftFill") {
            return ikxLeftFill;
        } else if (Name == u"CenterFill") {
            return ikxCenterFill;
        } else if (Name == u"RightFill") {
            return ikxRightFill;
        } else if (Name == u"Left") {
            return ikxLeft;
        } else if (Name == u"Center") {
            return ikxCenter;
        } else if (Name == u"Right") {
            return ikxRight;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetITDXbyNameGI. name=", Name}))));
        }
    }

    TImageKindYGI ParseImageKindYName(pas::WideString Name) {
        if (Name == u"TopFill") {
            return ikyTopFill;
        } else if (Name == u"CenterFill") {
            return ikyCenterFill;
        } else if (Name == u"BottomFill") {
            return ikyBottomFill;
        } else if (Name == u"Top") {
            return ikyTop;
        } else if (Name == u"Center") {
            return ikyCenter;
        } else if (Name == u"Bottom") {
            return ikyBottom;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetITDYbyNameGI. name=", Name}))));
        }
    }

    TTextAlignXGI ParseTextAlignXName(pas::WideString Name) {
        if (Name == u"Left") {
            return taxLeft;
        } else if (Name == u"Center") {
            return taxCenter;
        } else if (Name == u"Right") {
            return taxRight;
        } else if (Name == u"Auto") {
            return taxAuto;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetTTAXbyNameGI. name=", Name}))));
        }
    }

    TTextAlignYGI ParseTextAlignYName(pas::WideString Name) {
        if (Name == u"Top") {
            return tayTop;
        } else if (Name == u"Center") {
            return tayCenter;
        } else if (Name == u"CenterEx") {
            return tayCenterEx;
        } else if (Name == u"Bottom") {
            return tayBottom;
        } else if (Name == u"Auto") {
            return tayAuto;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetTTAYbyNameGI. name=", Name}))));
        }
    }

    std::uint8_t ParseEnabledNameGI(pas::WideString Name) {
        return Name == u"Yes" || Name == u"yes" || Name == u"True" || Name == u"true" || Name == u"TRUE" || Name == u"1";
    }

    std::uint32_t GetColorGI(pas::WideString ColorText) {
        if (EC_Str::CountDelimitedPartsW(ColorText, u","_wref.get()) < 3) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetColorGI. color=", ColorText}))));
        }
        std::uint8_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 2, u","_wref.get())));
        std::uint8_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 1, u","_wref.get())));
        std::uint8_t strToInt_3 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 0, u","_wref.get())));
        return GR_Main::CurrentPixelFormat->PackRgbBytes(strToInt_3, strToInt_2, strToInt);
    }

    Types::TPoint GetPointGI(pas::WideString PointText) {
        Types::TPoint Result{};
        if (EC_Str::CountDelimitedPartsW(PointText, u","_wref.get()) < 2) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetPointGI. tstr=", PointText}))));
        }
        Result = ([&] {
            std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 1, u","_wref.get())));
            std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 0, u","_wref.get())));
            return ClassesImports::Point(strToInt_2, strToInt);
        }());
        return Result;
    }

    std::int32_t ParseAutoGeometryFlagsGI(pas::WideString Values) {
        std::int32_t Index{};
        pas::WideString Part{};
        std::int32_t Flags = 0;
        std::int32_t Count = EC_Str::CountDelimitedPartsW(Values, u","_wref.get());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Part = EC_Str::LowerCaseWideString(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Values, Index, u","_wref.get())));
            if (Part == u"pos") {
                Flags |= agfPosition;
            } else if (Part == u"size") {
                Flags |= agfSize;
            }
        }
        return Flags;
    }

    EC_Struct::TPointF GetFloatPointGI(pas::WideString PointText) {
        EC_Struct::TPointF Result{};
        if (EC_Str::CountDelimitedPartsW(PointText, u","_wref.get()) < 2) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetFloatPointGI. tstr=", PointText}))));
        }
        Result = ([&] {
            float extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(PointText, 0, u","_wref.get()));
            float extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(PointText, 1, u","_wref.get()));
            return EC_Struct::MakePointF(extractDecimalToSingleW, extractDecimalToSingleW_2);
        }());
        return Result;
    }

    Types::TRect GetRectGI(pas::WideString RectText) {
        Types::TRect Result{};
        if (EC_Str::CountDelimitedPartsW(RectText, u","_wref.get()) < 4) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetRectGI. tstr=", RectText}))));
        }
        Result.Left = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 0, u","_wref.get())));
        Result.Top = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 1, u","_wref.get())));
        Result.Right = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 2, u","_wref.get())));
        Result.Bottom = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 3, u","_wref.get())));
        return Result;
    }

} // namespace GI_Main
