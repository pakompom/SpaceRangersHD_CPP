#include "layout/GI_TextButton.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheFont.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_TextButton.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

// Native class ownership follows reference/unit_ownership.json.
namespace GI_TextButton {
    void TTextButtonGI_Create(TTextButtonGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->FontCache = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->FontCache);
        Self->ImageCache = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->CaptionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->CaptionActiveColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
        Self->BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        Self->Kind = 0;
    }

    void TTextButtonGI_Destroy(TTextButtonGI* Self) {
        pas::free(Self->FontCache);
        Self->FontCache = nullptr;
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TTextButtonGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
        CaptionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        CaptionActiveColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
        BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(55, 55, 55);
        Caption = pas::WideString();
        MouseBlocking = true;
    }

    void TTextButtonGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (HitTestCursor()) {
            Hover = true;
        } else {
            Hover = false;
        }
        if (Kind == 0) {
            Down = false;
        }
        Invalidate();
    }

    void TTextButtonGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        Hover = false;
        Down = false;
        Invalidate();
    }

    void TTextButtonGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        if (!IsOccludedAtPoint(AbsolutePosition)) {
            Hover = true;
            Invalidate();
        }
    }

    void TTextButtonGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        if (Kind == 0) {
            if (Down) {
                Down = false;
                DispatchNamedEvent(2, 0, 0);
                if (pas::assigned(UpCallback)) {
                    UpCallback(this);
                }
            }
        }
        Hover = false;
        Invalidate();
    }

    void TTextButtonGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (IsOccludedAtPoint(Point)) {
            return;
        }
        if (Kind == 0) {
            Down = true;
            DispatchNamedEvent(1, Point.X, Point.Y);
            if (pas::assigned(DownCallback)) {
                DownCallback(this);
            }
        } else if (Down) {
            Down = false;
            DispatchNamedEvent(2, Point.X, Point.Y);
            if (pas::assigned(UpCallback)) {
                UpCallback(this);
            }
        } else {
            Down = true;
            DispatchNamedEvent(1, Point.X, Point.Y);
            if (pas::assigned(DownCallback)) {
                DownCallback(this);
            }
        }
        Invalidate();
    }

    void TTextButtonGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        if (IsOccludedAtPoint(Point)) {
            return;
        }
        if (Kind == 0) {
            Down = false;
            Invalidate();
            DispatchNamedEvent(2, Point.X, Point.Y);
            if (pas::assigned(UpCallback)) {
                if (MessageLoop->ConsumeTimerTickChange()) {
                    UpCallback(this);
                }
            }
        }
    }

    void TTextButtonGI_LoadFromConfigPath(TTextButtonGI* Self, const pas::WideString& Path) {
        EC_BlockPar::TBlockParEC* Block{};
        pas::WideString Text{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            Self->FontCache->SetCacheKey(Block->GetParam(u"Font"_wref.get()));
        }
        if (Block->CountParams(u"Caption"_wref.get()) > 0) {
            Self->Caption = Block->GetParam(u"Caption"_wref.get());
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Self->Caption) > 0) {
                Self->Caption = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(Self->Caption);
            }
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Self->ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"CaptionColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->CaptionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"CaptionActiveColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"CaptionActiveColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->CaptionActiveColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"BorderLightColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"BorderDarkColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            Self->BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            if (Block->GetParam(u"Kind"_wref.get()) == u"Normal") {
                Self->Kind = 0;
            } else {
                Self->Kind = 1;
            }
        }
    }

    void TTextButtonGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        if (Block->CountParams(u"Font"_wref.get()) > 0) {
            FontCache->SetCacheKey(Block->GetParam(u"Font"_wref.get()));
        }
        if (Block->CountParams(u"Caption"_wref.get()) > 0) {
            Caption = Block->GetParam(u"Caption"_wref.get());
            if (GR_Main::LanguageDataConfig->CountParamsByPath(Caption) > 0) {
                Caption = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(Caption);
            }
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            ImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"CaptionColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"CaptionColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            CaptionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"CaptionActiveColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"CaptionActiveColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            CaptionActiveColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"BorderLightColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"BorderLightColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            BorderLightColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"BorderDarkColor"_wref.get()) > 0) {
            Text = Block->GetParam(u"BorderDarkColor"_wref.get());
            Red = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())));
            Green = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get())));
            Blue = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get())));
            BorderDarkColor = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        }
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            if (Block->GetParam(u"Kind"_wref.get()) == u"Normal") {
                Kind = 0;
            } else {
                Kind = 1;
            }
        }
    }

    void TTextButtonGI::Draw(Types::TRect ClipRect) {
        Types::TRect Bounds{};
        EC_CacheFont::TCFontEC* Font = nullptr;
        EC_CacheBitmap::TCBitmapEC* Bitmap = nullptr;
        if (FontCache != nullptr) {
            std::exception_ptr cpp_error{};
            try {
                Font = EC_CacheFont::AcquireCachedFont(FontCache);
                Font->ResetTextMeasureState();
                if (ImageCache != nullptr && ImageCache->CacheKey != u"") {
                    Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(ImageCache);
                }
                Bounds = Font->MeasureTaggedTextBounds(Caption, 0, 0, nullptr);
                if (Bitmap != nullptr) {
                    void* pixels = Bitmap->Bitmap->GetPixels();
                    std::int32_t pitchBytes = Bitmap->Bitmap->PitchBytes;
                    std::int32_t cpp_arg = ClipRect.Left - HitTestBounds.Left;
                    std::int32_t cpp_arg_2 = ClipRect.Top - HitTestBounds.Top;
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels_2, pitchBytes_2, ClipRect.Left, ClipRect.Top, pixels, pitchBytes, cpp_arg, cpp_arg_2, ClipRect.Right - ClipRect.Left, ClipRect.Bottom - ClipRect.Top);
                }
                if (Hover == true) {
                    Font->DefaultColor = CaptionActiveColor;
                } else {
                    Font->DefaultColor = CaptionColor;
                }
                {
                    std::int32_t cpp_arg_3 = (HitTestBounds.Left + HitTestBounds.Right) / 2 - (Bounds.Right - Bounds.Left) / 2;
                    std::int32_t cpp_arg_4 = (HitTestBounds.Top + HitTestBounds.Bottom) / 2 - (Bounds.Bottom - Bounds.Top) / 2 - Bounds.Top;
                    auto caption = pas::borrow(Caption);
                    void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    Font->DrawTaggedText16(pixels_3, pitchBytes_3, cpp_arg_3, cpp_arg_4, caption.get(), ClipRect);
                }
                if (!Down) {
                    GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left, BorderLightColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Bottom - HitTestBounds.Top, BorderLightColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Right - HitTestBounds.Left - 1), BorderDarkColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Bottom - HitTestBounds.Top - 1), BorderDarkColor, ClipRect);
                } else {
                    GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left - 1, BorderDarkColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Left, HitTestBounds.Top, HitTestBounds.Bottom - HitTestBounds.Top - 1, BorderDarkColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawHorizontalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Right - HitTestBounds.Left), BorderLightColor, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawVerticalLine16Clipped(HitTestBounds.Right - 1, HitTestBounds.Bottom - 1, -(HitTestBounds.Bottom - HitTestBounds.Top), BorderLightColor, ClipRect);
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Font != nullptr) {
                FontCache->Release();
            }
            if (Bitmap != nullptr) {
                ImageCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        GI_MessageLoop::TObjectGI::Draw(ClipRect);
    }

    void TTextButtonGI::QueueImageLoad(pas::List* PendingLoads) {
        FontCache->QueueLoadIfMissing(PendingLoads);
        if (ImageCache != nullptr) {
            ImageCache->QueueLoadIfMissing(PendingLoads);
        }
    }

    void TTextButtonGI::p_destroy() {
        GI_TextButton::TTextButtonGI_Destroy(this);
    }

    void TTextButtonGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_TextButton::TTextButtonGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_TextButton
