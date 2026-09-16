#include "layout/EC_CacheBitmap.hpp"
#include "types/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"

namespace EC_CacheBitmap {
    TCBitmapEC* AcquireOrCreateBitmap(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCBitmapEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCBitmapEC>()));
    }

    void TCBitmapControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCBitmapControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCBitmapEC>()) == nullptr) {
            Control = pas::construct_call<TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCBitmapControlEC::CreateData() {
        return pas::construct_call<TCBitmapEC>(TCBitmapEC_Create);
    }

    EC_Cache::TCacheDataEC* TCBitmapControlEC::AcquireData() {
        return EC_CacheBitmap::AcquireOrCreateBitmap(this);
    }

    void TCBitmapEC_Create(TCBitmapEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->Bitmap = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
    }

    void TCBitmapEC_Destroy(TCBitmapEC* Self) {
        if (Self->Bitmap != nullptr) {
            pas::free(Self->Bitmap);
            Self->Bitmap = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCBitmapEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        if (LoadOption == u"RGBA") {
            Bitmap->LoadImageRgba(SourceBuffer);
        } else if (LoadOption == u"Gray") {
            Bitmap->LoadImageGrayscale(SourceBuffer);
        } else if (LoadOption == u"RGB") {
            Bitmap->LoadImageRgb(SourceBuffer);
        } else {
            Bitmap->LoadImage(SourceBuffer);
        }
        ResidentBytes = Bitmap->PitchBytes * Bitmap->Height;
    }

    void TCBitmapEC::p_destroy() {
        EC_CacheBitmap::TCBitmapEC_Destroy(this);
    }

} // namespace EC_CacheBitmap
