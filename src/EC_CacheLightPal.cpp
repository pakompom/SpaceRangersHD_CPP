#include "layout/EC_CacheLightPal.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheLightPal.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"

namespace EC_CacheLightPal {
    TCLightPalEC* AcquireOrCreateLightPalette(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCLightPalEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCLightPalEC>()));
    }

    void GetLightPaletteMaskInfo(std::uint32_t Mask, System::PCardinal Shift, System::PCardinal BitCount, System::PCardinal LevelCount) {
        pas::store_unaligned<std::uint32_t>(Shift, 0u);
        pas::store_unaligned<std::uint32_t>(BitCount, 0u);
        pas::store_unaligned<std::uint32_t>(LevelCount, 0u);
        if (Mask != 0) {
            while ((Mask & 1) == 0) {
                pas::inc_unaligned<std::uint32_t>(Shift, 1);
                Mask = Mask >> 1;
            }
            while ((Mask & 1) != 0) {
                pas::inc_unaligned<std::uint32_t>(BitCount, 1);
                Mask = Mask >> 1;
            }
            pas::store_unaligned<std::uint32_t>(LevelCount, static_cast<std::uint32_t>(pas::shl(1, pas::load_unaligned<std::uint32_t>(BitCount))));
        }
    }

    WindowsSdk::PWORD BuildLightPalette(System::PCardinal Palette, std::int32_t ColorCount, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask) {
        static constexpr std::int32_t BrightnessBits = 6;
        static constexpr std::int32_t BrightnessLevels = 1 << BrightnessBits;
        std::uint32_t RShift{};
        std::uint32_t RBits{};
        std::uint32_t RLevels{};
        std::uint32_t GShift{};
        std::uint32_t GBits{};
        std::uint32_t GLevels{};
        std::uint32_t BShift{};
        std::uint32_t BBits{};
        std::uint32_t BLevels{};
        double Red{};
        double Green{};
        double Blue{};
        std::int32_t ColorIndex{};
        std::int32_t Brightness{};
        std::uint32_t R{};
        std::uint32_t G{};
        std::uint32_t B{};
        double Factor{};
        EC_CacheLightPal::GetLightPaletteMaskInfo(RedMask, &RShift, &RBits, &RLevels);
        std::uint32_t RDiscard = 8 - RBits;
        EC_CacheLightPal::GetLightPaletteMaskInfo(GreenMask, &GShift, &GBits, &GLevels);
        std::uint32_t GDiscard = 8 - GBits;
        EC_CacheLightPal::GetLightPaletteMaskInfo(BlueMask, &BShift, &BBits, &BLevels);
        std::uint32_t BDiscard = 8 - BBits;
        WindowsSdk::PWORD First = static_cast<WindowsSdk::PWORD>(EC_Mem::AllocEC(pas::shl(ColorCount, BrightnessBits) * static_cast<std::int32_t>(sizeof(std::uint16_t))));
        WindowsSdk::PWORD Dest = First;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorCount - 1); cpp_range.next(ColorIndex); ) {
            Red = pas::load_unaligned<std::uint32_t>(Palette) & 0x000000ff;
            Green = pas::load_unaligned<std::uint32_t>(Palette) >> 8 & 0x000000ff;
            Blue = pas::load_unaligned<std::uint32_t>(Palette) >> 16 & 0x000000ff;
            Brightness = 0;
            do {
                Factor = pas::real_divide(Brightness, pas::constant(static_cast<long double>(BrightnessLevels - 1)));
                R = System::Trunc(static_cast<long double>(Red) * Factor);
                G = System::Trunc(static_cast<long double>(Green) * Factor);
                B = System::Trunc(static_cast<long double>(Blue) * Factor);
                pas::store_unaligned<std::uint16_t>(Dest, static_cast<std::uint16_t>(pas::shl(static_cast<std::uint16_t>(pas::shr(R, RDiscard)), RShift) | pas::shl(static_cast<std::uint16_t>(pas::shr(G, GDiscard)), GShift) | pas::shl(static_cast<std::uint16_t>(pas::shr(B, BDiscard)), BShift)));
                Dest = reinterpret_cast<WindowsSdk::PWORD>(reinterpret_cast<std::uint8_t*>(Dest) + static_cast<std::int32_t>(sizeof(std::uint16_t)));
                ++Brightness;
            } while (!(Brightness == BrightnessLevels));
            Palette = reinterpret_cast<System::PCardinal>(reinterpret_cast<std::uint8_t*>(Palette) + static_cast<std::int32_t>(sizeof(std::uint32_t)));
        }
        return First;
    }

    void FreeLightPalette(WindowsSdk::PWORD Palette) {
        if (Palette != nullptr) {
            EC_Mem::FreeEC(Palette);
        }
    }

    void TCLightPalControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCLightPalControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCLightPalEC>()) == nullptr) {
            Control = pas::construct_call<TCLightPalControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCLightPalControlEC::CreateData() {
        return pas::construct_call<TCLightPalEC>(TCLightPalEC_Create);
    }

    EC_Cache::TCacheDataEC* TCLightPalControlEC::AcquireData() {
        return EC_CacheLightPal::AcquireOrCreateLightPalette(this);
    }

    void TCLightPalEC_Create(TCLightPalEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->PaletteData = nullptr;
    }

    void TCLightPalEC_Destroy(TCLightPalEC* Self) {
        if (Self->PaletteData != nullptr) {
            EC_CacheLightPal::FreeLightPalette(Self->PaletteData);
            Self->PaletteData = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCLightPalEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        GR_GraphBufPal::TGraphBufPalGR* Bitmap = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Bitmap = pas::construct_call<GR_GraphBufPal::TGraphBufPalGR>(GR_GraphBufPal::TGraphBufPalGR_Create);
                Bitmap->LoadImage(SourceBuffer);
                if (Bitmap->PaletteCount < 1 || Bitmap->PaletteCount > 256 || Bitmap->Palette == nullptr) {
                    pas::raise(pas::make_exception<pas::Exception>("TCLightPalEC.Load. In error create light palette. "_a));
                }
                PaletteData = EC_CacheLightPal::BuildLightPalette(reinterpret_cast<System::PCardinal>(Bitmap->Palette), Bitmap->PaletteCount, GR_Main::CurrentPixelFormat->RedMask, GR_Main::CurrentPixelFormat->GreenMask, GR_Main::CurrentPixelFormat->BlueMask);
                if (PaletteData == nullptr) {
                    pas::raise(pas::make_exception<pas::Exception>("TCLightPalEC.Load. Out error create light palette."_a));
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Bitmap != nullptr) {
                pas::free(Bitmap);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TCLightPalEC::p_destroy() {
        EC_CacheLightPal::TCLightPalEC_Destroy(this);
    }

} // namespace EC_CacheLightPal
