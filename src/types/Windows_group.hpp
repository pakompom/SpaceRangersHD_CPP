#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"

namespace Windows {
    struct TWin32FindDataA;

} // namespace Windows

namespace WindowsSdk {
    struct BITMAPV4HEADER;

    struct pas_ICONINFO;

    struct pas_MEMORYSTATUS;

    struct pas_OSVERSIONINFOA;

    struct pas_SECURITY_ATTRIBUTES;

    struct pas_SYSTEM_INFO;

    struct pas_WIN32_FIND_DATAW;

    struct tagBITMAPINFO;

    struct tagWNDCLASSW;

} // namespace WindowsSdk

namespace Windows {
    struct TSystemTime;

    struct TFileTime;

    struct TMsg;

    struct TTrackMouseEvent;

} // namespace Windows

namespace WindowsSdk {
    struct tagRGBQUAD;

    struct tagCIEXYZ;

    struct tagICEXYZTRIPLE;

    struct tagBITMAPINFOHEADER;

} // namespace WindowsSdk

namespace Windows {
    #pragma pack(push, 1)
    struct TSystemTime {
        std::uint16_t wYear;
        std::uint16_t wMonth;
        std::uint16_t wDayOfWeek;
        std::uint16_t wDay;
        std::uint16_t wHour;
        std::uint16_t wMinute;
        std::uint16_t wSecond;
        std::uint16_t wMilliseconds;
    };
    #pragma pack(pop)

} // namespace Windows

namespace WindowsSdk {
    using TPoint = Types::TPoint;

    using TRect = Types::TRect;

} // namespace WindowsSdk

namespace Windows {
    #pragma pack(push, 1)
    struct TFileTime {
        std::uint32_t dwLowDateTime;
        std::uint32_t dwHighDateTime;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TWin32FindDataA {
        std::uint32_t dwFileAttributes;
        TFileTime ftCreationTime;
        TFileTime ftLastAccessTime;
        TFileTime ftLastWriteTime;
        std::uint32_t nFileSizeHigh;
        std::uint32_t nFileSizeLow;
        std::uint32_t dwReserved0;
        std::uint32_t dwReserved1;
        pas::Array<std::uint8_t, 0, 259> cFileName;
        pas::Array<std::uint8_t, 0, 13> cAlternateFileName;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

} // namespace Windows

namespace WindowsSdk {
    using BOOL = std::int32_t;

    using HWND = std::uint32_t;

    using THandle = SystemImports::THandle;

    using PInteger = System::PInteger;

    using HMODULE = SystemImports::HMODULE;

    using PDWORD = std::uint32_t*;

    using PSingle = float*;

    using TWOHandleArray = pas::Array<THandle, 0, 63>;

    using PWOHandleArray = TWOHandleArray*;

    using PWORD = std::uint16_t*;

    using TWin32FindData = Windows::TWin32FindDataA;

    using PRect = Types::PRect;

    using SHORT = std::int16_t;

    #pragma pack(push, 1)
    struct pas_SECURITY_ATTRIBUTES {
        std::uint32_t nLength;
        void* lpSecurityDescriptor;
        BOOL bInheritHandle;
    };
    #pragma pack(pop)

    using TSecurityAttributes = pas_SECURITY_ATTRIBUTES;

    using PSecurityAttributes = TSecurityAttributes*;

} // namespace WindowsSdk

namespace Windows {
    #pragma pack(push, 1)
    struct TMsg {
        std::uint32_t hwnd;
        std::uint32_t message;
        std::uint32_t wParam;
        std::int32_t lParam;
        std::uint32_t time;
        WindowsSdk::TPoint pt;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TTrackMouseEvent {
        std::uint32_t cbSize;
        std::uint32_t dwFlags;
        std::uint32_t hwndTrack;
        std::uint32_t dwHoverTime;
    };
    #pragma pack(pop)

} // namespace Windows

namespace WindowsSdk {
    using UINT = std::uint32_t;

    using WPARAM = std::int32_t;

    using LPARAM = std::int32_t;

