#include "layout/GI_ScrollBar.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_ScrollBar.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_ScrollBar {
    void TScrollBarGI_Create(TScrollBarGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->RepeatTimer = nullptr;
        Self->UpImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->UpImages[0]->SetDepth(1.0);
        Self->UpImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->UpImages[1]->SetDepth(1.0);
        Self->UpImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->UpImages[2]->SetDepth(1.0);
        Self->BeforeThumbBarImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BeforeThumbBarImages[0]->SetDepth(1.0);
        Self->BeforeThumbBarImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BeforeThumbBarImages[1]->SetDepth(1.0);
        Self->BeforeThumbBarImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->BeforeThumbBarImages[2]->SetDepth(1.0);
        Self->ThumbTopImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbTopImages[0]->SetDepth(0.0);
        Self->ThumbTopImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbTopImages[1]->SetDepth(0.0);
        Self->ThumbTopImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbTopImages[2]->SetDepth(0.0);
        Self->ThumbCenterImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbCenterImages[0]->SetDepth(0.0);
        Self->ThumbCenterImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbCenterImages[1]->SetDepth(0.0);
        Self->ThumbCenterImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbCenterImages[2]->SetDepth(0.0);
        Self->ThumbBottomImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbBottomImages[0]->SetDepth(0.0);
        Self->ThumbBottomImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbBottomImages[1]->SetDepth(0.0);
        Self->ThumbBottomImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->ThumbBottomImages[2]->SetDepth(0.0);
        Self->AfterThumbBarImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->AfterThumbBarImages[0]->SetDepth(1.0);
        Self->AfterThumbBarImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->AfterThumbBarImages[1]->SetDepth(1.0);
        Self->AfterThumbBarImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->AfterThumbBarImages[2]->SetDepth(1.0);
        Self->DownImages[0] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->DownImages[0]->SetDepth(1.0);
        Self->DownImages[1] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->DownImages[1]->SetDepth(1.0);
        Self->DownImages[2] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->DownImages[2]->SetDepth(1.0);
        Self->Minimum = 0;
        Self->Maximum = 99;
        Self->Position = 0;
        Self->LargeChange = 1;
        Self->SmallChange = 1;
        Self->PageSize = 1;
        Self->Orientation = 2;
        Self->CalculationMode = 0;
    }

    void TScrollBarGI_Destroy(TScrollBarGI* Self) {
        pas::free(Self->UpImages[0]);
        pas::free(Self->UpImages[1]);
        pas::free(Self->UpImages[2]);
        pas::free(Self->BeforeThumbBarImages[0]);
        pas::free(Self->BeforeThumbBarImages[1]);
        pas::free(Self->BeforeThumbBarImages[2]);
        pas::free(Self->ThumbTopImages[0]);
        pas::free(Self->ThumbTopImages[1]);
        pas::free(Self->ThumbTopImages[2]);
        pas::free(Self->ThumbCenterImages[0]);
        pas::free(Self->ThumbCenterImages[1]);
        pas::free(Self->ThumbCenterImages[2]);
        pas::free(Self->ThumbBottomImages[0]);
        pas::free(Self->ThumbBottomImages[1]);
        pas::free(Self->ThumbBottomImages[2]);
        pas::free(Self->AfterThumbBarImages[0]);
        pas::free(Self->AfterThumbBarImages[1]);
        pas::free(Self->AfterThumbBarImages[2]);
        pas::free(Self->DownImages[0]);
        pas::free(Self->DownImages[1]);
        pas::free(Self->DownImages[2]);
        GI_Panel::TPanelGI_Destroy(Self);
    }

    // Resets the range to 0..99 and clears callbacks; does not call inherited Clear.
    void TScrollBarGI::Clear() {
        Minimum = 0;
        Maximum = 99;
        Position = 0;
        LargeChange = 1;
        SmallChange = 1;
        PageSize = 1;
        Orientation = 2;
        CalculationMode = 0;
        PressedRegion = 0;
        HoveredRegion = 0;
        StopAutoRepeat();
        PositionChangedCallback = nullptr;
    }

    void TScrollBarGI::SetUpPosition(Types::TPoint Point) {
        UpImages[0]->SetPosition(Point);
        UpImages[1]->SetPosition(Point);
        UpImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetBeforeThumbBarPosition(Types::TPoint Point) {
        BeforeThumbBarImages[0]->SetPosition(Point);
        BeforeThumbBarImages[1]->SetPosition(Point);
        BeforeThumbBarImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetThumbTopPosition(Types::TPoint Point) {
        ThumbTopImages[0]->SetPosition(Point);
        ThumbTopImages[1]->SetPosition(Point);
        ThumbTopImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetThumbCenterPosition(Types::TPoint Point) {
        ThumbCenterImages[0]->SetPosition(Point);
        ThumbCenterImages[1]->SetPosition(Point);
        ThumbCenterImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetThumbBottomPosition(Types::TPoint Point) {
        ThumbBottomImages[0]->SetPosition(Point);
        ThumbBottomImages[1]->SetPosition(Point);
        ThumbBottomImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetAfterThumbBarPosition(Types::TPoint Point) {
        AfterThumbBarImages[0]->SetPosition(Point);
        AfterThumbBarImages[1]->SetPosition(Point);
        AfterThumbBarImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetDownPosition(Types::TPoint Point) {
        DownImages[0]->SetPosition(Point);
        DownImages[1]->SetPosition(Point);
        DownImages[2]->SetPosition(Point);
    }

    void TScrollBarGI::SetBeforeThumbBarSize(Types::TPoint Size) {
        BeforeThumbBarImages[0]->SetSize(Size);
        BeforeThumbBarImages[1]->SetSize(Size);
        BeforeThumbBarImages[2]->SetSize(Size);
    }

    void TScrollBarGI::SetThumbCenterSize(Types::TPoint Size) {
        ThumbCenterImages[0]->SetSize(Size);
        ThumbCenterImages[1]->SetSize(Size);
        ThumbCenterImages[2]->SetSize(Size);
    }

    void TScrollBarGI::SetAfterThumbBarSize(Types::TPoint Size) {
        AfterThumbBarImages[0]->SetSize(Size);
        AfterThumbBarImages[1]->SetSize(Size);
        AfterThumbBarImages[2]->SetSize(Size);
    }

    // Returns 0 outside, 1/2 arrows, 3/4 page regions, or 5 thumb; only tests the scrolling axis.
    std::int32_t TScrollBarGI::GetHitRegion(Types::TPoint Point) {
        std::int32_t UpEnd{};
        std::int32_t ThumbStart{};
        std::int32_t ThumbEnd{};
        std::int32_t DownStart{};
        if (Orientation == 1) {
            UpEnd = UpImages[0]->ClientSize.X;
            ThumbStart = ThumbTopImages[0]->LocalPosition.X;
            ThumbEnd = ThumbBottomImages[0]->LocalPosition.X + ThumbBottomImages[0]->ClientSize.X;
            DownStart = ClientSize.X - DownImages[0]->ClientSize.X;
            if (Point.X < 0) {
                return 0;
            } else if (Point.X <= UpEnd) {
                return 1;
            } else if (Point.X <= ThumbStart) {
                return 3;
            } else if (Point.X <= ThumbEnd) {
                return 5;
            } else if (Point.X <= DownStart) {
                return 4;
            } else if (Point.X <= ClientSize.X) {
                return 2;
            } else {
                return 0;
            }
        }
        UpEnd = UpImages[0]->ClientSize.Y;
        ThumbStart = ThumbTopImages[0]->LocalPosition.Y;
        ThumbEnd = ThumbBottomImages[0]->LocalPosition.Y + ThumbBottomImages[0]->ClientSize.Y;
        DownStart = ClientSize.Y - DownImages[0]->ClientSize.Y;
        if (Point.Y < 0) {
            return 0;
        } else if (Point.Y <= UpEnd) {
            return 1;
        } else if (Point.Y <= ThumbStart) {
            return 3;
        } else if (Point.Y <= ThumbEnd) {
            return 5;
        } else if (Point.Y <= DownStart) {
            return 4;
        } else if (Point.Y <= ClientSize.Y) {
            return 2;
        } else {
            return 0;
        }
    }

    void TScrollBarGI::SetRange(std::int32_t MinValue, std::int32_t MaxValue) {
        if (MinimumLabel != nullptr) {
            MinimumLabel->SetText(pas::wide_int_to_str(MinValue));
        }
        if (MaximumLabel != nullptr) {
            MaximumLabel->SetText(pas::wide_int_to_str(MaxValue));
        }
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
            if (Maximum - Minimum + 1 < PageSize) {
                SetPosition_2(Minimum);
            }
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // Does not invoke PositionChangedCallback.
    void TScrollBarGI::SetPositionInternal(std::int32_t NewPosition) {
        std::int32_t OldPosition = Position;
        if (PositionLabel != nullptr) {
            PositionLabel->SetText(pas::wide_int_to_str(NewPosition));
        }
        if (Position != NewPosition) {
            Position = NewPosition;
            if (Position < Minimum) {
                Position = Minimum;
            }
            if (CalculationMode == 0) {
                if (Position > Maximum) {
                    Position = Maximum;
                }
            } else {
                if (Maximum - PageSize + 1 < Position) {
                    Position = Maximum - PageSize + 1;
                }
                if (Position < Minimum) {
                    Position = Minimum;
                }
            }
            if (PositionLabel != nullptr) {
                PositionLabel->SetText(pas::wide_int_to_str(Position));
            }
            if (OldPosition != Position) {
                if (Active == true) {
                    UpdateLayout();
                    Invalidate();
                }
            }
        }
    }

    // Notifies only while Active and only when the clamped position changes.
    void TScrollBarGI::SetPosition_2(std::int32_t NewPosition) {
        std::int32_t OldPosition = Position;
        if (PositionLabel != nullptr) {
            PositionLabel->SetText(pas::wide_int_to_str(NewPosition));
        }
        if (Position != NewPosition) {
            Position = NewPosition;
            if (Position < Minimum) {
                Position = Minimum;
            }
            if (CalculationMode == 0) {
                if (Position > Maximum) {
                    Position = Maximum;
                }
            } else {
                if (Maximum - PageSize + 1 < Position) {
                    Position = Maximum - PageSize + 1;
                }
                if (Position < Minimum) {
                    Position = Minimum;
                }
            }
            if (PositionLabel != nullptr) {
                PositionLabel->SetText(pas::wide_int_to_str(Position));
            }
            if (OldPosition != Position) {
                if (Active == true) {
                    UpdateLayout();
                    Invalidate();
                    if (pas::assigned(PositionChangedCallback)) {
                        PositionChangedCallback(this);
                    }
                }
            }
        }
    }

    void TScrollBarGI::SetSmallChange(std::int32_t Value) {
        if (SmallChange != Value) {
            SmallChange = Value;
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // A value equal to SmallChange is ignored even if LargeChange differs.
    void TScrollBarGI::SetLargeChange(std::int32_t Value) {
        if (SmallChange != Value) {
            LargeChange = Value;
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // Caps at Maximum-Minimum+1; no lower bound check.
    void TScrollBarGI::SetPageSize(std::int32_t Value) {
        if (PageSize != Value) {
            PageSize = Value;
            if (PageSize > Maximum - Minimum + 1) {
                PageSize = Maximum - Minimum + 1;
            }
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    // Value 1 is horizontal; other values use vertical layout.
    void TScrollBarGI::SetOrientation(std::int32_t Value) {
        if (Orientation != Value) {
            Orientation = Value;
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    void TScrollBarGI::SetKindCalcMode(std::int32_t Value) {
        if (CalculationMode != Value) {
            CalculationMode = Value;
            if (Active == true) {
                UpdateLayout();
                Invalidate();
            }
        }
    }

    void TScrollBarGI::SetConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::SetConfigPath(Path);
        if (Active == true) {
            UpdateLayout();
            Invalidate();
        }
    }

    void TScrollBarGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        if (Active == true) {
            UpdateLayout();
        }
    }

    void TScrollBarGI::SetActive(std::uint8_t Enabled) {
        GI_MessageLoop::TObjectGI::SetActive(Enabled);
        if (Enabled == true) {
            UpdateLayout();
        }
    }

    void TScrollBarGI::UpdateLayout() {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t TrackLength{};
        std::int32_t BeforeLength{};
        std::int32_t ThumbLength{};
        std::int32_t AfterLength{};
        std::int32_t MinimumThumbLength{};
        if (Active) {
            if (Orientation == 1) {
                Y = ClientSize.Y / 2;
                if (UpImages[0]->ClientSize.X + DownImages[0]->ClientSize.X + ThumbTopImages[0]->ClientSize.X + ThumbBottomImages[0]->ClientSize.X >= ClientSize.X) {
                    X = 0;
                    SetUpPosition(ClassesImports::Point(X, Y - UpImages[0]->ClientSize.Y / 2));
                    X += UpImages[0]->ClientSize.X;
                    SetThumbTopPosition(ClassesImports::Point(X, Y - ThumbTopImages[0]->ClientSize.Y / 2));
                    SetBeforeThumbBarPosition(ClassesImports::Point(X, Y - BeforeThumbBarImages[0]->ClientSize.Y / 2));
                    SetBeforeThumbBarSize(ClassesImports::Point(BeforeThumbBarImages[0]->ClientSize.X, BeforeThumbBarImages[0]->ClientSize.Y));
                    X += ThumbTopImages[0]->ClientSize.X;
                    ThumbCenterImages[0]->SetActive(false);
                    ThumbCenterImages[1]->SetActive(false);
                    ThumbCenterImages[2]->SetActive(false);
                    SetThumbBottomPosition(ClassesImports::Point(X, Y - ThumbBottomImages[0]->ClientSize.Y / 2));
                    SetAfterThumbBarPosition(ClassesImports::Point(X, Y - AfterThumbBarImages[0]->ClientSize.Y / 2));
                    SetAfterThumbBarSize(ClassesImports::Point(AfterThumbBarImages[0]->ClientSize.X, AfterThumbBarImages[0]->ClientSize.Y));
                    X += ThumbBottomImages[0]->ClientSize.X;
                    SetDownPosition(ClassesImports::Point(X, Y - DownImages[0]->ClientSize.Y / 2));
                } else {
                    TrackLength = ClientSize.X - UpImages[0]->ClientSize.X - DownImages[0]->ClientSize.X;
                    if (Maximum - Minimum + 1 == 0) {
                        ThumbLength = TrackLength;
                    } else {
                        ThumbLength = System::Trunc(pas::real_divide(PageSize, Maximum - Minimum + 1) * TrackLength);
                        if (ThumbLength > TrackLength) {
                            ThumbLength = TrackLength;
                        }
                    }
                    MinimumThumbLength = 0;
                    if (ThumbTopImages[0]->ClientSize.X + ThumbBottomImages[0]->ClientSize.X >= ThumbLength) {
                        ThumbLength = ThumbTopImages[0]->ClientSize.X + ThumbBottomImages[0]->ClientSize.X;
                        MinimumThumbLength = ThumbLength;
                    }
                    if (CalculationMode == 0) {
                        if (MinimumThumbLength == 0) {
                            if (Maximum - Minimum + 1 == 0) {
                                BeforeLength = 0;
                            } else {
                                BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum + 1) * TrackLength);
                            }
                        } else if (Maximum - Minimum == 0) {
                            BeforeLength = 0;
                        } else {
                            BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum) * (TrackLength - MinimumThumbLength));
                        }
                    } else {
                        if (Maximum - Minimum + 1 == 0) {
                            BeforeLength = 0;
                        } else {
                            BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum + 1) * TrackLength);
                        }
                        if (BeforeLength + ThumbLength > TrackLength) {
                            BeforeLength = TrackLength - ThumbLength;
                        }
                    }
                    AfterLength = TrackLength - BeforeLength - ThumbLength;
                    if (ThumbTopImages[0]->ClientSize.X + ThumbBottomImages[0]->ClientSize.X >= ThumbLength) {
                        ThumbCenterImages[0]->SetActive(false);
                        ThumbCenterImages[1]->SetActive(false);
                        ThumbCenterImages[2]->SetActive(false);
                        SetThumbCenterSize(ClassesImports::Point(0, ThumbCenterImages[0]->ClientSize.Y));
                    } else {
                        ThumbCenterImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 5 || PressedRegion == 5));
                        ThumbCenterImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 5);
                        ThumbCenterImages[2]->SetActive(PressedRegion == 5);
                        SetThumbCenterPosition(ClassesImports::Point(UpImages[0]->ClientSize.X + ThumbTopImages[0]->ClientSize.X + BeforeLength, Y - ThumbCenterImages[0]->ClientSize.Y / 2));
                        SetThumbCenterSize(ClassesImports::Point(ThumbLength - (ThumbTopImages[0]->ClientSize.X + ThumbBottomImages[0]->ClientSize.X), ThumbCenterImages[0]->ClientSize.Y));
                    }
                    SetUpPosition(ClassesImports::Point(0, Y - UpImages[0]->ClientSize.Y / 2));
                    SetDownPosition(ClassesImports::Point(ClientSize.X - DownImages[0]->ClientSize.X, Y - DownImages[0]->ClientSize.Y / 2));
                    SetThumbTopPosition(ClassesImports::Point(UpImages[0]->ClientSize.X + BeforeLength, Y - ThumbTopImages[0]->ClientSize.Y / 2));
                    SetThumbBottomPosition(ClassesImports::Point(BeforeLength + ThumbLength - ThumbBottomImages[0]->ClientSize.X + UpImages[0]->ClientSize.X, Y - ThumbBottomImages[0]->ClientSize.Y / 2));
                    SetBeforeThumbBarPosition(ClassesImports::Point(UpImages[0]->ClientSize.X, Y - BeforeThumbBarImages[0]->ClientSize.Y / 2));
                    SetBeforeThumbBarSize(ClassesImports::Point(ThumbTopImages[0]->ClientSize.X + BeforeLength, BeforeThumbBarImages[0]->ClientSize.Y));
                    SetAfterThumbBarPosition(ClassesImports::Point(BeforeLength + ThumbLength - ThumbBottomImages[0]->ClientSize.X + DownImages[0]->ClientSize.X, Y - AfterThumbBarImages[0]->ClientSize.Y / 2));
                    SetAfterThumbBarSize(ClassesImports::Point(ThumbBottomImages[0]->ClientSize.X + AfterLength, AfterThumbBarImages[0]->ClientSize.Y));
                }
            } else {
                X = ClientSize.X / 2;
                if (UpImages[0]->ClientSize.Y + DownImages[0]->ClientSize.Y + ThumbTopImages[0]->ClientSize.Y + ThumbBottomImages[0]->ClientSize.Y >= ClientSize.Y) {
                    Y = 0;
                    SetUpPosition(ClassesImports::Point(X - UpImages[0]->ClientSize.X / 2, Y));
                    Y += UpImages[0]->ClientSize.Y;
                    SetThumbTopPosition(ClassesImports::Point(X - ThumbTopImages[0]->ClientSize.X / 2, Y));
                    SetBeforeThumbBarPosition(ClassesImports::Point(X - BeforeThumbBarImages[0]->ClientSize.X / 2, Y));
                    SetBeforeThumbBarSize(ClassesImports::Point(BeforeThumbBarImages[0]->ClientSize.X, BeforeThumbBarImages[0]->ClientSize.Y));
                    Y += ThumbTopImages[0]->ClientSize.Y;
                    ThumbCenterImages[0]->SetActive(false);
                    ThumbCenterImages[1]->SetActive(false);
                    ThumbCenterImages[2]->SetActive(false);
                    SetThumbBottomPosition(ClassesImports::Point(X - ThumbBottomImages[0]->ClientSize.X / 2, Y));
                    SetAfterThumbBarPosition(ClassesImports::Point(X - AfterThumbBarImages[0]->ClientSize.X / 2, Y));
                    SetAfterThumbBarSize(ClassesImports::Point(AfterThumbBarImages[0]->ClientSize.X, AfterThumbBarImages[0]->ClientSize.Y));
                    Y += ThumbBottomImages[0]->ClientSize.Y;
                    SetDownPosition(ClassesImports::Point(X - DownImages[0]->ClientSize.X / 2, Y));
                } else {
                    TrackLength = ClientSize.Y - UpImages[0]->ClientSize.Y - DownImages[0]->ClientSize.Y;
                    if (Maximum - Minimum + 1 == 0) {
                        ThumbLength = TrackLength;
                    } else {
                        ThumbLength = System::Trunc(pas::real_divide(PageSize, Maximum - Minimum + 1) * TrackLength);
                        if (ThumbLength > TrackLength) {
                            ThumbLength = TrackLength;
                        }
                    }
                    MinimumThumbLength = 0;
                    if (ThumbTopImages[0]->ClientSize.Y + ThumbBottomImages[0]->ClientSize.Y >= ThumbLength) {
                        ThumbLength = ThumbTopImages[0]->ClientSize.Y + ThumbBottomImages[0]->ClientSize.Y;
                        MinimumThumbLength = ThumbLength;
                    }
                    if (CalculationMode == 0) {
                        if (MinimumThumbLength == 0) {
                            if (Maximum - Minimum + 1 == 0) {
                                BeforeLength = 0;
                            } else {
                                BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum + 1) * TrackLength);
                            }
                        } else if (Maximum - Minimum == 0) {
                            BeforeLength = 0;
                        } else {
                            BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum) * (TrackLength - MinimumThumbLength));
                        }
                    } else {
                        if (Maximum - Minimum + 1 == 0) {
                            BeforeLength = 0;
                        } else {
                            BeforeLength = System::Trunc(pas::real_divide(Position - Minimum, Maximum - Minimum + 1) * TrackLength);
                        }
                        if (BeforeLength + ThumbLength > TrackLength) {
                            BeforeLength = TrackLength - ThumbLength;
                        }
                    }
                    AfterLength = TrackLength - BeforeLength - ThumbLength;
                    if (ThumbTopImages[0]->ClientSize.Y + ThumbBottomImages[0]->ClientSize.Y >= ThumbLength) {
                        ThumbCenterImages[0]->SetActive(false);
                        ThumbCenterImages[1]->SetActive(false);
                        ThumbCenterImages[2]->SetActive(false);
                        SetThumbCenterSize(ClassesImports::Point(ThumbCenterImages[0]->ClientSize.X, 0));
                    } else {
                        ThumbCenterImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 5 || PressedRegion == 5));
                        ThumbCenterImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 5);
                        ThumbCenterImages[2]->SetActive(PressedRegion == 5);
                        SetThumbCenterPosition(ClassesImports::Point(X - ThumbCenterImages[0]->ClientSize.X / 2, UpImages[0]->ClientSize.Y + ThumbTopImages[0]->ClientSize.Y + BeforeLength));
                        SetThumbCenterSize(ClassesImports::Point(ThumbCenterImages[0]->ClientSize.X, ThumbLength - (ThumbTopImages[0]->ClientSize.Y + ThumbBottomImages[0]->ClientSize.Y)));
                    }
                    SetUpPosition(ClassesImports::Point(X - UpImages[0]->ClientSize.X / 2, 0));
                    SetDownPosition(ClassesImports::Point(X - DownImages[0]->ClientSize.X / 2, ClientSize.Y - DownImages[0]->ClientSize.Y));
                    SetThumbTopPosition(ClassesImports::Point(X - ThumbTopImages[0]->ClientSize.X / 2, UpImages[0]->ClientSize.Y + BeforeLength));
                    SetThumbBottomPosition(ClassesImports::Point(X - ThumbBottomImages[0]->ClientSize.X / 2, BeforeLength + ThumbLength - ThumbBottomImages[0]->ClientSize.Y + UpImages[0]->ClientSize.Y));
                    SetBeforeThumbBarPosition(ClassesImports::Point(X - BeforeThumbBarImages[0]->ClientSize.X / 2, UpImages[0]->ClientSize.Y));
                    SetBeforeThumbBarSize(ClassesImports::Point(BeforeThumbBarImages[0]->ClientSize.X, ThumbTopImages[0]->ClientSize.Y + BeforeLength));
                    SetAfterThumbBarPosition(ClassesImports::Point(X - AfterThumbBarImages[0]->ClientSize.X / 2, BeforeLength + ThumbLength - ThumbBottomImages[0]->ClientSize.Y + DownImages[0]->ClientSize.Y));
                    SetAfterThumbBarSize(ClassesImports::Point(AfterThumbBarImages[0]->ClientSize.X, ThumbBottomImages[0]->ClientSize.Y + AfterLength));
                }
            }
            UpImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 1 || PressedRegion == 1));
            UpImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 1);
            UpImages[2]->SetActive(PressedRegion == 1);
            BeforeThumbBarImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 3 || PressedRegion == 3));
            BeforeThumbBarImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 3);
            BeforeThumbBarImages[2]->SetActive(PressedRegion == 3);
            ThumbTopImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 5 || PressedRegion == 5));
            ThumbTopImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 5);
            ThumbTopImages[2]->SetActive(PressedRegion == 5);
            ThumbBottomImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 5 || PressedRegion == 5));
            ThumbBottomImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 5);
            ThumbBottomImages[2]->SetActive(PressedRegion == 5);
            AfterThumbBarImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 4 || PressedRegion == 4));
            AfterThumbBarImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 4);
            AfterThumbBarImages[2]->SetActive(PressedRegion == 4);
            DownImages[0]->SetActive(!(PressedRegion == 0 && HoveredRegion == 2 || PressedRegion == 2));
            DownImages[1]->SetActive(PressedRegion == 0 && HoveredRegion == 2);
            DownImages[2]->SetActive(PressedRegion == 2);
        }
    }

    // Uses the up-arrow image for scrollbar thickness.
    void TScrollBarGI::UpdateSizeForOrientation() {
        if (Orientation == 1) {
            SetSize(ClassesImports::Point(ClientSize.X, UpImages[0]->ClientSize.Y));
        } else {
            SetSize(ClassesImports::Point(UpImages[0]->ClientSize.X, ClientSize.Y));
        }
    }

    void TScrollBarGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        GI_Panel::TPanelGI::ProcessMouseMove(KeyState, Point);
        if (PressedRegion == 5) {
            if (Orientation == 1) {
                if (Maximum - Minimum == 0) {
                    SetPosition_2(Minimum);
                } else if (CalculationMode == 0) {
                    SetPosition_2(static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.X - UpImages[0]->ClientSize.X - DownImages[0]->ClientSize.X - ThumbCenterImages[0]->ClientSize.X - ThumbTopImages[0]->ClientSize.X - ThumbBottomImages[0]->ClientSize.X) * (Point.X - HitTestBounds.Left - UpImages[0]->ClientSize.X))) + DragStartPosition);
                } else {
                    SetPosition_2(static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.X - UpImages[0]->ClientSize.X - DownImages[0]->ClientSize.X) * (Point.X - HitTestBounds.Left - UpImages[0]->ClientSize.X))) + DragStartPosition);
                }
            } else if (Maximum - Minimum == 0) {
                SetPosition_2(Minimum);
            } else if (CalculationMode == 0) {
                SetPosition_2(static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.Y - UpImages[0]->ClientSize.Y - DownImages[0]->ClientSize.Y - ThumbCenterImages[0]->ClientSize.Y - ThumbTopImages[0]->ClientSize.Y - ThumbBottomImages[0]->ClientSize.Y) * (Point.Y - HitTestBounds.Top - UpImages[0]->ClientSize.Y))) + DragStartPosition);
            } else {
                SetPosition_2(static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.Y - UpImages[0]->ClientSize.Y - DownImages[0]->ClientSize.Y) * (Point.Y - HitTestBounds.Top - UpImages[0]->ClientSize.Y))) + DragStartPosition);
            }
        }
        HoveredRegion = GetHitRegion(ToLocalPoint(Point));
        UpdateLayout();
        Invalidate();
    }

    void TScrollBarGI::OnMouseEnter() {
        GI_Panel::TPanelGI::OnMouseEnter();
    }

    void TScrollBarGI::OnMouseLeave() {
        GI_Panel::TPanelGI::OnMouseLeave();
        if (PressedRegion == 0) {
            HoveredRegion = 0;
        }
        if (Active == true) {
            UpdateLayout();
            Invalidate();
        }
    }

    void TScrollBarGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (Active) {
            MessageLoop->SetFocusedControl(this);
        }
        HoveredRegion = GetHitRegion(ToLocalPoint(Point));
        PressedRegion = HoveredRegion;
        if (PressedRegion == 1) {
            SetPosition_2(Position - SmallChange);
            StartAutoRepeat(500, 50);
        } else if (PressedRegion == 2) {
            SetPosition_2(Position + SmallChange);
            StartAutoRepeat(500, 50);
        } else if (PressedRegion == 3) {
            SetPosition_2(Position - LargeChange);
            StartAutoRepeat(500, 50);
        } else if (PressedRegion == 4) {
            SetPosition_2(Position + LargeChange);
            StartAutoRepeat(500, 50);
        } else if (PressedRegion == 5) {
            StopAutoRepeat();
            if (Maximum - Minimum == 0) {
                DragStartPosition = Minimum;
            } else if (Orientation == 1) {
                if (CalculationMode == 0) {
                    DragStartPosition = Position - static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.X - UpImages[0]->ClientSize.X - DownImages[0]->ClientSize.X - ThumbCenterImages[0]->ClientSize.X - ThumbTopImages[0]->ClientSize.X - ThumbBottomImages[0]->ClientSize.X) * (Point.X - HitTestBounds.Left - UpImages[0]->ClientSize.X)));
                } else {
                    DragStartPosition = Position - static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.X - UpImages[0]->ClientSize.X - DownImages[0]->ClientSize.X) * (Point.X - HitTestBounds.Left - UpImages[0]->ClientSize.X)));
                }
            } else if (CalculationMode == 0) {
                DragStartPosition = Position - static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.Y - UpImages[0]->ClientSize.Y - DownImages[0]->ClientSize.Y - ThumbCenterImages[0]->ClientSize.Y - ThumbTopImages[0]->ClientSize.Y - ThumbBottomImages[0]->ClientSize.Y) * (Point.Y - HitTestBounds.Top - UpImages[0]->ClientSize.Y)));
            } else {
                DragStartPosition = Position - static_cast<std::int32_t>(System::Trunc(pas::real_divide(Maximum - Minimum, ClientSize.Y - UpImages[0]->ClientSize.Y - DownImages[0]->ClientSize.Y) * (Point.Y - HitTestBounds.Top - UpImages[0]->ClientSize.Y)));
            }
        }
        UpdateLayout();
        Invalidate();
    }

    void TScrollBarGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        PressedRegion = 0;
        if (!HitTestCursor()) {
            HoveredRegion = 0;
        }
        if (MessageLoop->FocusedControl == this) {
            MessageLoop->SetFocusedControl(nullptr);
        }
        UpdateLayout();
        StopAutoRepeat();
    }

    void TScrollBarGI::ProcessLeftButtonDoubleClick(std::uint32_t KeyState, Types::TPoint Point) {
    }

    void TScrollBarGI::StartAutoRepeat(std::int32_t DelayMs, std::int32_t RepeatMs) {
        StopAutoRepeat();
        RepeatTimer = MessageLoop->ScheduleCallbackTimer(DelayMs, RepeatMs, pas::bind_method<&TScrollBarGI::AutoRepeat>(this), 0);
    }

    void TScrollBarGI::StopAutoRepeat() {
        if (RepeatTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(RepeatTimer);
        }
        RepeatTimer = nullptr;
    }

    void TScrollBarGI::AutoRepeat(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (PressedRegion == 1) {
            SetPosition_2(Position - SmallChange);
        } else if (PressedRegion == 2) {
            SetPosition_2(Position + SmallChange);
        } else if (PressedRegion == 3) {
            SetPosition_2(Position - LargeChange);
        } else if (PressedRegion == 4) {
            SetPosition_2(Position + LargeChange);
        }
    }

    void TScrollBarGI_LoadFromConfigPath(TScrollBarGI* Self, const pas::WideString& Path) {
        GI_Panel::TPanelGI_LoadFromConfigPath(Self, Path);
        Self->LoadScrollBarProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TScrollBarGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        LoadScrollBarProperties(Block);
    }

    void TScrollBarGI::LoadScrollBarProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Min"_wref.get()) > 0) {
            Minimum = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Min"_wref.get())));
        }
        if (Block->CountParams(u"Max"_wref.get()) > 0) {
            Maximum = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Max"_wref.get())));
        }
        if (Block->CountParams(u"PageSize"_wref.get()) > 0) {
            PageSize = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"PageSize"_wref.get())));
        }
        if (Block->CountParams(u"LargeChange"_wref.get()) > 0) {
            LargeChange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"LargeChange"_wref.get())));
        }
        if (Block->CountParams(u"SmallChange"_wref.get()) > 0) {
            SmallChange = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SmallChange"_wref.get())));
        }
        if (Block->CountParams(u"Position"_wref.get()) > 0) {
            Position = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Position"_wref.get())));
        }
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            if (Block->GetParam(u"Kind"_wref.get()) == u"x") {
                Orientation = 1;
            } else {
                Orientation = 2;
            }
        }
        if (Block->CountParams(u"KindCalc"_wref.get()) > 0) {
            CalculationMode = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"KindCalc"_wref.get())));
        }
        if (Block->CountParams(u"ImageUpN"_wref.get()) > 0) {
            UpImages[0]->SetImagePath(Block->GetParam(u"ImageUpN"_wref.get()));
        }
        if (Block->CountParams(u"ImageUpA"_wref.get()) > 0) {
            UpImages[1]->SetImagePath(Block->GetParam(u"ImageUpA"_wref.get()));
        }
        if (Block->CountParams(u"ImageUpD"_wref.get()) > 0) {
            UpImages[2]->SetImagePath(Block->GetParam(u"ImageUpD"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarN"_wref.get()) > 0) {
            BeforeThumbBarImages[0]->SetImagePath(Block->GetParam(u"ImageBarN"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarA"_wref.get()) > 0) {
            BeforeThumbBarImages[1]->SetImagePath(Block->GetParam(u"ImageBarA"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarD"_wref.get()) > 0) {
            BeforeThumbBarImages[2]->SetImagePath(Block->GetParam(u"ImageBarD"_wref.get()));
        }
        if (Block->CountParams(u"ImageTopN"_wref.get()) > 0) {
            ThumbTopImages[0]->SetImagePath(Block->GetParam(u"ImageTopN"_wref.get()));
        }
        if (Block->CountParams(u"ImageTopA"_wref.get()) > 0) {
            ThumbTopImages[1]->SetImagePath(Block->GetParam(u"ImageTopA"_wref.get()));
        }
        if (Block->CountParams(u"ImageTopD"_wref.get()) > 0) {
            ThumbTopImages[2]->SetImagePath(Block->GetParam(u"ImageTopD"_wref.get()));
        }
        if (Block->CountParams(u"ImageCenterN"_wref.get()) > 0) {
            ThumbCenterImages[0]->SetImagePath(Block->GetParam(u"ImageCenterN"_wref.get()));
        }
        if (Block->CountParams(u"ImageCenterA"_wref.get()) > 0) {
            ThumbCenterImages[1]->SetImagePath(Block->GetParam(u"ImageCenterA"_wref.get()));
        }
        if (Block->CountParams(u"ImageCenterD"_wref.get()) > 0) {
            ThumbCenterImages[2]->SetImagePath(Block->GetParam(u"ImageCenterD"_wref.get()));
        }
        if (Block->CountParams(u"ImageBottomN"_wref.get()) > 0) {
            ThumbBottomImages[0]->SetImagePath(Block->GetParam(u"ImageBottomN"_wref.get()));
        }
        if (Block->CountParams(u"ImageBottomA"_wref.get()) > 0) {
            ThumbBottomImages[1]->SetImagePath(Block->GetParam(u"ImageBottomA"_wref.get()));
        }
        if (Block->CountParams(u"ImageBottomD"_wref.get()) > 0) {
            ThumbBottomImages[2]->SetImagePath(Block->GetParam(u"ImageBottomD"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarN"_wref.get()) > 0) {
            AfterThumbBarImages[0]->SetImagePath(Block->GetParam(u"ImageBarN"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarA"_wref.get()) > 0) {
            AfterThumbBarImages[1]->SetImagePath(Block->GetParam(u"ImageBarA"_wref.get()));
        }
        if (Block->CountParams(u"ImageBarD"_wref.get()) > 0) {
            AfterThumbBarImages[2]->SetImagePath(Block->GetParam(u"ImageBarD"_wref.get()));
        }
        if (Block->CountParams(u"ImageDownN"_wref.get()) > 0) {
            DownImages[0]->SetImagePath(Block->GetParam(u"ImageDownN"_wref.get()));
        }
        if (Block->CountParams(u"ImageDownA"_wref.get()) > 0) {
            DownImages[1]->SetImagePath(Block->GetParam(u"ImageDownA"_wref.get()));
        }
        if (Block->CountParams(u"ImageDownD"_wref.get()) > 0) {
            DownImages[2]->SetImagePath(Block->GetParam(u"ImageDownD"_wref.get()));
        }
        {
            Types::TPoint contentSize = UpImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg = UpImages[0];
            cpp_arg->SetSize(contentSize);
        }
        UpImages[0]->SetImageKindX(GI_Main::ikxCenter);
        UpImages[0]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_2 = UpImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_2 = UpImages[1];
            cpp_arg_2->SetSize(contentSize_2);
        }
        UpImages[1]->SetImageKindX(GI_Main::ikxCenter);
        UpImages[1]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_3 = UpImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_3 = UpImages[2];
            cpp_arg_3->SetSize(contentSize_3);
        }
        UpImages[2]->SetImageKindX(GI_Main::ikxCenter);
        UpImages[2]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_4 = BeforeThumbBarImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_4 = BeforeThumbBarImages[0];
            cpp_arg_4->SetSize(contentSize_4);
        }
        BeforeThumbBarImages[0]->SetImageKindX(GI_Main::ikxLeftFill);
        BeforeThumbBarImages[0]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_5 = BeforeThumbBarImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_5 = BeforeThumbBarImages[1];
            cpp_arg_5->SetSize(contentSize_5);
        }
        BeforeThumbBarImages[1]->SetImageKindX(GI_Main::ikxLeftFill);
        BeforeThumbBarImages[1]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_6 = BeforeThumbBarImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_6 = BeforeThumbBarImages[2];
            cpp_arg_6->SetSize(contentSize_6);
        }
        BeforeThumbBarImages[2]->SetImageKindX(GI_Main::ikxLeftFill);
        BeforeThumbBarImages[2]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_7 = ThumbTopImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_7 = ThumbTopImages[0];
            cpp_arg_7->SetSize(contentSize_7);
        }
        ThumbTopImages[0]->SetImageKindX(GI_Main::ikxCenter);
        ThumbTopImages[0]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_8 = ThumbTopImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_8 = ThumbTopImages[1];
            cpp_arg_8->SetSize(contentSize_8);
        }
        ThumbTopImages[1]->SetImageKindX(GI_Main::ikxCenter);
        ThumbTopImages[1]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_9 = ThumbTopImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_9 = ThumbTopImages[2];
            cpp_arg_9->SetSize(contentSize_9);
        }
        ThumbTopImages[2]->SetImageKindX(GI_Main::ikxCenter);
        ThumbTopImages[2]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_10 = ThumbCenterImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_10 = ThumbCenterImages[0];
            cpp_arg_10->SetSize(contentSize_10);
        }
        ThumbCenterImages[0]->SetImageKindX(GI_Main::ikxLeftFill);
        ThumbCenterImages[0]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_11 = ThumbCenterImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_11 = ThumbCenterImages[1];
            cpp_arg_11->SetSize(contentSize_11);
        }
        ThumbCenterImages[1]->SetImageKindX(GI_Main::ikxLeftFill);
        ThumbCenterImages[1]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_12 = ThumbCenterImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_12 = ThumbCenterImages[2];
            cpp_arg_12->SetSize(contentSize_12);
        }
        ThumbCenterImages[2]->SetImageKindX(GI_Main::ikxLeftFill);
        ThumbCenterImages[2]->SetImageKindY(GI_Main::ikyTopFill);
        {
            Types::TPoint contentSize_13 = ThumbBottomImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_13 = ThumbBottomImages[0];
            cpp_arg_13->SetSize(contentSize_13);
        }
        ThumbBottomImages[0]->SetImageKindX(GI_Main::ikxCenter);
        ThumbBottomImages[0]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_14 = ThumbBottomImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_14 = ThumbBottomImages[1];
            cpp_arg_14->SetSize(contentSize_14);
        }
        ThumbBottomImages[1]->SetImageKindX(GI_Main::ikxCenter);
        ThumbBottomImages[1]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_15 = ThumbBottomImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_15 = ThumbBottomImages[2];
            cpp_arg_15->SetSize(contentSize_15);
        }
        ThumbBottomImages[2]->SetImageKindX(GI_Main::ikxCenter);
        ThumbBottomImages[2]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_16 = AfterThumbBarImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_16 = AfterThumbBarImages[0];
            cpp_arg_16->SetSize(contentSize_16);
        }
        AfterThumbBarImages[0]->SetImageKindX(GI_Main::ikxRightFill);
        AfterThumbBarImages[0]->SetImageKindY(GI_Main::ikyBottomFill);
        {
            Types::TPoint contentSize_17 = AfterThumbBarImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_17 = AfterThumbBarImages[1];
            cpp_arg_17->SetSize(contentSize_17);
        }
        AfterThumbBarImages[1]->SetImageKindX(GI_Main::ikxRightFill);
        AfterThumbBarImages[1]->SetImageKindY(GI_Main::ikyBottomFill);
        {
            Types::TPoint contentSize_18 = AfterThumbBarImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_18 = AfterThumbBarImages[2];
            cpp_arg_18->SetSize(contentSize_18);
        }
        AfterThumbBarImages[2]->SetImageKindX(GI_Main::ikxRightFill);
        AfterThumbBarImages[2]->SetImageKindY(GI_Main::ikyBottomFill);
        {
            Types::TPoint contentSize_19 = DownImages[0]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_19 = DownImages[0];
            cpp_arg_19->SetSize(contentSize_19);
        }
        DownImages[0]->SetImageKindX(GI_Main::ikxCenter);
        DownImages[0]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_20 = DownImages[1]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_20 = DownImages[1];
            cpp_arg_20->SetSize(contentSize_20);
        }
        DownImages[1]->SetImageKindX(GI_Main::ikxCenter);
        DownImages[1]->SetImageKindY(GI_Main::ikyCenter);
        {
            Types::TPoint contentSize_21 = DownImages[2]->GetContentSize();
            GI_Image::TImageGI* cpp_arg_21 = DownImages[2];
            cpp_arg_21->SetSize(contentSize_21);
        }
        DownImages[2]->SetImageKindX(GI_Main::ikxCenter);
        DownImages[2]->SetImageKindY(GI_Main::ikyCenter);
        UpdateLayout();
    }

    void TScrollBarGI::p_destroy() {
        GI_ScrollBar::TScrollBarGI_Destroy(this);
    }

    void TScrollBarGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_ScrollBar::TScrollBarGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_ScrollBar
