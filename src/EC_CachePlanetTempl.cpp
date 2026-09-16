#include "layout/EC_CachePlanetTempl.hpp"
#include "types/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CachePlanetTempl.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"

namespace EC_CachePlanetTempl {
    TCPlanetTemplEC* AcquireOrCreatePlanetTemplate(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCPlanetTemplEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCPlanetTemplEC>()));
    }

    void TCPlanetTemplControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCPlanetTemplControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCPlanetTemplEC>()) == nullptr) {
            Control = pas::construct_call<TCPlanetTemplControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCPlanetTemplControlEC::CreateData() {
        return pas::construct_call<TCPlanetTemplEC>(TCPlanetTemplEC_Create);
    }

    EC_Cache::TCacheDataEC* TCPlanetTemplControlEC::AcquireData() {
        return EC_CachePlanetTempl::AcquireOrCreatePlanetTemplate(this);
    }

    void TCPlanetTemplEC_Create(TCPlanetTemplEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->TemplateData = nullptr;
        Self->ImageHeight = 0;
    }

    void TCPlanetTemplEC_Destroy(TCPlanetTemplEC* Self) {
        if (Self->TemplateData != nullptr) {
            GR_Main::Ex_OKGR_Planet2_TemplDel(Self->TemplateData);
            Self->TemplateData = nullptr;
            Self->ImageHeight = 0;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCPlanetTemplEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        GR_GraphBuf::TGraphBufGR* Bitmap = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Bitmap->LoadImageRgba(SourceBuffer);
        std::int32_t TextureWidth = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(LoadOption, 0, u","_wref.get()));
        std::int32_t TextureHeight = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(LoadOption, 1, u","_wref.get()));
        {
            std::int32_t& residentBytes = ResidentBytes;
            void* pixels = Bitmap->GetPixels();
            std::int32_t height = Bitmap->Height;
            std::int32_t pitchBytes = Bitmap->PitchBytes;
            TemplateData = GR_Main::Ex_OKGR_Planet2_TemplBuild(pixels, pitchBytes, height, TextureWidth, TextureHeight, residentBytes);
        }
        if (TemplateData == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TCPlanetTemplEC.Load. Error create template planet."_a));
        }
        ImageHeight = Bitmap->Height;
        pas::free(Bitmap);
    }

    void TCPlanetTemplEC::p_destroy() {
        EC_CachePlanetTempl::TCPlanetTemplEC_Destroy(this);
    }

} // namespace EC_CachePlanetTempl
