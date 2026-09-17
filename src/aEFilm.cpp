#include "layout/aEFilm.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_GAIEffect.hpp"
#include "types/SE_Gate.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Weapon.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fFilm.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/aEFilm.hpp"
#include "units/aEObjInfo.hpp"
#include "units/aPlayer.hpp"

namespace aEFilm {
    // Native writer format, read from initialized storage.
    const std::int32_t FilmFormatVersion = 6;

    void TEFilm_Create(TEFilm* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->StringTable = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
        Self->DataBuffers = pas::make_object<pas::List>();
        Self->ObjectInfo = pas::construct_call<aEObjInfo::TEObjInfo>(aEObjInfo::TEObjInfo_Create);
    }

    void TEFilm_Destroy(TEFilm* Self) {
        PEFilmCommand Command{};
        Self->Clear();
        PEFilmCommand NextCommand = Self->FirstFreeCommand;
        while (NextCommand != nullptr) {
            Command = NextCommand;
            NextCommand = NextCommand->Next;
            EC_Mem::FreeEC(Command);
        }
        Self->FirstFreeCommand = nullptr;
        Self->LastFreeCommand = nullptr;
        if (Self->StringTable != nullptr) {
            pas::free(Self->StringTable);
            Self->StringTable = nullptr;
        }
        if (Self->DataBuffers != nullptr) {
            pas::free(Self->DataBuffers);
            Self->DataBuffers = nullptr;
        }
        if (Self->ObjectInfo != nullptr) {
            pas::free(Self->ObjectInfo);
            Self->ObjectInfo = nullptr;
        }
        Self->CameraEvents = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TEFilm::Clear() {
        std::int32_t I{};
        pas::Object* Obj{};
        if (FirstCommand != nullptr) {
            RecycleCommands(FirstCommand, LastCommand);
        }
        while (FirstObject != nullptr) {
            RemoveObject(LastObject);
        }
        StringTable->Clear();
        std::int32_t Count = pas::list_count(DataBuffers);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Obj = pas::list_at<pas::Object>(DataBuffers, I);
            pas::free(Obj);
        }
        pas::list_clear(DataBuffers);
        CameraEventCount = 0;
    }

    // Grows capacity and advances the used count without writing the new slot.
    void TEFilm::ReserveCameraEventSlot() {
        if (CameraEvents.length() - 1 <= CameraEventCount) {
            CameraEvents.set_length(CameraEventCount + 30);
        }
        ++CameraEventCount;
    }

    void TEFilm::AddCameraEvent(std::int32_t AStepIndex, EC_Struct::TPointF AStartPosition, EC_Struct::TPointF AEndPosition, std::int32_t APriority) {
        if (CameraEvents.length() - 1 <= CameraEventCount) {
            CameraEvents.set_length(CameraEventCount + 30);
        }
        {
            TEFilmCameraEvent& cpp_with = CameraEvents[CameraEventCount];
            cpp_with.StepIndex = AStepIndex;
            cpp_with.Priority = APriority;
            pas::store_unaligned<EC_Struct::TPointF>(&cpp_with.StartPosition, AStartPosition);
            pas::store_unaligned<EC_Struct::TPointF>(&cpp_with.EndPosition, AEndPosition);
        }
        ++CameraEventCount;
    }

    // Appends an object owned by this film.
    TEFilmObj* TEFilm::AllocateObject() {
        TEFilmObj* Obj{};
        try {
            Obj = pas::make_object<TEFilmObj>();
        } catch (...) {
            Obj = nullptr;
        }
        if (Obj == nullptr) {
            GR_Main::AppendLogTextThreadSafe("Failed to allocate memory for film object, trying to free some textures... "_a);
            GR_DX::EvictTextureCaches(true);
            try {
                Obj = pas::make_object<TEFilmObj>();
            } catch (...) {
                Obj = nullptr;
            }
            if (Obj != nullptr) {
                GR_Main::AppendLogLineThreadSafe("success"_a);
            } else {
                GR_Main::AppendLogLineThreadSafe("fail"_a);
                pas::raise(pas::make_exception<pas::Exception>("Error in TEFilm.ObjAlloc"_a));
            }
        }
        if (LastObject != nullptr) {
            LastObject->Next = Obj;
        }
        Obj->Prev = LastObject;
        Obj->Next = nullptr;
        LastObject = Obj;
        if (FirstObject == nullptr) {
            FirstObject = Obj;
        }
        return Obj;
    }

