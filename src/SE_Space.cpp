#include "layout/SE_Space.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_SpaceImg.hpp"
#include "types/GI_StarField.hpp"
#include "types/GI_StarFieldImg.hpp"
#include "types/GI_StarFieldM.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Star.hpp"
#include "types/SE_Weapon.hpp"
#include "types/Types.hpp"
#include "types/aEFilm.hpp"
#include "types/aEFilmEnd.hpp"
#include "types/fFilm.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Circle.hpp"
#include "units/GI_Frame.hpp"
#include "units/GI_GI.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SE_SoundRnd.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Space {
    // Overwrites Dest without releasing its previous reference.
    void RetainSpaceObject(pas::Var<TObjectSE*> Dest, TObjectSE* Source) {
        pas::store_unaligned<TObjectSE*>(Dest.address, Source);
        if (Source != nullptr) {
            ++Source->RefCount;
        }
    }

    // Clears Obj; frees its previous value when the decremented reference count is nonpositive.
    void ReleaseSpaceObject(pas::Var<TObjectSE*> Obj) {
        TObjectSE* Previous = pas::load_unaligned<TObjectSE*>(Obj.address);
        pas::store_unaligned<TObjectSE*>(Obj.address, nullptr);
        if (Previous != nullptr) {
            --Previous->RefCount;
            if (Previous->RefCount <= 0) {
                pas::free(Previous);
            }
        }
    }

    void TObjectSE_CreateEmpty(TObjectSE* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    // UnusedPosition is copied but does not initialize Position.
    void TObjectSE_Create(TObjectSE* Self, const pas::WideString& AGraphKey, WindowsSdk::TPoint UnusedPosition) {
        EC_Struct::TObjectEx_Create(Self);
        Self->GraphKey = AGraphKey;
        Self->LoadTemplate(GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.", EC_Str::ExtractDelimitedPartW(pas::view(AGraphKey), 0, u","sv)})));
    }

    void TObjectSE_Destroy(TObjectSE* Self) {
        TObjectSE* Obj{};
        Self->DetachFromSpace();
        {
            std::uint32_t cpp_left = WindowsImports::GetCurrentThreadId();
            if (cpp_left == GR_Main::MainRuntimeThreadId) {
                if (Globals::SpaceProcess != nullptr) {
                    if (Globals::SpaceProcess->Space != nullptr) {
                        Obj = Globals::SpaceProcess->Space->FirstObject;
                        while (Obj != nullptr) {
                            if (SE_Weapon::TWeaponSE* weaponSE = pas::class_cast_if<SE_Weapon::TWeaponSE*>(Obj)) {
                                if (Obj->IsAttachedToSpace()) {
                                    if (weaponSE->SourceObject == Self || weaponSE->TargetObject == Self) {
                                        Obj->DetachFromSpace();
                                    }
                                }
                            }
                            Obj = Obj->Next;
                        }
                    }
                }
            }
        }
        if (Globals::PrimaryFilm != nullptr) {
            Globals::PrimaryFilm->ReleaseObjectReferences(Self);
        }
        if (Globals::SecondaryFilm != nullptr) {
            Globals::SecondaryFilm->ReleaseObjectReferences(Self);
        }
        if (Globals::TrailingFilmEffects != nullptr) {
            Globals::TrailingFilmEffects->ReleaseObjectReferences(Self);
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Copies graph key, size, position and depth expression only.
    void TObjectSE::CopyTo(TObjectSE* Destination) {
        Destination->GraphKey = GraphKey;
        Destination->Size = Size;
        Destination->Position = Position;
        Destination->DepthExpression = DepthExpression;
    }

    void TObjectSE::AttachToSpace(TSpaceSE* ASpace) {
        ASpace->LinkObject(this);
        Space = ASpace;
        if (SoundLoopPath != u"") {
            LoopSound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
            LoopSound->Configure(SoundLoopPath, SoundGroup, true);
        }
    }

    void TObjectSE::DetachFromSpace() {
        if (LoopSound != nullptr) {
            pas::free(LoopSound);
            LoopSound = nullptr;
        }
        if (IsAttachedToSpace()) {
            Space->UnlinkObject(this);
            Space = nullptr;
        }
    }

    std::uint8_t TObjectSE::IsAttachedToSpace() {
        return !(Space == nullptr);
    }

    void TObjectSE::SetPosition(EC_Struct::TPointF APosition) {
        Position = APosition;
    }

    void TObjectSE::SetDepth(float Value) {
    }

    float TObjectSE::GetDepth() {
        return 0.0f;
    }

    void TObjectSE::SetOrbitCenter(EC_Struct::TPointF Center) {
    }

    // Subclasses interpret this point differently: Sputnik returns the orbit center, Ship2 returns scaled dimensions.
    EC_Struct::TPointF TObjectSE::GetOrbitCenter() {
        EC_Struct::TPointF Result{};
        Result = EC_Struct::MakePointF(0.0f, 0.0f);
        return Result;
    }

    std::uint8_t TObjectSE::GetAlpha() {
        return 0;
    }

    void TObjectSE::SetAlpha(std::uint8_t Value) {
    }

    // Base returns zero; TGateSE returns its stored angle.
    std::uint8_t TObjectSE::GetAngle() {
        return 0;
    }

    void TObjectSE::SetAngle(std::uint8_t Value) {
    }

    // Base returns empty; TGateSE overrides it with the label text.
    pas::WideString TObjectSE::GetText() {
        return pas::WideString();
    }

    void TObjectSE::SetText(const pas::WideString& Value) {
    }

    EC_Buf::TBufEC* TObjectSE::BuildStateBuffer() {
        return nullptr;
    }

    void TObjectSE::LoadStateBuffer(EC_Buf::TBufEC* Buffer) {
    }

    void TObjectSE::Advance() {
        float Distance{};
        std::uint32_t Now{};
        std::int32_t Delay{};
        if (IsAttachedToSpace()) {
            if (LoopSound != nullptr) {
                {
                    pas::Extended cpp_left = aMyFunction::PointDistance(Position, Globals::SpaceViewPosition);
                    Distance = pas::real_divide(cpp_left, pas::real_divide(static_cast<std::uint32_t>(GR_Main::GameScreenHeight), 2.0L));
                }
                if (Distance > 1.0L) {
                    LoopSound->SetVolume(0.0f);
                } else {
                    LoopSound->SetVolume((1.0L - Distance) * 0.5L + 0.5L);
                }
            }
            try {
                if (RandomSound != nullptr) {
                    if (RandomSoundGroup >= 0) {
                        Now = MMSystem::timeGetTime();
                        if (NextSoundTime < Now) {
                            Delay = aMyFunction::RandomIntRange(RandomSound->Groups[RandomSoundGroup].NextTimeMin, RandomSound->Groups[RandomSoundGroup].NextTimeMax);
                            NextSoundTime = Now + Delay;
                            if (Space->ContainsMapPoint(Position)) {
                                const pas::WideString& selectSound = RandomSound->SelectSound(RandomSoundGroup);
                                std::int32_t group = RandomSound->Groups[RandomSoundGroup].Group;
                                GR_Sound::TSoundControl* soundManager = GR_Main::SoundManager;
                                soundManager->PlayEffect(selectSound, group, 1.0f, 0.0f);
                            }
                        }
                    }
                }
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                } else {
                    throw;
                }
            }
        }
    }

    void TObjectSE::SetSize(WindowsSdk::TPoint Value) {
        Size = Value;
    }

    std::uint8_t TObjectSE::HitTestCursor() {
        return false;
    }

    void TObjectSE::DrawMap() {
    }

    void TObjectSE::ConfigureLoopSound(const pas::WideString& Name) {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.Sound.Loop.", Name}));
        std::int32_t Weight = 0;
        std::int32_t Count = Block->GetParamCount();
        if (Count >= 1) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                Weight += EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(Index)));
            }
            Weight = aMyFunction::RandomIntRange(0, Weight - 1);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
                Weight -= EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(Index)));
                if (Weight < 0) {
                    SoundLoopPath = Block->GetParamValue(Index);
                    SoundGroup = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(SoundLoopPath), 0, u","sv)));
                    SoundLoopPath = EC_Str::ExtractDelimitedPartW(pas::view(SoundLoopPath), 1, u","sv);
                    return;
                }
            }
        }
        SoundLoopPath = pas::WideString();
        SoundGroup = 0;
    }

    void TObjectSE::ConfigureRandomSound(const pas::WideString& Name) {
        RandomSound = SE_SoundRnd::FindRandomSound(Name, RandomSoundGroup);
        if (RandomSoundGroup >= 0) {
            NextSoundTime = ([&] {
                std::int32_t cpp_right = aMyFunction::RandomIntRange(RandomSound->Groups[RandomSoundGroup].NextTimeMin, RandomSound->Groups[RandomSoundGroup].NextTimeMax);
                return MMSystem::timeGetTime() + cpp_right;
            }());
        }
    }

    void TObjectSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"PosZ"_wref.get()) > 0) {
            DepthExpression = Block->GetParam(u"PosZ"sv);
        }
        if (Block->CountParams(u"SoundLoop"_wref.get()) > 0) {
            SoundLoopPath = Block->GetParam(u"SoundLoop"sv);
        }
        if (Block->CountParams(u"SoundGroup"_wref.get()) > 0) {
            SoundGroup = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"SoundGroup"sv)));
        }
    }

    void TObjectSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        if (Block->CountParams(u"Pos"_wref.get()) > 0) {
            Text = Block->GetParam(u"Pos"sv);
            SetPosition(([&] {
                float extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv));
                float extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv));
                return EC_Struct::MakePointF(extractDecimalToSingleW, extractDecimalToSingleW_2);
            }()));
        }
    }

    void TObjectSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    void TSpaceSE_Create(TSpaceSE* Self, GI_Panel::TPanelGI* AMapPanel, GI_MessageLoop::TMessageLoopGI* AScreen) {
        pas::object_create(Self);
        Self->MapPanel = AMapPanel;
        Self->Screen = AScreen;
        Self->MinimapScale = 0.125;
        Self->MinimapViewportFrame = nullptr;
        Self->MinimapRangeShade = pas::construct_call<GI_Circle::TCircleGI>(GI_Circle::TCircleGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        {
            GI_Circle::TCircleGI* cpp_with = Self->MinimapRangeShade;
            cpp_with->SetKind(GI_Circle::ckShrLight);
            cpp_with->SetPosition(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
            cpp_with->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
            cpp_with->SetSize(ClassesImports::Point(GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
            cpp_with->SetShrLightInner(0);
            cpp_with->SetShrLightOuter(1);
        }
        Self->MinimapRangeCircle = pas::construct_call<GI_Circle::TCircleGI>(GI_Circle::TCircleGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        {
            GI_Circle::TCircleGI* cpp_with_2 = Self->MinimapRangeCircle;
            cpp_with_2->SetKind(GI_Circle::ckCircle);
            cpp_with_2->SetPosition(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
            cpp_with_2->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
            cpp_with_2->SetSize(ClassesImports::Point(GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
        Self->MinimapBackground = pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        {
            GI_GI::TgiGI* cpp_with_3 = Self->MinimapBackground;
            cpp_with_3->SetImagePath(pas::concat_wide({u"Bm.PanelSpace2.", GR_Main::GiResourceSuffix(), u"RadarT"}));
            cpp_with_3->SetSize(ClassesImports::Point(GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
        Self->StarField = reinterpret_cast<GI_StarField::TStarFieldGI*>(Self->Screen->FindControlByPath(u"StarField"_wref.get()));
        Self->StarFieldM = reinterpret_cast<GI_StarFieldM::TStarFieldMGI*>(Self->Screen->FindControlByPath(u"StarFieldM"_wref.get()));
        Self->SpaceImages = reinterpret_cast<GI_SpaceImg::TSpaceImgGI*>(Self->Screen->FindControlByPath(u"SpaceImg"_wref.get()));
        Self->StarFieldImages = reinterpret_cast<GI_StarFieldImg::TStarFieldImgGI*>(Self->Screen->FindControlByPath(u"StarFieldImg"_wref.get()));
        Self->AlphaShift = 0;
    }

    // Requires all timers to have been removed.
    void TSpaceSE_Destroy(TSpaceSE* Self) {
        Self->ClearPath();
        pas::free(Self->MinimapRangeShade);
        Self->MinimapRangeShade = nullptr;
        pas::free(Self->MinimapRangeCircle);
        Self->MinimapRangeCircle = nullptr;
        pas::free(Self->MinimapBackground);
        Self->MinimapBackground = nullptr;
        Self->StarField = nullptr;
        Self->StarFieldM = nullptr;
        Self->SpaceImages = nullptr;
        Self->StarFieldImages = nullptr;
        while (Self->FirstObject != nullptr) {
            Self->LastObject->DetachFromSpace();
        }
        Self->MapPanel = nullptr;
        if (Self->FirstTimer != nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("destructor TSpaceSE.Destroy;"_a));
        }
        pas::object_destroy(Self);
    }

    // Only changes list links; does not retain Obj or set Obj.Space.
    void TSpaceSE::LinkObject(TObjectSE* Obj) {
        if (LastObject != nullptr) {
            LastObject->Next = Obj;
        }
        Obj->Prev = LastObject;
        Obj->Next = nullptr;
        LastObject = Obj;
        if (FirstObject == nullptr) {
            FirstObject = Obj;
        }
    }

    // Does not release Obj or clear its links.
    void TSpaceSE::UnlinkObject(TObjectSE* Obj) {
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
    }

    // Converts milliseconds to ticks by rounding division by 18. Callback receives Context, Timer, UserData in Delphi registers. Zero delay still waits for AdvanceTimers.
    PSpaceTimerSE TSpaceSE::CreateTimer(std::int32_t DelayMs, std::int32_t RepeatMs, TSpaceTimerEventSE Callback, std::int32_t UserData) {
        PSpaceTimerSE Timer = static_cast<PSpaceTimerSE>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TSpaceTimerSE))));
        if (LastTimer != nullptr) {
            LastTimer->Next = Timer;
        }
        Timer->Prev = LastTimer;
        Timer->Next = nullptr;
        LastTimer = Timer;
        if (FirstTimer == nullptr) {
            FirstTimer = Timer;
        }
        Timer->TicksRemaining = System::Round(pas::real_divide(DelayMs, 18.0L));
        Timer->RepeatTicks = System::Round(pas::real_divide(RepeatMs, 18.0L));
        Timer->Callback = Callback;
        Timer->UserData = UserData;
        return Timer;
    }

    // Raises if Timer is NextTimerToProcess.
    void TSpaceSE::DeleteTimer(PSpaceTimerSE Timer) {
        PSpaceTimerSE Entry = Timer;
        if (NextTimerToProcess == Entry) {
            pas::raise(pas::make_exception<pas::Exception>("procedure TSpaceSE.ST_Delete(id:DWORD);"_a));
        }
        if (Entry->Prev != nullptr) {
            Entry->Prev->Next = Entry->Next;
        }
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry->Prev;
        }
        if (LastTimer == Entry) {
            LastTimer = Entry->Prev;
        }
        if (FirstTimer == Entry) {
            FirstTimer = Entry->Next;
        }
        EC_Mem::FreeEC(Entry);
    }

    void TSpaceSE::AdvanceTimers() {
        PSpaceTimerSE Timer{};
        NextTimerToProcess = FirstTimer;
        while (NextTimerToProcess != nullptr) {
            Timer = NextTimerToProcess;
            NextTimerToProcess = NextTimerToProcess->Next;
            --Timer->TicksRemaining;
            if (Timer->TicksRemaining <= 0) {
                Timer->TicksRemaining = Timer->RepeatTicks;
                Timer->Callback(Timer, Timer->UserData);
            }
        }
        NextTimerToProcess = nullptr;
    }

    void TSpaceSE::AdvanceObjects() {
        TObjectSE* Obj = FirstObject;
        while (Obj != nullptr) {
            Obj->Advance();
            Obj = Obj->Next;
        }
    }

    void TSpaceSE::ClearPath() {
        if (PathPoints != nullptr) {
            EC_Mem::FreeEC(PathPoints);
            PathPoints = nullptr;
        }
        PathPointCount = 0;
    }

    // Copies Count points. A nonpositive count clears the path.
    void TSpaceSE::SetPath(EC_Struct::PPointF Points, std::int32_t Count) {
        ClearPath();
        if (Count < 1) {
            return;
        }
        PathPointCount = Count;
        PathPoints = static_cast<EC_Struct::PPointF>(EC_Mem::AllocEC(Count * static_cast<std::int32_t>(sizeof(EC_Struct::TPointF))));
        Windows::CopyMemory(PathPoints, Points, Count * static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
    }

    void TSpaceSE::DrawMinimap() {
        WindowsSdk::PSingle Coordinate{};
        std::int32_t Index{};
        std::int32_t X1{};
        std::int32_t Y1{};
        std::int32_t X2{};
        std::int32_t Y2{};
        std::int32_t CenterX{};
        std::int32_t CenterY{};
        std::uint32_t Color{};
        WindowsSdk::TRect Clip{};
        GR_GraphBuf::TGraphBufGR* SavedBuffer = GR_Main::ScreenRenderBuffer;
        GR_Main::ScreenRenderBuffer = GR_Main::RenderScratchBuffer;
        std::uint8_t SavedHardware = GlobalsV::HardwareRenderingEnabled;
        GlobalsV::HardwareRenderingEnabled = false;
        SE_Process::TProcessSE* CurrentProcess = pas::checked_cast<SE_Process::TProcessSE*>(Process);
        Clip = ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height);
        MinimapBackground->HitTestBounds = Clip;
        MinimapBackground->Draw(Clip);
        TObjectSE* Obj = FirstObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr) {
                Obj->DrawMap();
            }
            Obj = Obj->Next;
        }
        Obj = FirstObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr) {
                Obj->DrawMap();
            }
            Obj = Obj->Next;
        }
        Obj = FirstObject;
        while (Obj != nullptr) {
            if (!(pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr)) {
                if (!(pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr)) {
                    Obj->DrawMap();
                }
            }
            Obj = Obj->Next;
        }
        if (PathPoints != nullptr) {
            CenterX = pas::shr(GR_Main::RenderScratchBuffer->Width, 1);
            CenterY = pas::shr(GR_Main::RenderScratchBuffer->Height, 1);
            Coordinate = static_cast<WindowsSdk::PSingle>(static_cast<void*>(PathPoints));
            X1 = CenterX + System::Round(static_cast<long double>(pas::load_unaligned<float>(Coordinate)) * MinimapScale);
            Coordinate = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Coordinate) + static_cast<std::int32_t>(sizeof(float))));
            Y1 = CenterY + System::Round(static_cast<long double>(pas::load_unaligned<float>(Coordinate)) * MinimapScale);
            Coordinate = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Coordinate) + static_cast<std::int32_t>(sizeof(float))));
            for (auto cpp_range = pas::for_to<std::int32_t>(1, PathPointCount - 1); cpp_range.next(Index); ) {
                X2 = CenterX + System::Round(static_cast<long double>(pas::load_unaligned<float>(Coordinate)) * MinimapScale);
                Coordinate = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Coordinate) + static_cast<std::int32_t>(sizeof(float))));
                Y2 = CenterY + System::Round(static_cast<long double>(pas::load_unaligned<float>(Coordinate)) * MinimapScale);
                Coordinate = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Coordinate) + static_cast<std::int32_t>(sizeof(float))));
                if (!((Index / 199 & 1) != 0)) {
                    Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0x000000c3, 0x00000031, 0);
                } else {
                    Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0x0000008f, 0x000000c1, 0);
                }
                {
                    const WindowsSdk::TRect& clip = Clip;
                    void* pixels = GR_Main::RenderScratchBuffer->GetPixels();
                    std::int32_t pitchBytes = GR_Main::RenderScratchBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Line_DrawClip_WORD(pixels, pitchBytes, X1, Y1, X2, Y2, Color, clip);
                }
                X1 = X2;
                Y1 = Y2;
            }
        }
        if (CurrentProcess->RadarRange > 0) {
            {
                std::int32_t cpp_arg = pas::shr(GR_Main::RenderScratchBuffer->Height, 1) + System::Round(static_cast<long double>(CurrentProcess->RadarCenter.Y) * MinimapScale);
                std::int32_t cpp_arg_2 = pas::shr(GR_Main::RenderScratchBuffer->Width, 1) + System::Round(static_cast<long double>(CurrentProcess->RadarCenter.X) * MinimapScale);
                MinimapRangeShade->SetCenter(ClassesImports::Point(cpp_arg_2, cpp_arg));
            }
            MinimapRangeShade->SetRadius(System::Round(static_cast<long double>(CurrentProcess->ActionRange) * MinimapScale));
            MinimapRangeShade->HitTestBounds = ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height);
            MinimapRangeShade->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
            {
                std::int32_t cpp_arg_3 = pas::shr(GR_Main::RenderScratchBuffer->Height, 1) + System::Round(static_cast<long double>(CurrentProcess->RadarCenter.Y) * MinimapScale);
                std::int32_t cpp_arg_4 = pas::shr(GR_Main::RenderScratchBuffer->Width, 1) + System::Round(static_cast<long double>(CurrentProcess->RadarCenter.X) * MinimapScale);
                MinimapRangeCircle->SetCenter(ClassesImports::Point(cpp_arg_4, cpp_arg_3));
            }
            MinimapRangeCircle->SetRadius(System::Round(static_cast<long double>(CurrentProcess->ActionRange) * MinimapScale));
            MinimapRangeCircle->SetColor(CurrentProcess->ActionColor);
            MinimapRangeCircle->HitTestBounds = ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height);
            MinimapRangeCircle->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
        MinimapViewportFrame->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        GlobalsV::HardwareRenderingEnabled = SavedHardware;
        GR_Main::ScreenRenderBuffer = SavedBuffer;
    }

    void TSpaceSE::CreateMinimapViewport() {
        FreeMinimapViewport();
        MinimapViewportFrame = pas::construct_call<GI_Frame::TFrameGI>(GI_Frame::TFrameGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        MinimapViewportFrame->SetDepth(-99999.0);
        MinimapViewportFrame->SetKind(GI_Frame::fkRect);
        MinimapViewportFrame->SetColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
        std::int32_t Width = System::Round(static_cast<long double>(MapPanel->ClientSize.X) * MinimapScale);
        std::int32_t Height = System::Round(static_cast<long double>(MapPanel->ClientSize.Y) * MinimapScale);
        MinimapViewportFrame->SetSize(ClassesImports::Point(Width, Height));
        MinimapViewportFrame->SetOrigin(ClassesImports::Point(Width / 2, Height / 2));
        MapPanel->ScrollChangedCallback = pas::bind_method<&TSpaceSE::MapScrollChanged>(this);
        MapScrollChanged(nullptr);
    }

    void TSpaceSE::FreeMinimapViewport() {
        if (MinimapViewportFrame != nullptr) {
            pas::free(MinimapViewportFrame);
            MinimapViewportFrame = nullptr;
        }
    }

    void TSpaceSE::MapScrollChanged(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender == MapPanel) {
            Globals::FilmCameraFollow = false;
        }
        if (MinimapViewportFrame != nullptr) {
            std::int32_t round = System::Round(static_cast<long double>(MapPanel->ScrollOffset.Y) * MinimapScale);
            std::int32_t round_2 = System::Round(static_cast<long double>(MapPanel->ScrollOffset.X) * MinimapScale);
            MinimapViewportFrame->SetPosition(ClassesImports::Point(round_2, round));
        }
        if (StarField != nullptr) {
            StarField->SetViewPosition(EC_Struct::PointToPointF(MapPanel->ScrollOffset));
        }
        if (GlobalsV::Wind >= 1) {
            if (StarFieldM != nullptr) {
                StarFieldM->SetViewPosition(EC_Struct::PointToPointF(MapPanel->ScrollOffset));
            }
        }
        if (SpaceImages != nullptr) {
            SpaceImages->SetViewPosition(EC_Struct::PointToPointF(MapPanel->ScrollOffset));
        }
        if (GlobalsV::Wind >= 2) {
            if (StarFieldImages != nullptr) {
                StarFieldImages->SetViewPosition(EC_Struct::PointToPointF(MapPanel->ScrollOffset));
            }
        }
        if (pas::assigned(ScrollChangedCallback)) {
            ScrollChangedCallback();
        }
        pas::checked_cast<SE_Process::TProcessSE*>(Process)->UpdateViewRect();
        Globals::SpaceViewPosition = EC_Struct::PointToPointF(MapPanel->ScrollOffset);
    }

    void TSpaceSE::MinimapMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        WindowsSdk::TPoint Local{};
        GI_MessageLoop::TObjectGI* Child{};
        MinimapDragging = false;
        if (Sender->IsOccludedAtPoint(Point)) {
            return;
        }
        if (Screen == Globals::StarMapScreen) {
            if (Globals::StarMapScreen->CenterShipButton->HitTest(Point)) {
                return;
            }
            Child = Globals::StarMapScreen->SecondaryPartnerPanel->FirstChild;
            while (Child != nullptr) {
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Child)->HitTest(Point)) {
                    return;
                }
                Child = Child->NextSibling;
            }
        }
        if (Screen == Globals::FilmScreen) {
            if (Globals::FilmScreen->CenterShipButton->HitTest(Point)) {
                return;
            }
        }
        MinimapDragging = true;
        Globals::FilmCameraFollow = false;
        Local = Sender->ToLocalPoint(Point);
        {
            std::int32_t round = System::Round(pas::real_divide(Local.Y, MinimapScale));
            std::int32_t round_2 = System::Round(pas::real_divide(Local.X, MinimapScale));
            MapPanel->SetScrollOffset(ClassesImports::Point(round_2, round));
        }
        MapScrollChanged(nullptr);
        Globals::MinimapFrameCounter = 0;
    }

    void TSpaceSE::MinimapMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        MinimapDragging = false;
    }

    void TSpaceSE::MinimapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        WindowsSdk::TPoint Local{};
        if (MinimapDragging) {
            if (static_cast<std::int32_t>(KeyState & WindowsSdk::MK_LBUTTON) == WindowsSdk::MK_LBUTTON || (KeyState & WindowsSdk::MK_RBUTTON) == WindowsSdk::MK_RBUTTON) {
                if (Sender->IsOccludedAtPoint(Point)) {
                    return;
                }
                if (Screen == Globals::StarMapScreen) {
                    if (Globals::StarMapScreen->CenterShipButton->HitTest(Point)) {
                        return;
                    }
                }
                if (Screen == Globals::FilmScreen) {
                    if (Globals::FilmScreen->CenterShipButton->HitTest(Point)) {
                        return;
                    }
                }
                Local = Sender->ToLocalPoint(Point);
                {
                    std::int32_t round = System::Round(pas::real_divide(Local.Y, MinimapScale));
                    std::int32_t round_2 = System::Round(pas::real_divide(Local.X, MinimapScale));
                    MapPanel->SetScrollOffset(ClassesImports::Point(round_2, round));
                }
                MapScrollChanged(nullptr);
                Globals::MinimapFrameCounter = 0;
            }
        }
    }

    std::uint8_t TSpaceSE::ContainsMapPoint(EC_Struct::TPointF Point) {
        if (MapPanel == nullptr) {
            return false;
        }
        return MapPanel->ContainsPoint(MapPanel->ToAbsolutePoint(EC_Struct::TruncatePointF(Point)));
    }

    void TSpaceSE::p_destroy() {
        SE_Space::TSpaceSE_Destroy(this);
    }

    void TObjectSE::p_destroy() {
        SE_Space::TObjectSE_Destroy(this);
    }

} // namespace SE_Space
