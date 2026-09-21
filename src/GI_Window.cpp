#include "layout/GI_Window.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_Window.hpp"
#include "units/GR_Main.hpp"
#include "units/MathImports.hpp"

namespace GI_Window {
    void TWindowGI_Create(TWindowGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->LeftImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->LeftImage->SetDepth(1.0E+6);
        Self->RightImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->RightImage->SetDepth(1.0E+6);
        Self->TopImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->TopImage->SetDepth(1.0E+6);
        Self->BottomImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BottomImage->SetDepth(1.0E+6);
        Self->TopLeftImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->TopLeftImage->SetDepth(1.0E+6);
        Self->TopRightImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->TopRightImage->SetDepth(1.0E+6);
        Self->BottomLeftImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BottomLeftImage->SetDepth(1.0E+6);
        Self->BottomRightImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BottomRightImage->SetDepth(1.0E+6);
        Self->TextureImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->TextureImage->SetDepth(1.0E+6);
    }

    void TWindowGI_Destroy(TWindowGI* Self) {
        if (Self->LeftImage != nullptr) {
            pas::free(Self->LeftImage);
            Self->LeftImage = nullptr;
        }
        if (Self->RightImage != nullptr) {
            pas::free(Self->RightImage);
            Self->RightImage = nullptr;
        }
        if (Self->TopImage != nullptr) {
            pas::free(Self->TopImage);
            Self->TopImage = nullptr;
        }
        if (Self->BottomImage != nullptr) {
            pas::free(Self->BottomImage);
            Self->BottomImage = nullptr;
        }
        if (Self->TopLeftImage != nullptr) {
            pas::free(Self->TopLeftImage);
            Self->TopLeftImage = nullptr;
        }
        if (Self->TopRightImage != nullptr) {
            pas::free(Self->TopRightImage);
            Self->TopRightImage = nullptr;
        }
        if (Self->BottomLeftImage != nullptr) {
            pas::free(Self->BottomLeftImage);
            Self->BottomLeftImage = nullptr;
        }
        if (Self->BottomRightImage != nullptr) {
            pas::free(Self->BottomRightImage);
            Self->BottomRightImage = nullptr;
        }
        if (Self->TextureImage != nullptr) {
            pas::free(Self->TextureImage);
            Self->TextureImage = nullptr;
        }
        GI_Panel::TPanelGI_Destroy(Self);
    }

    Types::TPoint TWindowGI::AlignSizeToBorderTiles(Types::TPoint Size) {
        Types::TPoint Result{};
        Types::TPoint CornerSize{};
        Types::TPoint TileSize{};
        Size.X = std::max<std::int32_t>(Size.X, MinimumSize.X);
        Size.Y = std::max<std::int32_t>(Size.Y, MinimumSize.Y);
        CornerSize = TopLeftImage->GetContentSize();
        TileSize = TopRightImage->GetContentSize();
        std::int32_t BorderSize = CornerSize.X + TileSize.X;
        if (Size.X <= BorderSize) {
            Result.X = BorderSize;
        } else {
            TileSize = TopImage->GetContentSize();
            Result.X = MathImports::Ceil(pas::real_divide(Size.X - BorderSize, TileSize.X)) * TileSize.X + BorderSize;
        }
        TileSize = BottomLeftImage->GetContentSize();
        BorderSize = CornerSize.Y + TileSize.Y;
        if (Size.Y <= BorderSize) {
            Result.Y = BorderSize;
        } else {
            TileSize = LeftImage->GetContentSize();
            Result.Y = MathImports::Ceil(pas::real_divide(Size.Y - BorderSize, TileSize.Y)) * TileSize.Y + BorderSize;
        }
        return Result;
    }