    using HBITMAP = std::uint32_t;

    #pragma pack(push, 1)
    struct pas_ICONINFO {
        BOOL fIcon;
        std::uint32_t xHotspot;
        std::uint32_t yHotspot;
        HBITMAP hbmMask;
        HBITMAP hbmColor;
    };
    #pragma pack(pop)

    using TIconInfo = pas_ICONINFO;

    using HICON = std::uint32_t;

    using HGDIOBJ = std::uint32_t;

    using HCURSOR = HICON;

    using HDC = std::uint32_t;

    using FXPT2DOT30 = std::int32_t;

    #pragma pack(push, 1)
    struct tagCIEXYZ {
        FXPT2DOT30 ciexyzX;
        FXPT2DOT30 ciexyzY;
        FXPT2DOT30 ciexyzZ;
    };
    #pragma pack(pop)

    using TCIEXYZ = tagCIEXYZ;

    #pragma pack(push, 1)
    struct tagICEXYZTRIPLE {
        TCIEXYZ ciexyzRed;
        TCIEXYZ ciexyzGreen;
        TCIEXYZ ciexyzBlue;
    };
    #pragma pack(pop)

    using TCIEXYZTriple = tagICEXYZTRIPLE;

    #pragma pack(push, 1)
    struct BITMAPV4HEADER {
        std::uint32_t bV4Size;
        std::int32_t bV4Width;
        std::int32_t bV4Height;
        std::uint16_t bV4Planes;
        std::uint16_t bV4BitCount;
        std::uint32_t bV4V4Compression;
        std::uint32_t bV4SizeImage;
        std::int32_t bV4XPelsPerMeter;
        std::int32_t bV4YPelsPerMeter;
        std::uint32_t bV4ClrUsed;
        std::uint32_t bV4ClrImportant;
        std::uint32_t bV4RedMask;
        std::uint32_t bV4GreenMask;
        std::uint32_t bV4BlueMask;
        std::uint32_t bV4AlphaMask;
        std::uint32_t bV4CSType;
        TCIEXYZTriple bV4Endpoints;
        std::uint32_t bV4GammaRed;
        std::uint32_t bV4GammaGreen;
        std::uint32_t bV4GammaBlue;
    };
    #pragma pack(pop)

    using TBitmapV4Header = BITMAPV4HEADER;

    #pragma pack(push, 1)
    struct tagBITMAPINFOHEADER {
        std::uint32_t biSize;
        std::int32_t biWidth;
        std::int32_t biHeight;
        std::uint16_t biPlanes;
        std::uint16_t biBitCount;
        std::uint32_t biCompression;
        std::uint32_t biSizeImage;
        std::int32_t biXPelsPerMeter;
        std::int32_t biYPelsPerMeter;
        std::uint32_t biClrUsed;
        std::uint32_t biClrImportant;
    };
    #pragma pack(pop)

    using TBitmapInfoHeader = tagBITMAPINFOHEADER;

    #pragma pack(push, 1)
    struct tagRGBQUAD {
        std::uint8_t rgbBlue;
        std::uint8_t rgbGreen;
        std::uint8_t rgbRed;
        std::uint8_t rgbReserved;
    };
    #pragma pack(pop)

    using TRGBQuad = tagRGBQUAD;

    #pragma pack(push, 1)
    struct tagBITMAPINFO {
        TBitmapInfoHeader bmiHeader;
        pas::Array<TRGBQuad, 0, 0> bmiColors;
    };
    #pragma pack(pop)

    using TBitmapInfo = tagBITMAPINFO;

    using PBitmapInfo = TBitmapInfo*;

    using TFarProc = void*;

    using TFNThreadStartRoutine = TFarProc;

    using PPoint = Types::PPoint;

    using HKEY = std::uint32_t;

    using HRGN = std::uint32_t;

    using ACCESS_MASK = std::uint32_t;

    using REGSAM = ACCESS_MASK;

    #pragma pack(push, 1)
    struct pas_SYSTEM_INFO {
        std::uint32_t dwOemId;
        std::uint8_t cpp_padding[32];
    };
    #pragma pack(pop)

