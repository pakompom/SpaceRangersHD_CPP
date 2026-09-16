#include "layout/GI_Tail.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Tail.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

// Native GI_Tail metadata starts; methods end.
// TTailGI belongs to GI_Tail through its dynamic-array RTTI.
namespace GI_Tail {
    // Segments is a Delphi dynamic array, with inactive slots included in SegmentCapacity.
    // SegmentVelocity is displacement per 20 ms movement callback.
    void TTailGI_Create(TTailGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageCache = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->ImageCache);
        Self->EmitIntervalMs = 20;
        Self->Emitting = true;
        Self->LastSegmentIndex = -1;
    }

    void TTailGI_Destroy(TTailGI* Self) {
        if (Self->FrameTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->FrameTimer);
            Self->FrameTimer = nullptr;
        }
        if (Self->MoveTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->MoveTimer);
            Self->MoveTimer = nullptr;
        }
        if (Self->EmitTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->EmitTimer);
            Self->EmitTimer = nullptr;
        }
        Self->ClearSegments();
        pas::free(Self->ImageCache);
        Self->ImageCache = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Preserves timers and emission state.
    void TTailGI::ClearSegments() {
        LastSegmentIndex = -1;
        SegmentCapacity = 0;
        Segments = nullptr;
    }

    // Requires at least one GAI sequence. Existing segments are kept.
    void TTailGI::SetImagePath(const pas::WideString& ImagePath) {
        EC_CacheGAI::TCGaiEC* Data{};
        if (ImageCache->CacheKey != ImagePath) {
            Invalidate();
            ImageCache->SetCacheKey(ImagePath);
            Data = nullptr;
            {
                std::exception_ptr cpp_error{};
                try {
                    Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                    if (Data->GetSequenceCount() < 1) {
                        GR_Main::RaiseWideMessage(pas::concat_wide({u"TTailGI.SetImage.AnimCount Path=", ImagePath}));
                    }
                    FrameCount = Data->GetSequenceFrameCount(0);
                    ImageSize = Data->GetCanvasSize();
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                if (Data != nullptr) {
                    ImageCache->Release();
                }
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
            }
        }
    }

    pas::WideString TTailGI::GetImagePath() {
        return ImageCache->CacheKey;
    }

    // Reuses the last inactive slot or grows by 16. Growth can invalidate earlier pointers; only Active is initialized.
    PTailSegmentGI TTailGI::AllocateSegment() {
        std::int32_t I{};
        PTailSegmentGI Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
            if (!Segments[I].Active) {
                Result = &Segments[I];
                LastSegmentIndex = I;
            }
        }
        if (Result == nullptr) {
            Segments.set_length(SegmentCapacity + 16);
            Result = &Segments[SegmentCapacity];
            LastSegmentIndex = SegmentCapacity;
            SegmentCapacity += 16;
        }
        Result->Active = true;
        return Result;
    }

    void TTailGI::AdvanceSegmentFrames(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PTailSegmentGI Segment{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
            Segment = &Segments[I];
            if (Segment->Active) {
                ++Segment->FrameIndex;
                // The neutral additions preserve native operand materialization order.
                if (Segment->FrameIndex + 0 >= FrameCount) {
                    Segment->Active = false;
                    if (I + 0 == LastSegmentIndex) {
                        LastSegmentIndex = -1;
                    }
                }
            }
        }
    }

    void TTailGI::MoveSegments(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PTailSegmentGI Segment{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
            Segment = &Segments[I];
            if (Segment->Active) {
                Segment->Position.X = static_cast<long double>(Segment->Position.X) + Segment->Velocity.X;
                Segment->Position.Y = static_cast<long double>(Segment->Position.Y) + Segment->Velocity.Y;
                Segment->PixelPosition.X = System::Round(Segment->Position.X);
                Segment->PixelPosition.Y = System::Round(Segment->Position.Y);
            }
        }
    }

    // Suppresses emission within squared distance 0.001 of the last live segment.
    void TTailGI::EmitSegment(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        EC_Struct::TPointF Position{};
        Position.X = SegmentVelocity.X * 1.0L + EmitterPosition.X;
        Position.Y = SegmentVelocity.Y * 1.0L + EmitterPosition.Y;
        if (LastSegmentIndex >= 0) {
            if (aMyFunction::PointDistanceSquared(Position, Segments[LastSegmentIndex].Position) < 0.001L) {
                return;
            }
        }
        PTailSegmentGI Segment = AllocateSegment();
        Segment->FrameIndex = 0;
        pas::store_unaligned<EC_Struct::TPointF>(&Segment->Position, Position);
        pas::store_unaligned<EC_Struct::TPointF>(&Segment->Velocity, SegmentVelocity);
        Segment->PixelPosition.X = System::Round(Segment->Position.X);
        Segment->PixelPosition.Y = System::Round(Segment->Position.Y);
    }

    void TTailGI::OffsetSegments(EC_Struct::TPointF Delta) {
        PTailSegmentGI Segment{};
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
            Segment = &Segments[I];
            if (Segment->Active) {
                Segment->Position.X = static_cast<long double>(Segment->Position.X) + Delta.X;
                Segment->Position.Y = static_cast<long double>(Segment->Position.Y) + Delta.Y;
                Segment->PixelPosition.X = System::Round(Segment->Position.X);
                Segment->PixelPosition.Y = System::Round(Segment->Position.Y);
            }
        }
    }

    // Deactivation cancels timers. Drawing restarts them when Emitting is true.
    void TTailGI::SetActive(std::uint8_t Enabled) {
        if (Active != Enabled) {
            GI_MessageLoop::TObjectGI::SetActive(Enabled);
            if (!Active) {
                if (FrameTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(FrameTimer);
                    FrameTimer = nullptr;
                }
                if (MoveTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(MoveTimer);
                    MoveTimer = nullptr;
                }
                if (EmitTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(EmitTimer);
                    EmitTimer = nullptr;
                }
            }
        }
    }

    // Disabling emission leaves existing segments animating.
    void TTailGI::SetEmitting(std::uint8_t Enabled) {
        if (Emitting != Enabled) {
            Emitting = Enabled;
            if (!Emitting) {
                if (EmitTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(EmitTimer);
                    EmitTimer = nullptr;
                }
            } else {
                if (FrameTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(FrameTimer);
                    FrameTimer = nullptr;
                }
                if (MoveTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(MoveTimer);
                    MoveTimer = nullptr;
                }
                if (EmitTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(EmitTimer);
                    EmitTimer = nullptr;
                }
                FrameTimer = MessageLoop->ScheduleCallbackTimer(20, 20, pas::bind_method<&TTailGI::AdvanceSegmentFrames>(this), 0);
                MoveTimer = MessageLoop->ScheduleCallbackTimer(20, 20, pas::bind_method<&TTailGI::MoveSegments>(this), 0);
                EmitTimer = MessageLoop->ScheduleCallbackTimer(EmitIntervalMs, EmitIntervalMs, pas::bind_method<&TTailGI::EmitSegment>(this), 0);
            }
        }
    }

    void TTailGI::Invalidate() {
        PTailSegmentGI Segment{};
        std::int32_t I{};
        Types::TRect Bounds{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
            Segment = &Segments[I];
            if (Segment->Active) {
                Bounds.Left = AbsolutePosition.X + Segment->PixelPosition.X - pas::shr(ImageSize.X, 1);
                Bounds.Top = AbsolutePosition.Y + Segment->PixelPosition.Y - pas::shr(ImageSize.Y, 1);
                Bounds.Right = Bounds.Left + ImageSize.X;
                Bounds.Bottom = Bounds.Top + ImageSize.Y;
                MessageLoop->QueueUpdateRect(Bounds);
            }
        }
    }

    void TTailGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        TTailGI::LoadTailProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TTailGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        TTailGI::LoadTailProperties(Block);
    }

    // Empty in the native binary.
    void TTailGI::LoadTailProperties(EC_BlockPar::TBlockParEC* Block) {
    }

    // Empty; does not call inherited UpdateAutoGeometry.
    void TTailGI::UpdateAutoGeometry() {
    }

    void TTailGI::Draw(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        PTailSegmentGI Segment{};
        std::int32_t I{};
        GR_gi::TgiGR* Gi{};
        Types::TPoint Origin{};
        Types::TRect Bounds{};
        Types::TRect Intersection{};
        if (Emitting) {
            if (FrameTimer == nullptr) {
                Emitting = false;
                SetEmitting(true);
            }
        }
        EC_CacheGAI::TCGaiEC* Data = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
                    Segment = &Segments[I];
                    if (Segment->Active) {
                        Bounds.Left = AbsolutePosition.X + Segment->PixelPosition.X - pas::shr(ImageSize.X, 1);
                        Bounds.Top = AbsolutePosition.Y + Segment->PixelPosition.Y - pas::shr(ImageSize.Y, 1);
                        Bounds.Right = ImageSize.X + Bounds.Left;
                        Bounds.Bottom = ImageSize.Y + Bounds.Top;
                        if (EC_Struct::IntersectRects(Intersection, Bounds, ClipRect)) {
                            if (GlobalsV::HardwareRenderingEnabled) {
                                Origin = Data->GetFrameOrigin(Data->GetSequenceFrameIndex(0, Segment->FrameIndex));
                                {
                                    pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateFrameSurface = (Data->GetOrCreateFrameSurface(Data->GetSequenceFrameIndex(0, Segment->FrameIndex), cpp_result), cpp_result);
                                    std::int32_t cpp_arg = Origin.Y + Bounds.Top;
                                    std::int32_t cpp_arg_2 = Origin.X + Bounds.Left;
                                    GR_DX::DrawTexture(orCreateFrameSurface, cpp_arg_2, cpp_arg, 255, 0x00ffffffu, &ClipRect, false, false);
                                }
                            } else {
                                Gi = Data->LoadFrameGi(Data->GetSequenceFrameIndex(0, Segment->FrameIndex));
                                Gi->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Gi->GetBoundsRect().Left + Bounds.Left - Data->GetBoundsRect().Left, Gi->GetBoundsRect().Top + Bounds.Top - Data->GetBoundsRect().Top, ClipRect, 0, 255);
                            }
                        }
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Data != nullptr) {
                ImageCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    // Ignores ClipRect; uses the message loop's update rectangles.
    void TTailGI::DrawUpdateRects(Types::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        PTailSegmentGI Segment{};
        std::int32_t I{};
        GR_gi::TgiGR* Gi{};
        GR_Rect::TRectGR* RectNode{};
        Types::TPoint Origin{};
        Types::TRect Bounds{};
        Types::TRect Intersection{};
        if (Emitting) {
            if (FrameTimer == nullptr) {
                Emitting = false;
                SetEmitting(true);
            }
        }
        EC_CacheGAI::TCGaiEC* Data = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Data = EC_CacheGAI::AcquireCachedGai(ImageCache);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, SegmentCapacity - 1); cpp_range.next(I); ) {
                    Segment = &Segments[I];
                    if (Segment->Active) {
                        Bounds.Left = AbsolutePosition.X + Segment->PixelPosition.X - pas::shr(ImageSize.X, 1);
                        Bounds.Top = AbsolutePosition.Y + Segment->PixelPosition.Y - pas::shr(ImageSize.Y, 1);
                        Bounds.Right = ImageSize.X + Bounds.Left;
                        Bounds.Bottom = ImageSize.Y + Bounds.Top;
                        RectNode = MessageLoop->UpdateRects->FirstRect;
                        while (RectNode != nullptr) {
                            if (EC_Struct::IntersectRects(Intersection, RectNode->Bounds, Bounds)) {
                                if (GlobalsV::HardwareRenderingEnabled) {
                                    Origin = Data->GetFrameOrigin(Data->GetSequenceFrameIndex(0, Segment->FrameIndex));
                                    {
                                        pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateFrameSurface = (Data->GetOrCreateFrameSurface(Data->GetSequenceFrameIndex(0, Segment->FrameIndex), cpp_result), cpp_result);
                                        std::int32_t cpp_arg = Origin.Y + Bounds.Top;
                                        std::int32_t cpp_arg_2 = Origin.X + Bounds.Left;
                                        GR_DX::DrawTexture(orCreateFrameSurface, cpp_arg_2, cpp_arg, 255, 0x00ffffffu, &Intersection, false, false);
                                    }
                                } else {
                                    Gi = Data->LoadFrameGi(Data->GetSequenceFrameIndex(0, Segment->FrameIndex));
                                    Gi->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Gi->GetBoundsRect().Left + Bounds.Left - Data->GetBoundsRect().Left, Gi->GetBoundsRect().Top + Bounds.Top - Data->GetBoundsRect().Top, Intersection, 0, 255);
                                }
                            }
                            RectNode = RectNode->Next;
                        }
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Data != nullptr) {
                ImageCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    void TTailGI::p_destroy() {
        GI_Tail::TTailGI_Destroy(this);
    }

} // namespace GI_Tail
