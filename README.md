# Space Rangers HD: A War Apart - C++

*Космические рейнджеры HD: Революция*

A C++20 translation of the [recovered Delphi source](https://github.com/pakompom/SpaceRangersHD_decomp)
for **2.1.2500**, targeting **32-bit Windows**. The original menus, rendering,
music and gameplay code are retained. It builds and runs under Windows/Wine;
campaign, quest, arcade and save/load testing is still incomplete.

## Build

On Linux or macOS, install **Zig 0.17.0**, **Ninja** and **Python 3**, then run:

```sh
./build.sh -j 6
```

This produces `build/Rangers.exe` and debug symbols. The build uses `-O2`,
C++ exceptions and hardware-PC24 arithmetic, with SSE2 on 32-bit x86.
libffi is built from included sources; no Delphi compiler or translator is needed.

## Floating-point policy

The default is fixed **PC24, round to nearest/even**: game expressions explicitly
round at the original operation boundaries using hardware `float`, with `double`
storage for Extended values. Mixed-width midpoint corrections, certified constant
products, RNG endpoints and quest conversion retain the tested legacy rounding.
Bézier factors use a separate exponent to avoid premature float overflow and
underflow. Decimal and script parsing retain representable small values.

This imports the arithmetic from the native hardware-PC24 experiment onto clean
translator output (`6fd806c`), without its loading, equipment-query or combat-stat
caches. Source and runtime must be updated together; replacing only a header or
enabling a compiler precision flag does not supply the operation boundaries.

The arithmetic requires IEEE binary32/binary64, source-width evaluation, gradual
underflow and disabled contraction (`-ffp-contract=off`). The build selects
`-msse2 -mfpmath=sse`; compile-time checks reject excess precision and fast-math.
See the [Clang floating-point rules](https://clang.llvm.org/docs/UsersManual.html#controlling-floating-point-behavior)
and [GCC x86 evaluation options](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html).
Do not enable fast-math or change the host rounding/flush-to-zero modes.

This is a portable arithmetic policy, not full x87 emulation or a native Linux/macOS
game port. Control-word changes and floating traps are not emulated; general
expressions do not preserve the full binary80 exponent range. Host math libraries
can differ for transcendental functions. Raw Extended storage is eight bytes, not
Delphi's ten. The native experiment's finite replay results are evidence for that
variant, not a guarantee of complete campaign parity for this Windows build.

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