    using TSystemInfo = pas_SYSTEM_INFO;

    using PDouble = SystemImports::PDouble;

    using TFNWndProc = TFarProc;

    using HINST = SystemImports::HINST;

    using HBRUSH = std::uint32_t;

    #pragma pack(push, 1)
    struct tagWNDCLASSW {
        UINT style;
        TFNWndProc lpfnWndProc;
        std::int32_t cbClsExtra;
        std::int32_t cbWndExtra;
        HINST hInstance;
        HICON hIcon;
        HCURSOR hCursor;
        HBRUSH hbrBackground;
        char16_t* lpszMenuName;
        char16_t* lpszClassName;
    };
    #pragma pack(pop)

    using TWndClassW = tagWNDCLASSW;

} // namespace WindowsSdk

namespace Windows {
    using HResult = std::int32_t;

    using TLargeInteger = std::int64_t;

} // namespace Windows

namespace WindowsSdk {
    using MakeIntResourceA = std::uint8_t*;

    using MakeIntResource = MakeIntResourceA;

    using ATOM = std::uint16_t;

    using TFNTimerProc = TFarProc;

    using LRESULT = std::int32_t;

    #pragma pack(push, 1)
    struct pas_MEMORYSTATUS {
        std::uint32_t dwLength;
        std::uint32_t dwMemoryLoad;
        std::uint32_t dwTotalPhys;
        std::uint32_t dwAvailPhys;
        std::uint32_t dwTotalPageFile;
        std::uint32_t dwAvailPageFile;
        std::uint32_t dwTotalVirtual;
        std::uint32_t dwAvailVirtual;
    };
    #pragma pack(pop)

    using TMemoryStatus = pas_MEMORYSTATUS;

    #pragma pack(push, 1)
    struct pas_OSVERSIONINFOA {
        std::uint32_t dwOSVersionInfoSize;
        std::uint32_t dwMajorVersion;
        std::uint32_t dwMinorVersion;
        std::uint32_t dwBuildNumber;
        std::uint32_t dwPlatformId;
        // Maintenance AnsiString for PSS usage
        pas::Array<std::uint8_t, 0, 127> szCSDVersion;
    };
    #pragma pack(pop)

    using TOSVersionInfoA = pas_OSVERSIONINFOA;

    using TOSVersionInfo = TOSVersionInfoA;

    #pragma pack(push, 1)
    struct pas_WIN32_FIND_DATAW {
        std::uint32_t dwFileAttributes;
        Windows::TFileTime ftCreationTime;
        Windows::TFileTime ftLastAccessTime;
        Windows::TFileTime ftLastWriteTime;
        std::uint32_t nFileSizeHigh;
        std::uint32_t nFileSizeLow;
        std::uint32_t dwReserved0;
        std::uint32_t dwReserved1;
        pas::Array<char16_t, 0, 259> cFileName;
        pas::Array<char16_t, 0, 13> cAlternateFileName;
    };
    #pragma pack(pop)

    using TWin32FindDataW = pas_WIN32_FIND_DATAW;

    using HGLOBAL = THandle;

    using LCID = std::uint32_t;

    // Primary language IDs.
    inline constexpr std::int32_t LANG_NEUTRAL = 0x00000000;

    // user default
    inline constexpr std::int32_t SUBLANG_DEFAULT = 0x00000001;

    // Sorting IDs.
    // sorting default
    inline constexpr std::int32_t SORT_DEFAULT = 0x00000000;

    inline constexpr std::int32_t LANG_USER_DEFAULT = WindowsSdk::SUBLANG_DEFAULT << 10 | WindowsSdk::LANG_NEUTRAL;

    inline constexpr std::int32_t LOCALE_USER_DEFAULT = WindowsSdk::SORT_DEFAULT << 16 | WindowsSdk::LANG_USER_DEFAULT;

    // line 724
    inline constexpr std::int32_t STATUS_WAIT_0 = 0x00000000;

    inline constexpr std::int32_t STATUS_ABANDONED_WAIT_0 = 0x00000080;

