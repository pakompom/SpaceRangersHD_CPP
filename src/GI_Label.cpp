#include "layout/GI_Label.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheFont.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/WindowsSdk.hpp"

namespace GI_Label {
    // Includes the native right/bottom padding.
    WindowsSdk::TRect MeasureLabelTextBounds(const pas::WideString& Text, const pas::WideString& FontName) {
        WindowsSdk::TRect Result{};
        EC_CacheFont::TCFontControlEC* Control = nullptr;
        EC_CacheFont::TCFontEC* Font = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(FontName);
                Font = EC_CacheFont::AcquireCachedFont(Control);
                Font->ResetTextMeasureState();
                Font->UseARGBColors = true;
                Font->ColorTagsEnabled = false;
                Result = Font->MeasureTaggedTextBounds(Text, 0, 0, nullptr);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Font != nullptr) {
                Control->Release();
            }
            if (Control != nullptr) {
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        Result.Bottom += 2;
        Result.Right += 4;
        return Result;
    }

    WindowsSdk::TRect MeasureWrappedLabelBounds(std::int32_t Width, EC_Str::TStringsEC* TextLines, EC_CacheFont::TCFontEC* Font) {
        WindowsSdk::TRect Result{};
        EC_Str::TStringsEC* Lines{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect LineBounds{};
        TextLines->First();
        Bounds.Left = 0;
        Bounds.Right = 0;
        Bounds.Top = 0;
        Bounds.Bottom = 0;
        std::int32_t Y = 0;
        Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        std::uint8_t FirstLine = true;
        while (!TextLines->IsAtEnd()) {
            Font->ResetTextMeasureState();
            Font->WrapTaggedTextIntoLines(Lines, TextLines->GetCurrentText(), Width - 4);
            if (!Lines->IsEmpty()) {
                Lines->First();
                if (FirstLine) {
                    Font->ResetTextMeasureState();
                    Bounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, nullptr);
                    FirstLine = false;
                    Y += Font->GetLineHeight();
                    Lines->Next();
                }
                while (!Lines->IsAtEnd()) {
                    Font->ResetTextMeasureState();
                    LineBounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, nullptr);
                    WindowsSdk::UnionRect(Bounds, Bounds, LineBounds);
                    Y += Font->GetLineHeight();
                    Lines->Next();
                }
            }
            TextLines->Next();
        }
        pas::free(Lines);
        Bounds.Bottom += 2;
        Bounds.Right += 4;
        Result = Bounds;
        return Result;
    }

    void DrawWrappedLabelLines(GR_GraphBuf::TGraphBufGR* Buffer, std::int32_t Width, std::int32_t X, std::int32_t Y, EC_Str::TStringsEC* TextLines, EC_CacheFont::TCFontEC* Font) {
        EC_Str::TStringsEC* Lines{};
        WindowsSdk::TRect ClipRect{};
        Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        ClipRect = ClassesImports::Rect(0, 0, Buffer->Width, Buffer->Height);
        std::int32_t CurrentY = Y + 2 + Font->AboveBaseline - 2;
        TextLines->First();
        while (!TextLines->IsAtEnd()) {
            Font->ResetTextMeasureState();
            Font->WrapTaggedTextIntoLines(Lines, TextLines->GetCurrentText(), Width - 4);
            Lines->First();
            while (!Lines->IsAtEnd()) {
                Font->ResetTextMeasureState();
                if (!Lines->IsAtLast()) {
                    const pas::WideString& currentText = Lines->GetCurrentText();
                    void* pixels = Buffer->GetPixels();
                    std::int32_t pitchBytes = Buffer->PitchBytes;
                    EC_CacheFont::TCFontEC_DrawJustifiedTaggedText32(Font, pixels, pitchBytes, X, CurrentY, currentText, Width - 4, ClipRect);
                } else {
                    const pas::WideString& currentText_2 = Lines->GetCurrentText();
                    void* pixels_2 = Buffer->GetPixels();
                    std::int32_t pitchBytes_2 = Buffer->PitchBytes;
                    Font->DrawTaggedText32(pixels_2, pitchBytes_2, X, CurrentY, currentText_2, ClipRect);
                }
                CurrentY += Font->GetLineHeight();
                Lines->Next();
            }
            TextLines->Next();
        }
        pas::free(Lines);
    }

    void RenderLabelTextToBuffer(GR_GraphBuf::TGraphBufGR* Buffer, std::int32_t Width, std::int32_t BorderWidth, std::int32_t ShadowOffset, const pas::WideString& Text, const pas::WideString& FontName, std::uint32_t TextColor, std::uint32_t BorderColor, std::uint32_t ShadowColor) {
        std::int32_t InnerWidth{};
        std::int32_t X{};
        std::int32_t Y{};
        WindowsSdk::TRect cpp_with{};
        EC_CacheFont::TCFontControlEC* Control = nullptr;
        EC_CacheFont::TCFontEC* Font = nullptr;
        EC_Str::TStringsEC* Lines = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                Lines->SetText(Text);
                Control = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(FontName);
                Font = EC_CacheFont::AcquireCachedFont(Control);
                Font->ResetTextMeasureState();
                Font->UseARGBColors = true;
                Font->ColorTagsEnabled = false;
                InnerWidth = Width - BorderWidth - std::max<std::int32_t>(BorderWidth, ShadowOffset);
                cpp_with = GI_Label::MeasureWrappedLabelBounds(InnerWidth, Lines, Font);
                cpp_with.Left -= BorderWidth;
                cpp_with.Top -= BorderWidth;
                cpp_with.Right += std::max<std::int32_t>(BorderWidth, ShadowOffset);
                cpp_with.Bottom = cpp_with.Bottom + std::max<std::int32_t>(BorderWidth, ShadowOffset) + 4;
                Buffer->AllocateRgbaTight(InnerWidth, cpp_with.Bottom - cpp_with.Top);
                Buffer->ClearPixels();
                X = BorderWidth;
                Y = BorderWidth;
                if (ShadowOffset != 0) {
                    Font->DefaultColor = ShadowColor;
                    GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X + ShadowOffset, Y + ShadowOffset, Lines, Font);
                }
                if (BorderWidth > 0) {
                    Font->DefaultColor = BorderColor;
                    GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X - BorderWidth, Y - BorderWidth, Lines, Font);
                    GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X + BorderWidth, Y - BorderWidth, Lines, Font);
                    GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X - BorderWidth, Y + BorderWidth, Lines, Font);
                    GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X + BorderWidth, Y + BorderWidth, Lines, Font);
                }
                Font->ColorTagsEnabled = true;
                Font->DefaultColor = TextColor;
                GI_Label::DrawWrappedLabelLines(Buffer, InnerWidth, X, Y, Lines, Font);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Font != nullptr) {
                Control->Release();
            }
            if (Control != nullptr) {
                pas::free(Control);
            }
            if (Lines != nullptr) {
                pas::free(Lines);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TLabelGI_Create(TLabelGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->FontCache = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->FontCache);
        Self->EmbeddedImage = nullptr;
        Self->TextLines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        Self->TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        Self->TextBorderWidth = 0;
        Self->TextBorderColor = 0u;
        Self->BorderEnabled = false;
        Self->TextAlignX = GI_Main::taxCenter;
        Self->TextAlignY = GI_Main::tayCenter;
        Self->AutoHeightPadding = 4;
        Self->TextTexture = nullptr;
    }

    void TLabelGI_Destroy(TLabelGI* Self) {
        pas::free(Self->FontCache);
        Self->FontCache = nullptr;
        pas::free(Self->TextLines);
        Self->TextLines = nullptr;
        if (Self->TextTexture != nullptr) {
            GR_DX::FreeTextureCache(Self->TextTexture);
            Self->TextTexture = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TLabelGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
        TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        BorderEnabled = false;
        TextAlignX = GI_Main::taxCenter;
        TextAlignY = GI_Main::tayCenter;
        if (TextLines != nullptr) {
            TextLines->Clear();
        }
        if (EmbeddedImage != nullptr) {
            FreeOwnedChild(EmbeddedImage);
            EmbeddedImage = nullptr;
        }
        if (TextTexture != nullptr) {
            GR_DX::FreeTextureCache(TextTexture);
            TextTexture = nullptr;
        }
    }

    void TLabelGI::SetFontName(const pas::WideString& FontName) {
        Invalidate();
        FontCache->SetCacheKey(FontName);
        Invalidate();
        if (TextTexture != nullptr) {
            TextTexture->ReleaseSurfaces();
        }
    }

    void TLabelGI::SetTextBorderWidth(std::int32_t Value) {
        if (Value != TextBorderWidth) {
            TextBorderWidth = Value;
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::SetTextBorderColor(std::uint32_t Value) {
        if (Value != TextBorderColor) {
            TextBorderColor = Value;
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::SetShadowOffset(std::int32_t Value) {
        if (Value != TextShadowOffset) {
            TextShadowOffset = Value;
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->SetSurface(nullptr, 1);
            }
        }
    }

    void TLabelGI::SetShadowColor(std::uint32_t Value) {
        if (Value != TextShadowColor) {
            TextShadowColor = Value;
            Invalidate();
        }
    }

    void TLabelGI::SetText(const pas::WideString& Text) {
        if (TextLines->GetText() != Text) {
            Invalidate();
            TextLines->SetText(Text);
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::LoadTextLinesFromBlockParam(EC_BlockPar::TBlockParEC* Block, const pas::WideString& ParamName) {
        std::int32_t Index{};
        pas::WideString Key{};
        TextLines->Clear();
        std::int32_t Count = Block->CountParams(ParamName);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            const pas::WideString& paramByPath = Block->GetParamByPath(pas::concat_wide({ParamName, u":", pas::wide_int_to_str(Index)}));
            EC_Str::TStringsEC* textLines = TextLines;
            textLines->Add(paramByPath);
        }
        if (Count > 0) {
            Key = EC_Str::TrimWideString(TextLines->GetText());
            Count = GR_Main::LanguageDataConfig->CountParamsByPath(Key);
            if (Count > 0) {
                TextLines->Clear();
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
                    const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(pas::concat_wide({Key, u":", pas::wide_int_to_str(Index)}));
                    EC_Str::TStringsEC* textLines_2 = TextLines;
                    textLines_2->Add(paramByPathOrMarker);
                }
            }
        }
        UpdateAbsolutePosition();
        UpdateSubtreeHitBounds();
        Invalidate();
        if (TextTexture != nullptr) {
            TextTexture->ReleaseSurfaces();
        }
    }

    pas::WideString TLabelGI::GetText() {
        return TextLines->GetText();
    }

    void TLabelGI::SetTextAlignX(GI_Main::TTextAlignXGI Value) {
        if (TextAlignX != Value) {
            TextAlignX = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::SetTextAlignY(GI_Main::TTextAlignYGI Value) {
        if (TextAlignY != Value) {
            TextAlignY = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::SetWordWrapEnabled(std::uint8_t Value) {
        if (WordWrapEnabled != Value) {
            WordWrapEnabled = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    void TLabelGI::SetAutoHeightPadding(std::int32_t Value) {
        if (AutoHeightPadding != Value) {
            AutoHeightPadding = Value;
            UpdateAbsolutePosition();
            UpdateSubtreeHitBounds();
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->ReleaseSurfaces();
            }
        }
    }

    // An empty path frees the embedded child.
    void TLabelGI::SetEmbeddedImagePath(const pas::WideString& ImagePath) {
        Invalidate();
        if (ImagePath == u"") {
            if (EmbeddedImage != nullptr) {
                FreeOwnedChild(EmbeddedImage);
                EmbeddedImage = nullptr;
            }
        } else {
            if (EmbeddedImage == nullptr) {
                EmbeddedImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, this);
            }
            EmbeddedImage->SetImagePath(ImagePath);
            EmbeddedImage->SetImageKindX(GI_Main::ikxLeftFill);
            EmbeddedImage->SetImageKindY(GI_Main::ikyTopFill);
            EmbeddedImage->SetSize(ClientSize);
        }
    }

    void TLabelGI::SetEmbeddedImageKindX(GI_Main::TImageKindXGI Value) {
        if (EmbeddedImage != nullptr) {
            EmbeddedImage->SetImageKindX(Value);
        }
    }

    void TLabelGI::SetEmbeddedImageKindY(GI_Main::TImageKindYGI Value) {
        if (EmbeddedImage != nullptr) {
            EmbeddedImage->SetImageKindY(Value);
        }
    }

    void TLabelGI::SetEmbeddedImageHalfAlpha(std::uint8_t Value) {
        if (EmbeddedImage != nullptr) {
            EmbeddedImage->SetHalfAlpha(Value);
        }
    }

    void TLabelGI::SetTextColor(std::uint32_t Value) {
        if (TextColor != Value) {
            TextColor = Value;
            Invalidate();
            if (TextTexture != nullptr) {
                TextTexture->SetSurface(nullptr, 0);
            }
        }
    }

    void TLabelGI::SetBorderLightColor(std::uint32_t Value) {
        if (BorderLightColor != Value) {
            BorderLightColor = Value;
            Invalidate();
        }
    }

    void TLabelGI::SetBorderDarkColor(std::uint32_t Value) {
        if (BorderDarkColor != Value) {
            BorderDarkColor = Value;
            Invalidate();
        }
    }

    // TopAdjustment is optional; includes text outline/shadow padding.
    WindowsSdk::TPoint TLabelGI::MeasureContentSize(WindowsSdk::PInteger TopAdjustment) {
        WindowsSdk::TPoint Result{};
        EC_CacheFont::TCFontEC* Font{};
        std::int32_t Y{};
        EC_Str::TStringsEC* Lines{};
        std::uint8_t FirstLine{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect LineBounds{};
        if (TopAdjustment != nullptr) {
            pas::store_unaligned<std::int32_t>(TopAdjustment, 0);
        }
        Bounds.Left = 0;
        Bounds.Right = 0;
        Bounds.Top = 0;
        Bounds.Bottom = 0;
        std::int32_t Padding = 0;
        if (!FontCache->HasEmptyCacheKey()) {
            Font = EC_CacheFont::AcquireCachedFont(FontCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    FontCache->Release();
                };
                Font->ResetTextMeasureState();
                Y = 0;
                TextLines->First();
                if (!WordWrapEnabled) {
                    if (!TextLines->IsAtEnd()) {
                        Bounds = Font->MeasureTaggedTextBounds(TextLines->GetCurrentText(), 0, Y, TopAdjustment);
                        Y += Font->GetLineHeight();
                        TextLines->Next();
                    }
                    while (!TextLines->IsAtEnd()) {
                        LineBounds = Font->MeasureTaggedTextBounds(TextLines->GetCurrentText(), 0, Y, nullptr);
                        WindowsSdk::UnionRect(Bounds, Bounds, LineBounds);
                        Y += Font->GetLineHeight();
                        TextLines->Next();
                    }
                } else {
                    Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                    FirstLine = true;
                    while (!TextLines->IsAtEnd()) {
                        Font->WrapTaggedTextIntoLines(Lines, TextLines->GetCurrentText(), ClientSize.X - 4);
                        if (!Lines->IsEmpty()) {
                            Lines->First();
                            if (FirstLine) {
                                Bounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, TopAdjustment);
                                FirstLine = false;
                                Y += Font->GetLineHeight();
                                Lines->Next();
                            }
                            while (!Lines->IsAtEnd()) {
                                LineBounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, nullptr);
                                WindowsSdk::UnionRect(Bounds, Bounds, LineBounds);
                                Y += Font->GetLineHeight();
                                Lines->Next();
                            }
                        }
                        TextLines->Next();
                    }
                    pas::free(Lines);
                }
                Result.Y += 2;
                Padding = 2;
            }
        }
        Result = ClassesImports::Point(Bounds.Right - Bounds.Left + TextBorderWidth + std::max<std::int32_t>(TextBorderWidth, TextShadowOffset), Padding + Bounds.Bottom - Bounds.Top + TextBorderWidth + std::max<std::int32_t>(TextBorderWidth, TextShadowOffset));
        return Result;
    }

    std::int32_t TLabelGI::GetLineHeight() {
        std::int32_t Result{};
        EC_CacheFont::TCFontEC* Font = EC_CacheFont::AcquireCachedFont(FontCache);
        {
            Font->ResetTextMeasureState();
            Result = Font->GetLineHeight();
            FontCache->Release();
        }
        return Result;
    }

    // Includes word wrapping when enabled.
    std::int32_t TLabelGI::GetRenderedLineCount() {
        EC_Str::TStringsEC* Lines{};
        std::int32_t Result = 0;
        EC_CacheFont::TCFontEC* Font = nullptr;
        if (FontCache != nullptr) {
            std::exception_ptr cpp_error{};
            try {
                Font = EC_CacheFont::AcquireCachedFont(FontCache);
                if (!WordWrapEnabled) {
                    Result = TextLines->GetCount();
                } else {
                    Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                    TextLines->First();
                    while (!TextLines->IsAtEnd()) {
                        Font->WrapTaggedTextIntoLines(Lines, TextLines->GetCurrentText(), ClientSize.X - 4);
                        Result += Lines->GetCount();
                        TextLines->Next();
                    }
                    pas::free(Lines);
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Font != nullptr) {
                FontCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    // May resize the control to fit its text.
    void TLabelGI::UpdateHitTestBounds() {
        WindowsSdk::TPoint Size{};
        std::int32_t TopAdjustment{};
        Size = MeasureContentSize(&TopAdjustment);
        if (TextAlignX == GI_Main::taxLeft || WordWrapEnabled == true) {
            TextLeft = AbsolutePosition.X + 2;
        } else if (TextAlignX == GI_Main::taxRight) {
            TextLeft = AbsolutePosition.X + ClientSize.X - Size.X - 2;
        } else if (TextAlignX == GI_Main::taxCenter) {
            TextLeft = ClientSize.X / 2 + AbsolutePosition.X - Size.X / 2;
        } else if (TextAlignX == GI_Main::taxAuto) {
            TextLeft = AbsolutePosition.X + 2;
            ClientSize.X = Size.X + 4;
        }
        if (TextAlignY == GI_Main::tayTop) {
            TextTop = AbsolutePosition.Y + 2 + TopAdjustment;
        } else if (TextAlignY == GI_Main::tayBottom) {
            TextTop = AbsolutePosition.Y + ClientSize.Y - Size.Y - 2 + TopAdjustment;
        } else if (TextAlignY == GI_Main::tayCenter) {
            TextTop = ClientSize.Y / 2 + AbsolutePosition.Y - Size.Y / 2 + TopAdjustment;
        } else if (TextAlignY == GI_Main::tayCenterEx) {
            TextTop = ClientSize.Y / 2 + AbsolutePosition.Y - Size.Y / 2 + TopAdjustment;
        } else if (TextAlignY == GI_Main::tayAuto) {
            TextTop = AbsolutePosition.Y + 2 + TopAdjustment;
            ClientSize.Y = Size.Y + AutoHeightPadding;
        }
        GI_MessageLoop::TObjectGI::UpdateHitTestBounds();
    }

    void TLabelGI::SetSize(WindowsSdk::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        if (EmbeddedImage != nullptr) {
            EmbeddedImage->SetSize(Size);
        }
    }

    // Missing embedded controls are requested through the creation callback.
    void TLabelGI::UpdateEmbeddedControls(EC_CacheFont::TCFontEC* Font) {
        std::int32_t Index{};
        GI_MessageLoop::TObjectGI* Child{};
        EC_CacheFont::PFontObjectEC Item{};
        WindowsSdk::TPoint Position{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Font->ObjectCount - 1); cpp_range.next(Index); ) {
            Item = Font->GetEmbeddedObject(Index);
            if (GlobalsV::HardwareRenderingEnabled) {
                Position = ClassesImports::Point(Item->X, Item->Y);
            } else {
                Position = ToLocalPoint(ClassesImports::Point(Item->X, Item->Y));
            }
            Child = FirstChild;
            while (Child != nullptr) {
                if (Child->UserValue == Item->ObjectId && Child->UserIndex == Index) {
                    Child->SetPosition(Position);
                    Child->SetSize(ClassesImports::Point(Item->Width, Item->Height));
                    break;
                }
                Child = Child->NextSibling;
            }
            if (Child == nullptr && pas::assigned(CreateEmbeddedControl)) {
                Child = CreateEmbeddedControl(this, Item);
                if (Child != nullptr) {
                    Child->UserValue = Item->ObjectId;
                    Child->UserIndex = Index;
                    Child->SetPosition(Position);
                    Child->SetSize(ClassesImports::Point(Item->Width, Item->Height));
                }
            }
        }
    }

    void TLabelGI::RemoveUnusedEmbeddedControls(EC_CacheFont::TCFontEC* Font) {
        std::int32_t Index{};
        GI_MessageLoop::TObjectGI* Previous{};
        GI_MessageLoop::TObjectGI* Child = FirstChild;
        while (Child != nullptr) {
            Index = 0;
            while (Index < Font->ObjectCount) {
                if (Child->UserIndex == Index && Child->UserValue == Font->GetEmbeddedObject(Index)->ObjectId) {
                    break;
                }
                ++Index;
            }
            Previous = Child;
            Child = Child->NextSibling;
            if (Index >= Font->ObjectCount) {
                pas::free(Previous);
            }
        }
    }

    void TLabelGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        if (HelpText != u"" && MessageLoop->HoveredControl != this) {
            MessageLoop->SetHoveredControl(this);
            if (pas::assigned(HelpCallback)) {
                HelpCallback(this, true);
            }
        }
    }

    void TLabelGI::OnMouseLeave() {
        if (MessageLoop->HoveredControl == this) {
            MessageLoop->SetHoveredControl(nullptr);
            if (pas::assigned(HelpCallback)) {
                HelpCallback(this, false);
            }
        }
        GI_MessageLoop::TObjectGI::OnMouseLeave();
    }

    void TLabelGI::ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
    }

    void TLabelGI::ProcessLeftButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
    }

    void TLabelGI_LoadFromConfigPath(TLabelGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Alignment{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            Self->FontCache->SetCacheKey(Block->GetParam(u"Font"_wref.get()));
        }
        Self->LoadTextLinesFromBlockParam(Block, u"Text"_wref.get());
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Self->SetEmbeddedImagePath(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"ImageKindX"_wref.get()) > 0) {
            Self->SetEmbeddedImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"ImageKindX"_wref.get())));
        }
        if (Block->CountParams(u"ImageKindY"_wref.get()) > 0) {
            Self->SetEmbeddedImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"ImageKindY"_wref.get())));
        }
        if (Block->CountParams(u"TextColor"_wref.get()) > 0) {
            Self->SetTextColor(GI_Main::GetColorGI(Block->GetParam(u"TextColor"_wref.get())));
        }
        if (Block->CountParams(u"Border"_wref.get()) > 0) {
            if (Block->GetParam(u"Border"_wref.get()) == u"True") {
                Self->BorderEnabled = true;
            } else {
                Self->BorderEnabled = false;
            }
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            Self->SetBorderLightColor(GI_Main::GetColorGI(Block->GetParam(u"BorderLightColor"_wref.get())));
            Self->SetBorderDarkColor(Self->BorderLightColor);
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            Self->SetBorderDarkColor(GI_Main::GetColorGI(Block->GetParam(u"BorderDarkColor"_wref.get())));
        }
        if (Block->CountParams(u"WordWrap"_wref.get()) > 0) {
            Self->SetWordWrapEnabled(GI_Main::ParseEnabledNameGI(EC_Str::TrimWideString(Block->GetParam(u"WordWrap"_wref.get()))));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            Alignment = EC_Str::TrimWideString(Block->GetParam(u"AlignY"_wref.get()));
            Self->SetTextAlignY(GI_Main::ParseTextAlignYName(Alignment));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            Alignment = EC_Str::TrimWideString(Block->GetParam(u"AlignX"_wref.get()));
            Self->SetTextAlignX(GI_Main::ParseTextAlignXName(Alignment));
        }
    }

    void TLabelGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Alignment{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            FontCache->SetCacheKey(Block->GetParam(u"Font"_wref.get()));
        }
        LoadTextLinesFromBlockParam(Block, u"Text"_wref.get());
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            SetEmbeddedImagePath(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"ImageKindX"_wref.get()) > 0) {
            SetEmbeddedImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"ImageKindX"_wref.get())));
        }
        if (Block->CountParams(u"ImageKindY"_wref.get()) > 0) {
            SetEmbeddedImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"ImageKindY"_wref.get())));
        }
        if (Block->CountParams(u"TextColor"_wref.get()) > 0) {
            SetTextColor(GI_Main::GetColorGI(Block->GetParam(u"TextColor"_wref.get())));
        }
        if (Block->CountParams(u"Border"_wref.get()) > 0) {
            if (Block->GetParam(u"Border"_wref.get()) == u"True") {
                BorderEnabled = true;
            } else {
                BorderEnabled = false;
            }
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            SetBorderLightColor(GI_Main::GetColorGI(Block->GetParam(u"BorderLightColor"_wref.get())));
            SetBorderDarkColor(BorderLightColor);
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            SetBorderDarkColor(GI_Main::GetColorGI(Block->GetParam(u"BorderDarkColor"_wref.get())));
        }
        if (Block->CountParams(u"WordWrap"_wref.get()) > 0) {
            SetWordWrapEnabled(GI_Main::ParseEnabledNameGI(EC_Str::TrimWideString(Block->GetParam(u"WordWrap"_wref.get()))));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            Alignment = EC_Str::TrimWideString(Block->GetParam(u"AlignY"_wref.get()));
            SetTextAlignY(GI_Main::ParseTextAlignYName(Alignment));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            Alignment = EC_Str::TrimWideString(Block->GetParam(u"AlignX"_wref.get()));
            SetTextAlignX(GI_Main::ParseTextAlignXName(Alignment));
        }
        if (Block->CountParams(u"TextBorderColor"_wref.get()) > 0) {
            SetTextBorderColor(GI_Main::GetColorGI(Block->GetParam(u"TextBorderColor"_wref.get())));
        }
        if (Block->CountParams(u"TextShadowColor"_wref.get()) > 0) {
            SetShadowColor(GI_Main::GetColorGI(Block->GetParam(u"TextShadowColor"_wref.get())));
        }
        if (Block->CountParams(u"TextBorder"_wref.get()) > 0) {
            SetTextBorderWidth(EC_Str::ExtractDigitsToIntW(Block->GetParam(u"TextBorder"_wref.get())));
        }
        if (Block->CountParams(u"TextShadow"_wref.get()) > 0) {
            SetShadowOffset(EC_Str::ExtractDigitsToIntW(Block->GetParam(u"TextShadow"_wref.get())));
        }
    }

    void TLabelGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        std::int32_t Y{};
        EC_Str::TStringsEC* Lines{};
        std::uint32_t Color{};
        Direct3D9::IDirect3DTexture9 Texture{};
        WindowsSdk::TRect Bounds{};
        // Nested Draw helper; caller removes the parent-frame argument.
        auto SwitchLabelDrawFont = [&](pas::WideString FontName) -> void {
            this->FontCache->SetCacheKey(FontName);
            if (this->TextTexture != nullptr) {
                this->TextTexture->ReleaseSurfaces();
            }
        };
        EC_CacheFont::TCFontEC* Font = nullptr;
        if (FontCache != nullptr) {
            if (GlobalsV::FontSmoothingEnabled) {
                if (FontCache->CacheKey == GlobalsV::SmallFontName) {
                    SwitchLabelDrawFont(GlobalsV::SmoothSmallFontName);
                } else if (FontCache->CacheKey == GlobalsV::SmallBoldFontName) {
                    SwitchLabelDrawFont(GlobalsV::SmoothSmallBoldFontName);
                } else if (FontCache->CacheKey == GlobalsV::NormalFontName) {
                    SwitchLabelDrawFont(GlobalsV::SmoothNormalFontName);
                } else if (FontCache->CacheKey == GlobalsV::NormalBoldFontName) {
                    SwitchLabelDrawFont(GlobalsV::SmoothNormalBoldFontName);
                }
            } else if (FontCache->CacheKey == GlobalsV::SmoothSmallFontName) {
                SwitchLabelDrawFont(GlobalsV::SmallFontName);
            } else if (FontCache->CacheKey == GlobalsV::SmoothSmallBoldFontName) {
                SwitchLabelDrawFont(GlobalsV::SmallBoldFontName);
            } else if (FontCache->CacheKey == GlobalsV::SmoothNormalFontName) {
                SwitchLabelDrawFont(GlobalsV::NormalFontName);
            } else if (FontCache->CacheKey == GlobalsV::SmoothNormalBoldFontName) {
                SwitchLabelDrawFont(GlobalsV::NormalBoldFontName);
            }
            {
                std::exception_ptr cpp_error{};
                try {
                    Font = EC_CacheFont::AcquireCachedFont(FontCache);
                    Font->ResetTextMeasureState();
                    if (GlobalsV::HardwareRenderingEnabled) {
                        if (TextTexture == nullptr) {
                            TextTexture = GR_DX::CreateTextureCache();
                        }
                        Texture = (TextTexture->GetSurface(1, cpp_result), cpp_result);
                        if (Texture == nullptr && (TextShadowOffset > 0 || TextBorderWidth > 0)) {
                            Font->ColorTagsEnabled = false;
                            Font->DefaultColor = 0xffffffffu;
                            Font->RenderTaggedTextToTexture(TextLines->GetText(), ClientSize.X, ClientSize.Y, TextAlignX, TextAlignY, WordWrapEnabled, nullptr, Texture);
                            TextTexture->SetSurface(Texture, 1);
                        }
                        if (TextShadowOffset > 0) {
                            GR_DX::DrawTexture(Texture, AbsolutePosition.X + TextShadowOffset, AbsolutePosition.Y + TextShadowOffset, 255, GR_DX::Color565ToArgb(TextShadowColor), reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                        }
                        if (TextBorderWidth > 0) {
                            Color = GR_DX::Color565ToArgb(TextBorderColor);
                            GR_DX::DrawTexture(Texture, AbsolutePosition.X - TextBorderWidth, AbsolutePosition.Y - TextBorderWidth, 255, Color, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            GR_DX::DrawTexture(Texture, AbsolutePosition.X + TextBorderWidth, AbsolutePosition.Y - TextBorderWidth, 255, Color, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            GR_DX::DrawTexture(Texture, AbsolutePosition.X - TextBorderWidth, AbsolutePosition.Y + TextBorderWidth, 255, Color, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            GR_DX::DrawTexture(Texture, AbsolutePosition.X + TextBorderWidth, AbsolutePosition.Y + TextBorderWidth, 255, Color, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                        }
                        Texture = (TextTexture->GetSurface(0, cpp_result_2), cpp_result_2);
                        if (Texture == nullptr) {
                            Font->ColorTagsEnabled = true;
                            Font->DefaultColor = GR_DX::Color565ToArgb(TextColor);
                            Font->RenderTaggedTextToTexture(TextLines->GetText(), ClientSize.X, ClientSize.Y, TextAlignX, TextAlignY, WordWrapEnabled, nullptr, Texture);
                            UpdateEmbeddedControls(Font);
                            RemoveUnusedEmbeddedControls(Font);
                            TextTexture->SetSurface(Texture, 0);
                        }
                        GR_DX::DrawTexture(Texture, AbsolutePosition.X, AbsolutePosition.Y, 255, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                        if (BorderEnabled) {
                            Color = GR_DX::Color565ToArgb(BorderLightColor);
                            GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Top, Color, 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                            GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Left, HitTestBounds.Bottom - 1, Color, 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                            Color = GR_DX::Color565ToArgb(BorderDarkColor);
                            GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Bottom - 1, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, Color, 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                            GR_DX::DrawAlphaLine(HitTestBounds.Right - 1, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, Color, 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                        }
                    } else {
                        if (!WordWrapEnabled) {
                            if (TextAlignY == GI_Main::tayCenterEx) {
                                Y = HitTestBounds.Top + ClientSize.Y / 2 - (Font->GetLineHeight() * (TextLines->GetCount() - 1) + Font->GetCenteringHeight()) / 2 + Font->GetCenteringHeight();
                            } else {
                                Y = TextTop + Font->AboveBaseline - 2;
                            }
                            TextLines->First();
                            while (!TextLines->IsAtEnd()) {
                                Font->ColorTagsEnabled = false;
                                if (TextShadowOffset > 0) {
                                    Font->DefaultColor = TextShadowColor;
                                    {
                                        std::int32_t cpp_arg = TextLeft + TextShadowOffset;
                                        std::int32_t cpp_arg_2 = Y + TextShadowOffset;
                                        const pas::WideString& currentText = TextLines->GetCurrentText();
                                        WindowsSdk::TRect clipRect = ClipRect;
                                        void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                                        std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                                        Font->DrawTaggedText16(pixels, pitchBytes, cpp_arg, cpp_arg_2, currentText, clipRect);
                                    }
                                }
                                if (TextBorderWidth > 0) {
                                    Font->DefaultColor = TextBorderColor;
                                    {
                                        std::int32_t cpp_arg_3 = TextLeft - TextBorderWidth;
                                        std::int32_t cpp_arg_4 = Y - TextBorderWidth;
                                        const pas::WideString& currentText_2 = TextLines->GetCurrentText();
                                        WindowsSdk::TRect clipRect_2 = ClipRect;
                                        void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                                        std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                        Font->DrawTaggedText16(pixels_2, pitchBytes_2, cpp_arg_3, cpp_arg_4, currentText_2, clipRect_2);
                                    }
                                    {
                                        std::int32_t cpp_arg_5 = TextLeft + TextBorderWidth;
                                        std::int32_t cpp_arg_6 = Y - TextBorderWidth;
                                        const pas::WideString& currentText_3 = TextLines->GetCurrentText();
                                        WindowsSdk::TRect clipRect_3 = ClipRect;
                                        void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
                                        std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                        Font->DrawTaggedText16(pixels_3, pitchBytes_3, cpp_arg_5, cpp_arg_6, currentText_3, clipRect_3);
                                    }
                                    {
                                        std::int32_t cpp_arg_7 = TextLeft - TextBorderWidth;
                                        std::int32_t cpp_arg_8 = Y + TextBorderWidth;
                                        const pas::WideString& currentText_4 = TextLines->GetCurrentText();
                                        WindowsSdk::TRect clipRect_4 = ClipRect;
                                        void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                                        std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                        Font->DrawTaggedText16(pixels_4, pitchBytes_4, cpp_arg_7, cpp_arg_8, currentText_4, clipRect_4);
                                    }
                                    {
                                        std::int32_t cpp_arg_9 = TextLeft + TextBorderWidth;
                                        std::int32_t cpp_arg_10 = Y + TextBorderWidth;
                                        const pas::WideString& currentText_5 = TextLines->GetCurrentText();
                                        WindowsSdk::TRect clipRect_5 = ClipRect;
                                        void* pixels_5 = GR_Main::ScreenRenderBuffer->GetPixels();
                                        std::int32_t pitchBytes_5 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                        Font->DrawTaggedText16(pixels_5, pitchBytes_5, cpp_arg_9, cpp_arg_10, currentText_5, clipRect_5);
                                    }
                                }
                                Font->ColorTagsEnabled = true;
                                Font->DefaultColor = TextColor;
                                {
                                    std::int32_t textLeft = TextLeft;
                                    const pas::WideString& currentText_6 = TextLines->GetCurrentText();
                                    WindowsSdk::TRect clipRect_6 = ClipRect;
                                    void* pixels_6 = GR_Main::ScreenRenderBuffer->GetPixels();
                                    std::int32_t pitchBytes_6 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                    Font->DrawTaggedText16(pixels_6, pitchBytes_6, textLeft, Y, currentText_6, clipRect_6);
                                }
                                Y += Font->GetLineHeight();
                                TextLines->Next();
                            }
                        } else {
                            Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                            Y = TextTop + Font->AboveBaseline - 2;
                            TextLines->First();
                            while (!TextLines->IsAtEnd()) {
                                Font->WrapTaggedTextIntoLines(Lines, TextLines->GetCurrentText(), ClientSize.X - 4);
                                Lines->First();
                                while (!Lines->IsAtEnd()) {
                                    if (TextAlignX == GI_Main::taxLeft) {
                                        Font->ColorTagsEnabled = false;
                                        if (TextShadowOffset > 0) {
                                            Font->DefaultColor = TextShadowColor;
                                            {
                                                std::int32_t cpp_arg_11 = TextLeft + TextShadowOffset;
                                                std::int32_t cpp_arg_12 = Y + TextShadowOffset;
                                                const pas::WideString& currentText_7 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_7 = ClipRect;
                                                void* pixels_7 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_7 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_7, pitchBytes_7, cpp_arg_11, cpp_arg_12, currentText_7, clipRect_7);
                                            }
                                        }
                                        if (TextBorderWidth > 0) {
                                            Font->DefaultColor = TextBorderColor;
                                            {
                                                std::int32_t cpp_arg_13 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_14 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_8 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_8 = ClipRect;
                                                void* pixels_8 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_8 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_8, pitchBytes_8, cpp_arg_13, cpp_arg_14, currentText_8, clipRect_8);
                                            }
                                            {
                                                std::int32_t cpp_arg_15 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_16 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_9 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_9 = ClipRect;
                                                void* pixels_9 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_9 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_9, pitchBytes_9, cpp_arg_15, cpp_arg_16, currentText_9, clipRect_9);
                                            }
                                            {
                                                std::int32_t cpp_arg_17 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_18 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_10 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_10 = ClipRect;
                                                void* pixels_10 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_10 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_10, pitchBytes_10, cpp_arg_17, cpp_arg_18, currentText_10, clipRect_10);
                                            }
                                            {
                                                std::int32_t cpp_arg_19 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_20 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_11 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_11 = ClipRect;
                                                void* pixels_11 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_11 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_11, pitchBytes_11, cpp_arg_19, cpp_arg_20, currentText_11, clipRect_11);
                                            }
                                        }
                                        Font->ColorTagsEnabled = true;
                                        Font->DefaultColor = TextColor;
                                        {
                                            std::int32_t textLeft_2 = TextLeft;
                                            const pas::WideString& currentText_12 = Lines->GetCurrentText();
                                            WindowsSdk::TRect clipRect_12 = ClipRect;
                                            void* pixels_12 = GR_Main::ScreenRenderBuffer->GetPixels();
                                            std::int32_t pitchBytes_12 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                            Font->DrawTaggedText16(pixels_12, pitchBytes_12, textLeft_2, Y, currentText_12, clipRect_12);
                                        }
                                    } else if (TextAlignX == GI_Main::taxRight) {
                                        Font->ColorTagsEnabled = false;
                                        Bounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, 0, nullptr);
                                        if (TextShadowOffset > 0) {
                                            Font->DefaultColor = TextShadowColor;
                                            {
                                                std::int32_t cpp_arg_21 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2 + TextShadowOffset;
                                                std::int32_t cpp_arg_22 = Y + TextShadowOffset;
                                                const pas::WideString& currentText_13 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_13 = ClipRect;
                                                void* pixels_13 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_13 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_13, pitchBytes_13, cpp_arg_21, cpp_arg_22, currentText_13, clipRect_13);
                                            }
                                        }
                                        if (TextBorderWidth > 0) {
                                            Font->DefaultColor = TextBorderColor;
                                            {
                                                std::int32_t cpp_arg_23 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2 - TextBorderWidth;
                                                std::int32_t cpp_arg_24 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_14 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_14 = ClipRect;
                                                void* pixels_14 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_14 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_14, pitchBytes_14, cpp_arg_23, cpp_arg_24, currentText_14, clipRect_14);
                                            }
                                            {
                                                std::int32_t cpp_arg_25 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2 + TextBorderWidth;
                                                std::int32_t cpp_arg_26 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_15 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_15 = ClipRect;
                                                void* pixels_15 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_15 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_15, pitchBytes_15, cpp_arg_25, cpp_arg_26, currentText_15, clipRect_15);
                                            }
                                            {
                                                std::int32_t cpp_arg_27 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2 - TextBorderWidth;
                                                std::int32_t cpp_arg_28 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_16 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_16 = ClipRect;
                                                void* pixels_16 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_16 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_16, pitchBytes_16, cpp_arg_27, cpp_arg_28, currentText_16, clipRect_16);
                                            }
                                            {
                                                std::int32_t cpp_arg_29 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2 + TextBorderWidth;
                                                std::int32_t cpp_arg_30 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_17 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_17 = ClipRect;
                                                void* pixels_17 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_17 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_17, pitchBytes_17, cpp_arg_29, cpp_arg_30, currentText_17, clipRect_17);
                                            }
                                        }
                                        Font->ColorTagsEnabled = true;
                                        Font->DefaultColor = TextColor;
                                        {
                                            std::int32_t cpp_arg_31 = HitTestBounds.Right - (Bounds.Right - Bounds.Left) - 2;
                                            const pas::WideString& currentText_18 = Lines->GetCurrentText();
                                            WindowsSdk::TRect clipRect_18 = ClipRect;
                                            void* pixels_18 = GR_Main::ScreenRenderBuffer->GetPixels();
                                            std::int32_t pitchBytes_18 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                            Font->DrawTaggedText16(pixels_18, pitchBytes_18, cpp_arg_31, Y, currentText_18, clipRect_18);
                                        }
                                    } else if (TextAlignX == GI_Main::taxCenter) {
                                        Font->ColorTagsEnabled = false;
                                        Bounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, 0, nullptr);
                                        if (TextShadowOffset > 0) {
                                            Font->DefaultColor = TextShadowColor;
                                            {
                                                std::int32_t cpp_arg_32 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2 + TextShadowOffset;
                                                std::int32_t cpp_arg_33 = Y + TextShadowOffset;
                                                const pas::WideString& currentText_19 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_19 = ClipRect;
                                                void* pixels_19 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_19 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_19, pitchBytes_19, cpp_arg_32, cpp_arg_33, currentText_19, clipRect_19);
                                            }
                                        }
                                        if (TextBorderWidth > 0) {
                                            Font->DefaultColor = TextBorderColor;
                                            {
                                                std::int32_t cpp_arg_34 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2 - TextBorderWidth;
                                                std::int32_t cpp_arg_35 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_20 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_20 = ClipRect;
                                                void* pixels_20 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_20 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_20, pitchBytes_20, cpp_arg_34, cpp_arg_35, currentText_20, clipRect_20);
                                            }
                                            {
                                                std::int32_t cpp_arg_36 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2 + TextBorderWidth;
                                                std::int32_t cpp_arg_37 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_21 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_21 = ClipRect;
                                                void* pixels_21 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_21 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_21, pitchBytes_21, cpp_arg_36, cpp_arg_37, currentText_21, clipRect_21);
                                            }
                                            {
                                                std::int32_t cpp_arg_38 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2 - TextBorderWidth;
                                                std::int32_t cpp_arg_39 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_22 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_22 = ClipRect;
                                                void* pixels_22 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_22 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_22, pitchBytes_22, cpp_arg_38, cpp_arg_39, currentText_22, clipRect_22);
                                            }
                                            {
                                                std::int32_t cpp_arg_40 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2 + TextBorderWidth;
                                                std::int32_t cpp_arg_41 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_23 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_23 = ClipRect;
                                                void* pixels_23 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_23 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_23, pitchBytes_23, cpp_arg_40, cpp_arg_41, currentText_23, clipRect_23);
                                            }
                                        }
                                        Font->ColorTagsEnabled = true;
                                        Font->DefaultColor = TextColor;
                                        {
                                            std::int32_t cpp_arg_42 = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - (Bounds.Right - Bounds.Left) / 2;
                                            const pas::WideString& currentText_24 = Lines->GetCurrentText();
                                            WindowsSdk::TRect clipRect_24 = ClipRect;
                                            void* pixels_24 = GR_Main::ScreenRenderBuffer->GetPixels();
                                            std::int32_t pitchBytes_24 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                            Font->DrawTaggedText16(pixels_24, pitchBytes_24, cpp_arg_42, Y, currentText_24, clipRect_24);
                                        }
                                    } else if (TextAlignX == GI_Main::taxAuto && static_cast<std::uint8_t>(Lines->IsAtLast() ^ 1)) {
                                        Font->ColorTagsEnabled = false;
                                        if (TextShadowOffset > 0) {
                                            Font->DefaultColor = TextShadowColor;
                                            {
                                                std::int32_t cpp_arg_43 = TextLeft + TextShadowOffset;
                                                std::int32_t cpp_arg_44 = Y + TextShadowOffset;
                                                const pas::WideString& currentText_25 = Lines->GetCurrentText();
                                                std::int32_t cpp_arg_45 = ClientSize.X - 4;
                                                WindowsSdk::TRect clipRect_25 = ClipRect;
                                                void* pixels_25 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_25 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_25, pitchBytes_25, cpp_arg_43, cpp_arg_44, currentText_25, cpp_arg_45, clipRect_25);
                                            }
                                        }
                                        if (TextBorderWidth > 0) {
                                            Font->DefaultColor = TextBorderColor;
                                            {
                                                std::int32_t cpp_arg_46 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_47 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_26 = Lines->GetCurrentText();
                                                std::int32_t cpp_arg_48 = ClientSize.X - 4;
                                                WindowsSdk::TRect clipRect_26 = ClipRect;
                                                void* pixels_26 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_26 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_26, pitchBytes_26, cpp_arg_46, cpp_arg_47, currentText_26, cpp_arg_48, clipRect_26);
                                            }
                                            {
                                                std::int32_t cpp_arg_49 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_50 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_27 = Lines->GetCurrentText();
                                                std::int32_t cpp_arg_51 = ClientSize.X - 4;
                                                WindowsSdk::TRect clipRect_27 = ClipRect;
                                                void* pixels_27 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_27 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_27, pitchBytes_27, cpp_arg_49, cpp_arg_50, currentText_27, cpp_arg_51, clipRect_27);
                                            }
                                            {
                                                std::int32_t cpp_arg_52 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_53 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_28 = Lines->GetCurrentText();
                                                std::int32_t cpp_arg_54 = ClientSize.X - 4;
                                                WindowsSdk::TRect clipRect_28 = ClipRect;
                                                void* pixels_28 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_28 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_28, pitchBytes_28, cpp_arg_52, cpp_arg_53, currentText_28, cpp_arg_54, clipRect_28);
                                            }
                                            {
                                                std::int32_t cpp_arg_55 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_56 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_29 = Lines->GetCurrentText();
                                                std::int32_t cpp_arg_57 = ClientSize.X - 4;
                                                WindowsSdk::TRect clipRect_29 = ClipRect;
                                                void* pixels_29 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_29 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_29, pitchBytes_29, cpp_arg_55, cpp_arg_56, currentText_29, cpp_arg_57, clipRect_29);
                                            }
                                        }
                                        Font->ColorTagsEnabled = true;
                                        Font->DefaultColor = TextColor;
                                        {
                                            std::int32_t textLeft_3 = TextLeft;
                                            const pas::WideString& currentText_30 = Lines->GetCurrentText();
                                            std::int32_t cpp_arg_58 = ClientSize.X - 4;
                                            WindowsSdk::TRect clipRect_30 = ClipRect;
                                            void* pixels_30 = GR_Main::ScreenRenderBuffer->GetPixels();
                                            std::int32_t pitchBytes_30 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                            EC_CacheFont::TCFontEC_DrawJustifiedTaggedText16(Font, pixels_30, pitchBytes_30, textLeft_3, Y, currentText_30, cpp_arg_58, clipRect_30);
                                        }
                                    } else {
                                        Font->ColorTagsEnabled = false;
                                        if (TextShadowOffset > 0) {
                                            Font->DefaultColor = TextShadowColor;
                                            {
                                                std::int32_t cpp_arg_59 = TextLeft + TextShadowOffset;
                                                std::int32_t cpp_arg_60 = Y + TextShadowOffset;
                                                const pas::WideString& currentText_31 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_31 = ClipRect;
                                                void* pixels_31 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_31 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_31, pitchBytes_31, cpp_arg_59, cpp_arg_60, currentText_31, clipRect_31);
                                            }
                                        }
                                        if (TextBorderWidth > 0) {
                                            Font->DefaultColor = TextBorderColor;
                                            {
                                                std::int32_t cpp_arg_61 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_62 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_32 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_32 = ClipRect;
                                                void* pixels_32 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_32 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_32, pitchBytes_32, cpp_arg_61, cpp_arg_62, currentText_32, clipRect_32);
                                            }
                                            {
                                                std::int32_t cpp_arg_63 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_64 = Y - TextBorderWidth;
                                                const pas::WideString& currentText_33 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_33 = ClipRect;
                                                void* pixels_33 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_33 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_33, pitchBytes_33, cpp_arg_63, cpp_arg_64, currentText_33, clipRect_33);
                                            }
                                            {
                                                std::int32_t cpp_arg_65 = TextLeft - TextBorderWidth;
                                                std::int32_t cpp_arg_66 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_34 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_34 = ClipRect;
                                                void* pixels_34 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_34 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_34, pitchBytes_34, cpp_arg_65, cpp_arg_66, currentText_34, clipRect_34);
                                            }
                                            {
                                                std::int32_t cpp_arg_67 = TextLeft + TextBorderWidth;
                                                std::int32_t cpp_arg_68 = Y + TextBorderWidth;
                                                const pas::WideString& currentText_35 = Lines->GetCurrentText();
                                                WindowsSdk::TRect clipRect_35 = ClipRect;
                                                void* pixels_35 = GR_Main::ScreenRenderBuffer->GetPixels();
                                                std::int32_t pitchBytes_35 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                                Font->DrawTaggedText16(pixels_35, pitchBytes_35, cpp_arg_67, cpp_arg_68, currentText_35, clipRect_35);
                                            }
                                        }
                                        Font->ColorTagsEnabled = true;
                                        Font->DefaultColor = TextColor;
                                        {
                                            std::int32_t textLeft_4 = TextLeft;
                                            const pas::WideString& currentText_36 = Lines->GetCurrentText();
                                            WindowsSdk::TRect clipRect_36 = ClipRect;
                                            void* pixels_36 = GR_Main::ScreenRenderBuffer->GetPixels();
                                            std::int32_t pitchBytes_36 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                            Font->DrawTaggedText16(pixels_36, pitchBytes_36, textLeft_4, Y, currentText_36, clipRect_36);
                                        }
                                    }
                                    Y += Font->GetLineHeight();
                                    Lines->Next();
                                }
                                TextLines->Next();
                            }
                            pas::free(Lines);
                        }
                        if (BorderEnabled) {
                            GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left, BorderLightColor, ClipRect);
                            GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Bottom - HitTestBounds.Top, BorderLightColor, ClipRect);
                            GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Right - HitTestBounds.Left - 1), BorderDarkColor, ClipRect);
                            GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Bottom - HitTestBounds.Top - 1), BorderDarkColor, ClipRect);
                        }
                        UpdateEmbeddedControls(Font);
                        RemoveUnusedEmbeddedControls(Font);
                    }
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                if (Font != nullptr) {
                    FontCache->Release();
                }
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
            }
        }
        GI_MessageLoop::TObjectGI::Draw(ClipRect);
    }

    void TLabelGI::QueueImageLoad(pas::List* PendingLoads) {
        FontCache->QueueLoadIfMissing(PendingLoads);
    }

    void TLabelGI::p_destroy() {
        GI_Label::TLabelGI_Destroy(this);
    }

    void TLabelGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Label::TLabelGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Label
