#include "layout/EC_CacheTBitmap.hpp"
#include "types/EC_Buf.hpp"
#include "types/Types.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheTBitmap.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"

namespace EC_CacheTBitmap {
    TCTBitmapEC* AcquireCachedTransBitmap(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCTBitmapEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCTBitmapEC>()));
    }

    void TCTBitmapControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCTBitmapControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCTBitmapEC>()) == nullptr) {
            Control = pas::construct_call<TCTBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCTBitmapControlEC::CreateData() {
        return pas::construct_call<TCTBitmapEC>(TCTBitmapEC_Create);
    }

    EC_Cache::TCacheDataEC* TCTBitmapControlEC::AcquireData() {
        return EC_CacheTBitmap::AcquireCachedTransBitmap(this);
    }

    void TCTBitmapEC_Create(TCTBitmapEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
    }

    void TCTBitmapEC_Destroy(TCTBitmapEC* Self) {
        if (Self->TransBuffer != nullptr) {
            pas::free_mem(Self->TransBuffer);
            Self->TransBuffer = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    // Applies LoadOption image operations before building the transparent buffer.
    void TCTBitmapEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        GR_GraphBuf::TGraphBufGR* Bitmap = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Bitmap->LoadImage(SourceBuffer);
        Bitmap->ApplyOperations(LoadOption);
        std::int32_t height = Bitmap->Height;
        void* pixels = Bitmap->GetPixels();
        std::int32_t width = Bitmap->Width;
        std::int32_t pitchBytes = Bitmap->PitchBytes;
        std::uint32_t ByteCount = GR_Main::Ex_OKGR_TransBuf_Build_WORD(pixels, pitchBytes, width, height, nullptr, 0);
        if (ByteCount < 1) {
            pas::raise(pas::make_exception<pas::Exception>("TCTBitmapEC.Load. Error load file."_a));
        }
        pas::get_mem_at(&TransBuffer, static_cast<std::int32_t>(ByteCount));
        {
            std::int32_t height_2 = Bitmap->Height;
            void* transBuffer = TransBuffer;
            void* pixels_2 = Bitmap->GetPixels();
            std::int32_t width_2 = Bitmap->Width;
            std::int32_t pitchBytes_2 = Bitmap->PitchBytes;
            GR_Main::Ex_OKGR_TransBuf_Build_WORD(pixels_2, pitchBytes_2, width_2, height_2, transBuffer, 0);
        }
        ResidentBytes = ByteCount;
        PixelSize.X = Bitmap->Width;
        PixelSize.Y = Bitmap->Height;
        pas::free(Bitmap);
    }

    void TCTBitmapEC::p_destroy() {
        EC_CacheTBitmap::TCTBitmapEC_Destroy(this);
    }

} // namespace EC_CacheTBitmap
