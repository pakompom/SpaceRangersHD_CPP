#include "layout/GI_PolyLine.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Circle.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PolyLine.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Windows.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"

namespace GI_PolyLine {
    void TPolyLineGI_Create(TPolyLineGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->SegmentHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
        if (Self->SegmentHeap == 0) {
            pas::raise(pas::make_exception<pas::Exception>("TPolyLineGI.HeapCreate"_a));
        }
        Self->ClientSize = ClassesImports::Point(1, 1);
        Self->AnimationTimer = nullptr;
        Self->AnimationPhase = 0u;
        Self->FrameDrawing = false;
        Self->ShadowCircle = nullptr;
        Self->AutoRebuildBounds = true;
        Self->NormalizeBounds = true;
        Self->StartAnimation();
    }

    void TPolyLineGI_Destroy(TPolyLineGI* Self) {
        Self->ShadowCircle = nullptr;
        Self->StopAnimation();
        Self->Clear();
        if (Self->SegmentHeap != 0) {
            WindowsSdk::HeapDestroy(Self->SegmentHeap);
            Self->SegmentHeap = 0u;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPolyLineGI::Clear() {
        ShadowCircle = nullptr;
        ClientSize = ClassesImports::Point(1, 1);
        ClearSegments();
        GI_MessageLoop::TObjectGI::Clear();
    }

    PPolyLineSegmentGI TPolyLineGI::AllocateSegment() {
        PPolyLineSegmentGI Segment = static_cast<PPolyLineSegmentGI>(EC_Mem::AllocFromHeapEC(SegmentHeap, static_cast<std::int32_t>(sizeof(TPolyLineSegmentGI))));
        Segment->Next = nullptr;
        Segment->Prev = LastSegment;
        Segment->SavedPixels = nullptr;
        Segment->PreviousPixels = nullptr;
        Segment->Visible = false;
        Segment->PreviouslyVisible = false;
        if (LastSegment != nullptr) {
            LastSegment->Next = Segment;
        }
        if (FirstSegment == nullptr) {
            FirstSegment = Segment;
        }
        LastSegment = Segment;
        Segment->Kind = 0;
        return Segment;
    }

    void TPolyLineGI::ClearSegments() {
        while (FirstSegment != nullptr) {
            RemoveSegment(FirstSegment);
        }
    }

    void TPolyLineGI::RemoveSegment(PPolyLineSegmentGI Segment) {
        if (Segment->Next != nullptr) {
            Segment->Next->Prev = Segment->Prev;
        }
        if (Segment->Prev != nullptr) {
            Segment->Prev->Next = Segment->Next;
        }
        if (LastSegment == Segment) {
            LastSegment = Segment->Prev;
        }
        if (FirstSegment == Segment) {
            FirstSegment = Segment->Next;
        }
        if (SegmentHeap != 0) {
            if (Segment->SavedPixels != nullptr) {
                EC_Mem::FreeFromHeapEC(SegmentHeap, Segment->SavedPixels);
                Segment->SavedPixels = nullptr;
            }
            if (Segment->PreviousPixels != nullptr) {
                EC_Mem::FreeFromHeapEC(SegmentHeap, Segment->PreviousPixels);
                Segment->PreviousPixels = nullptr;
            }
            EC_Mem::FreeFromHeapEC(SegmentHeap, Segment);
        }
    }

    void TPolyLineGI::AllocatePixelBuffers(PPolyLineSegmentGI Segment) {
        if (Segment->SavedPixels == nullptr) {
            Segment->SavedPixels = EC_Mem::AllocFromHeapEC(SegmentHeap, Segment->PixelCount * 2 + 10);
        }
        if (Segment->PreviousPixels == nullptr) {
            Segment->PreviousPixels = EC_Mem::AllocFromHeapEC(SegmentHeap, Segment->PixelCount * 2 + 10);
        }
    }

    void TPolyLineGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        TPolyLineGI::LoadPolyLineProperties(Block);
    }

    void TPolyLineGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TPolyLineGI::LoadPolyLineProperties(Block);
    }

