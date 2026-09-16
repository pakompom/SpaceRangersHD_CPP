#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace GI_MessageLoop {
    struct TCallbackTimerGI;

    struct TCursorStateGI;

} // namespace GI_MessageLoop

namespace GR_Rect {
    struct TArrayRectGR;

} // namespace GR_Rect

namespace GI_MessageLoop {
    struct TObjectGI;

    struct TFormSoundGroup;

    struct TMessageLoopGI;

    struct TSavedLineGI;

    using TObjectHelpEventGI = pas::Method<void(TObjectGI*, std::uint8_t)>;

    using TObjectMouseEventGI = pas::Method<void(TObjectGI*, std::uint32_t, WindowsSdk::TPoint)>;

    using TObjectNotifyEventGI = pas::Method<void(TObjectGI*)>;

    using TObjectKeyEventGI = pas::Method<void(TObjectGI*, std::uint32_t)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TObjectGI : EC_Struct::TObjectEx {
        PAS_CLASS_META(TObjectGI, EC_Struct::TObjectEx, "TObjectGI", 288)
        void p_destroy() override;
        void FreeOwnedChildren();
        virtual void Clear();
        void AttachOwnedChild(TObjectGI* Child);
        void InsertOwnedChildBefore(TObjectGI* BeforeChild, TObjectGI* Child);
        void InsertOwnedChildByDepth(TObjectGI* Child, double NewDepth);
        void FreeOwnedChild(TObjectGI* Child);
        void UnlinkOwnedChild(TObjectGI* Child);
        void Reparent(TObjectGI* NewParent);
        void SetMouseViewUpdates(std::uint8_t Enabled);
        void UpdateAbsolutePosition();
        virtual WindowsSdk::TPoint GetChildAbsolutePosition(WindowsSdk::TPoint LocalPosition, std::uint8_t ModeW);
        virtual void UpdateHitTestBounds();
        void UpdateSubtreeHitBounds();
        void SetHelpCallbackRecursive(TObjectHelpEventGI Callback);
        WindowsSdk::TRect OffsetChildRect(WindowsSdk::TRect Rect, std::uint8_t ModeW);
        virtual void SetPosition(WindowsSdk::TPoint Position);
        virtual void SetDepth(double NewDepth);
        virtual void SetDepthByName(const pas::WideString& Name);
        virtual void SetSize(WindowsSdk::TPoint Size);
        virtual void SetOrigin(WindowsSdk::TPoint Origin);
        void SetPositionModeW(std::uint8_t Enabled);
        virtual void SetConfigPath(const pas::WideString& Path);
        virtual WindowsSdk::TRect GetLocalBounds();
        void SetName(const pas::WideString& Name);
        virtual void SetActive(std::uint8_t Enabled);
        virtual void SetHitTestDisabled(std::uint8_t Disabled);
        TObjectGI* FindDeepestChildAtPoint(WindowsSdk::TPoint Point);
        std::uint8_t IsOccludedAtPoint(WindowsSdk::TPoint Point);
        virtual void QueueImageLoad(pas::List* PendingLoads);
        virtual void ProcessMouseMove(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void OnMouseEnter();
        virtual void OnMouseLeave();
        virtual void OnActivate();
        virtual void NativeHook48();
        virtual void OnDeactivate();
        virtual void NativeHook50();
        virtual void ProcessLeftButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void ProcessLeftButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void ProcessRightButtonDown(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void ProcessRightButtonUp(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void ProcessLeftButtonDoubleClick(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void ProcessRightButtonDoubleClick(std::uint32_t KeyState, WindowsSdk::TPoint Point);
        virtual void BroadcastKeyDown(std::uint32_t Key);
        virtual void BroadcastKeyUp(std::uint32_t Key);
        virtual void OnHoverGained();
        virtual void OnHoverLost();
        virtual void OnFocusGained();
        virtual void OnFocusLost();
        virtual void ProcessKeyDown(std::int32_t Key);
        virtual void ProcessCharacter(char16_t Character);
        virtual void OnCaretBlink();
        std::uint8_t ContainsPoint(WindowsSdk::TPoint Point);
        std::uint8_t HitTestCursor();
        TObjectGI* FindByNameRecursive(const pas::WideString& Name);
        virtual WindowsSdk::TPoint ToLocalPoint(WindowsSdk::TPoint Point);
        virtual WindowsSdk::TPoint ToAbsolutePoint(WindowsSdk::TPoint Point);
        void DispatchNamedEvent(std::int32_t EventKind, std::int32_t Param1, std::int32_t Param2);
        virtual void InvalidateRect(WindowsSdk::TRect Rect);
        virtual void Invalidate();
        void InvalidateChildren(std::uint8_t IncludePanels);
        TObjectGI* InvalidateScrollOverlap(WindowsSdk::TRect Rect, WindowsSdk::TPoint Delta, TObjectGI* StartControl);
        virtual void Draw(WindowsSdk::TRect ClipRect);
        virtual void DrawUpdateRects(WindowsSdk::TRect ClipRect);
        virtual void CommitFrameDraw();
        virtual void ErasePreviousFrame();
        virtual void PrepareFrameDraw();
        virtual void PrepareRegionDraw(WindowsSdk::TRect ClipRect);
        virtual void LoadFromConfigPath(const pas::WideString& Path);
        virtual void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        void ReloadFromBlock();
        virtual void UpdateAutoGeometry();
        TObjectGI* FirstChild;
        TObjectGI* LastChild;
        TObjectGI* PrevSibling;
        TObjectGI* NextSibling;
        TObjectGI* Parent;
        TMessageLoopGI* MessageLoop;
        EC_BlockPar::TBlockParEC* SourceBlock;
        WindowsSdk::TPoint LocalPosition;
        WindowsSdk::TPoint ClientSize;
        WindowsSdk::TPoint OriginPoint;
        double Depth;
        std::uint8_t PositionModeW;
        std::uint8_t Active;
        std::uint8_t HitTestDisabled;
        std::uint8_t cpp_padding[1];
        pas::WideString ConfigPath;
        std::uint8_t AutoOffsetEnabled;
        std::uint8_t cpp_padding_2[3];
        EC_Struct::TPointF AutoOffsetScale;
        WindowsSdk::TPoint ScrollOffset;
        std::int32_t SkipOwnQueuedDraw;
        WindowsSdk::TRect HitTestBounds;
        WindowsSdk::TPoint AbsolutePosition;
        pas::WideString ControlName;
        pas::WideString HelpText;
        TObjectHelpEventGI HelpCallback;
        std::uint8_t MouseInside;
        std::uint8_t MouseBlocking;
        std::uint8_t MouseBlockingTest;
        std::uint8_t ScrollUpdate;
        std::int32_t UserValue;
        std::int32_t UserIndex;
        std::int32_t UserData;
        std::int32_t UserState;
        std::uint8_t cpp_padding_3[4];
        TObjectMouseEventGI MouseMoveCallback;
        TObjectMouseEventGI LeftButtonDownCallback;
        TObjectMouseEventGI LeftButtonUpCallback;
        TObjectMouseEventGI RightButtonDownCallback;
        TObjectMouseEventGI RightButtonUpCallback;
        TObjectMouseEventGI LeftButtonDoubleClickCallback;
        TObjectMouseEventGI RightButtonDoubleClickCallback;
        TObjectNotifyEventGI MouseEnterCallback;
        TObjectNotifyEventGI MouseLeaveCallback;
        TObjectNotifyEventGI ActivateCallback;
        TObjectNotifyEventGI DeactivateCallback;
        TObjectNotifyEventGI DestroyNotify;
        TObjectKeyEventGI KeyDownCallback;
        TObjectKeyEventGI KeyUpCallback;
        EC_BlockPar::TBlockParEC* OnKeyDownCode;
        EC_BlockPar::TBlockParEC* OnMouseEnterCode;
        EC_BlockPar::TBlockParEC* OnMouseLeaveCode;
        EC_BlockPar::TBlockParEC* OnRightButtonDownCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PCallbackTimerGI = TCallbackTimerGI*;

    #pragma pack(push, 1)
    struct TSavedLineGI {
        WindowsSdk::TPoint First;
        WindowsSdk::TPoint Last;
        void* Pixels;
        std::uint32_t Heap;
    };
    #pragma pack(pop)

    using TCallbackTimerEventGI = pas::Method<void(PCallbackTimerGI, std::int32_t)>;

    using PCursorStateGI = TCursorStateGI*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMessageLoopGI : EC_Struct::TObjectEx {
        PAS_CLASS_META(TMessageLoopGI, EC_Struct::TObjectEx, "TMessageLoopGI", 208)
        void p_destroy() override;
        void ResetRuntime();
        void QueueUpdateRect(WindowsSdk::TRect Rect);
        void InvalidateViewport();
        std::int32_t FindMouseViewUpdateControl(TObjectGI* Control);
        void AddMouseViewUpdateControl(TObjectGI* Control);
        void RemoveMouseViewUpdateControl(TObjectGI* Control);
        void InvalidateMouseViewControls();
        void DrawQueuedUpdateRects();
        void DrawQueuedControlRects();
        void FinishQueuedDraw();
        void CommitFrameDraw();
        void ErasePreviousFrame();
        void PrepareFrameDraw();
        virtual std::int32_t Run();
        virtual std::int32_t RunContinuous();
        virtual void AdvanceTimerTick();
        virtual void DrawFrame();
        virtual void Present();
        virtual void ProcessWindowMessage(std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam);
        virtual pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)> bind_ProcessWindowMessage();
        void CaptureScreenshot();
        void RequestClose(std::int32_t ResultCode);
        void SetHelpCallback(TObjectHelpEventGI Callback);
        TObjectGI* GetByName(const pas::WideString& Name);
        TObjectGI* FindControlByPath(const pas::WideString& Path);
        void SetFocusedControl(TObjectGI* Control);
        void SetHoveredControl(TObjectGI* Control);
        virtual void ProcessNamedControlEvent(pas::WideString ControlName, std::int32_t EventKind, std::int32_t Param1, std::int32_t Param2);
        virtual void OnOpen();
        virtual void OnClose();
        virtual void ProcessCallbackTimers();
        virtual void SelectMusic();
        PCallbackTimerGI ScheduleCallbackTimer(std::int32_t DelayMs, std::int32_t RepeatMs, TCallbackTimerEventGI Callback, std::int32_t UserData);
        void CancelCallbackTimer(PCallbackTimerGI Timer);
        void UpdateCallbackTimer(PCallbackTimerGI Timer, std::int32_t DelayMs, std::int32_t RepeatMs);
        void ReinsertCallbackTimer(PCallbackTimerGI Timer);
        void RefreshTimerTick();
        void SetCursorImage(const pas::WideString& ImagePath, WindowsSdk::TPoint HotSpot);
        void SetCursorByName(const pas::WideString& Name);
        std::uint8_t IsCursorImageSelected(const pas::WideString& RegisteredName);
        std::uint8_t IsCursorActive();
        void SetCursorActive(std::uint8_t Enabled);
        void CaptureCursorState(PCursorStateGI State);
        void RestoreCursorState(PCursorStateGI State);
        void UpdateCursorPosition();
        WindowsSdk::TPoint GetCursorPoint();
        static void SetSystemCursorPosition(WindowsSdk::TPoint Point);
        std::uint8_t ConsumeTimerTickChange();
        std::int32_t QueryPointOcclusionState(WindowsSdk::TPoint Point, TObjectGI* IgnoreControl, TObjectGI* StartControl);
        void FreeSavedPixels16();
        void FreeSecondaryPixelBuffer();
        void ResetSecondaryPixelCount();
        void FreeSavedLines();
        void AddSavedLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, void* Pixels);
        void RestoreSavedLines();
        void ResetSavedLineCount();
        virtual void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta);
        void ClearTransientControl();
        void InvalidateTransientControl();
        void InitializeDefaults();
        void InitializeFromConfig(EC_BlockPar::TBlockParEC* ConfigRoot, const pas::WideString& ScreenName, std::uint8_t UnusedFlag);
        virtual void InitializeLayout();
        virtual void UpdateActionCursor(std::uint8_t CanTake);
        virtual TMessageLoopGI* GetActionParentLoop();
        void QueueUiCode(EC_BlockPar::TBlockParEC* Block, std::uint8_t RefreshMouse);
        void RefreshMouseDispatch();
        virtual void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key);
        pas::WideString RegisteredLoopName;
        TMessageLoopGI* ParentLoop;
        TMessageLoopGI* ChildLoop;
        TObjectGI* DebugControl;
        TObjectGI* StatusLabel;
        TObjectGI* RootUiObject;
        TObjectGI* ContentPanel;
        TObjectGI* BackgroundPanel;
        TObjectGI* OverlayPanel;
        TObjectGI* CursorControl;
        TObjectGI* FocusedControl;
        TObjectGI* HoveredControl;
        TObjectGI* HelpLabel;
        TObjectGI* RegionDrawControl;
        pas::List* MouseViewUpdateControls;
        std::uint8_t RegionDrawPending;
        std::uint8_t cpp_padding[3];
        pas::WideString CursorImagePath;
        WindowsSdk::TRect ViewportRect;
        std::uint8_t UpdateRectsEnabled;
        std::uint8_t cpp_padding_2[3];
        GR_Rect::TArrayRectGR* UpdateRects;
        std::int32_t ExitCode;
        std::uint8_t CaretBlinkOn;
        std::uint8_t cpp_padding_3[3];
        std::uint32_t TimerTick;
        PCallbackTimerGI FirstTimer;
        PCallbackTimerGI LastTimer;
        PCallbackTimerGI NextTimerToProcess;
        std::uint32_t LastObservedTimerTick;
        void* SavedPixels16;
        std::int32_t SavedPixelCount16;
        std::int32_t SavedPixelCapacity16;
        void* SecondaryPixelBuffer;
        std::int32_t SecondaryPixelCount;
        std::int32_t SecondaryPixelCapacity;
        pas::DynArray<TSavedLineGI> SavedLines;
        std::int32_t SavedLineCount;
        std::uint8_t PendingRedraw;
        std::uint8_t ContinuousLoop;
        std::uint8_t cpp_padding_4[2];
        std::int32_t FramesPerSecond;
        std::uint8_t PlayTransitionSounds;
        std::uint8_t cpp_padding_5[3];
        pas::WideString OpenSoundName;
        pas::WideString CloseSoundName;
        std::int32_t SoundSection;
        pas::List* SoundGroupList;
        TObjectGI* TransientControl;
        pas::Object* TransientData;
        std::uint8_t IsOpen;
        std::uint8_t cpp_padding_6[3];
        TObjectGI* SavedBackgroundControl;
        pas::List* DeferredCodeBlocks;
        std::uint8_t RefreshMouseAfterCode;
        std::uint8_t cpp_padding_7[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCallbackTimerGI {
        TCallbackTimerEventGI Callback;
        std::int32_t UserData;
        std::int32_t RepeatMs;
        std::uint32_t DueTick;
        PCallbackTimerGI Prev;
        PCallbackTimerGI Next;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    using TDialogChoiceEventGI = pas::Method<void(std::int32_t)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TFormSoundGroup : EC_Struct::TObjectEx {
        PAS_CLASS_META(TFormSoundGroup, EC_Struct::TObjectEx, "TFormSoundGroup", 28)
        void p_destroy() override;
        void Clear();
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        void ScheduleNextPlayback();
        void PlayIfDue();
        std::int32_t Section;
        std::int32_t MinDelayMs;
        std::int32_t MaxDelayMs;
        std::uint32_t NextPlayTick;
        std::int32_t TotalWeight;
        EC_Str::TStringsEC* Sounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCursorStateGI {
        pas::WideString ImagePath;
        std::uint8_t Active;
        WindowsSdk::TPoint HotSpot;
        WindowsSdk::TPoint Position;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

} // namespace GI_MessageLoop
