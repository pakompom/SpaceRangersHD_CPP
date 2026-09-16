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
        void ReserveCameraEventSlot();
        void AddCameraEvent(std::int32_t AStepIndex, EC_Struct::TPointF AStartPosition, EC_Struct::TPointF AEndPosition, std::int32_t APriority);
        TEFilmObj* AllocateObject();
        void RemoveObject(TEFilmObj* Obj);
        std::int32_t ObjectCount();
        std::int32_t ObjToNom(TEFilmObj* Obj);
        std::int32_t FindObjectIndex(TEFilmObj* Obj);
        TEFilmObj* NomToObj(std::int32_t Index);
        std::uint8_t ContainsObject(TEFilmObj* Obj);
        TEFilmObj* FindObject(const pas::WideString& KindName, const pas::WideString& GraphKey, std::uint32_t ObjectId);
        TEFilmObj* FindObjectById(const pas::WideString& KindName, std::uint32_t ObjectId);
        void GrowCommandPool(std::int32_t Count);
        void RecycleCommands(PEFilmCommand First, PEFilmCommand Last);
        void AppendCommand(PEFilmCommand Command);
        void InsertCommand(PEFilmCommand Before, PEFilmCommand Command);
        PEFilmCommand AllocateCommand();
        PEFilmCommand AddCommand(std::int32_t StepIndex);
        std::int32_t CommandCount();
        TEFilmObj* AddObject(std::uint32_t ObjectId, SE_Space::TObjectSE* SceneObject, std::int32_t Unused1, std::int32_t Unused2);
        void SetObjectPosition(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position);
        void SetObjectOrbitCenter(std::int32_t StepIndex, TEFilmObj* Obj, EC_Struct::TPointF Position);
        void SetObjectAlpha(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Alpha);
        void SetObjectAngle(std::int32_t StepIndex, TEFilmObj* Obj, std::uint8_t Angle);
        void AdvanceObjects(std::int32_t StepIndex);
        void SetPlanetState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t RotationInterval, std::int32_t SurfaceMapStep, std::uint16_t ScaleThousandths, std::uint8_t RingKind, std::uint8_t Owner);
        void SetShipSizeAndTailMode(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Size, std::int32_t TailMode);
        void SetRuinsState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetWeaponHit(std::int32_t StepIndex, TEFilmObj* Obj, std::uint16_t Color, std::int32_t Damage, std::uint8_t Destroyed, std::uint8_t PlaySound);
        void SetWeaponEndpoints(std::int32_t StepIndex, TEFilmObj* Obj, TEFilmObj* Source, TEFilmObj* Target);
        void SetDestructionEffect(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Value);
        void SetEffectImagePosition(std::int32_t StepIndex, TEFilmObj* Obj, Types::TPoint Position);
        void SetEffectDurationScale(std::int32_t StepIndex, TEFilmObj* Obj, float Scale);
        void AttachObject(std::int32_t StepIndex, TEFilmObj* Obj);
        void DetachObject(std::int32_t StepIndex, TEFilmObj* Obj);
        void ReleaseObject(std::int32_t StepIndex, TEFilmObj* Obj);
        void ReleaseWeaponEffects(std::int32_t StepIndex);
        void SetViewCenter(std::int32_t StepIndex, EC_Struct::TPointF Position);
        void SetRadarCenter(std::int32_t StepIndex, EC_Struct::TPointF Position);
        void SetCameraAnchor(std::int32_t StepIndex, EC_Struct::TPointF Position, std::uint8_t ForceMovement);
        void OpenGate(std::int32_t StepIndex, TEFilmObj* Obj);
        void CloseGate(std::int32_t StepIndex, TEFilmObj* Obj);
        void SetGateState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetGateSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size);
        void SetGateEffectSize(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t Size);
        void SetHoleState(std::int32_t StepIndex, TEFilmObj* Obj, std::int32_t State);
        void SetObjectText(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text);
        void PlayObjectSound(std::int32_t StepIndex, TEFilmObj* Obj, const pas::WideString& Text);
        void SetObjectStateBuffer(std::int32_t StepIndex, TEFilmObj* Obj, EC_Buf::TBufEC* Buffer);
        void BeginTrailingEffects(std::int32_t StepIndex);
        void PlayPickupSound(std::int32_t StepIndex, TEFilmObj* Obj);
        void ExecuteCommand(SE_Process::TProcessSE* Process, PEFilmCommand Command, std::uint8_t ReplayMode);
        void ReleaseWeaponSceneObjects();
        void ReleaseObjectReferences(SE_Space::TObjectSE* Obj);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
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
        pas::List* DataBuffers;
        pas::WideString SystemProcessName;
        std::int32_t MapDiameter;
        std::int32_t RadarRange;
        std::int32_t Turn;
        std::uint8_t PlayerCombatRecorded;
        std::uint8_t cpp_padding[3];
        std::int32_t BackgroundImage;
        std::uint32_t StarGenerationSeed;
        std::int32_t InitialActivity;
        std::int32_t FinalActivity;
        EC_Struct::TPointF CameraAnchor;
        std::uint8_t ForceCameraMovement;
        std::uint8_t cpp_padding_2[3];
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
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        std::int32_t StepIndex;
        pas::Array<std::uint8_t, 0, 15> Payload;
    };
    #pragma pack(pop)

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

    inline constexpr std::int32_t efcSetObjectPosition = 0;

    inline constexpr std::int32_t efcSetObjectOrbitCenter = 1;

    inline constexpr std::int32_t efcSetObjectAlpha = 2;

    inline constexpr std::int32_t efcSetObjectAngle = 3;

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

    inline constexpr std::int32_t efcBeginTrailingEffects = 24;

    inline constexpr std::int32_t efcPlayPickupSound = 25;

    inline constexpr std::int32_t efcSetRuinsState = 26;

    inline constexpr std::int32_t efcSetGateSize = 27;

    inline constexpr std::int32_t efcPlayObjectSound = 28;

    inline constexpr std::int32_t efcSetGateEffectSize = 29;

    inline constexpr std::int32_t efcSetEffectImagePosition = 30;

    inline constexpr std::int32_t efcSetEffectDurationScale = 31;

    inline constexpr std::int32_t FilmNullObjectIndex = 65535;

} // namespace aEFilm
