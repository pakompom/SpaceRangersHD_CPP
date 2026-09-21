#include "layout/GI_AImage.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_AImage.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

namespace GI_AImage {
    void TAImageGI_Create(TAImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->HalfAlpha = false;
    }

    void TAImageGI_Destroy(TAImageGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Does not call inherited Clear.
    void TAImageGI::Clear() {
        HalfAlpha = false;
        if (FrameTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(FrameTimer);
            FrameTimer = nullptr;
        }
        CurrentFrame = nullptr;
    }

    // Returns the componentwise maximum size over child frames.
    Types::TPoint TAImageGI::GetContentSize() {
        Types::TPoint Result{};
        Types::TPoint Size{};
        Result = ClassesImports::Point(0, 0);
        GI_Image::TImageGI* Frame = pas::checked_cast<GI_Image::TImageGI*>(FirstChild);
        if (Frame != nullptr) {
            Result = Frame->GetContentSize();
            Frame = pas::checked_cast<GI_Image::TImageGI*>(Frame->NextSibling);
        }
        while (Frame != nullptr) {
            Size = Frame->GetContentSize();
            if (Result.X < Size.X) {
                Result.X = Size.X;
            }
            if (Result.Y < Size.Y) {
                Result.Y = Size.Y;
            }
            Frame = pas::checked_cast<GI_Image::TImageGI*>(Frame->NextSibling);
        }
        return Result;
    }

    void TAImageGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            if (CurrentFrame != nullptr) {
                pas::checked_cast<GI_Image::TImageGI*>(CurrentFrame)->SetImageKindX(Value);
            }
        }
    }

    void TAImageGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            if (CurrentFrame != nullptr) {
                pas::checked_cast<GI_Image::TImageGI*>(CurrentFrame)->SetImageKindY(Value);
            }
        }
    }

    void TAImageGI::SetHalfAlpha(std::uint8_t Value) {
        if (HalfAlpha != Value) {
            HalfAlpha = Value;
            if (CurrentFrame != nullptr) {
                pas::checked_cast<GI_Image::TImageGI*>(CurrentFrame)->SetHalfAlpha(Value);
            }
        }
    }

    void TAImageGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        if (CurrentFrame != nullptr) {
            pas::checked_cast<GI_Image::TImageGI*>(CurrentFrame)->SetSize(Size);
        }
    }

    void TAImageGI::AdvanceFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_MessageLoop::TObjectGI* Previous = reinterpret_cast<GI_MessageLoop::TObjectGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)));
        GI_Image::TImageGI* Next = pas::checked_cast<GI_Image::TImageGI*>(Previous->NextSibling);
        if (Next == nullptr) {
            Next = pas::checked_cast<GI_Image::TImageGI*>(FirstChild);
        }
        MessageLoop->CancelCallbackTimer(FrameTimer);
        FrameTimer = MessageLoop->ScheduleCallbackTimer(Next->UserValue, 0x00ffffff, pas::bind_method<&TAImageGI::AdvanceFrame>(this), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Next)));
        Previous->SetActive(false);
        Next->SetActive(true);
        Next->SetOrigin(OriginPoint);
        Next->SetSize(ClientSize);
        Next->SetImageKindX(ImageKindX);
        Next->SetImageKindY(ImageKindY);
        Next->SetHalfAlpha(HalfAlpha);
        CurrentFrame = Next;
    }

    // Uses rectangular child bounds, regardless of transparent pixels.
    std::uint8_t TAImageGI::HitTest(Types::TPoint Point) {
        if (CurrentFrame == nullptr) {
            return false;
        }
        return CurrentFrame->ContainsPoint(Point);
    }

    void TAImageGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        AdvanceFrame(nullptr, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(FirstChild)));
    }

    void TAImageGI_LoadFromConfigPath(TAImageGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        GI_AImage::TAImageGI_LoadAnimationProperties(Self, GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TAImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        GI_AImage::TAImageGI_LoadAnimationProperties(this, Block);
    }

    // Numeric parameter names supply frame delays; values select child images.
    void TAImageGI_LoadAnimationProperties(TAImageGI* Self, EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Index{};
        GI_Image::TImageGI* Frame{};
        std::uint8_t HaveFrame = false;
        std::int32_t Count = Block->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (EC_Str::IsIntegerTextW(pas::view(Block->GetParamName(Index)))) {
                if (!HaveFrame) {
                    Self->FreeOwnedChildren();
                }
                Frame = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
                Frame->UserValue = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParamName(Index)));
                Frame->SetDepth(Count + 1 - Index);
                Frame->SetImagePath(Block->GetParamValue(Index));
                if (HaveFrame) {
                    Frame->SetActive(false);
                }
                HaveFrame = true;
            }
        }
        Self->CurrentFrame = nullptr;
        if (Self->FrameTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->FrameTimer);
            Self->FrameTimer = nullptr;
        }
        GI_Image::TImageGI* First = pas::checked_cast<GI_Image::TImageGI*>(Self->FirstChild);
        if (First != nullptr) {
            Self->FrameTimer = Self->MessageLoop->ScheduleCallbackTimer(First->UserValue, 0x00ffffff, pas::bind_method<&TAImageGI::AdvanceFrame>(Self), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(First)));
            First->SetSize(Self->ClientSize);
            First->SetImageKindX(Self->ImageKindX);
            First->SetImageKindY(Self->ImageKindY);
            Self->CurrentFrame = First;
        }
        if (Block->CountParams(u"HalfAlpha"_wref.get()) > 0) {
            Self->SetHalfAlpha(GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"HalfAlpha"sv))));
        }
    }

    void TAImageGI::QueueImageLoad(pas::List* PendingLoads) {
        GI_Image::TImageGI* Frame = pas::checked_cast<GI_Image::TImageGI*>(FirstChild);
        while (Frame != nullptr) {
            Frame->QueueImageLoad(PendingLoads);
            Frame = pas::checked_cast<GI_Image::TImageGI*>(Frame->NextSibling);
        }
    }

    void TAImageGI::p_destroy() {
        GI_AImage::TAImageGI_Destroy(this);
    }

    void TAImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_AImage::TAImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_AImage
