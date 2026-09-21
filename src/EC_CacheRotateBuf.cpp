#include "layout/EC_CacheRotateBuf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheRotateBuf.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace EC_CacheRotateBuf {
    TCRotateBufEC* AcquireOrCreateRotateBuf(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCRotateBufEC*>(Control->AcquireDataFromDirectKey(pas::class_ref<TCRotateBufEC>()));
    }

    void TCRotateBufControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCRotateBufControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(pas::view(CacheKey), pas::class_ref<TCRotateBufEC>()) == nullptr) {
            Control = pas::construct_call<TCRotateBufControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCRotateBufControlEC::CreateData() {
        return pas::construct_call<TCRotateBufEC>(TCRotateBufEC_Create);
    }

    EC_Cache::TCacheDataEC* TCRotateBufControlEC::AcquireData() {
        return EC_CacheRotateBuf::AcquireOrCreateRotateBuf(this);
    }

    void TCRotateBufEC_Create(TCRotateBufEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->Buffer = nullptr;
    }

    void TCRotateBufEC_Destroy(TCRotateBufEC* Self) {
        if (Self->Buffer != nullptr) {
            GR_Main::Ex_OKGR_RotateBuf_Free(Self->Buffer);
            Self->Buffer = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    // Key contains width,height,source width,source height,center X,center Y as comma-delimited integers.
    void TCRotateBufEC::LoadFromKey(const pas::WideString& Key) {
        if (EC_Str::CountDelimitedPartsW(pas::view(Key), u","sv) != 6) {
            pas::raise(pas::make_exception<pas::Exception>("TCRotateBufEC.Load. Error create rotate buf."_a));
        }
        Buffer = ([&] {
            std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 3, u","sv)));
            std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 4, u","sv)));
            std::int32_t strToInt_3 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 5, u","sv)));
            std::int32_t strToInt_4 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 2, u","sv)));
            std::int32_t strToInt_5 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 1, u","sv)));
            std::int32_t strToInt_6 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Key), 0, u","sv)));
            return GR_Main::Ex_OKGR_RotateBuf_Build(strToInt_6, strToInt_5, strToInt_4, strToInt, strToInt_2, strToInt_3);
        }());
        if (Buffer == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("TCRotateBufEC.Load. Error create rotate buf."_a));
        }
    }

    void TCRotateBufEC::p_destroy() {
        EC_CacheRotateBuf::TCRotateBufEC_Destroy(this);
    }

} // namespace EC_CacheRotateBuf
