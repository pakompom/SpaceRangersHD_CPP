# Space Rangers HD: A War Apart - C++

*Космические рейнджеры HD: Революция*

A C++20 translation of the [recovered Delphi source](https://github.com/pakompom/SpaceRangersHD_decomp)
for **2.1.2500**, targeting **32-bit Windows**. The original menus, rendering,
music and gameplay code are retained. It builds and runs under Windows/Wine;
campaign, quest, arcade and save/load testing is still incomplete.

## Build

On Linux or macOS, install **Zig 0.16.0**, **Ninja** and **Python 3**, then run:

```sh
./build.sh
```

This produces `build/Rangers.exe` and debug symbols. The build uses `-O2`,
C++ exceptions and x87 extended precision. libffi is built from included sources;
no Delphi compiler or translator is needed.

## Run

Copy `Rangers.exe` into a separate copy of a **2.1.2500** game installation,
keeping its data directories and original DLLs. Run it with that directory as
the working directory, on Windows or through Wine/CrossOver with 32-bit support.
Game data and DLLs are not included here.

## Source

| Directory | Contents |
| --- | --- |
| `src/` | Translated game units and program entry point |
| `src/units/` | Unit APIs |
| `src/types/` | Class and record definitions |
| `src/layout/` | Recovered layout checks |
| `runtime/` | Delphi semantics and Windows adapters |
| `third_party/` | libffi source and its license |
| `resources/` | Executable resources |

Include `src/units/Unit.hpp` for a unit's API; `src/translated.hpp` is the optional
umbrella header. The C++ can be edited and rebuilt directly.

Unofficial; not affiliated with the game's developers or publisher.
See [rights and attribution](NOTICE.md).
