#include "layout/GI_Edit.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheFont.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Edit.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_Edit {
    void TEditGI_Create(TEditGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->FontCache = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->FontCache);
        Self->BackgroundCache = nullptr;
        Self->TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        Self->CaretColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
        Self->BorderEnabled = false;
        Self->TextAlignX = GI_Main::taxLeft;
        Self->MaxLength = 256;
        Self->ClearFocusOnEnter = true;
    }

    void TEditGI_Destroy(TEditGI* Self) {
        pas::free(Self->FontCache);
        Self->FontCache = nullptr;
        pas::free(Self->BackgroundCache);
        Self->BackgroundCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TEditGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
        HasFocus = false;
        MaxLength = 256;
        TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        CaretColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
        AutoScrollText = false;
        Text = pas::WideString();
    }

    void TEditGI::SetFontName(pas::WideString FontName) {
        FontCache->SetCacheKey(FontName);
        Invalidate();
    }

    void TEditGI::SetBorderEnabled(std::uint8_t Value) {
        if (Value != BorderEnabled) {
            BorderEnabled = Value;
            Invalidate();
        }
    }

    // Resets CaretPosition on change; does not clamp to MaxLength or invoke ChangedCallback.
    void TEditGI::SetText(pas::WideString Value) {
        if (Text != Value) {
            Text = std::move(Value);
            CaretPosition = 0;
            Invalidate();
        }
    }

    std::uint8_t TEditGI::HasGlyph(char16_t Character) {
        std::uint8_t Result{};
        EC_CacheFont::TCFontEC* Font{};
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                FontCache->Release();
            };
            Font = EC_CacheFont::AcquireCachedFont(FontCache);
            Font->ResetTextMeasureState();
            Result = Font->HasGlyph(Character);
        }
        return Result;
    }

    void TEditGI::SetTextColor(std::uint32_t Value) {
        if (TextColor != Value) {
            TextColor = Value;
            Invalidate();
        }
    }

    void TEditGI::SetBorderLightColor(std::uint32_t Value) {
        if (BorderLightColor != Value) {
            BorderLightColor = Value;
            Invalidate();
        }
    }

    void TEditGI::SetBorderDarkColor(std::uint32_t Value) {
        if (BorderDarkColor != Value) {
            BorderDarkColor = Value;
            Invalidate();
        }
    }

    // Only Left and Center are accepted; other values raise.
    void TEditGI::SetTextAlignX(GI_Main::TTextAlignXGI Value) {
        if (Value != GI_Main::taxLeft && Value != GI_Main::taxCenter) {
            pas::raise(pas::make_exception<pas::Exception>("Error TEditGI. This align not support."_a));
        }
        if (TextAlignX != Value) {
            TextAlignX = Value;
            Invalidate();
        }
    }

    // Clamps to 0..Length(Text).
    void TEditGI::SetCaretPosition(std::int32_t Value) {
        if (Value > Text.length()) {
            CaretPosition = Text.length();
        } else if (Value < 0) {
            CaretPosition = 0;
        } else {
            CaretPosition = Value;
        }
        Invalidate();
    }

    void TEditGI::ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (Active == true) {
            MessageLoop->SetFocusedControl(this);
        }
    }

    void TEditGI::OnFocusGained() {
        GI_MessageLoop::TObjectGI::OnFocusGained();
        HasFocus = true;
        CaretPosition = Text.length();
        Invalidate();
    }

    void TEditGI::OnFocusLost() {
        GI_MessageLoop::TObjectGI::OnFocusLost();
        HasFocus = false;
        if (pas::assigned(FocusLostCallback)) {
            FocusLostCallback(this);
        }
        Invalidate();
    }

    void TEditGI::ProcessKeyDown(std::int32_t Key) {
        std::int32_t I{};
        std::int32_t N{};
        GI_MessageLoop::TObjectGI* Control{};
        if (Key == WindowsSdk::VK_BACK && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1)) {
            if (CaretPosition > 0) {
                N = Text.length();
                I = CaretPosition;
                while (I < N) {
                    Text.write(I) = Text.read(I + 1);
                    ++I;
                }
                Text.set_length(N - 1);
                --CaretPosition;
                Invalidate();
                DispatchNamedEvent(4, 0, 0);
                if (pas::assigned(ChangedCallback)) {
                    ChangedCallback(this);
                }
            }
        } else if (Key == WindowsSdk::VK_BACK && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
            Text = pas::WideString();
            CaretPosition = 0;
            Invalidate();
            DispatchNamedEvent(4, 0, 0);
            if (pas::assigned(ChangedCallback)) {
                ChangedCallback(this);
            }
        } else if (Key == WindowsSdk::VK_DELETE) {
            N = Text.length();
            if (CaretPosition < N) {
                I = CaretPosition + 1;
                while (I < N) {
                    Text.write(I) = Text.read(I + 1);
                    ++I;
                }
                Text.set_length(N - 1);
                Invalidate();
                DispatchNamedEvent(4, 0, 0);
                if (pas::assigned(ChangedCallback)) {
                    ChangedCallback(this);
                }
            }
        } else if (Key == WindowsSdk::VK_LEFT) {
            if (CaretPosition > 0) {
                --CaretPosition;
                Invalidate();
            }
        } else if (Key == WindowsSdk::VK_RIGHT) {
            if (CaretPosition < Text.length()) {
                ++CaretPosition;
                Invalidate();
            }
        } else if (Key == WindowsSdk::VK_HOME) {
            CaretPosition = 0;
            Invalidate();
        } else if (Key == WindowsSdk::VK_END) {
            CaretPosition = Text.length();
            Invalidate();
        } else if (Key == WindowsSdk::VK_RETURN && ClearFocusOnEnter) {
            MessageLoop->SetFocusedControl(nullptr);
        } else if (Key == WindowsSdk::VK_TAB && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
            Control = NextSibling;
            while (Control != nullptr) {
                if (pas::class_cast_if<TEditGI*>(Control) != nullptr && Control->Active) {
                    MessageLoop->SetFocusedControl(Control);
                    break;
                }
                Control = Control->NextSibling;
            }
        } else if (Key == WindowsSdk::VK_TAB) {
            Control = PrevSibling;
            while (Control != nullptr) {
                if (pas::class_cast_if<TEditGI*>(Control) != nullptr && Control->Active) {
                    MessageLoop->SetFocusedControl(Control);
                    break;
                }
                Control = Control->PrevSibling;
            }
        }
    }

    // Requires a font glyph, acceptance by the optional callback, and length below MaxLength.
    void TEditGI::ProcessCharacter(char16_t Character) {
        std::int32_t I{};
        std::int32_t N{};
        GI_MessageLoop::TObjectGI::ProcessCharacter(Character);
        if (HasGlyph(Character)) {
            if (static_cast<std::uint8_t>(pas::assigned(AcceptCharCallback) ^ 1) || AcceptCharCallback(this, Character)) {
                N = Text.length();
                if (N < MaxLength) {
                    Text.set_length(N + 1);
                    I = N;
                    while (I >= CaretPosition) {
                        Text.write(I + 1) = Text.read(I);
                        --I;
                    }
                    Text.write(CaretPosition + 1) = Character;
                    ++CaretPosition;
                    Invalidate();
                    DispatchNamedEvent(4, 0, 0);
                    if (pas::assigned(ChangedCallback)) {
                        ChangedCallback(this);
                    }
                }
            }
        }
    }

    void TEditGI::OnCaretBlink() {
        Invalidate();
    }

    void TEditGI_LoadFromConfigPath(TEditGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString ColorText{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            Self->FontCache->SetCacheKey(Block->GetParam(u"Font"sv));
        }
        if (Block->CountParams(u"Text"_wref.get()) > 0) {
            Self->Text = Block->GetParam(u"Text"sv);
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Self->Text) > 0) {
                Self->Text = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(Self->Text);
            }
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Self->BackgroundCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Self->BackgroundCache);
            Self->BackgroundCache->SetCacheKey(Block->GetParam(u"Image"sv));
        }
        if (Block->CountParams(u"TextColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"TextColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            Self->TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"Border"_wref.get()) > 0) {
            if (Block->GetParam(u"Border"sv) == u"True") {
                Self->BorderEnabled = true;
            } else {
                Self->BorderEnabled = false;
            }
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"BorderLightColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            Self->BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
            Self->BorderDarkColor = Self->BorderLightColor;
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"BorderDarkColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            Self->BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"CursorColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"CursorColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            Self->CaretColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"MaxLen"_wref.get()) > 0) {
            Self->MaxLength = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxLen"sv)));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            GI_Main::TTextAlignXGI parseTextAlignXName = GI_Main::ParseTextAlignXName(pas::view(EC_Str::TrimWideString(Block->GetParam(u"AlignX"sv))));
            TEditGI* self = Self;
            self->SetTextAlignX(parseTextAlignXName);
        }
    }

    void TEditGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString ColorText{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        FontCache->SetCacheKey(Block->GetParam(u"Font"sv));
        if (Block->CountParams(u"ReturnFocusLeave"_wref.get()) > 0) {
            ClearFocusOnEnter = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(Block->GetParam(u"ReturnFocusLeave"sv))));
        }
        if (Block->CountParams(u"Text"_wref.get()) > 0) {
            Text = Block->GetParam(u"Text"sv);
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Text) > 0) {
                Text = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(Text);
            }
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            BackgroundCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(BackgroundCache);
            BackgroundCache->SetCacheKey(Block->GetParam(u"Image"sv));
        }
        if (Block->CountParams(u"TextColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"TextColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            TextColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"Border"_wref.get()) > 0) {
            if (Block->GetParam(u"Border"sv) == u"True") {
                BorderEnabled = true;
            } else {
                BorderEnabled = false;
            }
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"BorderLightColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
            BorderDarkColor = BorderLightColor;
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"BorderDarkColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"CursorColor"_wref.get()) > 0) {
            ColorText = Block->GetParam(u"CursorColor"sv);
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
            CaretColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"MaxLen"_wref.get()) > 0) {
            MaxLength = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxLen"sv)));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            GI_Main::TTextAlignXGI parseTextAlignXName = GI_Main::ParseTextAlignXName(pas::view(EC_Str::TrimWideString(Block->GetParam(u"AlignX"sv))));
            TEditGI* self = this;
            self->SetTextAlignX(parseTextAlignXName);
        }
    }

    void TEditGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t N{};
        std::int32_t I{};
        std::int32_t Advance{};
        std::int32_t FirstCharacter{};
        std::int32_t Width{};
        std::uint8_t Fits{};
        pas::WideString CharacterText{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        WindowsSdk::TRect cpp_with{};
        EC_CacheFont::TCFontEC* Font = nullptr;
        EC_CacheBitmap::TCBitmapEC* Bitmap = nullptr;
        if (FontCache == nullptr) {
            return;
        }
        if (GlobalsV::FontSmoothingEnabled) {
            if (FontCache->CacheKey == GlobalsV::SmallFontName) {
                FontCache->SetCacheKey(GlobalsV::SmoothSmallFontName);
            } else if (FontCache->CacheKey == GlobalsV::SmallBoldFontName) {
                FontCache->SetCacheKey(GlobalsV::SmoothSmallBoldFontName);
            } else if (FontCache->CacheKey == GlobalsV::NormalFontName) {
                FontCache->SetCacheKey(GlobalsV::SmoothNormalFontName);
            } else if (FontCache->CacheKey == GlobalsV::NormalBoldFontName) {
                FontCache->SetCacheKey(GlobalsV::SmoothNormalBoldFontName);
            }
        } else if (FontCache->CacheKey == GlobalsV::SmoothSmallFontName) {
            FontCache->SetCacheKey(GlobalsV::SmallFontName);
        } else if (FontCache->CacheKey == GlobalsV::SmoothSmallBoldFontName) {
            FontCache->SetCacheKey(GlobalsV::SmallBoldFontName);
        } else if (FontCache->CacheKey == GlobalsV::SmoothNormalFontName) {
            FontCache->SetCacheKey(GlobalsV::NormalFontName);
        } else if (FontCache->CacheKey == GlobalsV::SmoothNormalBoldFontName) {
            FontCache->SetCacheKey(GlobalsV::NormalBoldFontName);
        }
        {
            std::exception_ptr cpp_error{};
            try {
                Font = EC_CacheFont::AcquireCachedFont(FontCache);
                Font->ResetTextMeasureState();
                Font->DefaultColor = TextColor;
                if (BackgroundCache != nullptr) {
                    Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(BackgroundCache);
                    if (GlobalsV::HardwareRenderingEnabled) {
                        pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture = (Bitmap->Bitmap->GetTexture(cpp_result), cpp_result);
                        std::int32_t top = HitTestBounds.Top;
                        std::int32_t left = HitTestBounds.Left;
                        GR_DX::DrawTexture(texture, left, top, 255, GR_DX::RgbWhite, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                    } else {
                        std::int32_t top_2 = ClipRect.Top;
                        void* pixels = Bitmap->Bitmap->GetPixels();
                        std::int32_t pitchBytes = Bitmap->Bitmap->PitchBytes;
                        std::int32_t cpp_arg = ClipRect.Left - HitTestBounds.Left;
                        std::int32_t cpp_arg_2 = ClipRect.Top - HitTestBounds.Top;
                        std::int32_t cpp_arg_3 = ClipRect.Right - ClipRect.Left;
                        std::int32_t cpp_arg_4 = ClipRect.Bottom - ClipRect.Top;
                        void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t left_2 = ClipRect.Left;
                        std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels_2, pitchBytes_2, left_2, top_2, pixels, pitchBytes, cpp_arg, cpp_arg_2, cpp_arg_3, cpp_arg_4);
                    }
                }
                X = HitTestBounds.Left + 2;
                cpp_with = Font->MeasureTaggedTextBounds(Text, 0, 0, nullptr);
                if (TextAlignX == GI_Main::taxCenter) {
                    X = HitTestBounds.Left + (HitTestBounds.Right - HitTestBounds.Left) / 2 - (cpp_with.Right - cpp_with.Left) / 2;
                }
                Y = (HitTestBounds.Top + HitTestBounds.Bottom) / 2 - (Font->AboveBaseline + Font->BelowBaseline) / 2 + Font->AboveBaseline;
                N = Text.length();
                FirstCharacter = 0;
                if (AutoScrollText) {
                    do {
                        Fits = true;
                        Width = 0;
                        for (auto cpp_range = pas::for_to<std::int32_t>(FirstCharacter, N - 1); cpp_range.next(I); ) {
                            Width += EC_CacheFont::TCFontEC_GetGlyphAdvance(Font, Text.read(I + 1));
                            if (CaretPosition >= I && Width >= ClientSize.X - 2) {
                                ++FirstCharacter;
                                Fits = false;
                                break;
                            }
                        }
                    } while (!Fits);
                }
                if (FirstCharacter > N) {
                    FirstCharacter = N;
                }
                Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
                if (GlobalsV::HardwareRenderingEnabled) {
                    Buffer->AllocateRgbaTight(HitTestBounds.Right - HitTestBounds.Left, HitTestBounds.Bottom - HitTestBounds.Top);
                    Font->UseARGBColors = true;
                    Font->DefaultColor = GR_DX::ColorWithAlpha(GR_DX::Color565ToArgb(TextColor), 255u);
                }
                CharacterText.set_length(1);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(FirstCharacter, N); cpp_range_2.next(I); ) {
                    Advance = 0;
                    if (I < N) {
                        Advance = EC_CacheFont::TCFontEC_GetGlyphAdvance(Font, Text.read(I + 1));
                        CharacterText.write(1) = Text.read(I + 1);
                        if (GlobalsV::HardwareRenderingEnabled) {
                            Buffer->ClearPixels();
                            {
                                std::int32_t cpp_arg_5 = X - HitTestBounds.Left;
                                std::int32_t cpp_arg_6 = Y - HitTestBounds.Top;
                                auto characterText = pas::borrow(CharacterText);
                                WindowsSdk::TRect rect = ClassesImports::Rect(0, 0, Buffer->Width, Buffer->Height);
                                void* pixels_3 = Buffer->GetPixels();
                                std::int32_t pitchBytes_3 = Buffer->PitchBytes;
                                Font->DrawTaggedText32(pixels_3, pitchBytes_3, cpp_arg_5, cpp_arg_6, characterText.get(), rect);
                            }
                            {
                                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture_2 = (Buffer->GetTexture(cpp_result_2), cpp_result_2);
                                std::int32_t top_3 = HitTestBounds.Top;
                                std::int32_t left_3 = HitTestBounds.Left;
                                GR_DX::DrawTexture(texture_2, left_3, top_3, 255, GR_DX::RgbWhite, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                            }
                        } else {
                            auto characterText_2 = pas::borrow(CharacterText);
                            WindowsSdk::TRect clipRect = ClipRect;
                            void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                            std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                            Font->DrawTaggedText16(pixels_4, pitchBytes_4, X, Y, characterText_2.get(), clipRect);
                        }
                    }
                    if (HasFocus == true && CaretPosition == I && MessageLoop->CaretBlinkOn == true) {
                        if (GlobalsV::HardwareRenderingEnabled) {
                            GR_DX::DrawAlphaLine(X, Y - (Font->AboveBaseline - 1), X, Y - (Font->AboveBaseline - 1) + Font->AboveBaseline + Font->BelowBaseline, GR_DX::Color565ToArgb(CaretColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                            GR_DX::DrawAlphaLine(X + 1, Y - (Font->AboveBaseline - 1), X + 1, Y - (Font->AboveBaseline - 1) + Font->AboveBaseline + Font->BelowBaseline, GR_DX::Color565ToArgb(CaretColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                        } else {
                            GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(X, Y - (Font->AboveBaseline - 1), Font->AboveBaseline + Font->BelowBaseline, CaretColor, ClipRect);
                            GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(X + 1, Y - (Font->AboveBaseline - 1), Font->AboveBaseline + Font->BelowBaseline, CaretColor, ClipRect);
                        }
                    }
                    X += Advance;
                }
                if (Buffer != nullptr) {
                    pas::free(Buffer);
                }
                if (BorderEnabled) {
                    if (GlobalsV::HardwareRenderingEnabled) {
                        GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Top, GR_DX::Color565ToArgb(BorderLightColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                        GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Left, HitTestBounds.Bottom - 1, GR_DX::Color565ToArgb(BorderLightColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                        GR_DX::DrawAlphaLine(HitTestBounds.Left, HitTestBounds.Bottom - 1, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, GR_DX::Color565ToArgb(BorderDarkColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                        GR_DX::DrawAlphaLine(HitTestBounds.Right - 1, HitTestBounds.Top, HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, GR_DX::Color565ToArgb(BorderDarkColor), 255, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                    } else {
                        GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left, BorderLightColor, ClipRect);
                        GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Bottom - HitTestBounds.Top, BorderLightColor, ClipRect);
                        GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Right - HitTestBounds.Left - 1), BorderDarkColor, ClipRect);
                        GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Bottom - HitTestBounds.Top - 1), BorderDarkColor, ClipRect);
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Font != nullptr) {
                FontCache->Release();
            }
            if (Bitmap != nullptr) {
                BackgroundCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TEditGI::QueueImageLoad(pas::List* PendingLoads) {
        FontCache->QueueLoadIfMissing(PendingLoads);
        if (BackgroundCache != nullptr) {
            BackgroundCache->QueueLoadIfMissing(PendingLoads);
        }
    }

    void TEditGI::p_destroy() {
        GI_Edit::TEditGI_Destroy(this);
    }

    void TEditGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Edit::TEditGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Edit
