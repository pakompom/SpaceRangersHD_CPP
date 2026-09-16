#include "units/GR_Main.hpp"
#include "units/SE_Garbage.hpp"
#include "units/SysUtils.hpp"
#include "units/aGalaxy.hpp"

// Memory diagnostics. SE_Garbage ownership is inferred
// from the native aGalaxy dependency family beside FGInt/FGIntRSA and the
// process API units; the original discarded declarations remain unknown.
namespace SE_Garbage {
    std::int64_t PreviousVirtualUsageMB = 0;

    std::int64_t PreviousPhysicalUsageMB = 0;

    // Recursive checks return immediately.
    std::uint8_t CheckingMemoryUsage = false;

    // At most one warning per process.
    std::uint8_t LowMemoryWarningShown = false;

    void CheckMemoryUsage() {
        std::int64_t Usage{};
        std::uint8_t Changed{};
        GR_Main::TMemoryStatusEx Status{};
        // Nested in CheckMemoryUsage; unused caller-popped static link.
        auto ShowLowMemoryWarning = [&](pas::WideString TextKey) -> void {
            if (static_cast<std::uint8_t>(LowMemoryWarningShown ^ 1) && aGalaxy::Galaxy != nullptr) {
                LowMemoryWarningShown = true;
                aGalaxy::TGalaxy::ShowLocalizedWarning(TextKey);
            }
        };
        if (!CheckingMemoryUsage) {
            CheckingMemoryUsage = true;
            Changed = false;
            Status.Length = static_cast<std::int32_t>(sizeof(GR_Main::TMemoryStatusEx));
            GR_Main::GlobalMemoryStatusEx(Status);
            Usage = Status.TotalVirtual - Status.AvailVirtual >> 20;
            if (Usage > 512 && pas::abs(Usage - PreviousVirtualUsageMB) > 256) {
                if (PreviousVirtualUsageMB != 0) {
                    Changed = true;
                    if (Usage > PreviousVirtualUsageMB) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Virtual memory usage is increasing (", SysUtils::Int64ToStr(Usage), " MB)"}));
                    } else {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Virtual memory usage is decreasing (", SysUtils::Int64ToStr(Usage), " MB)"}));
                    }
                }
                PreviousVirtualUsageMB = Usage;
            }
            if (Status.AvailVirtual >> 20 < 100) {
                ShowLowMemoryWarning(u"Warning.LowVirtualMemory"_w);
            }
            Usage = Status.TotalPhys - Status.AvailPhys >> 20;
            if (Usage > 512 && pas::abs(Usage - PreviousPhysicalUsageMB) > 256) {
                if (PreviousPhysicalUsageMB != 0) {
                    Changed = true;
                    if (Usage > PreviousPhysicalUsageMB) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Physical memory usage is increasing (", SysUtils::Int64ToStr(Usage), " MB)"}));
                    } else {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Physical memory usage is decreasing (", SysUtils::Int64ToStr(Usage), " MB)"}));
                    }
                }
                PreviousPhysicalUsageMB = Usage;
            }
            if (Status.AvailPhys >> 20 < 100) {
                ShowLowMemoryWarning(u"Warning.LowPhysicalMemory"_w);
            }
            if (Changed) {
                GR_Main::LogMemoryUsage();
            }
            CheckingMemoryUsage = false;
        }
    }

} // namespace SE_Garbage