    void TEFilm::RemoveObject(TEFilmObj* Obj) {
        if (Obj->Prev != nullptr) {
            Obj->Prev->Next = Obj->Next;
        }
        if (Obj->Next != nullptr) {
            Obj->Next->Prev = Obj->Prev;
        }
        if (LastObject == Obj) {
            LastObject = Obj->Prev;
        }
        if (FirstObject == Obj) {
            FirstObject = Obj->Next;
        }
        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject));
        pas::free(Obj);
    }

    std::int32_t TEFilm::ObjectCount() {
        std::int32_t Count = 0;
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            ++Count;
            Entry = Entry->Next;
        }
        return Count;
    }

    // Zero-based list index; nil maps to 65535. Raises for an object outside this film.
    std::int32_t TEFilm::ObjToNom(TEFilmObj* Obj) {
        if (Obj == nullptr) {
            return FilmNullObjectIndex;
        }
        std::int32_t Index = 0;
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry == Obj) {
                return Index;
            }
            ++Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>("Error in TEFilm.ObjToNom"_a));
    }

    // Nil maps to 65535; an absent non-nil object maps to -1.
    std::int32_t TEFilm::FindObjectIndex(TEFilmObj* Obj) {
        if (Obj == nullptr) {
            return FilmNullObjectIndex;
        }
        std::int32_t Index = 0;
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry == Obj) {
                return Index;
            }
            ++Index;
            Entry = Entry->Next;
        }
        return -1;
    }

    // Returns a borrowed object. Index 65535 maps to nil; other missing indexes raise.
    TEFilmObj* TEFilm::NomToObj(std::int32_t Index) {
        if (Index == FilmNullObjectIndex) {
            return nullptr;
        }
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Index == 0) {
                return Entry;
            }
            --Index;
            Entry = Entry->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>("Error in TEFilm.NomToObj"_a));
    }

    std::uint8_t TEFilm::ContainsObject(TEFilmObj* Obj) {
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Obj == Entry) {
                return true;
            }
            Entry = Entry->Next;
        }
        return false;
    }

    // Matches all three keys; returns a borrowed object or nil.
    TEFilmObj* TEFilm::FindObject(const pas::WideString& KindName, const pas::WideString& GraphKey, std::uint32_t ObjectId) {
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry->ObjectId == ObjectId && Entry->KindName == KindName && Entry->GraphKey == GraphKey) {
                return Entry;
            }
            Entry = Entry->Next;
        }
        return nullptr;
    }

    TEFilmObj* TEFilm::FindObjectById(const pas::WideString& KindName, std::uint32_t ObjectId) {
        TEFilmObj* Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry->ObjectId == ObjectId && Entry->KindName == KindName) {
                return Entry;
            }
            Entry = Entry->Next;
        }
        return nullptr;
    }

    void TEFilm::GrowCommandPool(std::int32_t Count) {
        PEFilmCommand Command{};
        while (Count > 0) {
            Command = static_cast<PEFilmCommand>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TEFilmCommand))));
            if (LastFreeCommand != nullptr) {
                LastFreeCommand->Next = Command;
            }
            Command->Prev = LastFreeCommand;
            Command->Next = nullptr;
            LastFreeCommand = Command;
            if (FirstFreeCommand == nullptr) {
                FirstFreeCommand = Command;
            }
            --Count;
        }
    }

    // Moves an inclusive linked range to the free list.
    void TEFilm::RecycleCommands(PEFilmCommand First, PEFilmCommand Last) {
        if (First->Prev != nullptr) {
            First->Prev->Next = Last->Next;
        }
        if (Last->Next != nullptr) {
            Last->Next->Prev = First->Prev;
        }
        if (Last == LastCommand) {
            LastCommand = First->Prev;
        }
        if (First == FirstCommand) {
            FirstCommand = Last->Next;
        }
        if (LastFreeCommand != nullptr) {
            LastFreeCommand->Next = First;
        }
        First->Prev = LastFreeCommand;
        Last->Next = nullptr;
        LastFreeCommand = Last;
        if (FirstFreeCommand == nullptr) {
            FirstFreeCommand = First;
        }
    }

    void TEFilm::AppendCommand(PEFilmCommand Command) {
        if (LastCommand != nullptr) {
            LastCommand->Next = Command;
        }
        Command->Prev = LastCommand;
        Command->Next = nullptr;
        LastCommand = Command;
        if (FirstCommand == nullptr) {
            FirstCommand = Command;
        }
    }

    // Nil Before appends.
    void TEFilm::InsertCommand(PEFilmCommand Before, PEFilmCommand Command) {
        if (Before != nullptr) {
            Command->Prev = Before->Prev;
            Command->Next = Before;
            if (Before->Prev != nullptr) {
                Before->Prev->Next = Command;
            }
            Before->Prev = Command;
            if (Before == FirstCommand) {
                FirstCommand = Command;
            }
        } else {
            if (LastCommand != nullptr) {
                LastCommand->Next = Command;
            }
            Command->Prev = LastCommand;
            Command->Next = nullptr;
            LastCommand = Command;
            if (FirstCommand == nullptr) {
                FirstCommand = Command;
            }
        }
    }

    // Returns a zeroed pooled command without linking it into the command list.
    PEFilmCommand TEFilm::AllocateCommand() {
        PEFilmCommand Command{};
        if (FirstFreeCommand == LastFreeCommand) {
            GrowCommandPool(500);
        }
        Command = FirstFreeCommand;
        Command->Next->Prev = nullptr;
        FirstFreeCommand = Command->Next;
        pas::fill_memory(Command, static_cast<std::int32_t>(sizeof(TEFilmCommand)), static_cast<std::uint8_t>(0));
        return Command;
    }

    // Stable insertion by step index.
    PEFilmCommand TEFilm::AddCommand(std::int32_t StepIndex) {
        PEFilmCommand Command = AllocateCommand();
        Command->StepIndex = StepIndex;
        if (LastCommand == nullptr || LastCommand->StepIndex <= StepIndex) {
            AppendCommand(Command);
            return Command;
        }
        PEFilmCommand Entry = FirstCommand;
        while (Entry != nullptr) {
            if (Entry->StepIndex > StepIndex) {
                InsertCommand(Entry, Command);
                break;
            }
            Entry = Entry->Next;
        }
        if (Entry == nullptr) {
            AppendCommand(Command);
        }
        return Command;
    }

    std::int32_t TEFilm::CommandCount() {
        std::int32_t Count = 0;
        PEFilmCommand Entry = FirstCommand;
        while (Entry != nullptr) {
            ++Count;
            Entry = Entry->Next;
        }
        return Count;
    }

    // Retains SceneObject and copies its class name and graph key. Both stack arguments are unused.
    TEFilmObj* TEFilm::AddObject(std::uint32_t ObjectId, SE_Space::TObjectSE* SceneObject, std::int32_t Unused1, std::int32_t Unused2) {
        TEFilmObj* Obj{};
        Obj = AllocateObject();
        Obj->ObjectId = ObjectId;
        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject), SceneObject);
        Obj->KindName = SE_Process::ClassSEtoName(SceneObject);
        Obj->GraphKey = SceneObject->GraphKey;
        return Obj;
    }

    void TEFilm::SetObjectPosition(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmVectorCommand Command = reinterpret_cast<PEFilmVectorCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectPosition;
        Command->Obj = Obj;
        pas::store_unaligned<EC_Struct::TPointF>(&Command->Position, Position);
    }

    void TEFilm::SetObjectOrbitCenter(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position) {
        PEFilmVectorCommand Command = reinterpret_cast<PEFilmVectorCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectOrbitCenter;
        Command->Obj = Obj;
        pas::store_unaligned<EC_Struct::TPointF>(&Command->Position, Position);
    }

    void TEFilm::SetObjectAlpha(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Alpha) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmByteCommand Command = reinterpret_cast<PEFilmByteCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectAlpha;
        Command->Obj = Obj;
        Command->Value = Alpha;
    }

    // A full turn is 256 angle units.
    void TEFilm::SetObjectAngle(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Angle) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmByteCommand Command = reinterpret_cast<PEFilmByteCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectAngle;
        Command->Obj = Obj;
        Command->Value = Angle;
    }

    // Queues slot 0x40 on each retained scene object.
    void TEFilm::AdvanceObjects(std::int32_t StepIndex) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcAdvanceObjects;
    }

    // Scale is decoded as a signed 16-bit value divided by 1000 during playback.
    void TEFilm::SetPlanetState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t RotationInterval, std::int32_t SurfaceMapStep, std::uint16_t ScaleThousandths, std::uint8_t RingKind, std::uint8_t Owner) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetPlanetState;
        Command->Obj = Obj;
        Command->Value = pas::shl(static_cast<std::int32_t>(RingKind), 24) | RotationInterval;
        Command->ExtraValue = SurfaceMapStep;
        Command->Flags = ScaleThousandths | pas::shl(static_cast<std::int32_t>(Owner), 24);
    }

    void TEFilm::SetShipSizeAndTailMode(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Size, std::int32_t TailMode) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmSizeCommand Command = reinterpret_cast<PEFilmSizeCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetShipSizeAndTailMode;
        Command->Obj = Obj;
        pas::store_unaligned<Types::TPoint>(&Command->Size, Size);
        Command->TailMode = TailMode;
    }

    void TEFilm::SetRuinsState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetRuinsState;
        Command->Obj = Obj;
        Command->Value = State;
    }

    void TEFilm::SetWeaponHit(std::int32_t StepIndex, TEFilmObj* Obj, std::uint16_t Color, std::int32_t Damage, std::uint8_t Destroyed, std::uint8_t PlaySound) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmHitCommand Command = reinterpret_cast<PEFilmHitCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetWeaponHit;
        Command->Obj = Obj;
        Command->Color = Color;
        Command->Damage = Damage;
        Command->Destroyed = Destroyed;
        Command->PlaySound = PlaySound;
    }

    // Source and Target may be nil; Obj must exist.
    void TEFilm::SetWeaponEndpoints(std::int32_t StepIndex, TEFilmObj* Obj, TEFilmObj* Source, TEFilmObj* Target) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmEndpointsCommand Command = reinterpret_cast<PEFilmEndpointsCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetWeaponEndpoints;
        Command->Obj = Obj;
        Command->Source = Source;
        Command->Target = Target;
    }

    // Playback selects TWeaponSE destruction effects: 1=bomb, 2=asteroid, 5=kamikaze; other modes include fades and immediate removal.
    void TEFilm::SetDestructionEffect(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Value) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetDestructionEffect;
        Command->Obj = Obj;
        Command->Value = Value;
    }

    void TEFilm::SetEffectImagePosition(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Position) {
        PEFilmSizeCommand Command = reinterpret_cast<PEFilmSizeCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetEffectImagePosition;
        Command->Obj = Obj;
        pas::store_unaligned<Types::TPoint>(&Command->Size, Position);
    }

    // Records the Single duration multiplier consumed by TGAIEffectSE.SetDurationScale.
    void TEFilm::SetEffectDurationScale(std::int32_t StepIndex, TEFilmObj* Obj, float Scale) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetEffectDurationScale;
        Command->Obj = Obj;
        // Copy the Single payload without floating-point conversion or rounding.
        Command->Value = pas::load_unaligned<std::int32_t>(&Scale);
    }

    void TEFilm::AttachObject(std::int32_t StepIndex, TEFilmObj* Obj) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcAttachObject;
        Command->Obj = Obj;
    }

    void TEFilm::DetachObject(std::int32_t StepIndex, TEFilmObj* Obj) {
        if (Obj == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("obj=nil"_a));
        }
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcDetachObject;
        Command->Obj = Obj;
    }

    // Playback detaches and releases the scene reference, retaining the film entry.
    void TEFilm::ReleaseObject(std::int32_t StepIndex, TEFilmObj* Obj) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcReleaseObject;
        Command->Obj = Obj;
    }

    void TEFilm::ReleaseWeaponEffects(std::int32_t StepIndex) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcReleaseWeaponEffects;
    }

    void TEFilm::SetViewCenter(std::int32_t StepIndex, EC_Struct::TPointF Position) {
        PEFilmVectorCommand Command = reinterpret_cast<PEFilmVectorCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetViewCenter;
        pas::store_unaligned<EC_Struct::TPointF>(&Command->Position, Position);
    }

    void TEFilm::SetRadarCenter(std::int32_t StepIndex, EC_Struct::TPointF Position) {
        PEFilmVectorCommand Command = reinterpret_cast<PEFilmVectorCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetRadarCenter;
        pas::store_unaligned<EC_Struct::TPointF>(&Command->Position, Position);
    }

    void TEFilm::SetCameraAnchor(std::int32_t StepIndex, EC_Struct::TPointF Position, std::uint8_t ForceMovement) {
        PEFilmVectorCommand Command = reinterpret_cast<PEFilmVectorCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetCameraAnchor;
        pas::store_unaligned<EC_Struct::TPointF>(&Command->Position, Position);
        Command->ForceMovement = ForceMovement;
    }

    void TEFilm::OpenGate(std::int32_t StepIndex, TEFilmObj* Obj) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcOpenGate;
        Command->Obj = Obj;
    }

    // Playback changes gate state 2 to 3 and resets its timer.
    void TEFilm::CloseGate(std::int32_t StepIndex, TEFilmObj* Obj) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcCloseGate;
        Command->Obj = Obj;
    }

    void TEFilm::SetGateState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetGateState;
        Command->Obj = Obj;
        Command->Value = State;
    }

    void TEFilm::SetGateSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetGateSize;
        Command->Obj = Obj;
        Command->Value = Size;
    }

    void TEFilm::SetGateEffectSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetGateEffectSize;
        Command->Obj = Obj;
        Command->Value = Size;
    }

    void TEFilm::SetHoleState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetHoleState;
        Command->Obj = Obj;
        Command->Value = State;
    }

    void TEFilm::SetObjectText(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectText;
        Command->Obj = Obj;
        StringTable->Add(Text);
        Command->Value = StringTable->GetCount() - 1;
    }

    void TEFilm::PlayObjectSound(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcPlayObjectSound;
        Command->Obj = Obj;
        StringTable->Add(Text);
        Command->Value = StringTable->GetCount() - 1;
    }

    // Transfers ownership of Buffer to the film.
    void TEFilm::SetObjectStateBuffer(std::int32_t StepIndex, TEFilmObj* Obj, EC_Buf::TBufEC* Buffer) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcSetObjectStateBuffer;
        Command->Obj = Obj;
        pas::list_add(DataBuffers, reinterpret_cast<void*>(Buffer));
        Command->Value = pas::list_count(DataBuffers) - 1;
    }

    // Appends the kind-24 boundary consumed by film playback.
    void TEFilm::BeginTrailingEffects(std::int32_t StepIndex) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcBeginTrailingEffects;
    }

    void TEFilm::PlayPickupSound(std::int32_t StepIndex, TEFilmObj* Obj) {
        PEFilmObjectCommand Command = reinterpret_cast<PEFilmObjectCommand>(AddCommand(StepIndex));
        Command->Kind = efcPlayPickupSound;
        Command->Obj = Obj;
    }

    void TEFilm::ExecuteCommand(SE_Process::TProcessSE* Process, PEFilmCommand Command, std::uint8_t ReplayMode) {
        TEFilmObj* Obj{};
        SE_Space::TObjectSE* Source{};
        SE_Space::TObjectSE* Target{};
        std::int32_t ErrorStep{};
        SE_Ship2::TShip2SE* Ship{};
        SE_Ruins::TRuinsSE* Ruins{};
        // Process is unused in the native routine; playback uses the global SpaceProcess.
        ErrorStep = 0;
        try {
            switch (Command->Kind) {
                case efcSetObjectPosition: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->SetPosition(reinterpret_cast<PEFilmVectorCommand>(Command)->Position);
                    }
                    break;
                }
                case efcSetObjectOrbitCenter: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->SetOrbitCenter(reinterpret_cast<PEFilmVectorCommand>(Command)->Position);
                    }
                    break;
                }
                case efcSetObjectAlpha: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->SetAlpha(reinterpret_cast<PEFilmByteCommand>(Command)->Value);
                    }
                    break;
                }
                case efcSetObjectAngle: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->SetAngle(reinterpret_cast<PEFilmByteCommand>(Command)->Value);
                    }
                    break;
                }
                case efcAdvanceObject: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->Advance();
                    }
                    break;
                }
                case efcAdvanceObjects: {
                    Obj = FirstObject;
                    while (Obj != nullptr) {
                        if (Obj->SceneObject != nullptr) {
                            Obj->SceneObject->Advance();
                        }
                        Obj = Obj->Next;
                    }
                    break;
                }
                case efcSetPlanetState: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetRotationTimerInterval(reinterpret_cast<PEFilmObjectCommand>(Command)->Value & 0x00ffffff);
                        pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetRingKind(pas::shr(reinterpret_cast<PEFilmObjectCommand>(Command)->Value, 24));
                        pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetSurfaceMapStep(reinterpret_cast<PEFilmObjectCommand>(Command)->ExtraValue);
                        {
                            auto& cpp_target = pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->OrbitalVelocity;
                            cpp_target = pas::real_divide(static_cast<std::int16_t>(reinterpret_cast<PEFilmObjectCommand>(Command)->Flags & 0x0000ffff), 1.0E+3L);
                        }
                        pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetMinimapOwner(pas::shr(reinterpret_cast<PEFilmObjectCommand>(Command)->Flags, 24));
                        pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->Civilized = pas::checked_cast<SE_Planet::TPlanetSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->MinimapOwner != 6;
                    }
                    break;
                }
                case efcSetShipSizeAndTailMode: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Ship2::TShip2SE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        Ship = pas::checked_cast<SE_Ship2::TShip2SE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject);
                        Ship->SetSize(reinterpret_cast<PEFilmSizeCommand>(Command)->Size);
                        if (GlobalsV::ShipTail == 2 || GlobalsV::ShipTail == 1 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Id == static_cast<std::int32_t>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->ObjectId)) {
                            Ship->SetTailMode(reinterpret_cast<PEFilmSizeCommand>(Command)->TailMode);
                        } else {
                            Ship->SetTailMode(0);
                        }
                    }
                    break;
                }
                case efcSetRuinsState: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Ruins::TRuinsSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        Ruins = pas::checked_cast<SE_Ruins::TRuinsSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject);
                        Ruins->SetState(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    }
                    break;
                }
                case efcSetWeaponHit: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetHit(reinterpret_cast<PEFilmHitCommand>(Command)->Color, reinterpret_cast<PEFilmHitCommand>(Command)->Damage, reinterpret_cast<PEFilmHitCommand>(Command)->Destroyed, reinterpret_cast<PEFilmHitCommand>(Command)->PlaySound);
                    }
                    break;
                }
                case efcSetWeaponEndpoints: {
                    // Both endpoint scene-object tests are repeated in the native code.
                    Source = nullptr;
                    if (reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source != nullptr && reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source->SceneObject != nullptr && reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source->SceneObject != nullptr) {
                        Source = reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source->SceneObject;
                    }
                    Target = nullptr;
                    if (reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target != nullptr && reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target->SceneObject != nullptr && reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target->SceneObject != nullptr) {
                        Target = reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target->SceneObject;
                    }
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetEndpoints(Source, Target);
                    }
                    break;
                }
                case efcSetDestructionEffect: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Weapon::TWeaponSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->DestructionEffect = reinterpret_cast<PEFilmObjectCommand>(Command)->Value;
                    }
                    break;
                }
                case efcSetEffectImagePosition: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_GAIEffect::TGAIEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_GAIEffect::TGAIEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetImagePosition(reinterpret_cast<PEFilmSizeCommand>(Command)->Size);
                    }
                    break;
                }
                case efcSetEffectDurationScale: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_GAIEffect::TGAIEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_GAIEffect::TGAIEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetDurationScale(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                    }
                    break;
                }
                case efcAttachObject: {
                    ErrorStep = 1;
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        ErrorStep = 2;
                        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                            ErrorStep = 3;
                            {
                                SE_Ship2::TShip2SE* cpp_with = pas::checked_cast<SE_Ship2::TShip2SE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject);
                                ErrorStep = 4;
                                if (GlobalsV::ShipTail != 2 && (GlobalsV::ShipTail != 1 || aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->Id != static_cast<std::int32_t>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->ObjectId)) || cpp_with->TailMode <= 0) {
                                    cpp_with->SetTailMode(0);
                                }
                            }
                        }
                        ErrorStep = 5;
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->AttachToSpace(Globals::SpaceProcess->Space);
                    }
                    break;
                }
                case efcDetachObject: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->DetachFromSpace();
                    }
                    break;
                }
                case efcReleaseObject: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->DetachFromSpace();
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject));
                    }
                    break;
                }
                case efcReleaseWeaponEffects: {
                    Obj = FirstObject;
                    while (Obj != nullptr) {
                        if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Weapon::TWeaponSE*>(Obj->SceneObject) != nullptr) {
                            Obj->SceneObject->DetachFromSpace();
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject));
                        }
                        Obj = Obj->Next;
                    }
                    break;
                }
                case efcSetViewCenter: {
                    if (ReplayMode) {
                        Globals::FilmScreen->FollowViewOffset(EC_Struct::TruncatePointF(reinterpret_cast<PEFilmVectorCommand>(Command)->Position));
                    } else {
                        Globals::StarMapScreen->SetMapCenter(EC_Struct::TruncatePointF(reinterpret_cast<PEFilmVectorCommand>(Command)->Position));
                    }
                    break;
                }
                case efcSetRadarCenter: {
                    if (ReplayMode) {
                        Globals::FilmScreen->CameraTarget = reinterpret_cast<PEFilmVectorCommand>(Command)->Position;
                    }
                    Globals::SpaceProcess->RadarCenter = reinterpret_cast<PEFilmVectorCommand>(Command)->Position;
                    break;
                }
                case efcSetCameraAnchor: {
                    CameraAnchor = reinterpret_cast<PEFilmVectorCommand>(Command)->Position;
                    ForceCameraMovement = reinterpret_cast<PEFilmVectorCommand>(Command)->ForceMovement;
                    break;
                }
                case efcOpenGate: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->Open();
                    }
                    break;
                }
                case efcCloseGate: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->Close();
                    }
                    break;
                }
                case efcSetGateState: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetState(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    }
                    break;
                }
                case efcSetGateSize: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Gate::TGateSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetSize(ClassesImports::Point(reinterpret_cast<PEFilmObjectCommand>(Command)->Value, reinterpret_cast<PEFilmObjectCommand>(Command)->Value));
                    }
                    break;
                }
                case efcSetGateEffectSize: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Gate::TGateEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Gate::TGateEffectSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetSize(ClassesImports::Point(reinterpret_cast<PEFilmObjectCommand>(Command)->Value, reinterpret_cast<PEFilmObjectCommand>(Command)->Value));
                    }
                    break;
                }
                case efcSetHoleState: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && pas::class_cast_if<SE_Hole::THoleSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject) != nullptr) {
                        pas::checked_cast<SE_Hole::THoleSE*>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject)->SetState(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    }
                    break;
                }
                case efcSetObjectText: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        const pas::WideString& textAt = StringTable->GetTextAt(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                        SE_Space::TObjectSE* sceneObject = reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject;
                        sceneObject->SetText(textAt);
                    }
                    break;
                }
                case efcPlayObjectSound: {
                    if (Globals::FilmSoundEffectsEnabled && GlobalsV::SoundInSpaceEnabled && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->Space != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->Space->ContainsMapPoint(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->Position)) {
                        const pas::WideString& textAt_2 = StringTable->GetTextAt(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                        GR_Sound::TSoundControl* soundManager = GR_Main::SoundManager;
                        soundManager->PlaySound(textAt_2);
                    }
                    break;
                }
                case efcSetObjectStateBuffer: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->LoadStateBuffer(pas::list_at<EC_Buf::TBufEC>(DataBuffers, reinterpret_cast<PEFilmObjectCommand>(Command)->Value));
                    }
                    break;
                }
                case efcPlayPickupSound: {
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr && reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr && GlobalsV::SoundInSpaceEnabled && Globals::SpaceProcess->Space->ContainsMapPoint(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->Position)) {
                        GR_Main::SoundManager->PlaySound(u"Sound.Take"_wref.get());
                    }
                    break;
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Error in procedure TEFilm.RunOrder, order = ", SysUtils::IntToStr(Command->Kind), ", label = ", SysUtils::IntToStr(ErrorStep)}));
                if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj != nullptr) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->KindName));
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->GraphKey));
                    if (reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject != nullptr) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj->SceneObject->GraphKey));
                    }
                }
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TEFilm.RunOrder, order = ", SysUtils::IntToStr(Command->Kind), ", label = ", SysUtils::IntToStr(ErrorStep)})));
            } else {
                throw;
            }
        }
    }

    // Detaches weapon effects and releases their retained scene references.
    void TEFilm::ReleaseWeaponSceneObjects() {
        TEFilmObj* Entry{};
        Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry->SceneObject != nullptr) {
                if (pas::class_cast_if<SE_Weapon::TWeaponSE*>(Entry->SceneObject) != nullptr) {
                    Entry->SceneObject->DetachFromSpace();
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject));
                }
            }
            Entry = Entry->Next;
        }
    }

    void TEFilm::ReleaseObjectReferences(SE_Space::TObjectSE* Obj) {
        TEFilmObj* Entry{};
        Entry = FirstObject;
        while (Entry != nullptr) {
            if (Entry->SceneObject != nullptr) {
                if (Entry->SceneObject == Obj) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject));
                    Entry->ObjectId = 0u;
                    Entry->KindName = pas::WideString();
                    Entry->GraphKey = pas::WideString();
                }
            }
            Entry = Entry->Next;
        }
    }

    // Clears Buffer. Serializes object identities and commands, excluding live scene references and Turn.
    void TEFilm::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        TEFilmObj* Obj{};
        std::int32_t I{};
        EC_Buf::TBufEC* Data{};
        std::int32_t ObjectIndex{};
        Buffer->Clear();
        Buffer->AddWideStringZ(SystemProcessName);
        Buffer->AddIntegerValue(SystemImports::MaxInt);
        Buffer->AddIntegerValue(FilmFormatVersion);
        Buffer->AddIntegerValue(MapDiameter);
        Buffer->AddIntegerValue(RadarRange);
        Buffer->AddBoolean(PlayerCombatRecorded);
        Buffer->AddDWord(StarGenerationSeed);
        Buffer->AddIntegerValue(BackgroundImage);
        Buffer->AddDWord(InitialActivity);
        Buffer->AddDWord(FinalActivity);
        Buffer->AddSingle(CameraAnchor.X);
        Buffer->AddSingle(CameraAnchor.Y);
        std::int32_t Count = StringTable->GetCount();
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Buffer->AddWideStringZ(StringTable->GetTextAt(I));
        }
        Count = pas::list_count(DataBuffers);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Data = pas::list_at<EC_Buf::TBufEC>(DataBuffers, I);
            Buffer->AddBuffer(Data);
        }
        Buffer->AddWideChar(ObjectCount());
        Obj = FirstObject;
        while (Obj != nullptr) {
            Buffer->AddDWord(Obj->ObjectId);
            Buffer->AddWideStringZ(Obj->KindName);
            Buffer->AddWideStringZ(Obj->GraphKey);
            Obj = Obj->Next;
        }
        Buffer->AddDWord(CommandCount());
        PEFilmCommand Command = FirstCommand;
        while (Command != nullptr) {
            Buffer->AddAnsiChar(Command->Kind);
            Buffer->AddWideChar(Command->StepIndex);
            if (Command->Kind == efcSetObjectPosition) {
                Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y);
            } else if (Command->Kind == efcSetObjectOrbitCenter) {
                Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y);
            } else if (Command->Kind == efcSetObjectAlpha) {
                Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                Buffer->AddAnsiChar(reinterpret_cast<PEFilmByteCommand>(Command)->Value);
            } else if (Command->Kind == efcSetObjectAngle) {
                Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                Buffer->AddAnsiChar(reinterpret_cast<PEFilmByteCommand>(Command)->Value);
            } else if (Command->Kind == efcAdvanceObject) {
                Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
            } else if (!(Command->Kind == efcAdvanceObjects)) {
                if (Command->Kind == efcSetPlanetState) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->ExtraValue);
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Flags);
                } else if (Command->Kind == efcSetShipSizeAndTailMode) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddWideChar(reinterpret_cast<PEFilmSizeCommand>(Command)->Size.X);
                    Buffer->AddWideChar(reinterpret_cast<PEFilmSizeCommand>(Command)->Size.Y);
                    Buffer->AddAnsiChar(reinterpret_cast<PEFilmSizeCommand>(Command)->TailMode);
                } else if (Command->Kind == efcSetRuinsState) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddAnsiChar(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                } else if (Command->Kind == efcSetWeaponHit) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddAnsiChar(GR_Main::CurrentPixelFormat->UnpackRed(reinterpret_cast<PEFilmHitCommand>(Command)->Color));
                    Buffer->AddAnsiChar(GR_Main::CurrentPixelFormat->UnpackGreen(reinterpret_cast<PEFilmHitCommand>(Command)->Color));
                    Buffer->AddAnsiChar(GR_GraphBuf::TPixelFormatGR::UnpackBlue(reinterpret_cast<PEFilmHitCommand>(Command)->Color));
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmHitCommand>(Command)->Damage);
                    Buffer->AddBoolean(reinterpret_cast<PEFilmHitCommand>(Command)->Destroyed);
                    Buffer->AddBoolean(reinterpret_cast<PEFilmHitCommand>(Command)->PlaySound);
                } else if (Command->Kind == efcSetWeaponEndpoints) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    ObjectIndex = FindObjectIndex(reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source);
                    if (ObjectIndex == -1) {
                        ObjectIndex = FilmNullObjectIndex;
                        reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source = nullptr;
                    }
                    Buffer->AddWideChar(ObjectIndex);
                    ObjectIndex = FindObjectIndex(reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target);
                    if (ObjectIndex == -1) {
                        ObjectIndex = FilmNullObjectIndex;
                        reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target = nullptr;
                    }
                    Buffer->AddWideChar(ObjectIndex);
                } else if (Command->Kind == efcSetDestructionEffect) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddAnsiChar(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                } else if (Command->Kind == efcSetEffectImagePosition) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmSizeCommand>(Command)->Size.X);
                    Buffer->AddIntegerValue(reinterpret_cast<PEFilmSizeCommand>(Command)->Size.Y);
                } else if (Command->Kind == efcSetEffectDurationScale) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                } else if (Command->Kind == efcAttachObject) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                } else if (Command->Kind == efcDetachObject) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                } else if (Command->Kind == efcReleaseObject) {
                    Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                } else if (!(Command->Kind == efcReleaseWeaponEffects)) {
                    if (Command->Kind == efcSetViewCenter) {
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y);
                    } else if (Command->Kind == efcSetRadarCenter) {
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y);
                    } else if (Command->Kind == efcSetCameraAnchor) {
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X);
                        Buffer->AddSingle(reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y);
                        Buffer->AddBoolean(reinterpret_cast<PEFilmVectorCommand>(Command)->ForceMovement);
                    } else if (Command->Kind == efcOpenGate) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    } else if (Command->Kind == efcCloseGate) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                    } else if (Command->Kind == efcSetGateState) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcSetGateSize) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcSetGateEffectSize) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcSetHoleState) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddBoolean(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcSetObjectText) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcPlayObjectSound) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (Command->Kind == efcSetObjectStateBuffer) {
                        Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        Buffer->AddIntegerValue(reinterpret_cast<PEFilmObjectCommand>(Command)->Value);
                    } else if (!(Command->Kind == efcBeginTrailingEffects)) {
                        if (Command->Kind == efcPlayPickupSound) {
                            Buffer->AddWideChar(ObjToNom(reinterpret_cast<PEFilmObjectCommand>(Command)->Obj));
                        }
                    }
                }
            }
            Command = Command->Next;
        }
        Buffer->AddIntegerValue(CameraEventCount);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, CameraEventCount - 1); cpp_range_3.next(I); ) {
            TEFilmCameraEvent& cpp_with = CameraEvents[I];
            Buffer->AddIntegerValue(cpp_with.StepIndex);
            Buffer->AddIntegerValue(cpp_with.Priority);
            Buffer->AddSingle(cpp_with.StartPosition.X);
            Buffer->AddSingle(cpp_with.StartPosition.Y);
            Buffer->AddSingle(cpp_with.EndPosition.X);
            Buffer->AddSingle(cpp_with.EndPosition.Y);
        }
        pas::checked_cast<aEObjInfo::TEObjInfo*>(ObjectInfo)->SaveToBuffer(Buffer);
    }

    // Clears the film and rewinds Buffer before reading. Scene objects are recreated separately.
    void TEFilm::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        TEFilmObj* Obj{};
        PEFilmCommand Command{};
        std::uint8_t Red{};
        std::uint8_t Green{};
        std::uint8_t Blue{};
        EC_Buf::TBufEC* Data{};
        std::int32_t Version = 0;
        Clear();
        Buffer->SetPosition(0);
        SystemProcessName = Buffer->ReadWideString();
        MapDiameter = EC_Buf::TBufEC_GetInt32(Buffer);
        if (MapDiameter == SystemImports::MaxInt) {
            Version = EC_Buf::TBufEC_GetInt32(Buffer);
            MapDiameter = EC_Buf::TBufEC_GetInt32(Buffer);
        }
        RadarRange = EC_Buf::TBufEC_GetInt32(Buffer);
        PlayerCombatRecorded = EC_Buf::TBufEC_GetBoolean(Buffer);
        StarGenerationSeed = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Version >= 2) {
            BackgroundImage = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            BackgroundImage = 0;
        }
        InitialActivity = EC_Buf::TBufEC_GetUInt32(Buffer);
        FinalActivity = EC_Buf::TBufEC_GetUInt32(Buffer);
        CameraAnchor.X = EC_Buf::TBufEC_GetSingle(Buffer);
        CameraAnchor.Y = EC_Buf::TBufEC_GetSingle(Buffer);
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            const pas::WideString& readWideString = Buffer->ReadWideString();
            EC_Str::TStringsEC* stringTable = StringTable;
            stringTable->Add(readWideString);
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Data = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            Buffer->ReadLengthPrefixedBuffer(Data);
            pas::list_add(DataBuffers, reinterpret_cast<void*>(Data));
        }
        Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(I); ) {
            Obj = AllocateObject();
            Obj->ObjectId = EC_Buf::TBufEC_GetUInt32(Buffer);
            Obj->KindName = Buffer->ReadWideString();
            Obj->GraphKey = Buffer->ReadWideString();
            Obj->SceneObject = nullptr;
        }
        Count = EC_Buf::TBufEC_GetUInt32(Buffer);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(I); ) {
            Command = AllocateCommand();
            AppendCommand(Command);
            Command->Kind = EC_Buf::TBufEC_GetByte(Buffer);
            Command->StepIndex = EC_Buf::TBufEC_GetWord(Buffer);
            if (Command->Kind == efcSetObjectPosition) {
                reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            } else if (Command->Kind == efcSetObjectOrbitCenter) {
                reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            } else if (Command->Kind == efcSetObjectAlpha) {
                reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                reinterpret_cast<PEFilmByteCommand>(Command)->Value = EC_Buf::TBufEC_GetByte(Buffer);
            } else if (Command->Kind == efcSetObjectAngle) {
                reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                reinterpret_cast<PEFilmByteCommand>(Command)->Value = EC_Buf::TBufEC_GetByte(Buffer);
            } else if (Command->Kind == efcAdvanceObject) {
                reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
            } else if (!(Command->Kind == efcAdvanceObjects)) {
                if (Command->Kind == efcSetPlanetState) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    reinterpret_cast<PEFilmObjectCommand>(Command)->ExtraValue = EC_Buf::TBufEC_GetInt32(Buffer);
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Flags = EC_Buf::TBufEC_GetInt32(Buffer);
                } else if (Command->Kind == efcSetShipSizeAndTailMode) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmSizeCommand>(Command)->Size.X = EC_Buf::TBufEC_GetWord(Buffer);
                    reinterpret_cast<PEFilmSizeCommand>(Command)->Size.Y = EC_Buf::TBufEC_GetWord(Buffer);
                    reinterpret_cast<PEFilmSizeCommand>(Command)->TailMode = EC_Buf::TBufEC_GetByte(Buffer);
                } else if (Command->Kind == efcSetRuinsState) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetByte(Buffer);
                } else if (Command->Kind == efcSetWeaponHit) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    Red = EC_Buf::TBufEC_GetByte(Buffer);
                    Green = EC_Buf::TBufEC_GetByte(Buffer);
                    Blue = EC_Buf::TBufEC_GetByte(Buffer);
                    reinterpret_cast<PEFilmHitCommand>(Command)->Color = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
                    reinterpret_cast<PEFilmHitCommand>(Command)->Damage = EC_Buf::TBufEC_GetInt32(Buffer);
                    reinterpret_cast<PEFilmHitCommand>(Command)->Destroyed = EC_Buf::TBufEC_GetBoolean(Buffer);
                    reinterpret_cast<PEFilmHitCommand>(Command)->PlaySound = EC_Buf::TBufEC_GetBoolean(Buffer);
                } else if (Command->Kind == efcSetWeaponEndpoints) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmEndpointsCommand>(Command)->Source = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmEndpointsCommand>(Command)->Target = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                } else if (Command->Kind == efcSetDestructionEffect) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetByte(Buffer);
                } else if (Command->Kind == efcSetEffectImagePosition) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmSizeCommand>(Command)->Size.X = EC_Buf::TBufEC_GetInt32(Buffer);
                    reinterpret_cast<PEFilmSizeCommand>(Command)->Size.Y = EC_Buf::TBufEC_GetInt32(Buffer);
                } else if (Command->Kind == efcSetEffectDurationScale) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                } else if (Command->Kind == efcAttachObject) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                } else if (Command->Kind == efcDetachObject) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                } else if (Command->Kind == efcReleaseObject) {
                    reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                } else if (!(Command->Kind == efcReleaseWeaponEffects)) {
                    if (Command->Kind == efcSetViewCenter) {
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                    } else if (Command->Kind == efcSetRadarCenter) {
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                    } else if (Command->Kind == efcSetCameraAnchor) {
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.X = EC_Buf::TBufEC_GetSingle(Buffer);
                        reinterpret_cast<PEFilmVectorCommand>(Command)->Position.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                        reinterpret_cast<PEFilmVectorCommand>(Command)->ForceMovement = EC_Buf::TBufEC_GetBoolean(Buffer);
                    } else if (Command->Kind == efcOpenGate) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    } else if (Command->Kind == efcCloseGate) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                    } else if (Command->Kind == efcSetGateState) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (Command->Kind == efcSetGateSize) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (Command->Kind == efcSetGateEffectSize) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (Command->Kind == efcSetHoleState) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetBoolean(Buffer) & 127;
                    } else if (Command->Kind == efcSetObjectText) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (Command->Kind == efcPlayObjectSound) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (Command->Kind == efcSetObjectStateBuffer) {
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        reinterpret_cast<PEFilmObjectCommand>(Command)->Value = EC_Buf::TBufEC_GetInt32(Buffer);
                    } else if (!(Command->Kind == efcBeginTrailingEffects)) {
                        if (Command->Kind == efcPlayPickupSound) {
                            reinterpret_cast<PEFilmObjectCommand>(Command)->Obj = NomToObj(EC_Buf::TBufEC_GetWord(Buffer));
                        }
                    }
                }
            }
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(I); ) {
            ReserveCameraEventSlot();
            {
                TEFilmCameraEvent& cpp_with = CameraEvents[CameraEventCount - 1];
                cpp_with.StepIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                cpp_with.Priority = EC_Buf::TBufEC_GetInt32(Buffer);
                cpp_with.StartPosition.X = EC_Buf::TBufEC_GetSingle(Buffer);
                cpp_with.StartPosition.Y = EC_Buf::TBufEC_GetSingle(Buffer);
                cpp_with.EndPosition.X = EC_Buf::TBufEC_GetSingle(Buffer);
                cpp_with.EndPosition.Y = EC_Buf::TBufEC_GetSingle(Buffer);
            }
        }
        if (Version > 0) {
            pas::checked_cast<aEObjInfo::TEObjInfo*>(ObjectInfo)->LoadFromBuffer(Buffer, Version);
        } else {
            pas::checked_cast<aEObjInfo::TEObjInfo*>(ObjectInfo)->Clear();
        }
    }

    void TEFilm::p_destroy() {
        aEFilm::TEFilm_Destroy(this);
    }

} // namespace aEFilm
