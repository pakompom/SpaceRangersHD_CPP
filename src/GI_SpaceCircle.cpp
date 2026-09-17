#include "layout/GI_SpaceCircle.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SpaceCircle.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"

namespace GI_SpaceCircle {
    void TSpaceCircleGI_Create(TSpaceCircleGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->Radius = 100;
    }

    void TSpaceCircleGI_Destroy(TSpaceCircleGI* Self) {
        Self->ClearSegments();
        Self->ClearPreviousLines();
        if (Self->SavedPixels != nullptr) {
            EC_Mem::FreeEC(Self->SavedPixels);
            Self->SavedPixels = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TSpaceCircleGI::SetRadius(std::int32_t Value) {
        if (Radius != Value) {
            Radius = Value;
            if (Active) {
                RebuildSegments();
                GeometryDirty = true;
            }
        }
    }

    void TSpaceCircleGI::SetCenter(Types::TPoint Value) {
        if (Center.X != Value.X || Center.Y != Value.Y) {
            Center = Value;
            if (Active) {
                RebuildSegments();
                GeometryDirty = true;
            }
        }
    }

    void TSpaceCircleGI::ClearSegments() {
        if (Segments != nullptr) {
            EC_Mem::FreeEC(Segments);
            Segments = nullptr;
        }
        SegmentCount = 0;
    }

    void TSpaceCircleGI::ClearPreviousLines() {
        if (PreviousLines != nullptr) {
            EC_Mem::FreeEC(PreviousLines);
            PreviousLines = nullptr;
        }
        PreviousLineCount = 0;
    }

    void TSpaceCircleGI::RebuildSegments() {
        float Spacing{};
        float Circumference{};
        float Angle{};
        float Step{};
        float Length{};
        std::int32_t I{};
        std::int32_t Count{};
        PSpaceCircleSegmentGI Segment{};
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF Delta{};
        ClearSegments();
        if (Radius > 0) {
            Spacing = 2.0E+1f;
            Circumference = Radius * 6.2831852L;
            Count = System::Round(pas::real_divide(Circumference, Spacing));
            if (Count < 10) {
                Count = 10;
            }
            Segments = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AllocEC(Count * static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
            Angle = 0.0f;
            Step = pas::real_divide(6.2831852L, Count);
            {
                float cpp_arg = System::Sin(Angle) * Radius;
                float cpp_arg_2 = System::Cos(Angle) * -Radius;
                Point = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
            }
            Segment = Segments;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                pas::store_unaligned<EC_Struct::TPointF>(&Segment->First, EC_Struct::AddPointsF(Point, EC_Struct::PointToPointF(Center)));
                Angle = static_cast<long double>(Angle) + Step;
                {
                    float cpp_arg_3 = System::Sin(Angle) * Radius;
                    float cpp_arg_4 = System::Cos(Angle) * -Radius;
                    Point = EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4);
                }
                pas::store_unaligned<EC_Struct::TPointF>(&Segment->Last, EC_Struct::AddPointsF(Point, EC_Struct::PointToPointF(Center)));
                Delta = EC_Struct::SubtractPointsF(Segment->Last, Segment->First);
                Length = System::Sqrt(static_cast<long double>(Delta.X) * Delta.X + static_cast<long double>(Delta.Y) * Delta.Y);
                Delta.X = pas::real_divide(Delta.X, Length);
                Delta.Y = pas::real_divide(Delta.Y, Length);
                Segment->Last.X = static_cast<long double>(Delta.X) * Length * 0.75L + Segment->First.X;
                Segment->Last.Y = static_cast<long double>(Delta.Y) * Length * 0.75L + Segment->First.Y;
                Segment->First.X = static_cast<long double>(Delta.X) * Length * 0.25L + Segment->First.X;
                Segment->First.Y = static_cast<long double>(Delta.Y) * Length * 0.25L + Segment->First.Y;
                Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
            }
            SegmentCount = Count;
        }
    }

    void TSpaceCircleGI::ProjectAndClipSegments() {
        PSpaceCircleSegmentGI Segment{};
        std::int32_t I{};
        Types::TRect Clip{};
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Clip, 0), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&HitTestBounds, 0)));
        Clip.Right = HitTestBounds.Right - 1;
        Clip.Bottom = HitTestBounds.Bottom - 1;
        Segment = Segments;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCount - 1); cpp_range.next(I); ) {
            pas::store_unaligned<Types::TPoint>(&Segment->PixelFirst, EC_Struct::AddPoints(EC_Struct::RoundPointF(Segment->First), AbsolutePosition));
            pas::store_unaligned<Types::TPoint>(&Segment->PixelLast, EC_Struct::AddPoints(EC_Struct::RoundPointF(Segment->Last), AbsolutePosition));
            Segment->ClipResult = GR_Main::Ex_OKGR_Line_Clip(pas::Var<std::int32_t>(&Segment->PixelFirst.X), pas::Var<std::int32_t>(&Segment->PixelFirst.Y), pas::Var<std::int32_t>(&Segment->PixelLast.X), pas::Var<std::int32_t>(&Segment->PixelLast.Y), Clip);
            Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
        }
    }

    void TSpaceCircleGI::RotateSegments(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PSpaceCircleSegmentGI Segment{};
        std::int32_t I{};
        float Sine{};
        float Cosine{};
        float X{};
        float Y{};
        float Angle{};
        if (Radius > 0) {
            Angle = pas::real_divide(-2.0L, Radius * 6.2831852L) * 3.1415926L * 2.0L;
            Sine = System::Sin(Angle);
            Cosine = System::Cos(Angle);
            Segment = Segments;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCount - 1); cpp_range.next(I); ) {
                X = static_cast<long double>(Segment->First.X) - Center.X;
                Y = static_cast<long double>(Segment->First.Y) - Center.Y;
                Segment->First.X = static_cast<long double>(Cosine) * X + static_cast<long double>(Sine) * Y + Center.X;
                Segment->First.Y = static_cast<long double>(-Sine) * X + static_cast<long double>(Cosine) * Y + Center.Y;
                X = static_cast<long double>(Segment->Last.X) - Center.X;
                Y = static_cast<long double>(Segment->Last.Y) - Center.Y;
                Segment->Last.X = static_cast<long double>(Cosine) * X + static_cast<long double>(Sine) * Y + Center.X;
                Segment->Last.Y = static_cast<long double>(-Sine) * X + static_cast<long double>(Cosine) * Y + Center.Y;
                Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
            }
            GeometryDirty = true;
        }
    }

    // Deactivation is deferred until CommitFrameDraw.
    void TSpaceCircleGI::SetActive(std::uint8_t Enabled) {
        if (Active != Enabled) {
            if (Enabled) {
                GI_MessageLoop::TObjectGI::SetActive(Enabled);
                RebuildSegments();
                GeometryDirty = true;
            } else {
                if (AnimationTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(AnimationTimer);
                    AnimationTimer = nullptr;
                }
                DeactivateAfterFrame = true;
                ClearSegments();
            }
        }
    }

    void TSpaceCircleGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (Active) {
            RebuildSegments();
            GeometryDirty = true;
        }
    }

    void TSpaceCircleGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        ClearSegments();
        ClearPreviousLines();
        if (SavedPixels != nullptr) {
            EC_Mem::FreeEC(SavedPixels);
            SavedPixels = nullptr;
        }
    }

    void TSpaceCircleGI_LoadFromConfigPath(TSpaceCircleGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        TSpaceCircleGI::LoadSpaceCircleProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TSpaceCircleGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TSpaceCircleGI::LoadSpaceCircleProperties(Block);
    }

    // Empty in native code.
    void TSpaceCircleGI::LoadSpaceCircleProperties(EC_BlockPar::TBlockParEC* Block) {
    }

    // Empty in native code.
    void TSpaceCircleGI::Invalidate() {
    }

    void TSpaceCircleGI::ErasePreviousFrame() {
        PSpaceCircleSavedLineGI Line{};
        std::int32_t I{};
        void* Buffer{};
        std::int32_t Count{};
        if (GeometryDirty) {
            ProjectAndClipSegments();
            GeometryDirty = false;
            if (AnimationTimer == nullptr) {
                AnimationTimer = MessageLoop->ScheduleCallbackTimer(50, 50, pas::bind_method<&TSpaceCircleGI::RotateSegments>(this), 0);
            }
        }
        if (static_cast<std::uint8_t>(GlobalsV::HardwareRenderingEnabled ^ 1) && static_cast<std::uint8_t>(GlobalsV::SkipSavedPixelRestore ^ 1)) {
            if (!GlobalsV::BGImage) {
                Line = PreviousLines;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, PreviousLineCount - 1); cpp_range.next(I); ) {
                    GR_Main::ScreenRenderBuffer->DrawLine16Clipped(Line->First, Line->Last, 0u, HitTestBounds);
                    Line = static_cast<PSpaceCircleSavedLineGI>(EC_Mem::AddPointerOffset(Line, static_cast<std::int32_t>(sizeof(TSpaceCircleSavedLineGI))));
                }
            } else if (SavedPixels != nullptr) {
                Buffer = SavedPixels;
                Line = PreviousLines;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PreviousLineCount - 1); cpp_range_2.next(I); ) {
                    {
                        std::int32_t x = Line->First.X;
                        std::int32_t y = Line->First.Y;
                        std::int32_t x_2 = Line->Last.X;
                        std::int32_t y_2 = Line->Last.Y;
                        void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                        Count = GR_Main::Ex_OKGR_Line_CopyFromBuf_WORD(Buffer, pixels, pitchBytes, x, y, x_2, y_2);
                    }
                    Buffer = EC_Mem::AddPointerOffset(Buffer, Count * 2);
                    Line = static_cast<PSpaceCircleSavedLineGI>(EC_Mem::AddPointerOffset(Line, static_cast<std::int32_t>(sizeof(TSpaceCircleSavedLineGI))));
                }
            }
        }
    }

    void TSpaceCircleGI::PrepareFrameDraw() {
        PSpaceCircleSegmentGI Segment{};
        std::int32_t Copied{};
        std::int32_t Count{};
        std::int32_t Capacity{};
        std::int32_t I{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            if (GlobalsV::BGImage && static_cast<std::uint8_t>(DeactivateAfterFrame ^ 1)) {
                Count = 0;
                Capacity = 100;
                SavedPixels = EC_Mem::ReAllocREC(SavedPixels, Capacity * 2);
                Segment = Segments;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCount - 1); cpp_range.next(I); ) {
                    if (Segment->ClipResult > 0) {
                        {
                            std::int32_t x = Segment->PixelFirst.X;
                            std::int32_t y = Segment->PixelFirst.Y;
                            std::int32_t x_2 = Segment->PixelLast.X;
                            std::int32_t y_2 = Segment->PixelLast.Y;
                            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                            void* addPointerOffset = EC_Mem::AddPointerOffset(SavedPixels, Count * 2);
                            std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                            Copied = GR_Main::Ex_OKGR_Line_CopyToBuf_WORD(addPointerOffset, pixels, pitchBytes, x, y, x_2, y_2);
                        }
                        Count += Copied;
                        if (Count + 30 > Capacity) {
                            Capacity = Count + 100;
                            SavedPixels = EC_Mem::ReAllocREC(SavedPixels, Capacity * 2);
                        }
                    }
                    Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
                }
            } else if (SavedPixels != nullptr) {
                EC_Mem::FreeEC(SavedPixels);
                SavedPixels = nullptr;
            }
        }
    }

    void TSpaceCircleGI::DrawUpdateRects(Types::TRect ClipRect) {
        Draw(ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
    }

    void TSpaceCircleGI::Draw(Types::TRect ClipRect) {
        PSpaceCircleSegmentGI Segment{};
        std::int32_t I{};
        DrawnSegmentCount = 0;
        if (!DeactivateAfterFrame) {
            Segment = Segments;
            if (GlobalsV::HardwareRenderingEnabled) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCount - 1); cpp_range.next(I); ) {
                    if (Segment->ClipResult > 0) {
                        GR_DX::DrawAntialiasedLineDX(Segment->PixelFirst.X, Segment->PixelFirst.Y, Segment->PixelLast.X, Segment->PixelLast.Y, GR_DX::Color565ToArgb(Color), 255, nullptr);
                        ++DrawnSegmentCount;
                    }
                    Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
                }
            } else {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SegmentCount - 1); cpp_range_2.next(I); ) {
                    if (Segment->ClipResult > 0) {
                        GR_Main::ScreenRenderBuffer->DrawLine16(Segment->PixelFirst, Segment->PixelLast, Color);
                        ++DrawnSegmentCount;
                    }
                    Segment = static_cast<PSpaceCircleSegmentGI>(EC_Mem::AddPointerOffset(Segment, static_cast<std::int32_t>(sizeof(TSpaceCircleSegmentGI))));
                }
            }
        }
    }

    void TSpaceCircleGI::CommitFrameDraw() {
        // Native code retains this empty renderer test before deferred deactivation.
        static_cast<void>(!GlobalsV::HardwareRenderingEnabled);
        if (DeactivateAfterFrame) {
            GI_MessageLoop::TObjectGI::SetActive(false);
            DeactivateAfterFrame = false;
        }
    }

    void TSpaceCircleGI::p_destroy() {
        GI_SpaceCircle::TSpaceCircleGI_Destroy(this);
    }

    void TSpaceCircleGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_SpaceCircle::TSpaceCircleGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_SpaceCircle