    void TPolyLineGI::LoadPolyLineProperties(EC_BlockPar::TBlockParEC* Block) {
    }

    void TPolyLineGI::RebuildBounds() {
        PPolyLineSegmentGI Segment{};
        WindowsSdk::TPoint Minimum{};
        WindowsSdk::TPoint Size{};
        if (NormalizeBounds) {
            Segment = FirstSegment;
            if (Segment == nullptr) {
                SetSize(ClassesImports::Point(1, 1));
            } else {
                Minimum = Segment->First;
                while (Segment != nullptr) {
                    if (Minimum.X > Segment->First.X) {
                        Minimum.X = Segment->First.X;
                    }
                    if (Minimum.Y > Segment->First.Y) {
                        Minimum.Y = Segment->First.Y;
                    }
                    if (Minimum.X > Segment->Last.X) {
                        Minimum.X = Segment->Last.X;
                    }
                    if (Minimum.Y > Segment->Last.Y) {
                        Minimum.Y = Segment->Last.Y;
                    }
                    Segment = Segment->Next;
                }
                SetPosition(ClassesImports::Point(LocalPosition.X + Minimum.X, LocalPosition.Y + Minimum.Y));
                Size = ClassesImports::Point(1, 1);
                Segment = FirstSegment;
                while (Segment != nullptr) {
                    pas::store_unaligned<WindowsSdk::TPoint>(&Segment->First, ClassesImports::Point(Segment->First.X - Minimum.X, Segment->First.Y - Minimum.Y));
                    pas::store_unaligned<WindowsSdk::TPoint>(&Segment->Last, ClassesImports::Point(Segment->Last.X - Minimum.X, Segment->Last.Y - Minimum.Y));
                    if (Size.X <= Segment->First.X) {
                        Size.X = Segment->First.X + 1;
                    }
                    if (Size.Y <= Segment->First.Y) {
                        Size.Y = Segment->First.Y + 1;
                    }
                    if (Size.X <= Segment->Last.X) {
                        Size.X = Segment->Last.X + 1;
                    }
                    if (Size.Y <= Segment->Last.Y) {
                        Size.Y = Segment->Last.Y + 1;
                    }
                    Segment = Segment->Next;
                }
                SetSize(Size);
            }
        }
    }

