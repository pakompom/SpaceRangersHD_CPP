#include "layout/EC_Cache.hpp"
#include "types/EC_CacheAlphaBitmap.hpp"
#include "types/EC_CacheBitmap.hpp"
#include "types/EC_CacheGAI.hpp"
#include "types/EC_CacheGI.hpp"
#include "types/EC_CachePlanetTempl.hpp"
#include "types/EC_CacheSound.hpp"
#include "types/EC_CacheTBitmap.hpp"
#include "types/EC_Data.hpp"
#include "types/SystemImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_Cache {
    void EvictMainMenuShipCachesWhenAddressSpaceHigh() {
        std::int32_t i{};
        TCacheControlEC* Control{};
        GR_Main::TMemoryStatusEx Status{};
        Status.Length = static_cast<std::int32_t>(sizeof(GR_Main::TMemoryStatusEx));
        GR_Main::GlobalMemoryStatusEx(Status);
        if (Status.TotalVirtual - Status.AvailVirtual >= 0x30000000) {
            Control = pas::construct_call<TCacheControlEC>(TCacheControlEC_Create);
            TCacheEC::ResetControl(Control);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 3); cpp_range.next(i); ) {
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.2ShipA", SysUtils::IntToStr(i)})));
                Control->EvictData(pas::class_ref<EC_CacheGAI::TCGaiEC>());
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 3); cpp_range_2.next(i); ) {
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.2Ship", SysUtils::IntToStr(i)})));
                Control->EvictData(pas::class_ref<EC_CacheGI::TCGiEC>());
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, 3); cpp_range_3.next(i); ) {
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.AnimGaalShip0", SysUtils::IntToStr(i), "A"})));
                Control->EvictData(pas::class_ref<EC_CacheGAI::TCGaiEC>());
            }
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, 3); cpp_range_4.next(i); ) {
                Control->SetCacheKey(static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormMain3.AnimGaalShip0", SysUtils::IntToStr(i)})));
                Control->EvictData(pas::class_ref<EC_CacheGI::TCGiEC>());
            }
            Control->SetCacheKey(u"Bm.FormMain3.2BG"_wref.get());
            Control->EvictData(pas::class_ref<EC_CacheGI::TCGiEC>());
            pas::free(Control);
        }
    }

    void EvictRuinsAndGovernmentCaches() {
        EC_Cache::EvictBlockChildrenFromCache(u"Bm.FormRuins"_w, pas::class_ref<EC_CacheGAI::TCGaiEC>());
        EC_Cache::EvictBlockChildrenFromCache(u"Bm.GovHD"_w, pas::class_ref<EC_CacheGAI::TCGaiEC>());
        EC_Cache::EvictBlockChildrenFromCache(u"Bm.Gov"_w, pas::class_ref<EC_CacheGAI::TCGaiEC>());
    }

    void EvictStarAndBackgroundCaches() {
        EC_Cache::EvictBlockChildrenFromCache(u"Bm.Star"_w, pas::class_ref<EC_CacheGAI::TCGaiEC>());
        EC_Cache::EvictBlockChildrenFromCache(u"Bm.BGO"_w, pas::class_ref<EC_CacheGAI::TCGaiEC>());
    }

    void EvictBlockChildrenFromCache(pas::WideString BlockPath, TCacheDataClass CacheDataClass) {
        std::int32_t i{};
        EC_Data::TDataEC* Data{};
        Data = GR_Main::CacheDataRoot;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(BlockPath, u"."_wref.get()) - 1); cpp_range.next(i); ) {
            Data = Data->GetData(EC_Str::ExtractDelimitedPartW(BlockPath, i, u"."_wref.get()));
        }
        TCacheControlEC* Control = pas::construct_call<TCacheControlEC>(TCacheControlEC_Create);
        TCacheEC::ResetControl(Control);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Data->IndexedEntryCount - 1); cpp_range_2.next(i); ) {
            Control->SetCacheKey(pas::concat_wide({BlockPath, u".", Data->IndexedEntries[i]->Name}));
            Control->EvictData(CacheDataClass);
        }
        pas::free(Control);
    }

    void TCacheControlEC_Create(TCacheControlEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TCacheControlEC_Destroy(TCacheControlEC* Self) {
        Self->Reset();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TCacheControlEC::Reset() {
        RetainCount = 0;
        if (BoundData != nullptr) {
            pas::critical_enter(GR_Main::GlobalCache->CacheLock);
            BoundData->UnlinkControl(this);
            pas::critical_leave(GR_Main::GlobalCache->CacheLock);
            BoundData = nullptr;
        }
        CacheKey = pas::WideString();
    }

    void TCacheControlEC::SetCacheKey(const pas::WideString& NewKey) {
        Reset();
        CacheKey = NewKey;
        if (GlobalsV::FontSmoothingEnabled) {
            if (NewKey == GlobalsV::SmallFontName) {
                CacheKey = GlobalsV::SmoothSmallFontName;
            } else if (NewKey == GlobalsV::SmallBoldFontName) {
                CacheKey = GlobalsV::SmoothSmallBoldFontName;
            } else if (NewKey == GlobalsV::NormalFontName) {
                CacheKey = GlobalsV::SmoothNormalFontName;
            } else if (NewKey == GlobalsV::NormalBoldFontName) {
                CacheKey = GlobalsV::SmoothNormalBoldFontName;
            }
        }
    }

    std::uint8_t TCacheControlEC::HasEmptyCacheKey() {
        return CacheKey.length() < 1;
    }

    void TCacheControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
    }

    TCacheDataEC* TCacheControlEC::CreateData() {
        return nullptr;
    }

    TCacheDataEC* TCacheControlEC::AcquireDataFromConfig(TCacheDataClass CacheDataClass) {
        TCacheDataEC* Data{};
        EC_Buf::TBufEC* Buffer{};
        std::int32_t PartCount{};
        pas::WideString Path{};
        pas::WideString LoadOption{};
        if (GlobalsV::HardwareRenderingEnabled && static_cast<std::uint8_t>(GR_DX::TextureManagerDisabled ^ 1)) {
            GR_DX::EvictTextureCaches(false);
        }
        if (RetainCount > 0) {
            ++RetainCount;
        } else {
            pas::critical_enter(GR_Main::GlobalCache->CacheLock);
            if (BoundData == nullptr) {
                BoundData = GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, CacheDataClass);
                if (BoundData == nullptr) {
                    Data = CreateData();
                    Data->CacheKey = CacheKey;
                    if (GR_Main::CacheLoadLoggingEnabled) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Cache Add=", Data->CacheKey})));
                    }
                    Data->LoadCompleteEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
                    Data->AppendControl(this);
                    BoundData = Data;
                    RetainCount = 1;
                    GR_Main::GlobalCache->AddDataToLruHead(Data);
                    pas::critical_leave(GR_Main::GlobalCache->CacheLock);
                    PartCount = EC_Str::CountDelimitedPartsW(CacheKey, u"?"_wref.get());
                    if (PartCount < 2) {
                        Path = CacheKey;
                        LoadOption = pas::WideString();
                    } else {
                        Path = EC_Str::ExtractDelimitedPartW(CacheKey, 0, u"?"_wref.get());
                        LoadOption = EC_Str::ExtractDelimitedRangeW(CacheKey, 1, PartCount - 1, u"?"_wref.get());
                    }
                    Buffer = GR_Main::GlobalCache->OpenDataBuffer(Path);
                    {
                        try {
                            Data->LoadFromConfigBuffer(Buffer, LoadOption);
                        } catch (...) {
                            pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                            GR_Main::GlobalCache->ResidentBytes += Data->ResidentBytes;
                            WindowsSdk::SetEvent(Data->LoadCompleteEvent);
                            WindowsImports::CloseHandle(Data->LoadCompleteEvent);
                            Data->LoadCompleteEvent = 0u;
                            pas::free(Buffer);
                            throw;
                        }
                        pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                        GR_Main::GlobalCache->ResidentBytes += Data->ResidentBytes;
                        WindowsSdk::SetEvent(Data->LoadCompleteEvent);
                        WindowsImports::CloseHandle(Data->LoadCompleteEvent);
                        Data->LoadCompleteEvent = 0u;
                        pas::free(Buffer);
                    }
                } else {
                    if (BoundData->LoadCompleteEvent != 0) {
                        pas::critical_leave(GR_Main::GlobalCache->CacheLock);
                        WindowsSdk::WaitForSingleObject(BoundData->LoadCompleteEvent, WindowsSdk::INFINITE);
                        pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                    }
                    BoundData->AppendControl(this);
                    RetainCount = 1;
                }
            } else {
                RetainCount = 1;
            }
            GR_Main::GlobalCache->TouchData(BoundData);
            pas::critical_leave(GR_Main::GlobalCache->CacheLock);
        }
        TCacheDataEC* Result = BoundData;
        GR_Main::GlobalCache->TrimToBudget(GR_Main::GlobalCache->ResidentByteLimit);
        return Result;
    }

    TCacheDataEC* TCacheControlEC::AcquireDataFromDirectKey(TCacheDataClass CacheDataClass) {
        TCacheDataEC* Data{};
        if (GlobalsV::HardwareRenderingEnabled) {
            GR_DX::EvictTextureCaches(false);
        }
        if (RetainCount > 0) {
            ++RetainCount;
        } else {
            pas::critical_enter(GR_Main::GlobalCache->CacheLock);
            if (BoundData == nullptr) {
                BoundData = GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, CacheDataClass);
                if (BoundData == nullptr) {
                    Data = CreateData();
                    Data->CacheKey = CacheKey;
                    if (GR_Main::CacheLoadLoggingEnabled) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Cache Add=", Data->CacheKey})));
                    }
                    Data->AppendControl(this);
                    Data->LoadCompleteEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
                    BoundData = Data;
                    RetainCount = 1;
                    GR_Main::GlobalCache->AddDataToLruHead(Data);
                    pas::critical_leave(GR_Main::GlobalCache->CacheLock);
                    {
                        try {
                            Data->LoadFromKey(CacheKey);
                        } catch (...) {
                            pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                            GR_Main::GlobalCache->ResidentBytes += Data->ResidentBytes;
                            WindowsSdk::SetEvent(Data->LoadCompleteEvent);
                            WindowsImports::CloseHandle(Data->LoadCompleteEvent);
                            Data->LoadCompleteEvent = 0u;
                            throw;
                        }
                        pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                        GR_Main::GlobalCache->ResidentBytes += Data->ResidentBytes;
                        WindowsSdk::SetEvent(Data->LoadCompleteEvent);
                        WindowsImports::CloseHandle(Data->LoadCompleteEvent);
                        Data->LoadCompleteEvent = 0u;
                    }
                } else {
                    if (BoundData->LoadCompleteEvent != 0) {
                        pas::critical_leave(GR_Main::GlobalCache->CacheLock);
                        WindowsSdk::WaitForSingleObject(BoundData->LoadCompleteEvent, WindowsSdk::INFINITE);
                        pas::critical_enter(GR_Main::GlobalCache->CacheLock);
                    }
                    BoundData->AppendControl(this);
                    RetainCount = 1;
                }
            } else {
                RetainCount = 1;
            }
            GR_Main::GlobalCache->TouchData(BoundData);
            pas::critical_leave(GR_Main::GlobalCache->CacheLock);
        }
        TCacheDataEC* Result = BoundData;
        GR_Main::GlobalCache->TrimToBudget(GR_Main::GlobalCache->ResidentByteLimit);
        return Result;
    }

    TCacheDataEC* TCacheControlEC::AcquireData() {
        return nullptr;
    }

    void TCacheControlEC::Release() {
        if (RetainCount > 0) {
            --RetainCount;
        } else {
            RetainCount = 0;
        }
    }

    void TCacheControlEC::EvictData(TCacheDataClass CacheDataClass) {
        if (RetainCount > 0) {
            return;
        }
        pas::critical_enter(GR_Main::GlobalCache->CacheLock);
        if (BoundData == nullptr) {
            BoundData = GR_Main::GlobalCache->FindDataByKeyAndClass(CacheKey, CacheDataClass);
        }
        if (BoundData != nullptr) {
            GR_Main::GlobalCache->ResidentBytes -= BoundData->ResidentBytes;
            GR_Main::GlobalCache->RemoveAndFreeData(BoundData);
            BoundData = nullptr;
        }
        pas::critical_leave(GR_Main::GlobalCache->CacheLock);
    }

    void TCacheDataEC_Create(TCacheDataEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TCacheDataEC_Destroy(TCacheDataEC* Self) {
        while (Self->FirstBoundControl != nullptr) {
            Self->UnlinkControl(Self->LastBoundControl);
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TCacheDataEC::AppendControl(TCacheControlEC* Control) {
        if (LastBoundControl != nullptr) {
            LastBoundControl->NextBoundControl = Control;
        }
        Control->PrevBoundControl = LastBoundControl;
        Control->NextBoundControl = nullptr;
        LastBoundControl = Control;
        if (FirstBoundControl == nullptr) {
            FirstBoundControl = Control;
        }
    }

    void TCacheDataEC::UnlinkControl(TCacheControlEC* Control) {
        if (Control->PrevBoundControl != nullptr) {
            Control->PrevBoundControl->NextBoundControl = Control->NextBoundControl;
        }
        if (Control->NextBoundControl != nullptr) {
            Control->NextBoundControl->PrevBoundControl = Control->PrevBoundControl;
        }
        if (LastBoundControl == Control) {
            LastBoundControl = Control->PrevBoundControl;
        }
        if (FirstBoundControl == Control) {
            FirstBoundControl = Control->NextBoundControl;
        }
        Control->PrevBoundControl = nullptr;
        Control->NextBoundControl = nullptr;
        Control->BoundData = nullptr;
    }

    void TCacheDataEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
    }

    void TCacheDataEC::LoadFromKey(const pas::WideString& Key) {
    }

    void TCacheEC_Create(TCacheEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->CacheLock = pas::make_critical_section<pas::CriticalSection>();
    }

    void TCacheEC_Destroy(TCacheEC* Self) {
        Self->Clear();
        pas::free(Self->CacheLock);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TCacheEC::Clear() {
        while (MostRecentData != nullptr) {
            RemoveAndFreeData(LeastRecentData);
        }
        ResidentBytes = 0;
    }

    void TCacheEC::SetDataRoot(EC_Data::TDataEC* Root) {
        Clear();
        DataRoot = Root;
    }

    void TCacheEC::ResetControl(TCacheControlEC* Control) {
        Control->Reset();
    }

    void TCacheEC::AddDataToLruHead(TCacheDataEC* Data) {
        if (MostRecentData != nullptr) {
            MostRecentData->PrevData = Data;
        }
        Data->PrevData = nullptr;
        Data->NextData = MostRecentData;
        MostRecentData = Data;
        if (LeastRecentData == nullptr) {
            LeastRecentData = Data;
        }
    }

    void TCacheEC::RemoveAndFreeData(TCacheDataEC* Data) {
        if (Data != nullptr) {
            if (Data->PrevData != nullptr) {
                Data->PrevData->NextData = Data->NextData;
            }
            if (Data->NextData != nullptr) {
                Data->NextData->PrevData = Data->PrevData;
            }
            if (LeastRecentData == Data) {
                LeastRecentData = Data->PrevData;
            }
            if (MostRecentData == Data) {
                MostRecentData = Data->NextData;
            }
            pas::free(Data);
        }
    }

    TCacheDataEC* TCacheEC::FindDataByKeyAndClass(const pas::WideString& Key, TCacheDataClass CacheDataClass) {
        TCacheDataEC* Data = MostRecentData;
        while (Data != nullptr) {
            if (pas::class_type(Data) == CacheDataClass) {
                if (Data->CacheKey == Key) {
                    return Data;
                }
            }
            Data = Data->NextData;
        }
        return nullptr;
    }

    void TCacheEC::TouchData(TCacheDataEC* Data) {
        if (Data != MostRecentData) {
            if (Data->PrevData != nullptr) {
                Data->PrevData->NextData = Data->NextData;
            }
            if (Data->NextData != nullptr) {
                Data->NextData->PrevData = Data->PrevData;
            }
            if (LeastRecentData == Data) {
                LeastRecentData = Data->PrevData;
            }
            if (MostRecentData == Data) {
                MostRecentData = Data->NextData;
            }
            AddDataToLruHead(Data);
        }
    }

    EC_Buf::TBufEC* TCacheEC::OpenDataBuffer(const pas::WideString& Path) {
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        DataRoot->ReadBufferByPath(Path, Buffer);
        return Buffer;
    }

    void TCacheEC::TrimToBudget(std::int32_t BudgetBytes) {
        TCacheDataEC* Removed{};
        TCacheControlEC* Control{};
        pas::critical_enter(CacheLock);
        std::int32_t RemainingBytes = ResidentBytes;
        if (RemainingBytes < BudgetBytes) {
            pas::critical_leave(CacheLock);
            return;
        }
        GR_DX::EvictTextureCaches(true);
        TCacheDataEC* Data = LeastRecentData;
        while (Data != nullptr && RemainingBytes >= BudgetBytes) {
            Removed = Data;
            Data = Data->PrevData;
            Control = Removed->FirstBoundControl;
            while (Control != nullptr) {
                if (Control->RetainCount > 0) {
                    break;
                }
                Control = Control->NextBoundControl;
            }
            if (Control != nullptr) {
                continue;
            }
            RemainingBytes -= Removed->ResidentBytes;
            ResidentBytes -= Removed->ResidentBytes + 0;
            RemoveAndFreeData(Removed);
        }
        pas::critical_leave(CacheLock);
    }

    void TCacheEC::QueueNamedLoadIfMissing(pas::List* PendingLoads, const pas::WideString& CacheKind, const pas::WideString& Key) {
        EC_CacheAlphaBitmap::TCAlphaBitmapControlEC* AlphaBitmap{};
        EC_CacheBitmap::TCBitmapControlEC* Bitmap{};
        EC_CacheTBitmap::TCTBitmapControlEC* TBitmap{};
        EC_CacheSound::TCSoundControlEC* Sound{};
        EC_CacheGAI::TCGaiControlEC* Gai{};
        EC_CacheGI::TCGiControlEC* Gi{};
        EC_CachePlanetTempl::TCPlanetTemplControlEC* PlanetTempl{};
        if (CacheKind == u"Alpha") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheAlphaBitmap::TCAlphaBitmapEC>()) == nullptr) {
                AlphaBitmap = pas::construct_call<EC_CacheAlphaBitmap::TCAlphaBitmapControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(AlphaBitmap);
                AlphaBitmap->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(AlphaBitmap));
            }
        } else if (CacheKind == u"Bitmap") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheBitmap::TCBitmapEC>()) == nullptr) {
                Bitmap = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(Bitmap);
                Bitmap->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(Bitmap));
            }
        } else if (CacheKind == u"Trans") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheTBitmap::TCTBitmapEC>()) == nullptr) {
                TBitmap = pas::construct_call<EC_CacheTBitmap::TCTBitmapControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(TBitmap);
                TBitmap->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(TBitmap));
            }
        } else if (CacheKind == u"GI") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheGI::TCGiEC>()) == nullptr) {
                Gi = pas::construct_call<EC_CacheGI::TCGiControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(Gi);
                Gi->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(Gi));
            }
        } else if (CacheKind == u"GAI") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheGAI::TCGaiEC>()) == nullptr) {
                Gai = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(Gai);
                Gai->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(Gai));
            }
        } else if (CacheKind == u"Sound") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CacheSound::TCSoundEC>()) == nullptr) {
                Sound = pas::construct_call<EC_CacheSound::TCSoundControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(Sound);
                Sound->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(Sound));
            }
        } else if (CacheKind == u"PlanetTempl") {
            if (FindDataByKeyAndClass(Key, pas::class_ref<EC_CachePlanetTempl::TCPlanetTemplEC>()) == nullptr) {
                PlanetTempl = pas::construct_call<EC_CachePlanetTempl::TCPlanetTemplControlEC>(TCacheControlEC_Create);
                TCacheEC::ResetControl(PlanetTempl);
                PlanetTempl->SetCacheKey(Key);
                pas::list_add(PendingLoads, reinterpret_cast<void*>(PlanetTempl));
            }
        }
    }

    void TCacheDataEC::p_destroy() {
        EC_Cache::TCacheDataEC_Destroy(this);
    }

    void TCacheControlEC::p_destroy() {
        EC_Cache::TCacheControlEC_Destroy(this);
    }

    void TCacheEC::p_destroy() {
        EC_Cache::TCacheEC_Destroy(this);
    }

} // namespace EC_Cache
