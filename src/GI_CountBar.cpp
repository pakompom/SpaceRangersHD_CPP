#include "layout/GI_CountBar.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_CountBar.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"

namespace GI_CountBar {
    void TCountBarGI_Create(TCountBarGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Orientation = 1;
        Self->Minimum = 0;
        Self->Maximum = 100;
        Self->Position = 0;
        Self->Step = 1;
        Self->DecreaseButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Self);
        Self->IncreaseButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Self);
        Self->AfterThumbImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BeforeThumbImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbButton = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, Self);
        Self->MarkerImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->DecreaseButton->DownCallback = pas::bind_method<&TCountBarGI::DecreasePressed>(Self);
        Self->IncreaseButton->DownCallback = pas::bind_method<&TCountBarGI::IncreasePressed>(Self);
        Self->AfterThumbImage->SetImageKindX(GI_Main::ikxLeftFill);
        Self->AfterThumbImage->SetImageKindY(GI_Main::ikyTopFill);
        Self->BeforeThumbImage->SetImageKindX(GI_Main::ikxLeftFill);
        Self->BeforeThumbImage->SetImageKindY(GI_Main::ikyTopFill);
        Self->ThumbButton->SetKind(GI_GraphButton::gbkFix);
    }

    void TCountBarGI_Destroy(TCountBarGI* Self) {
        if (Self->DecreaseButton != nullptr) {
            pas::free(Self->DecreaseButton);
            Self->DecreaseButton = nullptr;
        }
        if (Self->IncreaseButton != nullptr) {
            pas::free(Self->IncreaseButton);
            Self->IncreaseButton = nullptr;
        }
        if (Self->AfterThumbImage != nullptr) {
            pas::free(Self->AfterThumbImage);
            Self->AfterThumbImage = nullptr;
        }
        if (Self->BeforeThumbImage != nullptr) {
            pas::free(Self->BeforeThumbImage);
            Self->BeforeThumbImage = nullptr;
        }
        if (Self->ThumbButton != nullptr) {
            pas::free(Self->ThumbButton);
            Self->ThumbButton = nullptr;
        }
        if (Self->MarkerImage != nullptr) {
            pas::free(Self->MarkerImage);
            Self->MarkerImage = nullptr;
        }
        if (Self->RepeatTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->RepeatTimer);
            Self->RepeatTimer = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TCountBarGI::SetRange(std::int32_t MinValue, std::int32_t MaxValue) {
        if (Maximum != MaxValue || Minimum != MinValue) {
            if (MinValue > MaxValue) {
                MinValue = MaxValue;
            }
            Minimum = MinValue;
            Maximum = MaxValue;
            if (Position < Minimum) {
                SetPositionInternal(Minimum);
            }
            if (Position > Maximum) {
                SetPositionInternal(Maximum);
            }
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // Clamps without invoking PositionChangedCallback.
    void TCountBarGI::SetPositionInternal(std::int32_t Value) {
        if (Position != Value) {
            Position = Value;
            if (Position < Minimum) {
                Position = Minimum;
            }
            if (Position > Maximum) {
                Position = Maximum;
            }
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // Notifies only while Active and when the requested value differs from the previous position.
    void TCountBarGI::SetPosition_2(std::int32_t Value) {
        if (Position != Value) {
            Position = Value;
            if (Position < Minimum) {
                Position = Minimum;
            }
            if (Position > Maximum) {
                Position = Maximum;
            }
            if (Active == true) {
                UpdateLayout();
                Invalidate();
                if (pas::assigned(PositionChangedCallback)) {
                    PositionChangedCallback(this);
                }
            }
        }
    }

    void TCountBarGI::UpdateLayout() {
        std::int32_t TrackWidth{};
        std::int32_t ThumbLeft{};
        std::int32_t ThumbRight{};
        Types::TPoint ThumbSize{};
        Types::TPoint IncreaseSize{};
        Types::TPoint DecreaseSize{};
        if (Orientation == 1) {
            ThumbSize = ThumbButton->GetMaxStateImageSize();
            DecreaseSize = DecreaseButton->GetMaxStateImageSize();
            IncreaseSize = IncreaseButton->GetMaxStateImageSize();
            TrackWidth = ClientSize.X - ThumbSize.X - IncreaseSize.X - DecreaseSize.X;
            if (Maximum - Minimum == 0) {
                ThumbLeft = IncreaseSize.X;
            } else {
                ThumbLeft = static_cast<std::int32_t>(System::Round(pas::real_divide(TrackWidth * (Position - Minimum), Maximum - Minimum))) - ThumbSize.X / 2 + IncreaseSize.X + ThumbSize.X / 2;
            }
            ThumbRight = ThumbLeft + ThumbSize.X;
            DecreaseButton->SetPosition(ClassesImports::Point(0, 0));
            DecreaseButton->SetSize(DecreaseSize);
            IncreaseButton->SetPosition(ClassesImports::Point(ClientSize.X - IncreaseSize.X, 0));
            IncreaseButton->SetSize(IncreaseSize);
            BeforeThumbImage->SetPosition(ClassesImports::Point(DecreaseSize.X, 0));
            {
                Types::TPoint point = ClassesImports::Point(ThumbLeft - DecreaseSize.X, AfterThumbImage->GetContentSize().Y);
                GI_Image::TImageGI* beforeThumbImage = BeforeThumbImage;
                beforeThumbImage->SetSize(point);
            }
            AfterThumbImage->SetPosition(ClassesImports::Point(ThumbRight, 0));
            {
                std::int32_t y = BeforeThumbImage->GetContentSize().Y;
                std::int32_t cpp_arg = ClientSize.X - ThumbRight - IncreaseSize.X;
                Types::TPoint point_2 = ClassesImports::Point(cpp_arg, y);
                GI_Image::TImageGI* afterThumbImage = AfterThumbImage;
                afterThumbImage->SetSize(point_2);
            }
            ThumbButton->SetPosition(ClassesImports::Point(ThumbLeft, 0));
            ThumbButton->SetSize(ClassesImports::Point(ThumbRight - ThumbLeft, ThumbSize.Y));
            AfterThumbImage->SetImageKindX(GI_Main::ikxRightFill);
            BeforeThumbImage->SetImageKindX(GI_Main::ikxLeftFill);
            MarkerImage->SetPosition(EC_Struct::AddPoints(ThumbButton->LocalPosition, EC_Struct::HalfPoint(ThumbButton->ClientSize)));
            if (DecreaseButton->Kind == GI_GraphButton::gbkDisable) {
                DecreaseButton->SetDisabled(Position <= Minimum);
            }
            if (IncreaseButton->Kind == GI_GraphButton::gbkDisable) {
                IncreaseButton->SetDisabled(Position >= Maximum);
            }
        }
    }

    void TCountBarGI::AutoRepeat(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (DecreaseButton->Down) {
            SetPosition_2(Position - Step);
        } else if (IncreaseButton->Down) {
            SetPosition_2(Position + Step);
        } else if (RepeatTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
    }

    void TCountBarGI::DecreasePressed(GI_MessageLoop::TObjectGI* Sender) {
        SetPosition_2(Position - Step);
        if (RepeatTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
        RepeatTimer = MessageLoop->ScheduleCallbackTimer(300, 50, pas::bind_method<&TCountBarGI::AutoRepeat>(this), 0);
    }

    void TCountBarGI::IncreasePressed(GI_MessageLoop::TObjectGI* Sender) {
        SetPosition_2(Position + Step);
        if (RepeatTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(RepeatTimer);
            RepeatTimer = nullptr;
        }
        RepeatTimer = MessageLoop->ScheduleCallbackTimer(300, 50, pas::bind_method<&TCountBarGI::AutoRepeat>(this), 0);
    }

    void TCountBarGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        std::int32_t TrackStart{};
        std::int32_t TrackEnd{};
        GI_MessageLoop::TObjectGI::ProcessMouseMove(KeyState, Point);
        Point = ToLocalPoint(Point);
        if (ThumbButton->Down && Orientation == 1) {
            {
                std::int32_t cpp_left = DecreaseButton->GetMaxStateImageSize().X;
                TrackStart = cpp_left + ThumbButton->GetMaxStateImageSize().X / 2;
            }
            {
                std::int32_t cpp_right = IncreaseButton->GetMaxStateImageSize().X;
                std::int32_t cpp_left_2 = ClientSize.X - cpp_right;
                std::int32_t cpp_left_3 = ThumbButton->GetMaxStateImageSize().X;
                TrackEnd = cpp_left_2 - (cpp_left_3 - ThumbButton->GetMaxStateImageSize().X / 2);
            }
            if (Maximum - Minimum == 0) {
                SetPosition_2(Minimum);
            } else {
                SetPosition_2(static_cast<std::int32_t>(System::Round(pas::real_divide(Point.X - TrackStart, TrackEnd - TrackStart) * (Maximum - Minimum))) + Minimum);
            }
        }
    }

    void TCountBarGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
    }

    void TCountBarGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
    }

    void TCountBarGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        std::int32_t TrackStart{};
        std::int32_t TrackEnd{};
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (Active) {
            MessageLoop->SetFocusedControl(this);
        }
        Point = ToLocalPoint(Point);
        if (Orientation == 1) {
            {
                std::int32_t cpp_left = DecreaseButton->GetMaxStateImageSize().X;
                TrackStart = cpp_left + ThumbButton->GetMaxStateImageSize().X / 2;
            }
            {
                std::int32_t cpp_right = IncreaseButton->GetMaxStateImageSize().X;
                std::int32_t cpp_left_2 = ClientSize.X - cpp_right;
                std::int32_t cpp_left_3 = ThumbButton->GetMaxStateImageSize().X;
                TrackEnd = cpp_left_2 - (cpp_left_3 - ThumbButton->GetMaxStateImageSize().X / 2);
            }
            if (Point.X >= DecreaseButton->GetMaxStateImageSize().X && Point.X <= ([&] {
                std::int32_t cpp_right_2 = IncreaseButton->GetMaxStateImageSize().X;
                return ClientSize.X - cpp_right_2;
            }())) {
                if (Maximum - Minimum == 0) {
                    SetPosition_2(Minimum);
                } else {
                    SetPosition_2(static_cast<std::int32_t>(System::Round(pas::real_divide(Point.X - TrackStart, TrackEnd - TrackStart) * (Maximum - Minimum))) + Minimum);
                }
                ThumbButton->SetDown(true);
            }
        }
    }

    void TCountBarGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        ThumbButton->SetDown(false);
        if (MessageLoop->FocusedControl == this) {
            MessageLoop->SetFocusedControl(nullptr);
        }
    }

    void TCountBarGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        LoadCountBarProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TCountBarGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadCountBarProperties(Block);
    }

    void TCountBarGI::LoadCountBarProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"ImageDecNormal"_wref.get()) > 0) {
            DecreaseButton->SetImageNormalPath(Block->GetParam(u"ImageDecNormal"_wref.get()));
        }
        if (Block->CountParams(u"ImageDecNormalA"_wref.get()) > 0) {
            DecreaseButton->SetImageNormalActivePath(Block->GetParam(u"ImageDecNormalA"_wref.get()));
        }
        if (Block->CountParams(u"ImageDecDown"_wref.get()) > 0) {
            DecreaseButton->SetImageDownPath(Block->GetParam(u"ImageDecDown"_wref.get()));
        }
        if (Block->CountParams(u"ImageIncNormal"_wref.get()) > 0) {
            IncreaseButton->SetImageNormalPath(Block->GetParam(u"ImageIncNormal"_wref.get()));
        }
        if (Block->CountParams(u"ImageIncNormalA"_wref.get()) > 0) {
            IncreaseButton->SetImageNormalActivePath(Block->GetParam(u"ImageIncNormalA"_wref.get()));
        }
        if (Block->CountParams(u"ImageIncDown"_wref.get()) > 0) {
            IncreaseButton->SetImageDownPath(Block->GetParam(u"ImageIncDown"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackMin"_wref.get()) > 0) {
            AfterThumbImage->SetImagePath(Block->GetParam(u"ImageTrackMin"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackMax"_wref.get()) > 0) {
            BeforeThumbImage->SetImagePath(Block->GetParam(u"ImageTrackMax"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackPolNormal"_wref.get()) > 0) {
            ThumbButton->SetImageNormalPath(Block->GetParam(u"ImageTrackPolNormal"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackPolNormalA"_wref.get()) > 0) {
            ThumbButton->SetImageNormalActivePath(Block->GetParam(u"ImageTrackPolNormalA"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackPolDown"_wref.get()) > 0) {
            ThumbButton->SetImageDownPath(Block->GetParam(u"ImageTrackPolDown"_wref.get()));
        }
        if (Block->CountParams(u"ImageTrackUp"_wref.get()) > 0) {
            MarkerImage->SetImagePath(Block->GetParam(u"ImageTrackUp"_wref.get()));
        }
        UpdateLayout();
    }

    void TCountBarGI::p_destroy() {
        GI_CountBar::TCountBarGI_Destroy(this);
    }

} // namespace GI_CountBar