    inline constexpr std::int32_t STATUS_TIMEOUT = 0x00000102;

    // value that gets a thread to LowRealtime-1
    inline constexpr std::int32_t THREAD_BASE_PRIORITY_LOWRT = 15;

    inline constexpr std::int32_t SYNCHRONIZE = 0x00100000;

    inline constexpr std::int32_t FILE_ATTRIBUTE_READONLY = 0x00000001;

    inline constexpr std::int32_t READ_CONTROL = 0x00020000;

    // SYNCHRONIZE              = $00100000; defined above
    // STANDARD_RIGHTS_REQUIRED = $000F0000; defined above
    inline constexpr std::int32_t STANDARD_RIGHTS_READ = WindowsSdk::READ_CONTROL;

    inline constexpr std::int32_t STANDARD_RIGHTS_WRITE = WindowsSdk::READ_CONTROL;

    inline constexpr std::int32_t STANDARD_RIGHTS_ALL = 0x001f0000;

    // line 4700
    // Registry Specific Access Rights.
    inline constexpr std::int32_t KEY_QUERY_VALUE = 0x00000001;

    inline constexpr std::int32_t KEY_SET_VALUE = 0x00000002;

    inline constexpr std::int32_t KEY_CREATE_SUB_KEY = 0x00000004;

    inline constexpr std::int32_t KEY_ENUMERATE_SUB_KEYS = 0x00000008;

    inline constexpr std::int32_t KEY_NOTIFY = 0x00000010;

    inline constexpr std::int32_t KEY_CREATE_LINK = 0x00000020;

    inline constexpr std::int32_t KEY_WOW64_RES = 0x00000300;

    inline constexpr std::int32_t KEY_READ = (WindowsSdk::STANDARD_RIGHTS_READ | WindowsSdk::KEY_QUERY_VALUE | WindowsSdk::KEY_ENUMERATE_SUB_KEYS | WindowsSdk::KEY_NOTIFY) & ~WindowsSdk::SYNCHRONIZE;

    inline constexpr std::int32_t KEY_WRITE = (WindowsSdk::STANDARD_RIGHTS_WRITE | WindowsSdk::KEY_SET_VALUE | WindowsSdk::KEY_CREATE_SUB_KEY) & ~WindowsSdk::SYNCHRONIZE;

    inline constexpr std::int32_t KEY_ALL_ACCESS = (WindowsSdk::STANDARD_RIGHTS_ALL | WindowsSdk::KEY_QUERY_VALUE | WindowsSdk::KEY_SET_VALUE | WindowsSdk::KEY_CREATE_SUB_KEY | WindowsSdk::KEY_ENUMERATE_SUB_KEYS | WindowsSdk::KEY_NOTIFY | WindowsSdk::KEY_CREATE_LINK) & ~WindowsSdk::SYNCHRONIZE;

    inline constexpr std::int32_t REG_SZ = 1;

    inline constexpr std::uint32_t WAIT_FAILED = 0xffffffffu;

    inline constexpr std::int32_t WAIT_OBJECT_0 = WindowsSdk::STATUS_WAIT_0 + 0;

    inline constexpr std::int32_t WAIT_ABANDONED_0 = WindowsSdk::STATUS_ABANDONED_WAIT_0 + 0;

    inline constexpr std::int32_t WAIT_TIMEOUT = WindowsSdk::STATUS_TIMEOUT;

    inline constexpr std::int32_t GMEM_MOVEABLE = 2;

    inline constexpr std::int32_t CREATE_SUSPENDED = 0x00000004;

    inline constexpr std::int32_t REALTIME_PRIORITY_CLASS = 0x00000100;

    inline constexpr std::int32_t THREAD_PRIORITY_NORMAL = 0;

    inline constexpr std::int32_t THREAD_PRIORITY_TIME_CRITICAL = WindowsSdk::THREAD_BASE_PRIORITY_LOWRT;

    // Infinite timeout
    inline constexpr std::uint32_t INFINITE = 0xffffffffu;

