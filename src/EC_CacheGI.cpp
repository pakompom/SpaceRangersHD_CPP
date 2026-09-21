#include "layout/EC_CacheGI.hpp"
#include "types/EC_Buf.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_gi.hpp"
#include "units/Windows.hpp"

namespace EC_CacheGI {
    TCGiEC* AcquireCachedGi(EC_Cache::TCacheControlEC* Control) {
        return pas::checked_cast<TCGiEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCGiEC>()));
    }

    void TCGiControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCGiControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(pas::view(CacheKey), pas::class_ref<TCGiEC>()) == nullptr) {
            Control = pas::construct_call<TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCGiControlEC::CreateData() {
        return pas::construct_call<TCGiEC>(TCGiEC_Create);
    }

    EC_Cache::TCacheDataEC* TCGiControlEC::AcquireData() {
        return EC_CacheGI::AcquireCachedGi(this);
    }

    // TileCount is zero for a single surface.
    void TCGiEC_Create(TCGiEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->Image = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
        Self->SurfaceCache = nullptr;
        Self->UsesTiledSurfaces = false;
        Self->TileCount = 0;
    }

    void TCGiEC_Destroy(TCGiEC* Self) {
        pas::free(Self->Image);
        if (Self->SurfaceCache != nullptr) {
            GR_DX::FreeTextureCache(Self->SurfaceCache);
            Self->SurfaceCache = nullptr;
            Self->UsesTiledSurfaces = false;
            Self->TileOrigins.set_length(0);
            Self->TileCount = 0;
        }
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    WindowsSdk::TPoint TCGiEC::GetTileOrigin(std::int32_t TileIndex) {
        WindowsSdk::TPoint Result{};
        Result = TileOrigins[TileIndex];
        return Result;
    }

    // Caches the last requested surface at index zero; non-square tile grids use an incorrect stride.
    void TCGiEC::GetOrCreateSurface(std::int32_t SurfaceIndex, Direct3D9::IDirect3DTexture9& Result) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        Direct3D9::IDirect3DTexture9 cpp_result_4{};
        Direct3D9::IDirect3DTexture9 cpp_result_5{};
        Direct3D9::IDirect3DTexture9 cpp_result_6{};
        Direct3D9::IDirect3DTexture9 Texture{};
        WindowsSdk::TPoint ImageSize{};
        Direct3D9::TD3DLockedRect Locked{};
        std::int32_t TileIndex{};
        std::int32_t Columns{};
        std::int32_t Rows{};
        std::int32_t TileWidth{};
        std::int32_t TileHeight{};
        std::int32_t Column{};
        std::int32_t Row{};
        WindowsSdk::TPoint Origin{};
        auto GiTileDivideRoundUp = [&](std::int32_t Value, std::int32_t Divisor) -> std::int32_t {
            return pas::idiv(Divisor - 1 + Value, Divisor);
        };
        if (SurfaceCache == nullptr) {
            SurfaceCache = GR_DX::CreateTextureCache();
        }
        Texture = (SurfaceCache->GetSurface(SurfaceIndex, cpp_result), cpp_result);
        if (Texture == nullptr && Image != nullptr && static_cast<std::uint8_t>(UsesTiledSurfaces ^ 1)) {
            ImageSize = Image->GetContentSize();
            if (ImageSize.X == 0 || ImageSize.Y == 0) {
                Result = nullptr;
                return;
            }
            if (GR_DX::MaxTextureSize.X < ImageSize.X || GR_DX::MaxTextureSize.Y < ImageSize.Y) {
                UsesTiledSurfaces = true;
                Columns = GiTileDivideRoundUp(ImageSize.X, GR_DX::MaxTextureSize.X);
                Rows = GiTileDivideRoundUp(ImageSize.Y, GR_DX::MaxTextureSize.Y);
                TileCount = Columns * Rows;
                TileOrigins.set_length(TileCount);
                Row = 0;
                while (Row < Rows) {
                    Column = 0;
                    if (GR_DX::MaxTextureSize.Y >= ImageSize.Y) {
                        TileHeight = ImageSize.Y;
                    } else if ((Row + 1) * GR_DX::MaxTextureSize.Y > ImageSize.Y) {
                        TileHeight = ImageSize.Y - GR_DX::MaxTextureSize.Y * Row;
                    } else {
                        TileHeight = GR_DX::MaxTextureSize.Y;
                    }
                    while (Column < Columns) {
                        TileIndex = Row * Rows + Column;
                        if (GR_DX::MaxTextureSize.X >= ImageSize.X) {
                            TileWidth = ImageSize.X;
                        } else if ((Column + 1) * GR_DX::MaxTextureSize.X > ImageSize.X) {
                            TileWidth = ImageSize.X - GR_DX::MaxTextureSize.X * Column;
                        } else {
                            TileWidth = GR_DX::MaxTextureSize.X;
                        }
                        if (Image->Header->Format == 0 && Image->Header->AlphaMask == 0) {
                            Texture = (GR_DX::GR_CreateTexture(TileWidth, TileHeight, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result_2), cpp_result_2);
                        } else {
                            Texture = (GR_DX::GR_CreateTexture(TileWidth, TileHeight, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_3), cpp_result_3);
                        }
                        Origin = ClassesImports::Point(GR_DX::MaxTextureSize.X * Column, GR_DX::MaxTextureSize.Y * Row);
                        if (Texture != nullptr) {
                            Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                            if (Locked.Bits != nullptr) {
                                Image->DecodeRawRegion(Locked.Bits, Locked.Pitch, Origin.X, Origin.Y, TileWidth, TileHeight, true);
                                Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                            }
                        }
                        SurfaceCache->SetSurface(Texture, TileIndex);
                        TileOrigins[TileIndex] = Origin;
                        ++Column;
                    }
                    ++Row;
                }
                Texture = (SurfaceCache->GetSurface(SurfaceIndex, cpp_result_4), cpp_result_4);
            } else {
                if (Image->Header->Format == 0 && Image->Header->AlphaMask == 0) {
                    Texture = (GR_DX::GR_CreateTexture(ImageSize.X, ImageSize.Y, Direct3D9::D3DFMT_R5G6B5, Direct3D9::D3DPOOL_MANAGED, cpp_result_5), cpp_result_5);
                } else {
                    Texture = (GR_DX::GR_CreateTexture(ImageSize.X, ImageSize.Y, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result_6), cpp_result_6);
                }
                if (Texture != nullptr) {
                    Direct3D9::IDirect3DTexture9_LockRect(Texture, 0u, Locked, nullptr, 0u);
                    if (Locked.Bits != nullptr) {
                        Image->DecodeToPixels(Locked.Bits, Locked.Pitch, ImageSize.X, ImageSize.Y, true);
                        Direct3D9::IDirect3DTexture9_UnlockRect(Texture, 0u);
                    }
                }
            }
            SurfaceCache->SetSurface(Texture, 0);
        }
        Result = Texture;
        return;
    }

    // May modify SourceBuffer for resource-specific layout fixups. Ignores LoadOption.
    void TCGiEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        TCGiEC::ApplyWideScreenLayoutFixups(SourceBuffer, CacheKey);
        Image->LoadRawGiFromBuffer(SourceBuffer);
        ResidentBytes = Image->DataSize;
    }

    // Modifies SourceBuffer in place.
    void TCGiEC::ApplyWideScreenLayoutFixups(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& ResourceKey) {
        GR_gi::TgiGR* WorkingImage{};
        std::uint8_t Quiet{};
        GR_GraphBuf::TGraphBufGR* SourceGraph{};
        GR_GraphBuf::TGraphBufGR* DestGraph{};
        std::int32_t VerticalAlign{};
        std::int32_t ImageWidth{};
        std::int32_t ImageHeight{};
        GR_gi::PgiHeaderGR Header{};
        void* HeaderBytes{};
        std::int32_t Delta{};
        std::int32_t Remainder{};
        std::uint8_t PreserveAlpha{};
        auto RenderGiBufferToGraphBuf = [&](EC_Buf::TBufEC* SourceBuffer, GR_GraphBuf::TGraphBufGR* DestGraphBuf) -> void {
            WorkingImage = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
            WorkingImage->LoadRawGiFromBuffer(SourceBuffer);
            DestGraphBuf->AllocateRgbaTight(WorkingImage->GetContentSize().X, WorkingImage->GetContentSize().Y);
            WorkingImage->DecodeToGraphBuf(DestGraphBuf, false);
            WorkingImage->ClearData();
            pas::free(WorkingImage);
        };
        auto StoreGraphBufAsRawGiBuffer = [&](EC_Buf::TBufEC* DestBuffer, GR_GraphBuf::TGraphBufGR* SourceGraphBuf, std::int32_t StorageMode) -> void {
            WorkingImage = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
            WorkingImage->CreateFromGraphBuf(SourceGraphBuf, StorageMode);
            DestBuffer->Clear();
            DestBuffer->AddBytes(WorkingImage->Data, WorkingImage->DataSize);
            WorkingImage->ClearData();
            pas::free(WorkingImage);
        };
        auto StoreGraphBufAsGiBuffer = [&](EC_Buf::TBufEC* DestBuffer, GR_GraphBuf::TGraphBufGR* SourceGraphBuf, WindowsSdk::TPoint TopLeft) -> void {
            WorkingImage = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
            WorkingImage->CreateFormat2FromGraphBuf(SourceGraphBuf, TopLeft);
            DestBuffer->Clear();
            DestBuffer->AddBytes(WorkingImage->Data, WorkingImage->DataSize);
            WorkingImage->ClearData();
            pas::free(WorkingImage);
        };
        auto LogWideScreenGiRescaleStart = [&]() -> void {
            if (!Quiet) {
                GR_Main::AppendLogTextThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Rescaling ", ResourceKey, u"... "})));
            }
        };
        auto LogWideScreenGiRescaleDone = [&]() -> void {
            if (!Quiet) {
                GR_Main::AppendLogLineThreadSafe("ok"_a);
            }
        };
        std::int32_t ExtraHeight = GR_Main::ExtraScreenHeight;
        if (ExtraHeight < 0) {
            ExtraHeight = 0;
        }
        Quiet = false;
        if (GR_Main::ExtraScreenWidth > 0 && ResourceKey == pas::concat_wide({u"Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"BG"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, 0, 0, SourceGraph, ClassesImports::Rect(0, 0, SourceGraph->Width, SourceGraph->Height), ClassesImports::Rect(430, 0, 593, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, GR_Main::GameScreenHeight - DestGraph->Height - 1));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if ((GR_Main::ExtraScreenWidth > 0 || ExtraHeight > 0) && ResourceKey == u"Bm.FormMain2.2AnimMain") {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, std::max<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight)), static_cast<std::int64_t>(768)));
            Delta = GR_Main::ExtraScreenWidth / 2 / 3 * 3;
            if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1600) {
                Delta -= 249;
            }
            Remainder = ExtraHeight % 3;
            if (Remainder != 0) {
                Remainder = 3 - Remainder;
            }
            DestGraph->DrawNinePatch(0, 0, Delta, 0, SourceGraph, ClassesImports::Rect(0, Remainder, 3, SourceGraph->Height), ClassesImports::Rect(0, 0, 0, 765 - Remainder));
            DestGraph->DrawNinePatch(Delta, 0, 0, 0, SourceGraph, ClassesImports::Rect(3, Remainder, SourceGraph->Width - 4, SourceGraph->Height), ClassesImports::Rect(1005, 0, 0, 765 - Remainder));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ExtraHeight > 0 && ResourceKey == pas::concat_wide({u"Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"TWin"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Delta = 3;
            Delta = pas::idiv(std::min<std::int32_t>(GR_Main::ExtraScreenHeight, 250), Delta) * Delta;
            DestGraph->AllocateRgbaTight(SourceGraph->Width, SourceGraph->Height + Delta);
            Remainder = Delta / 3 / 4 * 3;
            Delta -= Remainder;
            DestGraph->DrawNinePatch(0, 0, DestGraph->Width, Delta + 530, SourceGraph, ClassesImports::Rect(0, 0, SourceGraph->Width, 530), ClassesImports::Rect(0, 380, 0, 147));
            DestGraph->DrawNinePatch(0, Delta + 530, DestGraph->Width, Remainder + 90, SourceGraph, ClassesImports::Rect(0, 530, SourceGraph->Width, SourceGraph->Height), ClassesImports::Rect(0, 0, 0, 87));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ExtraHeight > 0 && ResourceKey == pas::concat_wide({u"Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"TWinB"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Delta = std::min<std::int32_t>(GR_Main::ExtraScreenHeight, 250) / 3 / 4 * 3;
            DestGraph->AllocateRgbaTight(SourceGraph->Width, SourceGraph->Height + Delta);
            DestGraph->DrawNinePatch(0, 0, DestGraph->Width, DestGraph->Height, SourceGraph, ClassesImports::Rect(0, 0, SourceGraph->Width, SourceGraph->Height), ClassesImports::Rect(0, 97, 0, 53));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ExtraHeight > 0 && ResourceKey == pas::concat_wide({u"Bm.FormInfo3.", GR_Main::GiResourceSuffix(), u"BG"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Delta = 3;
            Delta = pas::idiv(std::min<std::int32_t>(GR_Main::ExtraScreenHeight, 432), Delta) * Delta;
            DestGraph->AllocateRgbaTight(SourceGraph->Width, SourceGraph->Height + Delta);
            DestGraph->DrawNinePatch(0, 0, DestGraph->Width, DestGraph->Height, SourceGraph, ClassesImports::Rect(0, 0, SourceGraph->Width, SourceGraph->Height), ClassesImports::Rect(0, 449, 0, 148));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (GR_Main::ExtraScreenWidth > 0 && ResourceKey == u"Bm.FormShop2.2bg") {
            Delta = GR_Main::ExtraScreenWidth / 198 * 198;
            if (Delta > 198) {
                Delta = 198;
            }
            if (Delta != 0) {
                LogWideScreenGiRescaleStart();
                SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
                DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                DestGraph->AllocateRgbaTight(SourceGraph->Width + Delta, SourceGraph->Height);
                DestGraph->DrawNinePatch(0, 0, Delta / 2 + 226, 34, SourceGraph, ClassesImports::Rect(0, 0, 226, 34), ClassesImports::Rect(225, 0, 0, 0));
                DestGraph->DrawNinePatch(Delta / 2 + 226, 0, 0, 34, SourceGraph, ClassesImports::Rect(226, 0, 0, 34), ClassesImports::Rect(326, 0, 213, 0));
                DestGraph->DrawNinePatch(0, 34, 0, 354, SourceGraph, ClassesImports::Rect(0, 34, 0, 388), ClassesImports::Rect(218, 0, 548, 0));
                DestGraph->DrawNinePatch(0, 388, Delta / 2 + 226, 0, SourceGraph, ClassesImports::Rect(0, 388, 226, 0), ClassesImports::Rect(225, 0, 0, 0));
                DestGraph->DrawNinePatch(Delta / 2 + 226, 388, 0, 0, SourceGraph, ClassesImports::Rect(226, 388, 0, 0), ClassesImports::Rect(326, 0, 213, 0));
                SourceGraph->Clear();
                pas::free(SourceGraph);
                StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
                DestGraph->Clear();
                pas::free(DestGraph);
                LogWideScreenGiRescaleDone();
            }
        } else if (GR_Main::ExtraScreenWidth > 0 && (ResourceKey == u"Bm.FormShop2.2Fei" || ResourceKey == u"Bm.FormShop2.2Gaal" || ResourceKey == u"Bm.FormShop2.2Peleng" || ResourceKey == u"Bm.FormShop2.2People")) {
            Delta = GR_Main::ExtraScreenWidth / 198 * 198;
            if (Delta > 198) {
                Delta = 198;
            }
            if (Delta != 0) {
                LogWideScreenGiRescaleStart();
                SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
                DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                DestGraph->AllocateRgbaTight(SourceGraph->Width + Delta, SourceGraph->Height);
                DestGraph->DrawNinePatch(0, 0, 0, 0, SourceGraph, ClassesImports::Rect(0, 0, 0, 0), ClassesImports::Rect(121, 0, 483, 0));
                SourceGraph->Clear();
                pas::free(SourceGraph);
                StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
                DestGraph->Clear();
                pas::free(DestGraph);
                LogWideScreenGiRescaleDone();
            }
        } else if (ExtraHeight > 0 && (ResourceKey == pas::concat_wide({u"Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"Left"}) || ResourceKey == pas::concat_wide({u"Bm.FormOptions2.", GR_Main::GiResourceSuffix(), u"Right"}))) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(SourceGraph->Width, GR_Main::ExtraScreenHeight + SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, 37, DestGraph->Height, SourceGraph, ClassesImports::Rect(0, 0, 37, SourceGraph->Height), ClassesImports::Rect(0, 324, 0, 338));
            DestGraph->DrawNinePatch(37, 0, 193, DestGraph->Height, SourceGraph, ClassesImports::Rect(37, 0, 230, SourceGraph->Height), ClassesImports::Rect(0, 280, 0, 375));
            DestGraph->DrawNinePatch(230, 0, 513, DestGraph->Height, SourceGraph, ClassesImports::Rect(230, 0, 743, SourceGraph->Height), ClassesImports::Rect(0, 325, 0, 337));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (GR_Main::ExtraScreenWidth > 0 && ResourceKey == pas::concat_wide({u"Bm.FormGameSet2.", GR_Main::GiResourceSuffix(), u"Footer"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, GR_Main::ExtraScreenWidth / 2 + 342, 0, SourceGraph, ClassesImports::Rect(0, 0, 342, 0), ClassesImports::Rect(341, 0, 0, 0));
            GR_GraphBuf::TGraphBufGR_CopyRect32(DestGraph, ClassesImports::Point(GR_Main::ExtraScreenWidth / 2 + 342, 0), SourceGraph, ClassesImports::Rect(342, 0, 682, SourceGraph->Height));
            DestGraph->DrawNinePatch(GR_Main::ExtraScreenWidth / 2 + 682, 0, GR_Main::ExtraScreenWidth / 2 + 342, 0, SourceGraph, ClassesImports::Rect(682, 0, 0, 0), ClassesImports::Rect(0, 0, 341, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ResourceKey == u"Bm.FormIntro2.PanelTop" || ResourceKey == u"Bm.FormEnd2.PanelTop") {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, 0, 0, SourceGraph, ClassesImports::Rect(0, 0, 0, 0), ClassesImports::Rect(0, 0, 0, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsRawGiBuffer(SourceBuffer, DestGraph, 1);
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (GR_Main::ExtraScreenWidth > 0 && ResourceKey == u"Bm.FormIntro2.PanelBottom") {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, GR_Main::ExtraScreenWidth / 2 + 302, 0, SourceGraph, ClassesImports::Rect(0, 0, 302, 0), ClassesImports::Rect(301, 0, 0, 0));
            DestGraph->DrawNinePatch(GR_Main::ExtraScreenWidth / 2 + 302, 0, 0, 0, SourceGraph, ClassesImports::Rect(302, 0, 0, 0), ClassesImports::Rect(420, 0, 301, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsRawGiBuffer(SourceBuffer, DestGraph, 1);
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ResourceKey == u"Bm.FormEnd2.PanelBottom") {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, SourceGraph->Height);
            DestGraph->DrawNinePatch(0, 0, 0, 0, SourceGraph, ClassesImports::Rect(0, 0, 0, 0), ClassesImports::Rect(0, 0, 154, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsRawGiBuffer(SourceBuffer, DestGraph, 1);
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if ((GR_Main::ExtraScreenWidth > 0 || ExtraHeight > 0) && ResourceKey == u"Bm.FormPQuest2.2Panel") {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            DestGraph->AllocateRgbaTight(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
            Delta = 39 - GR_Main::ExtraScreenWidth;
            if (Delta < 0) {
                Delta = 0;
            }
            DestGraph->DrawNinePatch(0, GR_Main::ExtraScreenHeight / 2 + 492, 0, 0, SourceGraph, ClassesImports::Rect(Delta, 492, 0, 0), ClassesImports::Rect(302 - Delta, 0, 760, 275));
            DestGraph->DrawNinePatch(0, 0, 0, GR_Main::ExtraScreenHeight / 2 + 492, SourceGraph, ClassesImports::Rect(Delta, 0, 0, 492), ClassesImports::Rect(345 - Delta, 410, 717, 81));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if ((GR_Main::ExtraScreenWidth > 0 || ExtraHeight > 0) && (EC_Str::FindTextOffsetW(ResourceKey, u"Bm.FormPQuest2.2S"_wref.get(), 0) == 0 && EC_Str::IsIntegerTextW(pas::view(EC_Str::CopyWideStringUnchecked(ResourceKey, 18, ResourceKey.length() - 17))) || EC_Str::FindTextOffsetW(ResourceKey, u"Bm.FormPQuest2."_wref.get(), 0) == 0 && EC_Str::FindTextOffsetW(ResourceKey, u"rescale"_wref.get(), 0) > 0)) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            DestGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Delta = GR_Main::ExtraScreenWidth - 39;
            if (Delta < 0) {
                Delta = 0;
            }
            DestGraph->AllocateRgbaTight(SourceGraph->Width + Delta, SourceGraph->Height + ExtraHeight);
            DestGraph->DrawNinePatch(0, 0, 0, ExtraHeight / 2 + 500, SourceGraph, ClassesImports::Rect(0, 0, 0, 500), ClassesImports::Rect(289, 385, 289, 114));
            DestGraph->DrawNinePatch(0, ExtraHeight / 2 + 500, 0, 0, SourceGraph, ClassesImports::Rect(0, 500, 0, 0), ClassesImports::Rect(289, 0, 289, 206));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            StoreGraphBufAsGiBuffer(SourceBuffer, DestGraph, ClassesImports::Point(0, 0));
            DestGraph->Clear();
            pas::free(DestGraph);
            LogWideScreenGiRescaleDone();
        } else if (ResourceKey == pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), u"WBbg"}) || ResourceKey == pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), u"CBbg"}) || ResourceKey == pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), u"DestroyerBridgebg"})) {
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            SourceGraph->RescaleRGBA_HW(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, true, 1, 1);
            StoreGraphBufAsGiBuffer(SourceBuffer, SourceGraph, ClassesImports::Point(0, 0));
            SourceGraph->Clear();
            pas::free(SourceGraph);
            LogWideScreenGiRescaleDone();
        } else {
            VerticalAlign = 1;
            PreserveAlpha = EC_Str::FindTextOffsetW(ResourceKey, u"Alpha"_wref.get(), 0) > 0;
            do {
                if (ResourceKey == u"Bm.FormAbout2.Bg" || ResourceKey == u"Bm.FormGameSet2.2bg" || ResourceKey == u"Bm.FormOptions2.2Bg" || ResourceKey == u"Bm.FormScore2.2bg" || EC_Str::FindTextOffsetW(ResourceKey, u"Bm.City."_wref.get(), 0) == 0 || EC_Str::FindTextOffsetW(ResourceKey, u"Bm.Gov."_wref.get(), 0) == 0 && (EC_Str::FindTextOffsetW(ResourceKey, u"MalocBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"MalocPirateBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PelengBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PelengPirateBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PeopleBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PeoplePirateBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"FeiBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"FeiPirateBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"GaalBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"GaalPirateBG"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PirateBG"_wref.get(), 0) > 0)) {
                    break;
                }
                if (EC_Str::FindTextOffsetW(ResourceKey, u"Bm.FormRuins."_wref.get(), 0) == 0) {
                    if (EC_Str::FindTextOffsetW(ResourceKey, u"BKbg"_wref.get(), 0) > 0) {
                        VerticalAlign = 2;
                        break;
                    } else if (EC_Str::FindTextOffsetW(ResourceKey, u"MCbg"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"PBbg"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"RCbg"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"SBbg"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"WBbg2"_wref.get(), 0) > 0 || EC_Str::FindTextOffsetW(ResourceKey, u"bg"_wref.get(), 0) > 0 && EC_Str::FindTextOffsetW(ResourceKey, u"table"_wref.get(), 0) <= 0) {
                        break;
                    }
                }
                if (EC_Str::FindTextOffsetW(ResourceKey, u"Bm.PlanetBG"_wref.get(), 0) == 0) {
                    if (SourceBuffer->DataSize < static_cast<std::int32_t>(sizeof(GR_gi::TgiHeaderGR))) {
                        return;
                    }
                    HeaderBytes = EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(GR_gi::TgiHeaderGR)));
                    Windows::CopyMemory(HeaderBytes, SourceBuffer->Data, static_cast<std::int32_t>(sizeof(GR_gi::TgiHeaderGR)));
                    Header = static_cast<GR_gi::PgiHeaderGR>(HeaderBytes);
                    ImageWidth = Header->Bounds.Right - Header->Bounds.Left;
                    ImageHeight = Header->Bounds.Bottom - Header->Bounds.Top;
                    EC_Mem::FreeEC(HeaderBytes);
                    if (ImageWidth > 1024 || ImageHeight > 768) {
                        break;
                    }
                }
                if (EC_Str::FindTextOffsetW(ResourceKey, u"Bm.FormLoad2.Shutter"_wref.get(), 0) != 0) {
                    return;
                }
                PreserveAlpha = true;
                Quiet = true;
            } while (!true);
            LogWideScreenGiRescaleStart();
            SourceGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            RenderGiBufferToGraphBuf(SourceBuffer, SourceGraph);
            Delta = SourceGraph->Width;
            Remainder = SourceGraph->Height;
            SourceGraph->RescaleRGBA_HW(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight, true, 1, VerticalAlign);
            if (Delta != SourceGraph->Width || Remainder != SourceGraph->Height) {
                if (PreserveAlpha) {
                    StoreGraphBufAsGiBuffer(SourceBuffer, SourceGraph, ClassesImports::Point(0, 0));
                } else {
                    StoreGraphBufAsRawGiBuffer(SourceBuffer, SourceGraph, 1);
                }
            }
            SourceGraph->Clear();
            pas::free(SourceGraph);
            LogWideScreenGiRescaleDone();
        }
    }

    void TCGiEC::p_destroy() {
        EC_CacheGI::TCGiEC_Destroy(this);
    }

} // namespace EC_CacheGI
