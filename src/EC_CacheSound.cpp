#include "layout/EC_CacheSound.hpp"
#include "types/EC_Buf.hpp"
#include "types/MMSystemSdk.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheSound.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_Main.hpp"

namespace EC_CacheSound {
    TCSoundEC* AcquireCachedSound(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCSoundEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCSoundEC>()));
    }

    void TCSoundControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCSoundControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, pas::class_ref<TCSoundEC>()) == nullptr) {
            Control = pas::construct_call<TCSoundControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCSoundControlEC::CreateData() {
        return pas::construct_call<TCSoundEC>(TCSoundEC_Create);
    }

    EC_Cache::TCacheDataEC* TCSoundControlEC::AcquireData() {
        return EC_CacheSound::AcquireCachedSound(this);
    }

    void TCSoundEC_Create(TCSoundEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
    }

    void TCSoundEC_Destroy(TCSoundEC* Self) {
        if (Self->SampleData != nullptr) {
            EC_Mem::FreeEC(Self->SampleData);
            Self->SampleData = nullptr;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCSoundEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        std::int32_t Offset{};
        TWaveFileHeader Header{};
        SourceBuffer->ReadBytes(&Header, static_cast<std::int32_t>(sizeof(TWaveFileHeader)));
        if (Header.DataId != WaveDataChunkId) {
            Offset = 0;
            while (SourceBuffer->DataSize - WaveChunkHeaderSize > Offset) {
                if (SourceBuffer->GetUInt32At(Offset) == WaveDataChunkId) {
                    break;
                }
                ++Offset;
            }
            if (SourceBuffer->DataSize - WaveChunkHeaderSize <= Offset) {
                GR_Main::RaiseWideMessage(u"WAVE format"_wref.get());
            }
            Header.DataId = WaveDataChunkId;
            Header.DataSize = SourceBuffer->GetUInt32At(Offset + static_cast<std::int32_t>(sizeof(std::uint32_t)));
            SourceBuffer->SetPosition(Offset + WaveChunkHeaderSize);
        }
        Format.FormatTag = MMSystemSdk::WAVE_FORMAT_PCM;
        Format.Channels = Header.Channels;
        Format.SamplesPerSecond = Header.SamplesPerSecond;
        Format.BitsPerSample = Header.BitsPerSample;
        Format.BlockAlign = Header.BlockAlign;
        Format.AverageBytesPerSecond = Format.BlockAlign * Format.SamplesPerSecond;
        Format.ExtraSize = 0;
        SampleDataSize = Header.DataSize;
        if (SampleData != nullptr) {
            EC_Mem::FreeEC(SampleData);
            SampleData = nullptr;
        }
        SampleData = EC_Mem::AllocEC(SampleDataSize);
        SourceBuffer->ReadBytes(SampleData, SampleDataSize);
    }

    void TCSoundEC::p_destroy() {
        EC_CacheSound::TCSoundEC_Destroy(this);
    }

} // namespace EC_CacheSound