    inline constexpr std::int32_t HEAP_ZERO_MEMORY = 0x00000008;

    inline constexpr std::int32_t FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200;

    inline constexpr std::int32_t FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;

    inline constexpr std::int32_t FORMAT_MESSAGE_ARGUMENT_ARRAY = 0x00002000;

    // Define the severity codes
    // The operation completed successfully.
    inline constexpr std::int32_t ERROR_SUCCESS = 0;

    inline constexpr std::int32_t BI_RGB = 0;

    inline constexpr std::int32_t BLACK_BRUSH = 4;

    // color table in RGBs
    inline constexpr std::int32_t DIB_RGB_COLORS = 0;

    inline constexpr std::int32_t SW_SHOWNORMAL = 1;

    inline constexpr std::int32_t SW_SHOWMAXIMIZED = 3;

    inline constexpr std::int32_t SW_MINIMIZE = 6;

    inline constexpr std::int32_t VK_BACK = 8;

    inline constexpr std::int32_t VK_TAB = 9;

    inline constexpr std::int32_t VK_RETURN = 13;

    inline constexpr std::int32_t VK_SHIFT = 0x00000010;

    inline constexpr std::int32_t VK_CONTROL = 17;

    inline constexpr std::int32_t VK_MENU = 18;

    inline constexpr std::int32_t VK_PAUSE = 19;

    inline constexpr std::int32_t VK_ESCAPE = 27;

    inline constexpr std::int32_t VK_SPACE = 0x00000020;

    inline constexpr std::int32_t VK_PRIOR = 33;

    inline constexpr std::int32_t VK_NEXT = 34;

    inline constexpr std::int32_t VK_END = 35;

    inline constexpr std::int32_t VK_HOME = 36;

    inline constexpr std::int32_t VK_LEFT = 37;

    inline constexpr std::int32_t VK_UP = 38;

    inline constexpr std::int32_t VK_RIGHT = 39;

    inline constexpr std::int32_t VK_DOWN = 40;

    inline constexpr std::int32_t VK_INSERT = 45;

    inline constexpr std::int32_t VK_DELETE = 46;

    inline constexpr std::int32_t VK_NUMPAD0 = 96;

    inline constexpr std::int32_t VK_ADD = 107;

    inline constexpr std::int32_t VK_SUBTRACT = 109;

    inline constexpr std::int32_t VK_DECIMAL = 110;

    inline constexpr std::int32_t VK_F1 = 112;

    inline constexpr std::int32_t VK_F2 = 113;

    inline constexpr std::int32_t VK_F3 = 114;

    inline constexpr std::int32_t VK_F5 = 116;

    inline constexpr std::int32_t VK_F6 = 117;

    inline constexpr std::int32_t VK_F7 = 118;

    inline constexpr std::int32_t VK_F8 = 119;

    inline constexpr std::int32_t VK_F9 = 120;

    inline constexpr std::int32_t VK_F11 = 122;

    inline constexpr std::int32_t VK_OEM_PLUS = 187;

    inline constexpr std::int32_t VK_OEM_3 = 192;

    inline constexpr std::int32_t GWL_STYLE = -16;

    // Value for rolling one detent
    inline constexpr std::int32_t WHEEL_DELTA = 120;

    inline constexpr std::int32_t MK_LBUTTON = 1;

    inline constexpr std::int32_t MK_RBUTTON = 2;

    inline constexpr std::int32_t TME_LEAVE = 0x00000002;

    inline constexpr std::int32_t CF_UNICODETEXT = 13;

    inline constexpr std::int32_t PM_REMOVE = 1;

    inline constexpr std::int32_t SWP_SHOWWINDOW = 0x00000040;

    inline constexpr WindowsSdk::HWND HWND_NOTOPMOST = static_cast<WindowsSdk::HWND>(-2);

    inline constexpr std::int32_t MB_OK = 0x00000000;

    inline constexpr std::int32_t MB_TASKMODAL = 0x00002000;

    inline constexpr std::uint32_t HKEY_CURRENT_USER = 0x80000001u;

} // namespace WindowsSdk