    void TWindowGI::UpdateBorderLayout() {
        {
            Types::TPoint contentSize = TopLeftImage->GetContentSize();
            GI_Image::TImageGI* topLeftImage = TopLeftImage;
            topLeftImage->SetSize(contentSize);
        }
        TopLeftImage->SetPosition(ClassesImports::Point(0, 0));
        {
            Types::TPoint contentSize_2 = TopRightImage->GetContentSize();
            GI_Image::TImageGI* topRightImage = TopRightImage;
            topRightImage->SetSize(contentSize_2);
        }
        TopRightImage->SetPosition(ClassesImports::Point(ClientSize.X - TopRightImage->ClientSize.X, 0));
        {
            Types::TPoint contentSize_3 = BottomLeftImage->GetContentSize();
            GI_Image::TImageGI* bottomLeftImage = BottomLeftImage;
            bottomLeftImage->SetSize(contentSize_3);
        }
        BottomLeftImage->SetPosition(ClassesImports::Point(0, ClientSize.Y - BottomLeftImage->ClientSize.Y));
        {
            Types::TPoint contentSize_4 = BottomRightImage->GetContentSize();
            GI_Image::TImageGI* bottomRightImage = BottomRightImage;
            bottomRightImage->SetSize(contentSize_4);
        }
        BottomRightImage->SetPosition(ClassesImports::Point(ClientSize.X - BottomRightImage->ClientSize.X, ClientSize.Y - BottomRightImage->ClientSize.Y));
        std::int32_t First = TopLeftImage->ClientSize.X;
        std::int32_t Last = ClientSize.X - TopRightImage->ClientSize.X;
        if (Last - First <= 0) {
            TopImage->SetActive(false);
        } else {
            TopImage->SetActive(true);
            {
                Types::TPoint point = ClassesImports::Point(Last - First, TopImage->GetContentSize().Y);
                GI_Image::TImageGI* topImage = TopImage;
                topImage->SetSize(point);
            }
            TopImage->SetPosition(ClassesImports::Point(First, 0));
            TopImage->SetImageKindX(GI_Main::ikxLeftFill);
        }
        First = BottomLeftImage->ClientSize.X;
        Last = ClientSize.X - BottomRightImage->ClientSize.X;
        if (Last - First <= 0) {
            BottomImage->SetActive(false);
        } else {
            BottomImage->SetActive(true);
            {
                Types::TPoint point_2 = ClassesImports::Point(Last - First, BottomImage->GetContentSize().Y);
                GI_Image::TImageGI* bottomImage = BottomImage;
                bottomImage->SetSize(point_2);
            }
            BottomImage->SetPosition(ClassesImports::Point(First, ClientSize.Y - BottomImage->ClientSize.Y));
            BottomImage->SetImageKindX(GI_Main::ikxLeftFill);
        }
        First = TopLeftImage->ClientSize.Y;
        Last = ClientSize.Y - BottomLeftImage->ClientSize.Y;
        if (Last - First <= 0) {
            LeftImage->SetActive(false);
        } else {
            LeftImage->SetActive(true);
            {
                Types::TPoint point_3 = ClassesImports::Point(LeftImage->GetContentSize().X, Last - First);
                GI_Image::TImageGI* leftImage = LeftImage;
                leftImage->SetSize(point_3);
            }
            LeftImage->SetPosition(ClassesImports::Point(0, First));
            LeftImage->SetImageKindY(GI_Main::ikyTopFill);
        }
        First = TopRightImage->ClientSize.Y;
        Last = ClientSize.Y - BottomRightImage->ClientSize.Y;
        if (Last - First <= 0) {
            RightImage->SetActive(false);
        } else {
            RightImage->SetActive(true);
            {
                Types::TPoint point_4 = ClassesImports::Point(RightImage->GetContentSize().X, Last - First);
                GI_Image::TImageGI* rightImage = RightImage;
                rightImage->SetSize(point_4);
            }
            RightImage->SetPosition(ClassesImports::Point(ClientSize.X - RightImage->ClientSize.X, First));
            RightImage->SetImageKindY(GI_Main::ikyTopFill);
        }
        std::int32_t Width = ClientSize.X - LeftImage->ClientSize.X - RightImage->ClientSize.X;
        std::int32_t Height = ClientSize.Y - TopImage->ClientSize.Y - BottomImage->ClientSize.Y;
        if (Width <= 0 || Height <= 0 || LeftImage->ClientSize.Y <= 0 && TopImage->ClientSize.X <= 0) {
            TextureImage->SetActive(false);
        } else {
            TextureImage->SetActive(true);
            TextureImage->SetPosition(ClassesImports::Point(LeftImage->ClientSize.X, TopImage->ClientSize.Y));
            TextureImage->SetSize(ClassesImports::Point(Width, Height));
            TextureImage->SetImageKindX(GI_Main::ikxLeftFill);
            TextureImage->SetImageKindY(GI_Main::ikyTopFill);
        }
    }

    void TWindowGI_LoadFromConfigPath(TWindowGI* Self, const pas::WideString& Path) {
        GI_Panel::TPanelGI_LoadFromConfigPath(Self, Path);
        Self->LoadWindowProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TWindowGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        LoadWindowProperties(Block);
    }

    void TWindowGI::LoadWindowProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"ImageTopLeft"_wref.get()) > 0) {
            TopLeftImage->SetImagePath(Block->GetParam(u"ImageTopLeft"sv));
        }
        if (Block->CountParams(u"ImageTopRight"_wref.get()) > 0) {
            TopRightImage->SetImagePath(Block->GetParam(u"ImageTopRight"sv));
        }
        if (Block->CountParams(u"ImageBottomLeft"_wref.get()) > 0) {
            BottomLeftImage->SetImagePath(Block->GetParam(u"ImageBottomLeft"sv));
        }
        if (Block->CountParams(u"ImageBottomRight"_wref.get()) > 0) {
            BottomRightImage->SetImagePath(Block->GetParam(u"ImageBottomRight"sv));
        }
        if (Block->CountParams(u"ImageLeft"_wref.get()) > 0) {
            LeftImage->SetImagePath(Block->GetParam(u"ImageLeft"sv));
        }
        if (Block->CountParams(u"ImageRight"_wref.get()) > 0) {
            RightImage->SetImagePath(Block->GetParam(u"ImageRight"sv));
        }
        if (Block->CountParams(u"ImageTop"_wref.get()) > 0) {
            TopImage->SetImagePath(Block->GetParam(u"ImageTop"sv));
        }
        if (Block->CountParams(u"ImageBottom"_wref.get()) > 0) {
            BottomImage->SetImagePath(Block->GetParam(u"ImageBottom"sv));
        }
        if (Block->CountParams(u"ImageTexture"_wref.get()) > 0) {
            TextureImage->SetImagePath(Block->GetParam(u"ImageTexture"sv));
        }
        if (Block->CountParams(u"WorkSubRect"_wref.get()) > 0) {
            WorkSubRect = GI_Main::GetRectGI(pas::view(Block->GetParam(u"WorkSubRect"sv)));
        }
        if (Block->CountParams(u"MinSize"_wref.get()) > 0) {
            MinimumSize = GI_Main::GetPointGI(pas::view(Block->GetParam(u"MinSize"sv)));
        }
    }

    void TWindowGI::UpdateAutoGeometry() {
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
        SetSize(AlignSizeToBorderTiles(ClientSize));
        UpdateBorderLayout();
    }

    void TWindowGI::p_destroy() {
        GI_Window::TWindowGI_Destroy(this);
    }

    void TWindowGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Window::TWindowGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Window
