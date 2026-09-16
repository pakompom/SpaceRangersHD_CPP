#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GI_Circle {
    struct TCircleGI;

} // namespace GI_Circle

namespace GI_Frame {
    struct TFrameGI;

} // namespace GI_Frame

namespace GI_GI {
    struct TgiGI;

} // namespace GI_GI

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace GI_SpaceImg {
    struct TSpaceImgGI;

} // namespace GI_SpaceImg

namespace GI_StarField {
    struct TStarFieldGI;

} // namespace GI_StarField

namespace GI_StarFieldImg {
    struct TStarFieldImgGI;

} // namespace GI_StarFieldImg

namespace GI_StarFieldM {
    struct TStarFieldMGI;

} // namespace GI_StarFieldM

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace SE_SoundRnd {
    struct TSoundRndSE;

} // namespace SE_SoundRnd

namespace SE_Space {
    struct TSpaceTimerSE;

    struct TSpaceSE;

    struct TObjectSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TObjectSE : EC_Struct::TObjectEx {
        PAS_CLASS_META(TObjectSE, EC_Struct::TObjectEx, "TObjectSE", 76)
        void p_destroy() override;
        virtual void CopyTo(TObjectSE* Destination);
        virtual void AttachToSpace(TSpaceSE* ASpace);
        virtual void DetachFromSpace();
        std::uint8_t IsAttachedToSpace();
        virtual void SetPosition(EC_Struct::TPointF APosition);
        virtual void SetDepth(float Value);
        virtual float GetDepth();
        virtual void SetOrbitCenter(EC_Struct::TPointF Center);
        virtual EC_Struct::TPointF GetOrbitCenter();
        virtual std::uint8_t GetAlpha();
        virtual void SetAlpha(std::uint8_t Value);
        virtual std::uint8_t GetAngle();
        virtual void SetAngle(std::uint8_t Value);
        virtual pas::WideString GetText();
        virtual void SetText(const pas::WideString& Value);
        virtual EC_Buf::TBufEC* BuildStateBuffer();
        virtual void LoadStateBuffer(EC_Buf::TBufEC* Buffer);
        virtual void Advance();
        virtual void SetSize(WindowsSdk::TPoint Value);
        virtual std::uint8_t HitTestCursor();
        virtual void DrawMap();
        void ConfigureLoopSound(const pas::WideString& Name);
        void ConfigureRandomSound(const pas::WideString& Name);
        virtual void LoadTemplate(EC_BlockPar::TBlockParEC* Block);
        virtual void ApplyConfig(EC_BlockPar::TBlockParEC* Block);
        virtual void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        TObjectSE* Prev;
        TObjectSE* Next;
        TObjectSE* ProcessPrev;
        TObjectSE* ProcessNext;
        TSpaceSE* Space;
        pas::WideString GraphKey;
        WindowsSdk::TPoint Size;
        EC_Struct::TPointF Position;
        pas::WideString DepthExpression;
        pas::WideString SoundLoopPath;
        std::int32_t SoundGroup;
        GR_Sound::TSoundBufferControl* LoopSound;
        SE_SoundRnd::TSoundRndSE* RandomSound;
        std::int32_t RandomSoundGroup;
        std::uint32_t NextSoundTime;
        std::int32_t RefCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PSpaceTimerSE = TSpaceTimerSE*;

    using TSpaceScrollEventSE = pas::Method<void()>;

    using TSpaceTimerEventSE = pas::Method<void(PSpaceTimerSE, std::int32_t)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSpaceSE : pas::Object {
        PAS_CLASS_META(TSpaceSE, pas::Object, "TSpaceSE", 108)
        void p_destroy() override;
        void LinkObject(TObjectSE* Obj);
        void UnlinkObject(TObjectSE* Obj);
        PSpaceTimerSE CreateTimer(std::int32_t DelayMs, std::int32_t RepeatMs, TSpaceTimerEventSE Callback, std::int32_t UserData);
        void DeleteTimer(PSpaceTimerSE Timer);
        void AdvanceTimers();
        void AdvanceObjects();
        void ClearPath();
        void SetPath(EC_Struct::PPointF Points, std::int32_t Count);
        void DrawMinimap();
        void CreateMinimapViewport();
        void FreeMinimapViewport();
        void MapScrollChanged(GI_MessageLoop::TObjectGI* Sender);
        void MinimapMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MinimapMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void MinimapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        std::uint8_t ContainsMapPoint(EC_Struct::TPointF Point);
        TObjectSE* FirstObject;
        TObjectSE* LastObject;
        PSpaceTimerSE FirstTimer;
        PSpaceTimerSE LastTimer;
        PSpaceTimerSE NextTimerToProcess;
        double MinimapScale;
        GI_Panel::TPanelGI* MapPanel;
        GI_MessageLoop::TObjectGI* MinimapControl;
        GI_Frame::TFrameGI* MinimapViewportFrame;
        GI_MessageLoop::TMessageLoopGI* Screen;
        GI_GI::TgiGI* MinimapBackground;
        GI_Circle::TCircleGI* MinimapRangeShade;
        GI_Circle::TCircleGI* MinimapRangeCircle;
        GI_StarField::TStarFieldGI* StarField;
        GI_StarFieldM::TStarFieldMGI* StarFieldM;
        GI_SpaceImg::TSpaceImgGI* SpaceImages;
        GI_StarFieldImg::TStarFieldImgGI* StarFieldImages;
        std::uint8_t MinimapDragging;
        std::uint8_t cpp_padding[3];
        pas::Object* Process;
        std::uint8_t cpp_padding_2[4];
        TSpaceScrollEventSE ScrollChangedCallback;
        EC_Struct::PPointF PathPoints;
        std::int32_t PathPointCount;
        std::int32_t AlphaShift;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TSpaceTimerSE {
        PSpaceTimerSE Prev;
        PSpaceTimerSE Next;
        std::int32_t TicksRemaining;
        std::int32_t RepeatTicks;
        TSpaceTimerEventSE Callback;
        std::int32_t UserData;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

} // namespace SE_Space
