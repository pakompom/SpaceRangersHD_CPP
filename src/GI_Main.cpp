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

// Configuration helper placement in GI_Main is inferred; no explicit unit RTTI was recovered here.
namespace GI_Main {
    // Sets the shared exception-log-copy suppression flag and raises EBreakMessageGI. Placement in GI_Main is inferred.
    void BreakUiMessage() {
        GR_Main::SuppressExceptionLogCopy = true;
        pas::raise(pas::make_exception<BreakMessageGIException::EBreakMessageGI>("No error"_a));
    }

    // Exact type-name lookup; returns nil for unknown names. Placement in GI_Main is inferred from its configuration-helper region.
    GI_MessageLoop::TObjectGI* CreateControlByName(const std::u16string_view& Name, GI_MessageLoop::TObjectGI* Owner) {
        if (Name == u"Panel"sv) {
            return pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Owner);
        } else if (Name == u"PanelScrollBar"sv) {
            return pas::construct_call<GI_PanelScrollBar::TPanelScrollBarGI>(GI_PanelScrollBar::TPanelScrollBarGI_Create, Owner);
        } else if (Name == u"Window"sv) {
            return pas::construct_call<GI_Window::TWindowGI>(GI_Window::TWindowGI_Create, Owner);
        } else if (Name == u"SimpleImage"sv) {
            return pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, Owner);
        } else if (Name == u"TransImage"sv) {
            return pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, Owner);
        } else if (Name == u"AlphaImage"sv) {
            return pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Owner);
        } else if (Name == u"RotateImage"sv) {
            return pas::construct_call<GI_RotateImage::TRotateImageGI>(GI_RotateImage::TRotateImageGI_Create, Owner);
        } else if (Name == u"RotateImage2"sv) {
            return pas::construct_call<GI_RotateImage2::TRotateImage2GI>(GI_RotateImage2::TRotateImage2GI_Create, Owner);
        } else if (Name == u"RotateImage5"sv) {
            return pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, Owner);
        } else if (Name == u"RotateImageGAI"sv) {
            return pas::construct_call<GI_RotateImageGAI::TRotateImageGaiGI>(GI_RotateImageGAI::TRotateImageGaiGI_Create, Owner);
        } else if (Name == u"Image"sv) {
            return pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        } else if (Name == u"InfiniteImage"sv) {
            return pas::construct_call<GI_InfiniteImage::TInfiniteImageGI>(GI_InfiniteImage::TInfiniteImageGI_Create, Owner);
        } else if (Name == u"AImage"sv) {
            return pas::construct_call<GI_AImage::TAImageGI>(GI_AImage::TAImageGI_Create, Owner);
        } else if (Name == u"GI"sv) {
            return pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, Owner);
        } else if (Name == u"GAI"sv) {
            return pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        } else if (Name == u"GAIFile"sv) {
            return pas::construct_call<GI_GAIFile::TGAIFileGI>(GI_GAIFile::TGAIFileGI_Create, Owner);
        } else if (Name == u"MultiImage"sv) {
            return pas::construct_call<GI_MultiImage::TMultiImageGI>(GI_MultiImage::TMultiImageGI_Create, Owner);
        } else if (Name == u"Door"sv) {
            return pas::construct_call<GI_Door::TDoorGI>(GI_Door::TDoorGI_Create, Owner);
        } else if (Name == u"SimpleButton"sv) {
            return pas::construct_call<GI_SimpleButton::TSimpleButtonGI>(GI_SimpleButton::TSimpleButtonGI_Create, Owner);
        } else if (Name == u"TextButton"sv) {
            return pas::construct_call<GI_TextButton::TTextButtonGI>(GI_TextButton::TTextButtonGI_Create, Owner);
        } else if (Name == u"GraphButton"sv) {
            return pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Owner);
        } else if (Name == u"Zone"sv) {
            return pas::construct_call<GI_Zone::TZoneGI>(GI_Zone::TZoneGI_Create, Owner);
        } else if (Name == u"Label"sv) {
            return pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
        } else if (Name == u"Edit"sv) {
            return pas::construct_call<GI_Edit::TEditGI>(GI_Edit::TEditGI_Create, Owner);
        } else if (Name == u"ScrollBar"sv) {
            return pas::construct_call<GI_ScrollBar::TScrollBarGI>(GI_ScrollBar::TScrollBarGI_Create, Owner);
        } else if (Name == u"CountBar"sv) {
            return pas::construct_call<GI_CountBar::TCountBarGI>(GI_CountBar::TCountBarGI_Create, Owner);
        } else if (Name == u"SBPath"sv) {
            return pas::construct_call<GI_SBPath::TSBPathGI>(GI_SBPath::TSBPathGI_Create, Owner);
        } else if (Name == u"StatusBar"sv) {
            return pas::construct_call<GI_StatusBar::TStatusBarGI>(GI_StatusBar::TStatusBarGI_Create, Owner);
        } else if (Name == u"Planet"sv) {
            return pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Owner);
        } else if (Name == u"PlanetButton"sv) {
            return pas::construct_call<GI_PlanetButton::TPlanetButtonGI>(GI_PlanetButton::TPlanetButtonGI_Create, Owner);
        } else if (Name == u"CheckBox"sv) {
            return pas::construct_call<GI_CheckBox::TCheckBoxGI>(GI_CheckBox::TCheckBoxGI_Create, Owner);
        } else if (Name == u"RadioGroup"sv) {
            return pas::construct_call<GI_RadioGroup::TRadioGroupGI>(GI_RadioGroup::TRadioGroupGI_Create, Owner);
        } else if (Name == u"Grid"sv) {
            return pas::construct_call<GI_Grid::TGridGI>(GI_Grid::TGridGI_Create, Owner);
        } else if (Name == u"Line"sv) {
            return pas::construct_call<GI_Line::TLineGI>(GI_Line::TLineGI_Create, Owner);
        } else if (Name == u"Circle"sv) {
            return pas::construct_call<GI_Circle::TCircleGI>(GI_Circle::TCircleGI_Create, Owner);
        } else if (Name == u"Frame"sv) {
            return pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, Owner);
        } else if (Name == u"ShrLight"sv) {
            return pas::construct_call<GI_ShrLight::TShrLightGI>(GI_ShrLight::TShrLightGI_Create, Owner);
        } else if (Name == u"GraphBuf"sv) {
            return pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
        } else if (Name == u"StarField"sv) {
            return pas::construct_call<GI_StarField::TStarFieldGI>(GI_StarField::TStarFieldGI_Create, Owner);
        } else if (Name == u"StarFieldM"sv) {
            return pas::construct_call<GI_StarFieldM::TStarFieldMGI>(GI_StarFieldM::TStarFieldMGI_Create, Owner);
        } else if (Name == u"StarFieldImg"sv) {
            return pas::construct_call<GI_StarFieldImg::TStarFieldImgGI>(GI_StarFieldImg::TStarFieldImgGI_Create, Owner);
        } else if (Name == u"SpaceCircle"sv) {
            return pas::construct_call<GI_SpaceCircle::TSpaceCircleGI>(GI_SpaceCircle::TSpaceCircleGI_Create, Owner);
        } else if (Name == u"SpaceImg"sv) {
            return pas::construct_call<GI_SpaceImg::TSpaceImgGI>(GI_SpaceImg::TSpaceImgGI_Create, Owner);
        } else if (Name == u"PolyLine"sv) {
            return pas::construct_call<GI_PolyLine::TPolyLineGI>(GI_PolyLine::TPolyLineGI_Create, Owner);
        } else if (Name == u"XviD"sv) {
            return pas::construct_call<GI_XviD::TxvidGI>(GI_XviD::TxvidGI_Create, Owner);
        } else {
            return nullptr;
        }
    }

    // Exact spelling required; unknown names raise.
    TImageKindXGI ParseImageKindXName(const std::u16string_view& Name) {
        if (Name == u"LeftFill"sv) {
            return ikxLeftFill;
        } else if (Name == u"CenterFill"sv) {
            return ikxCenterFill;
        } else if (Name == u"RightFill"sv) {
            return ikxRightFill;
        } else if (Name == u"Left"sv) {
            return ikxLeft;
        } else if (Name == u"Center"sv) {
            return ikxCenter;
        } else if (Name == u"Right"sv) {
            return ikxRight;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetITDXbyNameGI. name=", Name}))));
        }
    }

    // Exact spelling required; unknown names raise.
    TImageKindYGI ParseImageKindYName(const std::u16string_view& Name) {
        if (Name == u"TopFill"sv) {
            return ikyTopFill;
        } else if (Name == u"CenterFill"sv) {
            return ikyCenterFill;
        } else if (Name == u"BottomFill"sv) {
            return ikyBottomFill;
        } else if (Name == u"Top"sv) {
            return ikyTop;
        } else if (Name == u"Center"sv) {
            return ikyCenter;
        } else if (Name == u"Bottom"sv) {
            return ikyBottom;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetITDYbyNameGI. name=", Name}))));
        }
    }

    TTextAlignXGI ParseTextAlignXName(const std::u16string_view& Name) {
        if (Name == u"Left"sv) {
            return taxLeft;
        } else if (Name == u"Center"sv) {
            return taxCenter;
        } else if (Name == u"Right"sv) {
            return taxRight;
        } else if (Name == u"Auto"sv) {
            return taxAuto;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetTTAXbyNameGI. name=", Name}))));
        }
    }

    TTextAlignYGI ParseTextAlignYName(const std::u16string_view& Name) {
        if (Name == u"Top"sv) {
            return tayTop;
        } else if (Name == u"Center"sv) {
            return tayCenter;
        } else if (Name == u"CenterEx"sv) {
            return tayCenterEx;
        } else if (Name == u"Bottom"sv) {
            return tayBottom;
        } else if (Name == u"Auto"sv) {
            return tayAuto;
        } else {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetTTAYbyNameGI. name=", Name}))));
        }
    }

    // True only for Yes, yes, True, true, TRUE or 1.
    std::uint8_t ParseEnabledNameGI(const std::u16string_view& Name) {
        return Name == u"Yes"sv || Name == u"yes"sv || Name == u"True"sv || Name == u"true"sv || Name == u"TRUE"sv || Name == u"1"sv;
    }

    // At least three comma-separated components are required; only their low bytes are used.
    std::uint32_t GetColorGI(const std::u16string_view& ColorText) {
        if (EC_Str::CountDelimitedPartsW(ColorText, u","sv) < 3) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetColorGI. color=", ColorText}))));
        }
        std::uint8_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 2, u","sv)));
        std::uint8_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 1, u","sv)));
        std::uint8_t strToInt_3 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(ColorText, 0, u","sv)));
        return GR_Main::CurrentPixelFormat->PackRgbBytes(strToInt_3, strToInt_2, strToInt);
    }

    Types::TPoint GetPointGI(const std::u16string_view& PointText) {
        Types::TPoint Result{};
        if (EC_Str::CountDelimitedPartsW(PointText, u","sv) < 2) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetPointGI. tstr=", PointText}))));
        }
        Result = ([&] {
            std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 1, u","sv)));
            std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 0, u","sv)));
            return ClassesImports::Point(strToInt_2, strToInt);
        }());
        return Result;
    }

    // Comma-separated pos and size names, trimmed and case-insensitive; unknown names are ignored. Placement in GI_Main is inferred.
    std::int32_t ParseAutoGeometryFlagsGI(pas::WideString Values) {
        std::int32_t Index{};
        pas::WideString Part{};
        std::int32_t Flags = 0;
        std::int32_t Count = EC_Str::CountDelimitedPartsW(pas::view(Values), u","sv);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Part = EC_Str::LowerCaseWideString(EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Values), Index, u","sv)));
            if (Part == u"pos") {
                Flags |= agfPosition;
            } else if (Part == u"size") {
                Flags |= agfSize;
            }
        }
        return Flags;
    }

    EC_Struct::TPointF GetFloatPointGI(const std::u16string_view& PointText) {
        EC_Struct::TPointF Result{};
        if (EC_Str::CountDelimitedPartsW(PointText, u","sv) < 2) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetFloatPointGI. tstr=", PointText}))));
        }
        Result = ([&] {
            float extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(PointText, 0, u","sv));
            float extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(PointText, 1, u","sv));
            return EC_Struct::MakePointF(extractDecimalToSingleW, extractDecimalToSingleW_2);
        }());
        return Result;
    }

    Types::TRect GetRectGI(const std::u16string_view& RectText) {
        Types::TRect Result{};
        if (EC_Str::CountDelimitedPartsW(RectText, u","sv) < 4) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetRectGI. tstr=", RectText}))));
        }
        Result.Left = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 0, u","sv)));
        Result.Top = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 1, u","sv)));
        Result.Right = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 2, u","sv)));
        Result.Bottom = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(RectText, 3, u","sv)));
        return Result;
    }

} // namespace GI_Main
