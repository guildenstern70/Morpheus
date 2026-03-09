# AGENTS

Agent onboarding and working agreement for the `Morpheus` repository.

## 1) Project Snapshot

- **Name**: `Morpheus`
- **Type**: C++ game prototype
- **Style goal**: Atari Asteroids-like 2D gameplay
- **Current gameplay state**:
  - A wireframe green ship shaped like an `A`
  - Ship starts centered on screen (`800x600`)
  - Rotation controls:
    - `Left Arrow`: counterclockwise
    - `Right Arrow`: clockwise
  - Rotation speed is delta-time scaled
  - Main loop is capped to ~60 FPS

## 2) Tech Stack

- **Language**: C++17
- **Build system**: CMake (minimum 3.15)
- **Primary library**: SDL3 (`find_package(SDL3 3.4.2 REQUIRED)`)
- **Compiler warnings**:
  - MSVC: `/W4`
  - Others: `-Wall -Wextra -Wpedantic`

## 3) Repository Layout

- `src/` - C++ source files (current game loop lives in `src/main.cpp`)
- `include/` - headers (currently minimal/empty)
- `assets/` - runtime assets copied to executable output dir after build
  - `images/ship.png` is a visual reference for ship shape
- `CMakeLists.txt` - build definition and SDL3 linkage
- `README.md` - user-facing project setup notes
- `build/`, `cmake-build-*` - generated build output (do not hand-edit)

## 4) Build and Run

Preferred from repository root:

```bash
cmake -S . -B build
cmake --build build
./build/Morpheus
```

Alternative (as documented in README):

```bash
cd build
cmake ..
cmake --build .
./Morpheus
```

## 5) Runtime and Render Model

- Single executable target: `Morpheus`
- Single-threaded SDL event/update/render loop in `mainLoop`
- Frame flow:
  1. Poll input events
  2. Compute `deltaSeconds`
  3. Update ship angle
  4. Render scene
  5. Delay remaining frame budget for 60 FPS cap

## 6) Controls (Current)

- `ESC`: Quit
- `Left Arrow`: Turn left
- `Right Arrow`: Turn right

## 7) Source-of-Truth Notes

- SDL version in code/CMake is **SDL3**.

## 8) Coding Guidelines for Agents

- Keep changes minimal and targeted.
- Prefer small, composable constants for gameplay tuning.
- Use Object Oriented Programming (OOP) principles for future extensibility (e.g. `Ship` class).
- Follow naming style as in Google C++ Style guide - https://google.github.io/styleguide/cppguide.html#General_Naming_Rules
- Preserve the existing style in touched files.
- Avoid introducing non-ASCII unless already present and needed.
- Add comments only where logic is non-obvious.
- Do not refactor unrelated code in the same change.

## 9) Safety Rules for Editing

- Never edit generated files or directories:
  - `build/`
  - `cmake-build-*/`
  - `CMakeFiles/`, `CMakeCache.txt`, `compile_commands.json`
- Do not remove or overwrite user changes unrelated to the current task.
- If unexpected modifications appear during work, pause and request guidance.

## 10) Validation Checklist for Code Changes

When code is modified, run:

```bash
cmake --build build
```

Then perform a quick run smoke test to ensure startup and input/render still work.

## 11) Near-Term Backlog Suggestions

- Add ship thrust and velocity integration
- Add asteroid spawning and movement
- Add projectile shooting and collision detection
- Add score/lives/game-over loop
- Update `README.md` controls and SDL wording to match implementation

