#include "layout/GI_MultiImage.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_MultiImage.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"

namespace GI_MultiImage {
    // Neutral integer expressions retain DCC32 operand materialization order.
    // See docs/development.md; they emit no extra arithmetic.
    void TMultiImageImageGI_Create(TMultiImageImageGI* Self) {
        pas::object_create(Self);
        Self->ImageCache = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
    }

    void TMultiImageImageGI_Destroy(TMultiImageImageGI* Self) {
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        pas::object_destroy(Self);
    }

    void TMultiImageImageGI::SetImage(pas::WideString Path) {
        EC_CacheGI::TCGiEC* Data{};
        Types::TPoint Size{};
        if (ImageCache->CacheKey != Path) {
            ImageCache->SetCacheKey(Path);
            Data = EC_CacheGI::AcquireCachedGi(ImageCache);
            {
                Size = Data->Image->GetContentSize();
                ImageCache->Release();
            }
            Bounds.Left = -Size.X / 2;
            Bounds.Top = -Size.Y / 2;
            Bounds.Right = Bounds.Left + Size.X;
            Bounds.Bottom = Bounds.Top + Size.Y;
        }
    }

    void TMultiImageGI_Create(TMultiImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Images = pas::make_object<pas::List>();
        Self->CellSize = 128;
    }

