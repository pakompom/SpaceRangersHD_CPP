#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGI {
    struct TCGiControlEC;

} // namespace EC_CacheGI

namespace GI_MultiImage {
    struct TMultiImageUnitGI;

    struct TMultiImageColGI;

    struct TMultiImageRowGI;

    struct TMultiImageImageGI;

    struct TMultiImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMultiImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TMultiImageGI, GI_MessageLoop::TObjectGI, "TMultiImageGI", 312)
        void p_destroy() override;
        void Clear() override;
        TMultiImageUnitGI* AddUnit();
        void RemoveUnit(TMultiImageUnitGI* Item);
        void ClearUnits();
        void UnlinkUnitFromColumn(TMultiImageUnitGI* Item);
        void ClearSpatialIndex();
        TMultiImageRowGI* GetOrCreateRow(std::int32_t Index);
        static TMultiImageColGI* GetOrCreateColumn(TMultiImageRowGI* Row, std::int32_t Index);
        void SetUnitPosition(TMultiImageUnitGI* Item, Types::TPoint Position);
        void ClearImages();
        std::int32_t AddImage(pas::WideString Path);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        static void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void Invalidate() override;
        void Draw(Types::TRect ClipRect) override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        TMultiImageUnitGI* FirstUnit;
        TMultiImageUnitGI* LastUnit;
        TMultiImageRowGI* FirstRow;
        TMultiImageRowGI* LastRow;
        std::int32_t CellSize;
        pas::List* Images;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMultiImageUnitGI : pas::Object {
        PAS_CLASS_META(TMultiImageUnitGI, pas::Object, "TMultiImageUnitGI", 40)
        TMultiImageUnitGI* Prev;
        TMultiImageUnitGI* Next;
        TMultiImageUnitGI* PrevInColumn;
        TMultiImageUnitGI* NextInColumn;
        TMultiImageColGI* Column;
        std::int32_t ImageIndex;
        Types::TPoint Position;
        void* UserData;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMultiImageColGI : pas::Object {
        PAS_CLASS_META(TMultiImageColGI, pas::Object, "TMultiImageColGI", 28)
        TMultiImageColGI* Prev;
        TMultiImageColGI* Next;
        TMultiImageUnitGI* First;
        TMultiImageUnitGI* Last;
        TMultiImageRowGI* Row;
        std::int32_t Index;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMultiImageRowGI : pas::Object {
        PAS_CLASS_META(TMultiImageRowGI, pas::Object, "TMultiImageRowGI", 24)
        TMultiImageRowGI* Prev;
        TMultiImageRowGI* Next;
        TMultiImageColGI* First;
        TMultiImageColGI* Last;
        std::int32_t Index;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMultiImageImageGI : pas::Object {
        PAS_CLASS_META(TMultiImageImageGI, pas::Object, "TMultiImageImageGI", 24)
        void p_destroy() override;
        void SetImage(pas::WideString Path);
        EC_CacheGI::TCGiControlEC* ImageCache;
        Types::TRect Bounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_MultiImage
