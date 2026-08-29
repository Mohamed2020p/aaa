# ALIENTX — C++ Control-Panel UI

A clean, self-contained **Windows desktop menu application written in C++17** —
developed by **AlienX**. Zero external dependencies: the entire UI is hand-drawn
with the Windows GDI+ API (no ImGui, no offsets, no memory code, no game
integration of any kind). It is an educational reference for building polished
dark-theme desktop menus with custom controls.

> **Scope note:** this project is a standalone UI demo. It is *not* and cannot
> be used to tamper with Free Fire or any other online game — doing so violates
> the game's terms of service and is something this repo intentionally does not
> contain.

![status](https://img.shields.io/badge/status-maintenance-blue)
![deps](https://img.shields.io/badge/dependencies-none-brightgreen)
![license](https://img.shields.io/badge/license-MIT-lightgrey)

---

## Features

- **Login screen** — username + password with masked input, focus ring, blinking
  caret, inline error message, Enter-to-submit.
  - Demo credentials: **user `2`**, **password `2`** (in `src/settings.h`)
- **Dashboard** — custom title bar (draggable, close button), sidebar navigation
  with icons, top bar with user chip + logout, content cards, footer.
- **Six pages with working options**
  - **Overview** — welcome card, live uptime, version & accent stats, quick
    toggles, simulated live status.
  - **General** — notifications / start-on-boot / auto-update toggles, quality
    preset dropdown, master-volume slider.
  - **Interface** — **live accent-color theming** (5 swatches, recolors the
    whole UI instantly), live font-scale slider, animations toggle, compact
    mode (changes row density in real time).
  - **Network** — P2P mode, show-latency toggle, max-peers slider, live
    (simulated) status panel.
  - **Security** — 2FA toggle, session-timeout slider, change-password form
    with validation (in-memory only).
  - **About** — version, developer, build date, license.
- **UI/UX details** — dark theme, rounded cards, hover + pressed states on
  every control, anti-aliased vector icons, flicker-free double-buffered
  rendering, keyboard + mouse input, immediate-mode widget toolkit.

## Project structure

```
aaa/
├── CMakeLists.txt          # build (MSVC or MinGW)
├── LICENSE                 # MIT
├── README.md
└── src/
    ├── main.cpp            # WinMain, window class, message loop, paint
    ├── app.h / app.cpp     # app state, frame pipeline, login/logout
    ├── login.cpp           # sign-in screen
    ├── pages.cpp           # sidebar, top bar, the six option pages
    ├── ui.h / ui.cpp       # immediate-mode widget toolkit (buttons,
    │                       #   toggles, sliders, combos, swatches, fields)
    ├── theme.h / theme.cpp # palette, accents, rounded-rect helpers
    └── settings.h          # all options + account constants
```

## Building

Target: **Windows** (Windows 7+). Only system libraries are needed
(`gdiplus`, `ole32`).

### Visual Studio 2022 (easiest)

1. *File → Open → Folder* and select this repo.
2. In the CMake settings, pick **Debug/Release x64**.
3. Press **F5** (or build the `alienx` target).

### CMake + MSVC (command line)

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
build\Release\alienx.exe
```

### MinGW-w64 (g++)

```bat
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\alienx.exe
```

## Demo login

| Field    | Value |
|----------|-------|
| Username | `2`   |
| Password | `2`   |

These are **hard-coded demo credentials** in `src/settings.h`
(`account::kDefaultUser` / `kDefaultPassword`). Replace them with real
authentication (hashed storage, transport over TLS, …) before using this in
any real application — never ship plaintext passwords.

## How it works

- `main.cpp` creates a borderless `WS_POPUP` window and runs the message loop.
- Every paint is drawn to a memory DC (double buffering) and blitted once.
- A per-frame **input snapshot** (`ui::Input`) coalesces mouse/keyboard events
  between paints; widgets in `ui.cpp` are pure immediate-mode functions that
  test hit-rects and mutate the app's `Settings`.
- All state lives in `App::settings_` — no widget persists across frames,
  which keeps the toolkit small and predictable.

## Disclaimer

Educational project. No game reverse-engineering, no memory offsets, no
injection — by design.

---

Developed by **AlienX** · MIT License