    void TMultiImageGI_Destroy(TMultiImageGI* Self) {
        Self->ClearImages();
        Self->ClearUnits();
        pas::free(Self->Images);
        Self->Images = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TMultiImageGI::Clear() {
        ClearImages();
        ClearUnits();
        GI_MessageLoop::TObjectGI::Clear();
    }

    TMultiImageUnitGI* TMultiImageGI::AddUnit() {
        TMultiImageUnitGI* Item = pas::make_object<TMultiImageUnitGI>();
        if (LastUnit != nullptr) {
            LastUnit->Next = Item;
        }
        Item->Prev = LastUnit;
        Item->Next = nullptr;
        LastUnit = Item;
        if (FirstUnit == nullptr) {
            FirstUnit = Item;
        }
        return Item;
    }

    void TMultiImageGI::RemoveUnit(TMultiImageUnitGI* Item) {
        UnlinkUnitFromColumn(Item);
        if (Item->Prev != nullptr) {
            Item->Prev->Next = Item->Next;
        }
        if (Item->Next != nullptr) {
            Item->Next->Prev = Item->Prev;
        }
        if (LastUnit == Item) {
            LastUnit = Item->Prev;
        }
        if (FirstUnit == Item) {
            FirstUnit = Item->Next;
        }
        pas::free(Item);
    }

    void TMultiImageGI::ClearUnits() {
        ClearSpatialIndex();
        while (FirstUnit != nullptr) {
            RemoveUnit(LastUnit);
        }
    }

    // Prunes empty columns and rows.
    void TMultiImageGI::UnlinkUnitFromColumn(TMultiImageUnitGI* Item) {
        TMultiImageColGI* Column{};
        TMultiImageRowGI* Row{};
        if (Item->Column != nullptr) {
            Column = Item->Column;
            if (Item->PrevInColumn != nullptr) {
                Item->PrevInColumn->NextInColumn = Item->NextInColumn;
            }
            if (Item->NextInColumn != nullptr) {
                Item->NextInColumn->PrevInColumn = Item->PrevInColumn;
            }
            if (Column->Last == Item) {
                Column->Last = Item->PrevInColumn;
            }
            if (Column->First == Item) {
                Column->First = Item->NextInColumn;
            }
            Item->PrevInColumn = nullptr;
            Item->NextInColumn = nullptr;
            if (Column->Last != nullptr) {
                Item->Column = nullptr;
            } else {
                Row = Item->Column->Row;
                Item->Column = nullptr;
                if (Column->Prev != nullptr) {
                    Column->Prev->Next = Column->Next;
                }
                if (Column->Next != nullptr) {
                    Column->Next->Prev = Column->Prev;
                }
                if (Row->Last == Column) {
                    Row->Last = Column->Prev;
                }
                if (Row->First == Column) {
                    Row->First = Column->Next;
                }
                pas::free(Column);
                if (Row->Last == nullptr) {
                    if (Row->Prev != nullptr) {
                        Row->Prev->Next = Row->Next;
                    }
                    if (Row->Next != nullptr) {
                        Row->Next->Prev = Row->Prev;
                    }
                    if (LastRow == Row) {
                        LastRow = Row->Prev;
                    }
                    if (FirstRow == Row) {
                        FirstRow = Row->Next;
                    }
                    pas::free(Row);
                }
            }
        }
    }

    // Preserves units and clears their spatial links.
    void TMultiImageGI::ClearSpatialIndex() {
        TMultiImageRowGI* OldRow{};
        TMultiImageColGI* Column{};
        TMultiImageColGI* OldColumn{};
        TMultiImageRowGI* Row = FirstRow;
        while (Row != nullptr) {
            OldRow = Row;
            Row = Row->Next;
            Column = OldRow->First;
            while (Column != nullptr) {
                OldColumn = Column;
                Column = Column->Next;
                pas::free(OldColumn);
            }
            pas::free(OldRow);
        }
        FirstRow = nullptr;
        LastRow = nullptr;
        TMultiImageUnitGI* Item = FirstUnit;
        while (Item != nullptr) {
            Item->Column = nullptr;
            Item->PrevInColumn = nullptr;
            Item->NextInColumn = nullptr;
            Item = Item->Next;
        }
    }

    TMultiImageRowGI* TMultiImageGI::GetOrCreateRow(std::int32_t Index) {
        TMultiImageRowGI* Row = FirstRow;
        while (Row != nullptr) {
            if (Row->Index == Index) {
                return Row;
            }
            if (Row->Index > Index) {
                break;
            }
            Row = Row->Next;
        }
        TMultiImageRowGI* Result = pas::make_object<TMultiImageRowGI>();
        Result->Index = Index;
        if (Row == nullptr) {
            if (LastRow != nullptr) {
                LastRow->Next = Result;
            }
            Result->Prev = LastRow;
            Result->Next = nullptr;
            LastRow = Result;
            if (FirstRow == nullptr) {
                FirstRow = Result;
            }
        } else {
            Result->Prev = Row->Prev;
            Result->Next = Row;
            if (Row->Prev != nullptr) {
                Row->Prev->Next = Result;
            }
            Row->Prev = Result;
            if (FirstRow == Row) {
                FirstRow = Result;
            }
        }
        return Result;
    }

    TMultiImageColGI* TMultiImageGI::GetOrCreateColumn(TMultiImageRowGI* Row, std::int32_t Index) {
        TMultiImageColGI* Column = Row->First;
        while (Column != nullptr) {
            if (Column->Index == Index) {
                return Column;
            }
            if (Column->Index > Index) {
                break;
            }
            Column = Column->Next;
        }
        TMultiImageColGI* Result = pas::make_object<TMultiImageColGI>();
        Result->Row = Row;
        Result->Index = Index;
        if (Column == nullptr) {
            if (Row->Last != nullptr) {
                Row->Last->Next = Result;
            }
            Result->Prev = Row->Last;
            Result->Next = nullptr;
            Row->Last = Result;
            if (Row->First == nullptr) {
                Row->First = Result;
            }
        } else {
            Result->Prev = Column->Prev;
            Result->Next = Column;
            if (Column->Prev != nullptr) {
                Column->Prev->Next = Result;
            }
            Column->Prev = Result;
            if (Row->First == Column) {
                Row->First = Result;
            }
        }
        return Result;
    }

    // Native early-out compares the control's Position, not the item's old position. CellSize must be nonzero.
    void TMultiImageGI::SetUnitPosition(TMultiImageUnitGI* Item, Types::TPoint Position) {
        TMultiImageColGI* Column{};
        // Preserve the native comparison against the control's position.
        if (Item->Column == nullptr || LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            Item->Position = Position;
            {
                TMultiImageRowGI* orCreateRow = GetOrCreateRow(pas::idiv(Position.Y, reinterpret_cast<TMultiImageGI*>(reinterpret_cast<std::uint8_t*>(this) + 0)->CellSize));
                std::int32_t cpp_arg = pas::idiv(Position.X, reinterpret_cast<TMultiImageGI*>(reinterpret_cast<std::uint8_t*>(this) + 0)->CellSize);
                Column = TMultiImageGI::GetOrCreateColumn(orCreateRow, cpp_arg);
            }
            if (Item->Column != Column) {
                UnlinkUnitFromColumn(Item);
                Item->Column = Column;
                if (Column->Last != nullptr) {
                    Column->Last->NextInColumn = Item;
                }
                Item->PrevInColumn = Column->Last;
                Item->NextInColumn = nullptr;
                Column->Last = reinterpret_cast<TMultiImageUnitGI*>(reinterpret_cast<std::uint8_t*>(Item) + 0);
                if (Column->First == nullptr) {
                    Column->First = reinterpret_cast<TMultiImageUnitGI*>(reinterpret_cast<std::uint8_t*>(Item) + 0);
                }
            }
        }
    }

    void TMultiImageGI::ClearImages() {
        TMultiImageImageGI* Image{};
        std::int32_t I{};
        if (Images != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Images) - 1); cpp_range.next(I); ) {
                Image = pas::list_at<TMultiImageImageGI>(Images, I);
                pas::free(Image);
            }
            pas::list_clear(Images);
        }
    }

    std::int32_t TMultiImageGI::AddImage(pas::WideString Path) {
        TMultiImageImageGI* Image = pas::construct_call<TMultiImageImageGI>(TMultiImageImageGI_Create);
        Image->SetImage(Path);
        pas::list_add(Images, reinterpret_cast<void*>(Image));
        return pas::list_count(Images) - 1;
    }

    void TMultiImageGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        TMultiImageGI::LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TMultiImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TMultiImageGI::LoadImageProperties(Block);
    }

    // Empty in native code.
    void TMultiImageGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
    }

    void TMultiImageGI::Invalidate() {
        std::int32_t MinColumn{};
        std::int32_t MaxColumn{};
        std::int32_t MinRow{};
        std::int32_t MaxRow{};
        TMultiImageRowGI* Row{};
        TMultiImageColGI* Column{};
        TMultiImageUnitGI* Item{};
        Types::TPoint Position{};
        TMultiImageImageGI* Image{};
        Types::TRect Bounds{};
        if (!MessageLoop->UpdateRectsEnabled) {
            return;
        }
        if (!Active) {
            return;
        }
        if (EC_Struct::IntersectRects(Bounds, HitTestBounds, GR_Main::GameScreenRect)) {
            Bounds.Left -= AbsolutePosition.X;
            Bounds.Top -= AbsolutePosition.Y;
            Bounds.Right -= AbsolutePosition.X;
            Bounds.Bottom -= AbsolutePosition.Y;
            MinColumn = pas::idiv(Bounds.Left, reinterpret_cast<TMultiImageGI*>(reinterpret_cast<std::uint8_t*>(this) + 0)->CellSize) - 1;
            MaxColumn = pas::idiv(Bounds.Right - 1, CellSize) + 1;
            MinRow = pas::idiv(Bounds.Top, reinterpret_cast<TMultiImageGI*>(reinterpret_cast<std::uint8_t*>(this) + 0)->CellSize) - 1;
            MaxRow = pas::idiv(Bounds.Bottom - 1, CellSize) + 1;
            Row = FirstRow;
            while (Row != nullptr) {
                if (Row->Index >= MinRow && Row->Index <= MaxRow) {
                    Column = Row->First;
                    while (Column != nullptr) {
                        if (Column->Index >= MinColumn && Column->Index <= MaxColumn) {
                            Item = Column->First;
                            while (Item != nullptr) {
                                Position.X = AbsolutePosition.X + Item->Position.X;
                                Position.Y = AbsolutePosition.Y + Item->Position.Y;
                                Image = pas::list_at<TMultiImageImageGI>(reinterpret_cast<pas::List*>(reinterpret_cast<std::uint8_t*>(Images) + 0), Item->ImageIndex);
                                Bounds.Left = Position.X + 0 + Image->Bounds.Left;
                                Bounds.Top = Position.Y + 0 + Image->Bounds.Top;
                                Bounds.Right = Position.X + 0 + Image->Bounds.Right;
                                Bounds.Bottom = Position.Y + 0 + Image->Bounds.Bottom;
                                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(reinterpret_cast<std::uint8_t*>(MessageLoop) + 0)->QueueUpdateRect(Bounds);
                                Item = Item->NextInColumn;
                            }
                        } else if (Column->Index > MaxColumn) {
                            break;
                        }
                        Column = Column->Next;
                    }
                } else if (Row->Index > MaxRow) {
                    break;
                }
                Row = Row->Next;
            }
        }
    }

    void TMultiImageGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        TMultiImageColGI* Column{};
        TMultiImageUnitGI* Item{};
        Types::TPoint Position{};
        TMultiImageImageGI* Image{};
        EC_CacheGI::TCGiEC* Data{};
        Types::TRect Bounds{};
        Types::TRect Intersection{};
        Bounds.Left = ClipRect.Left - AbsolutePosition.X;
        Bounds.Top = ClipRect.Top - AbsolutePosition.Y;
        Bounds.Right = ClipRect.Right - AbsolutePosition.X;
        Bounds.Bottom = ClipRect.Bottom - AbsolutePosition.Y;
        std::int32_t MinColumn = pas::idiv(Bounds.Left, CellSize) - 1;
        std::int32_t MaxColumn = pas::idiv(Bounds.Right - 1, CellSize) + 1;
        std::int32_t MinRow = pas::idiv(Bounds.Top, CellSize) - 1;
        std::int32_t MaxRow = pas::idiv(Bounds.Bottom - 1, CellSize) + 1;
        TMultiImageRowGI* Row = FirstRow;
        while (Row != nullptr) {
            if (Row->Index >= MinRow && Row->Index <= MaxRow) {
                Column = Row->First;
                while (Column != nullptr) {
                    if (Column->Index >= MinColumn && Column->Index <= MaxColumn) {
                        Item = Column->First;
                        while (Item != nullptr) {
                            Position.X = AbsolutePosition.X + Item->Position.X;
                            Position.Y = AbsolutePosition.Y + Item->Position.Y;
                            Image = pas::list_at<TMultiImageImageGI>(Images, Item->ImageIndex);
                            Bounds.Left = Image->Bounds.Left + Position.X;
                            Bounds.Top = Image->Bounds.Top + Position.Y;
                            Bounds.Right = Image->Bounds.Right + Position.X;
                            Bounds.Bottom = Image->Bounds.Bottom + Position.Y;
                            if (EC_Struct::IntersectRects(Intersection, Bounds, ClipRect)) {
                                Data = EC_CacheGI::AcquireCachedGi(Image->ImageCache);
                                {
                                    pas::ScopeExit cpp_cleanup = [&]() noexcept {
                                        Image->ImageCache->Release();
                                    };
                                    if (GlobalsV::HardwareRenderingEnabled) {
                                        pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateSurface = (Data->GetOrCreateSurface(0, cpp_result), cpp_result);
                                        std::int32_t top = Bounds.Top;
                                        std::int32_t left = Bounds.Left;
                                        GR_DX::DrawTexture(orCreateSurface, left, top, 255, 0x00ffffffu, &ClipRect, false, false);
                                    } else {
                                        Data->Image->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Bounds.Left, Bounds.Top, ClipRect, 0, 255);
                                    }
                                }
                            }
                            Item = Item->NextInColumn;
                        }
                    } else if (Column->Index > MaxColumn) {
                        break;
                    }
                    Column = Column->Next;
                }
            } else if (Row->Index > MaxRow) {
                break;
            }
            Row = Row->Next;
        }
    }

    void TMultiImageGI::QueueImageLoad(pas::List* PendingLoads) {
        TMultiImageImageGI* Image{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Images) - 1); cpp_range.next(I); ) {
            Image = pas::list_at<TMultiImageImageGI>(Images, I);
            Image->ImageCache->QueueLoadIfMissing(PendingLoads);
        }
    }

    void TMultiImageImageGI::p_destroy() {
        GI_MultiImage::TMultiImageImageGI_Destroy(this);
    }

    void TMultiImageGI::p_destroy() {
        GI_MultiImage::TMultiImageGI_Destroy(this);
    }

} // namespace GI_MultiImage
