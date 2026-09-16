#include "layout/EC_CacheBuf.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/GR_Main.hpp"

namespace EC_CacheBuf {
    // Rewinds the shared buffer.
    TCBufEC* AcquireOrCreateBuffer(EC_Cache::TCacheControlEC* Control) {
        TCBufEC* Result = pas::checked_cast<TCBufEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCBufEC>()));
        Result->Buffer->SetPosition(0);
        return Result;
    }

    void TCBufControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCBufControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCBufEC>()) == nullptr) {
            Control = pas::construct_call<TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCBufControlEC::CreateData() {
        return pas::construct_call<TCBufEC>(TCBufEC_Create);
    }

    EC_Cache::TCacheDataEC* TCBufControlEC::AcquireData() {
        return EC_CacheBuf::AcquireOrCreateBuffer(this);
    }

    void TCBufEC_Create(TCBufEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
    }

    void TCBufEC_Destroy(TCBufEC* Self) {
        if (Self->Buffer != nullptr) {
            pas::free(Self->Buffer);
            Self->Buffer = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    // Ignores LoadOption; ResidentBytes is not updated.
    void TCBufEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddBytes(SourceBuffer->Data, SourceBuffer->DataSize);
    }

    void TCBufEC::p_destroy() {
        EC_CacheBuf::TCBufEC_Destroy(this);
    }

} // namespace EC_CacheBuf
