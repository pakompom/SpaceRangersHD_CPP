#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace SE_Process {
    struct TProcessSE;

} // namespace SE_Process

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilmByteCommand;

    struct TEFilmCommand;

    struct TEFilmEndpointsCommand;

    struct TEFilmHitCommand;

    struct TEFilmObjectCommand;

    struct TEFilmSizeCommand;

    struct TEFilmVectorCommand;

    struct TEFilmObj;

    struct TEFilmCameraEvent;

    struct TEFilm;

    using PEFilmCommand = TEFilmCommand*;

    #pragma pack(push, 1)
    struct TEFilmCameraEvent {
        std::int32_t StepIndex;
        std::int32_t Priority;
        EC_Struct::TPointF StartPosition;
        EC_Struct::TPointF EndPosition;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEFilm : EC_Struct::TObjectEx {
        PAS_CLASS_META(TEFilm, EC_Struct::TObjectEx, "TEFilm", 96)
        void p_destroy() override;
        void Clear();
        // Grows capacity and advances the used count without writing the new slot.
        void ReserveCameraEventSlot();
        void AddCameraEvent(std::int32_t AStepIndex, EC_Struct::TPointF AStartPosition, EC_Struct::TPointF AEndPosition, std::int32_t APriority);
        // Appends an object owned by this film.
        TEFilmObj* AllocateObject();
        void RemoveObject(TEFilmObj* Obj);
        std::int32_t ObjectCount();
        // Zero-based list index; nil maps to 65535. Raises for an object outside this film.
        std::int32_t ObjToNom(TEFilmObj* Obj);
        // Nil maps to 65535; an absent non-nil object maps to -1.
        std::int32_t FindObjectIndex(TEFilmObj* Obj);
        // Returns a borrowed object. Index 65535 maps to nil; other missing indexes raise.
        TEFilmObj* NomToObj(std::int32_t Index);
        std::uint8_t ContainsObject(TEFilmObj* Obj);
        // Matches all three keys; returns a borrowed object or nil.
        TEFilmObj* FindObject(const pas::WideString& KindName, const pas::WideString& GraphKey, std::uint32_t ObjectId);
        TEFilmObj* FindObjectById(const pas::WideString& KindName, std::uint32_t ObjectId);
        void GrowCommandPool(std::int32_t Count);
        // Moves an inclusive linked range to the free list.
        void RecycleCommands(PEFilmCommand First, PEFilmCommand Last);
        void AppendCommand(PEFilmCommand Command);
        // Nil Before appends.
        void InsertCommand(PEFilmCommand Before, PEFilmCommand Command);
        // Returns a zeroed pooled command without linking it into the command list.
        PEFilmCommand AllocateCommand();
        // Stable insertion by step index.
        PEFilmCommand AddCommand(std::int32_t StepIndex);
        std::int32_t CommandCount();
        // Retains SceneObject and copies its class name and graph key. Both stack arguments are unused.
        TEFilmObj* AddObject(std::uint32_t ObjectId, SE_Space::TObjectSE* SceneObject, std::int32_t Unused1, std::int32_t Unused2);
        void SetObjectPosition(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position);
        void SetObjectOrbitCenter(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position);
        void SetObjectAlpha(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Alpha);
        // A full turn is 256 angle units.
        void SetObjectAngle(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Angle);
        // Queues slot 0x40 on each retained scene object.
        void AdvanceObjects(std::int32_t StepIndex);
        // Scale is decoded as a signed 16-bit value divided by 1000 during playback.
        void SetPlanetState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t RotationInterval, std::int32_t SurfaceMapStep, std::uint16_t ScaleThousandths, std::uint8_t RingKind, std::uint8_t Owner);
        void SetShipSizeAndTailMode(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Size, std::int32_t TailMode);
        void SetRuinsState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetWeaponHit(std::int32_t StepIndex, TEFilmObj* Obj, std::uint16_t Color, std::int32_t Damage, std::uint8_t Destroyed, std::uint8_t PlaySound);
        // Source and Target may be nil; Obj must exist.
        void SetWeaponEndpoints(std::int32_t StepIndex, TEFilmObj* Obj, TEFilmObj* Source, TEFilmObj* Target);
        // Playback selects TWeaponSE destruction effects: 1=bomb, 2=asteroid, 5=kamikaze; other modes include fades and immediate removal.
        void SetDestructionEffect(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Value);
        void SetEffectImagePosition(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Position);
        // Records the Single duration multiplier consumed by TGAIEffectSE.SetDurationScale.
        void SetEffectDurationScale(std::int32_t StepIndex, TEFilmObj* Obj, float Scale);
        void AttachObject(std::int32_t StepIndex, TEFilmObj* Obj);
        void DetachObject(std::int32_t StepIndex, TEFilmObj* Obj);
        // Playback detaches and releases the scene reference, retaining the film entry.
        void ReleaseObject(std::int32_t StepIndex, TEFilmObj* Obj);
        void ReleaseWeaponEffects(std::int32_t StepIndex);
        void SetViewCenter(std::int32_t StepIndex, EC_Struct::TPointF Position);
        void SetRadarCenter(std::int32_t StepIndex, EC_Struct::TPointF Position);
        void SetCameraAnchor(std::int32_t StepIndex, EC_Struct::TPointF Position, std::uint8_t ForceMovement);
        void OpenGate(std::int32_t StepIndex, TEFilmObj* Obj);
        // Playback changes gate state 2 to 3 and resets its timer.
        void CloseGate(std::int32_t StepIndex, TEFilmObj* Obj);
        void SetGateState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetGateSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size);
        void SetGateEffectSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size);
        void SetHoleState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetObjectText(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text);
        void PlayObjectSound(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text);
        // Transfers ownership of Buffer to the film.
        void SetObjectStateBuffer(std::int32_t StepIndex, TEFilmObj* Obj, EC_Buf::TBufEC* Buffer);
        // Appends the kind-24 boundary consumed by film playback.
        void BeginTrailingEffects(std::int32_t StepIndex);
        void PlayPickupSound(std::int32_t StepIndex, TEFilmObj* Obj);
        void ExecuteCommand(SE_Process::TProcessSE* Process, PEFilmCommand Command, std::uint8_t ReplayMode);
        // Detaches weapon effects and releases their retained scene references.
        void ReleaseWeaponSceneObjects();
        void ReleaseObjectReferences(SE_Space::TObjectSE* Obj);
        // Clears Buffer. Serializes object identities and commands, excluding live scene references and Turn.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Clears the film and rewinds Buffer before reading. Scene objects are recreated separately.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        TEFilmObj* FirstObject;
        TEFilmObj* LastObject;
        PEFilmCommand FirstCommand;
        PEFilmCommand LastCommand;
        PEFilmCommand FirstFreeCommand;
        PEFilmCommand LastFreeCommand;
        pas::DynArray<TEFilmCameraEvent> CameraEvents;
        std::int32_t CameraEventCount;
        EC_Str::TStringsEC* StringTable;
        // Owned TBufEC entries.
        pas::List* DataBuffers;
        pas::WideString SystemProcessName;
        std::int32_t MapDiameter;
        std::int32_t RadarRange;
        // Stored by TFilmFile, outside this film's serialized payload.
        std::int32_t Turn;
        // Set from RecordFilm and the star player-combat flag.
        std::uint8_t PlayerCombatRecorded;
        std::uint8_t cpp_padding[3];
        std::int32_t BackgroundImage;
        std::uint32_t StarGenerationSeed;
        // Activity categories used to select film playback speed.
        std::int32_t InitialActivity;
        std::int32_t FinalActivity;
        EC_Struct::TPointF CameraAnchor;
        std::uint8_t ForceCameraMovement;
        std::uint8_t cpp_padding_2[3];
        // Native callers cast this snapshot to TEObjInfo.
        pas::Object* ObjectInfo;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEFilmObj : pas::Object {
        PAS_CLASS_META(TEFilmObj, pas::Object, "TEFilmObj", 28)
        TEFilmObj* Prev;
        TEFilmObj* Next;
        std::uint32_t ObjectId;
        // Retained reference; nil after deserialization.
        SE_Space::TObjectSE* SceneObject;
        pas::WideString KindName;
        pas::WideString GraphKey;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TEFilmCommand {
        PEFilmCommand Prev;
        PEFilmCommand Next;
        // efc* tag; payload is interpreted through the command views below.
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        std::int32_t StepIndex;
        pas::Array<std::uint8_t, 0, 15> Payload;
    };
    #pragma pack(pop)

    // Scalar object-command payload view.
    #pragma pack(push, 1)
    struct TEFilmObjectCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        std::int32_t Value;
        std::int32_t ExtraValue;
        std::int32_t Flags;
    };
    #pragma pack(pop)

    using PEFilmObjectCommand = TEFilmObjectCommand*;

    #pragma pack(push, 1)
    struct TEFilmByteCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        std::uint8_t Value;
        std::uint8_t cpp_padding_3[11];
    };
    #pragma pack(pop)

    using PEFilmByteCommand = TEFilmByteCommand*;

    #pragma pack(push, 1)
    struct TEFilmVectorCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        EC_Struct::TPointF Position;
        std::uint8_t ForceMovement;
        std::uint8_t cpp_padding_3[3];
    };
    #pragma pack(pop)

    using PEFilmVectorCommand = TEFilmVectorCommand*;

    #pragma pack(push, 1)
    struct TEFilmHitCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        std::uint16_t Color;
        std::uint8_t cpp_padding_3[2];
        std::int32_t Damage;
        std::uint8_t Destroyed;
        std::uint8_t PlaySound;
        std::uint8_t cpp_padding_4[2];
    };
    #pragma pack(pop)

    using PEFilmHitCommand = TEFilmHitCommand*;

    #pragma pack(push, 1)
    struct TEFilmEndpointsCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        TEFilmObj* Source;
        TEFilmObj* Target;
        std::uint8_t cpp_padding_3[4];
    };
    #pragma pack(pop)

    using PEFilmEndpointsCommand = TEFilmEndpointsCommand*;

    #pragma pack(push, 1)
    struct TEFilmSizeCommand {
        std::uint8_t cpp_padding[8];
        std::uint8_t Kind;
        std::uint8_t cpp_padding_2[3];
        std::int32_t StepIndex;
        TEFilmObj* Obj;
        Types::TPoint Size;
        std::int32_t TailMode;
    };
    #pragma pack(pop)

    using PEFilmSizeCommand = TEFilmSizeCommand*;

    // Native serialized tags. Kind remains Byte so unknown values stay representable.
    inline constexpr std::int32_t efcSetObjectPosition = 0;

    inline constexpr std::int32_t efcSetObjectOrbitCenter = 1;

    inline constexpr std::int32_t efcSetObjectAlpha = 2;

    inline constexpr std::int32_t efcSetObjectAngle = 3;

    // No recovered writer; playback calls TObjectSE.Advance.
    inline constexpr std::int32_t efcAdvanceObject = 4;

    inline constexpr std::int32_t efcAdvanceObjects = 5;

    inline constexpr std::int32_t efcSetPlanetState = 6;

    inline constexpr std::int32_t efcSetShipSizeAndTailMode = 7;

    inline constexpr std::int32_t efcSetWeaponHit = 8;

    inline constexpr std::int32_t efcSetWeaponEndpoints = 9;

    inline constexpr std::int32_t efcSetDestructionEffect = 10;

    inline constexpr std::int32_t efcAttachObject = 11;

    inline constexpr std::int32_t efcDetachObject = 12;

    inline constexpr std::int32_t efcReleaseObject = 13;

    inline constexpr std::int32_t efcReleaseWeaponEffects = 14;

    inline constexpr std::int32_t efcSetViewCenter = 15;

    inline constexpr std::int32_t efcSetRadarCenter = 16;

    inline constexpr std::int32_t efcSetCameraAnchor = 17;

    inline constexpr std::int32_t efcOpenGate = 18;

    inline constexpr std::int32_t efcCloseGate = 19;

    inline constexpr std::int32_t efcSetGateState = 20;

    inline constexpr std::int32_t efcSetHoleState = 21;

    inline constexpr std::int32_t efcSetObjectText = 22;

    inline constexpr std::int32_t efcSetObjectStateBuffer = 23;

    // Playback barrier; ExecuteCommand has no action for this tag.
    inline constexpr std::int32_t efcBeginTrailingEffects = 24;

    inline constexpr std::int32_t efcPlayPickupSound = 25;

    inline constexpr std::int32_t efcSetRuinsState = 26;

    inline constexpr std::int32_t efcSetGateSize = 27;

    inline constexpr std::int32_t efcPlayObjectSound = 28;

    inline constexpr std::int32_t efcSetGateEffectSize = 29;

    inline constexpr std::int32_t efcSetEffectImagePosition = 30;

    inline constexpr std::int32_t efcSetEffectDurationScale = 31;

    // Serialized nil; -1 separately means an unlisted object.
    inline constexpr std::int32_t FilmNullObjectIndex = 65535;

} // namespace aEFilm