    PPolyLineSegmentGI TPolyLineGI::AddParentLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t UserData) {
        PPolyLineSegmentGI Segment = AllocateSegment();
        pas::store_unaligned<WindowsSdk::TPoint>(&Segment->First, ClassesImports::Point(First.X - LocalPosition.X, First.Y - LocalPosition.Y));
        pas::store_unaligned<WindowsSdk::TPoint>(&Segment->Last, ClassesImports::Point(Last.X - LocalPosition.X, Last.Y - LocalPosition.Y));
        Segment->Color = Color;
        Segment->PixelCount = aMyFunction::IntegerPointDistancePlusOne(First, Last);
        Segment->PixelCapacity = Segment->PixelCount;
        Segment->UserData = UserData;
        Segment->Animated = true;
        if (AutoRebuildBounds) {
            RebuildBounds();
        }
        return Segment;
    }

    PPolyLineSegmentGI TPolyLineGI::AddLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color) {
        return AddLocalLine(First, Last, Color, 0);
    }

    PPolyLineSegmentGI TPolyLineGI::AddLocalLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t UserData) {
        PPolyLineSegmentGI Segment = AllocateSegment();
        pas::store_unaligned<WindowsSdk::TPoint>(&Segment->First, First);
        pas::store_unaligned<WindowsSdk::TPoint>(&Segment->Last, Last);
        Segment->Color = Color;
        Segment->PixelCount = aMyFunction::IntegerPointDistancePlusOne(First, Last);
        Segment->PixelCapacity = Segment->PixelCount;
        Segment->UserData = UserData;
        Segment->Animated = true;
        if (AutoRebuildBounds) {
            RebuildBounds();
        }
        return Segment;
    }

    void TPolyLineGI::UpdateSegmentLength(PPolyLineSegmentGI Segment) {
        std::int32_t Count = aMyFunction::IntegerPointDistancePlusOne(Segment->First, Segment->Last);
        if (Count > Segment->PixelCapacity) {
            Segment->PixelCount = Count;
            Segment->PixelCapacity = Segment->PixelCount;
            Segment->SavedPixels = EC_Mem::ReAllocFromHeapREC(SegmentHeap, Segment->SavedPixels, Segment->PixelCount * 2 + 10);
            Segment->PreviousPixels = EC_Mem::ReAllocFromHeapREC(SegmentHeap, Segment->PreviousPixels, Segment->PixelCount * 2 + 10);
        } else {
            Segment->PixelCount = Count;
        }
    }

    void TPolyLineGI::RetireSegment(PPolyLineSegmentGI Segment) {
        void* Buffer{};
        if (Segment->PreviouslyVisible && Segment->PreviousPixels != nullptr) {
            Buffer = EC_Mem::AllocEC(Segment->PixelCount * 2 + 10);
            Windows::CopyMemory(Buffer, Segment->PreviousPixels, Segment->PixelCount * 2 + 10);
            MessageLoop->AddSavedLine(Segment->PreviousFirst, Segment->PreviousLast, Buffer);
        }
        RemoveSegment(Segment);
    }

    void TPolyLineGI::StartAnimation() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        AnimationTimer = MessageLoop->ScheduleCallbackTimer(100, 100, pas::bind_method<&TPolyLineGI::AdvanceAnimation>(this), 0);
    }

    void TPolyLineGI::StopAnimation() {
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
    }

    void TPolyLineGI::AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        AnimationPhase += 30;
        if (AnimationPhase >= 360) {
            AnimationPhase -= 360;
        }
        Invalidate();
    }

    void TPolyLineGI::Invalidate() {
        if (!FrameDrawing) {
            GI_MessageLoop::TObjectGI::Invalidate();
            return;
        }
        if (ShadowCircle == nullptr) {
            MessageLoop->UpdateRects->Clear();
            MessageLoop->UpdateRectsEnabled = true;
            MessageLoop->InvalidateViewport();
            MessageLoop->UpdateRectsEnabled = false;
        }
    }

    void TPolyLineGI::ErasePreviousFrame() {
        PPolyLineSegmentGI Segment{};
        FrameDrawing = true;
        if (!GlobalsV::SkipSavedPixelRestore) {
            Segment = FirstSegment;
            while (Segment != nullptr) {
                AllocatePixelBuffers(Segment);
                if (Segment->PreviouslyVisible) {
                    std::int32_t x = Segment->PreviousFirst.X;
                    std::int32_t y = Segment->PreviousFirst.Y;
                    std::int32_t x_2 = Segment->PreviousLast.X;
                    std::int32_t y_2 = Segment->PreviousLast.Y;
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* previousPixels = Segment->PreviousPixels;
                    GR_Main::Ex_OKGR_Line_CopyFromBuf_WORD(previousPixels, pixels, pitchBytes, x, y, x_2, y_2);
                }
                Segment = Segment->Next;
            }
        }
    }

    void TPolyLineGI::PrepareFrameDraw() {
        PPolyLineSegmentGI Segment{};
        WindowsSdk::TRect Clip{};
        FrameDrawing = true;
        if (ShadowCircle != nullptr) {
            Clip = ShadowCircle->HitTestBounds;
        } else {
            Clip = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        }
        Segment = FirstSegment;
        while (Segment != nullptr) {
            AllocatePixelBuffers(Segment);
            pas::store_unaligned<WindowsSdk::TPoint>(&Segment->PixelFirst, ClassesImports::Point(Segment->First.X + AbsolutePosition.X, Segment->First.Y + AbsolutePosition.Y));
            pas::store_unaligned<WindowsSdk::TPoint>(&Segment->PixelLast, ClassesImports::Point(Segment->Last.X + AbsolutePosition.X, Segment->Last.Y + AbsolutePosition.Y));
            if (Segment->Kind != 2) {
                if (GR_Main::Ex_OKGR_Line_Clip(pas::Var<std::int32_t>(&Segment->PixelFirst.X), pas::Var<std::int32_t>(&Segment->PixelFirst.Y), pas::Var<std::int32_t>(&Segment->PixelLast.X), pas::Var<std::int32_t>(&Segment->PixelLast.Y), Clip) == 0) {
                    Segment->Visible = false;
                } else {
                    Segment->Visible = true;
                }
            } else {
                Segment->ClippedColor = Segment->Color;
                Segment->ClippedEndColor = Segment->EndColor;
                if (GR_Main::Ex_OKGR_LineColor_Clip(pas::Var<std::int32_t>(&Segment->PixelFirst.X), pas::Var<std::int32_t>(&Segment->PixelFirst.Y), pas::Var<std::uint32_t>(&Segment->ClippedColor), pas::Var<std::int32_t>(&Segment->PixelLast.X), pas::Var<std::int32_t>(&Segment->PixelLast.Y), pas::Var<std::uint32_t>(&Segment->ClippedEndColor), Clip) == 0) {
                    Segment->Visible = false;
                } else {
                    Segment->Visible = true;
                }
            }
            Segment = Segment->Next;
        }
        return;
    }

    void TPolyLineGI::DrawUpdateRects(WindowsSdk::TRect ClipRect) {
        WindowsSdk::TRect Clip{};
        FrameDrawing = true;
        Clip = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        PPolyLineSegmentGI Segment = FirstSegment;
        while (Segment != nullptr) {
            if (Segment->Visible) {
                DrawFrameSegment(Segment, Clip);
            }
            Segment = Segment->Next;
        }
    }

    void TPolyLineGI::Draw(WindowsSdk::TRect ClipRect) {
        FrameDrawing = false;
        PPolyLineSegmentGI Segment = FirstSegment;
        while (Segment != nullptr) {
            pas::store_unaligned<WindowsSdk::TPoint>(&Segment->PixelFirst, ClassesImports::Point(Segment->First.X + AbsolutePosition.X, Segment->First.Y + AbsolutePosition.Y));
            pas::store_unaligned<WindowsSdk::TPoint>(&Segment->PixelLast, ClassesImports::Point(Segment->Last.X + AbsolutePosition.X, Segment->Last.Y + AbsolutePosition.Y));
            DrawSegment(Segment, ClipRect);
            Segment = Segment->Next;
        }
    }

    void TPolyLineGI::DrawSegment(PPolyLineSegmentGI Segment, WindowsSdk::TRect ClipRect) {
        if (GlobalsV::HardwareRenderingEnabled) {
            if (Segment->Animated) {
                GR_DX::DrawAnimatedLineDX(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, GR_DX::Color565ToArgb(Segment->Color), AnimationPhase, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
            } else {
                GR_DX::DrawAnimatedLineDX(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, GR_DX::Color565ToArgb(Segment->Color), 0, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
            }
        } else if (Segment->Animated) {
            GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, AnimationPhase, ClipRect);
        } else {
            GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, 0, ClipRect);
        }
    }

    void TPolyLineGI::DrawFrameSegment(PPolyLineSegmentGI Segment, WindowsSdk::TRect ClipRect) {
        GR_Main::TLineRasterizer16 Rasterizer{};
        std::int32_t X = 0;
        std::int32_t Y = 0;
        if (ShadowCircle == nullptr) {
            if (Segment->Kind == 0) {
                if (GlobalsV::HardwareRenderingEnabled) {
                    if (Segment->Animated) {
                        GR_DX::DrawAnimatedLineDX(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, GR_DX::Color565ToArgb(Segment->Color), AnimationPhase, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                    } else {
                        GR_DX::DrawAnimatedLineDX(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, GR_DX::Color565ToArgb(Segment->Color), 0, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                    }
                } else if (Segment->Animated) {
                    GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, AnimationPhase, ClipRect);
                } else {
                    GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, 0, ClipRect);
                }
            } else if (Segment->Kind == 1) {
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawAlphaLine(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, Segment->Color, Segment->Color >> 24 & 0x000000ff, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                } else {
                    GR_Main::ScreenRenderBuffer->DrawLine16Clipped(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, ClipRect);
                }
            } else if (GlobalsV::HardwareRenderingEnabled) {
                GR_DX::DrawGradientLine(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->ClippedColor, Segment->PixelLast.X, Segment->PixelLast.Y, Segment->ClippedEndColor, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
            } else {
                Rasterizer = GR_Main::LineRasterizer16;
                {
                    std::uint32_t clippedEndColor = Segment->ClippedEndColor;
                    std::int32_t y = Segment->PixelLast.Y;
                    std::int32_t x = Segment->PixelLast.X;
                    std::uint32_t clippedColor = Segment->ClippedColor;
                    std::int32_t y_2 = Segment->PixelFirst.Y;
                    std::int32_t x_2 = Segment->PixelFirst.X;
                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    Rasterizer(pixels, pitchBytes, x_2, y_2, clippedColor, x, y, clippedEndColor);
                }
            }
        } else if (ShadowCircle->LightBuffer != nullptr && ShadowCircle->LightBuffer->GetPixels() != nullptr && Segment->Visible) {
            if (!GlobalsV::SkipSavedPixelRestore) {
                {
                    std::int32_t x_3 = Segment->PixelFirst.X;
                    std::int32_t y_3 = Segment->PixelFirst.Y;
                    std::int32_t x_4 = Segment->PixelLast.X;
                    std::int32_t y_4 = Segment->PixelLast.Y;
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    void* savedPixels = Segment->SavedPixels;
                    GR_Main::Ex_OKGR_Line_CopyFromBuf_WORD(savedPixels, pixels_2, pitchBytes_2, x_3, y_3, x_4, y_4);
                }
                if (Segment->Animated) {
                    std::uint32_t color = Segment->Color;
                    std::int32_t animationPhase = AnimationPhase;
                    WindowsSdk::TRect clipRect = ClipRect;
                    std::int32_t cpp_arg = ShadowCircle->LightBuffer->PitchBytes * Y + X;
                    void* pixels_3 = ShadowCircle->LightBuffer->GetPixels();
                    void* addPointerOffset = EC_Mem::AddPointerOffset(pixels_3, cpp_arg);
                    std::int32_t pitchBytes_3 = ShadowCircle->LightBuffer->PitchBytes;
                    WindowsSdk::TPoint point = ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y);
                    WindowsSdk::TPoint point_2 = ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y);
                    GR_GraphBuf::TGraphBufGR* screenRenderBuffer = GR_Main::ScreenRenderBuffer;
                    screenRenderBuffer->DrawShadowLine16(point_2, point, color, animationPhase, clipRect, addPointerOffset, pitchBytes_3);
                } else {
                    std::uint32_t color_2 = Segment->Color;
                    WindowsSdk::TRect clipRect_2 = ClipRect;
                    std::int32_t cpp_arg_2 = ShadowCircle->LightBuffer->PitchBytes * Y + X;
                    void* pixels_4 = ShadowCircle->LightBuffer->GetPixels();
                    void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels_4, cpp_arg_2);
                    std::int32_t pitchBytes_4 = ShadowCircle->LightBuffer->PitchBytes;
                    WindowsSdk::TPoint point_3 = ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y);
                    WindowsSdk::TPoint point_4 = ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y);
                    GR_GraphBuf::TGraphBufGR* screenRenderBuffer_2 = GR_Main::ScreenRenderBuffer;
                    screenRenderBuffer_2->DrawShadowLine16(point_4, point_3, color_2, 0, clipRect_2, addPointerOffset_2, pitchBytes_4);
                }
            } else if (Segment->Animated) {
                GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, AnimationPhase, ClipRect);
            } else {
                GR_Main::ScreenRenderBuffer->DrawAnimatedLine16(ClassesImports::Point(Segment->PixelFirst.X, Segment->PixelFirst.Y), ClassesImports::Point(Segment->PixelLast.X, Segment->PixelLast.Y), Segment->Color, 0, ClipRect);
            }
        }
    }

    void TPolyLineGI::CommitFrameDraw() {
    }

    void TPolyLineGI::p_destroy() {
        GI_PolyLine::TPolyLineGI_Destroy(this);
    }

} // namespace GI_PolyLine
