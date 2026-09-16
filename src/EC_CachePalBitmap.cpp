#include "layout/EC_CachePalBitmap.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CachePalBitmap.hpp"
#include "units/GR_GraphBufPal.hpp"
#include "units/GR_Main.hpp"

namespace EC_CachePalBitmap {
    TCPalBitmapEC* AcquireOrCreatePalBitmap(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCPalBitmapEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCPalBitmapEC>()));
    }

    void TCPalBitmapControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCPalBitmapControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCPalBitmapEC>()) == nullptr) {
            Control = pas::construct_call<TCPalBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCPalBitmapControlEC::CreateData() {
        return pas::construct_call<TCPalBitmapEC>(TCPalBitmapEC_Create);
    }

    EC_Cache::TCacheDataEC* TCPalBitmapControlEC::AcquireData() {
        return EC_CachePalBitmap::AcquireOrCreatePalBitmap(this);
    }

    void TCPalBitmapEC_Create(TCPalBitmapEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->Bitmap = pas::construct_call<GR_GraphBufPal::TGraphBufPalGR>(GR_GraphBufPal::TGraphBufPalGR_Create);
    }

    void TCPalBitmapEC_Destroy(TCPalBitmapEC* Self) {
        if (Self->Bitmap != nullptr) {
            pas::free(Self->Bitmap);
            Self->Bitmap = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCPalBitmapEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        Bitmap->LoadImage(SourceBuffer);
        ResidentBytes = Bitmap->PitchBytes * Bitmap->Height + Bitmap->PaletteCount * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
    }

    void TCPalBitmapEC::p_destroy() {
        EC_CachePalBitmap::TCPalBitmapEC_Destroy(this);
    }

} // namespace EC_CachePalBitmap
